/*
 *  Copyright © 2017-2023 Wellington Wallace
 *
 *  This file is part of Easy Effects.
 *
 *  Easy Effects is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Easy Effects is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Easy Effects. If not, see <https://www.gnu.org/licenses/>.
 */

#include "plugin_base.hpp"

PluginBase::PluginBase(std::string plugin_name,
                       std::string package,
                       const std::string& schema,
                       const std::string& schema_path)
    : name(std::move(plugin_name)),
      package(std::move(package)),
      settings(g_settings_new_with_path(schema.c_str(), schema_path.c_str())) {
  if (name != "output_level" && name != "spectrum") {
    bypass = g_settings_get_boolean(settings, "bypass") != 0;

    gconnections.push_back(g_signal_connect(settings, "changed::bypass",
                                            G_CALLBACK(+[](GSettings* settings, char* key, gpointer user_data) {
                                              auto self = static_cast<PluginBase*>(user_data);

                                              self->bypass = g_settings_get_boolean(settings, "bypass") != 0;
                                            }),
                                            this));
  }

  pf_data.pb = this;

  const auto filter_name = "ee_" + log_tag.substr(0U, log_tag.size() - 2U) + "_" + name;

  pm->lock();

  auto* props_filter = pw_properties_new(nullptr, nullptr);

  pw_properties_set(props_filter, PW_KEY_APP_ID, tags::app::id);
  pw_properties_set(props_filter, PW_KEY_NODE_NAME, filter_name.c_str());
  pw_properties_set(props_filter, PW_KEY_NODE_NICK, name.c_str());
  pw_properties_set(props_filter, PW_KEY_NODE_DESCRIPTION, name.c_str());
  pw_properties_set(props_filter, PW_KEY_MEDIA_TYPE, "Audio");
  pw_properties_set(props_filter, PW_KEY_MEDIA_CATEGORY, "Filter");
  pw_properties_set(props_filter, PW_KEY_MEDIA_ROLE, "DSP");

  filter = pw_filter_new(pm->core, filter_name.c_str(), props_filter);

  // left channel input

  auto* props_in_left = pw_properties_new(nullptr, nullptr);

  pw_properties_set(props_in_left, PW_KEY_FORMAT_DSP, "32 bit float mono audio");
  pw_properties_set(props_in_left, PW_KEY_PORT_NAME, "input_FL");
  pw_properties_set(props_in_left, "audio.channel", "FL");

  pf_data.in_left = static_cast<port*>(pw_filter_add_port(filter, PW_DIRECTION_INPUT, PW_FILTER_PORT_FLAG_MAP_BUFFERS,
                                                          sizeof(port), props_in_left, nullptr, 0));

  // right channel input

  auto* props_in_right = pw_properties_new(nullptr, nullptr);

  pw_properties_set(props_in_right, PW_KEY_FORMAT_DSP, "32 bit float mono audio");
  pw_properties_set(props_in_right, PW_KEY_PORT_NAME, "input_FR");
  pw_properties_set(props_in_right, "audio.channel", "FR");

  pf_data.in_right = static_cast<port*>(pw_filter_add_port(filter, PW_DIRECTION_INPUT, PW_FILTER_PORT_FLAG_MAP_BUFFERS,
                                                           sizeof(port), props_in_right, nullptr, 0));

  // left channel output

  auto* props_out_left = pw_properties_new(nullptr, nullptr);

  pw_properties_set(props_out_left, PW_KEY_FORMAT_DSP, "32 bit float mono audio");
  pw_properties_set(props_out_left, PW_KEY_PORT_NAME, "output_FL");
  pw_properties_set(props_out_left, "audio.channel", "FL");

  pf_data.out_left = static_cast<port*>(pw_filter_add_port(filter, PW_DIRECTION_OUTPUT, PW_FILTER_PORT_FLAG_MAP_BUFFERS,
                                                           sizeof(port), props_out_left, nullptr, 0));

  // right channel output

  auto* props_out_right = pw_properties_new(nullptr, nullptr);

  pw_properties_set(props_out_right, PW_KEY_FORMAT_DSP, "32 bit float mono audio");
  pw_properties_set(props_out_right, PW_KEY_PORT_NAME, "output_FR");
  pw_properties_set(props_out_right, "audio.channel", "FR");

  pf_data.out_right = static_cast<port*>(pw_filter_add_port(
      filter, PW_DIRECTION_OUTPUT, PW_FILTER_PORT_FLAG_MAP_BUFFERS, sizeof(port), props_out_right, nullptr, 0));

  pm->sync_wait_unlock();
}

PluginBase::~PluginBase() {
  post_messages = false;

  if (listener.link.next != nullptr || listener.link.prev != nullptr) {
    spa_hook_remove(&listener);
  }

  pw_filter_destroy(filter);

  for (auto& handler_id : gconnections) {
    g_signal_handler_disconnect(settings, handler_id);
  }

  gconnections.clear();

  g_object_unref(settings);
}

void PluginBase::set_post_messages(const bool& state) {
  post_messages = state;
}

void PluginBase::reset_settings() {
  util::reset_all_keys_except(settings);
}

void PluginBase::setup_input_output_gain() {
  input_gain = static_cast<float>(util::db_to_linear(g_settings_get_double(settings, "input-gain")));
  output_gain = static_cast<float>(util::db_to_linear(g_settings_get_double(settings, "output-gain")));

  g_signal_connect(settings, "changed::input-gain", G_CALLBACK(+[](GSettings* settings, char* key, gpointer user_data) {
                     auto self = static_cast<PluginBase*>(user_data);

                     self->input_gain = util::db_to_linear(g_settings_get_double(settings, key));
                   }),
                   this);

  g_signal_connect(settings, "changed::output-gain",
                   G_CALLBACK(+[](GSettings* settings, char* key, gpointer user_data) {
                     auto self = static_cast<PluginBase*>(user_data);

                     self->output_gain = util::db_to_linear(g_settings_get_double(settings, key));
                   }),
                   this);
}

void PluginBase::apply_gain(std::span<float>& left, std::span<float>& right, const float& gain) {
  if (left.empty() || right.empty()) {
    return;
  }

  std::ranges::for_each(left, [&](auto& v) { v *= gain; });
  std::ranges::for_each(right, [&](auto& v) { v *= gain; });
}

void PluginBase::notify() {
  const auto input_peak_db_l = util::linear_to_db(input_peak_left);
  const auto input_peak_db_r = util::linear_to_db(input_peak_right);

  const auto output_peak_db_l = util::linear_to_db(output_peak_left);
  const auto output_peak_db_r = util::linear_to_db(output_peak_right);

  input_level.emit(input_peak_db_l, input_peak_db_r);
  output_level.emit(output_peak_db_l, output_peak_db_r);

  input_peak_left = util::minimum_linear_level;
  input_peak_right = util::minimum_linear_level;
  output_peak_left = util::minimum_linear_level;
  output_peak_right = util::minimum_linear_level;
}

void PluginBase::update_probe_links() {}
