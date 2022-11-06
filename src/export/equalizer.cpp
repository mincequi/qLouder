#include "equalizer.hpp"

using namespace std::string_literals;

Equalizer::Equalizer(const std::string& schema,
                     const std::string& schema_path,
                     const std::string& schema_channel,
                     const std::string& schema_channel_left_path,
                     const std::string& schema_channel_right_path)
    : PluginBase(tags::plugin_name::equalizer, tags::plugin_package::lsp, schema, schema_path),
      settings_left(g_settings_new_with_path(schema_channel.c_str(), schema_channel_left_path.c_str())),
      settings_right(g_settings_new_with_path(schema_channel.c_str(), schema_channel_right_path.c_str())),
      lv2_wrapper(std::make_unique<lv2::Lv2Wrapper>("http://lsp-plug.in/plugins/lv2/para_equalizer_x32_lr")) {
    package_installed = lv2_wrapper->found_plugin;

    if (!package_installed) {
        util::debug(log_tag + "http://lsp-plug.in/plugins/lv2/para_equalizer_x32_lr is not installed");
    }

    lv2_wrapper->bind_key_enum<"mode", "mode">(settings);

    lv2_wrapper->bind_key_double<"bal", "balance">(settings);

    lv2_wrapper->bind_key_double<"frqs_l", "pitch-left">(settings);

    lv2_wrapper->bind_key_double<"frqs_r", "pitch-right">(settings);

    bind_bands(std::make_index_sequence<max_bands>());

    on_split_channels();

    gconnections.push_back(g_signal_connect(settings, "changed::num-bands",
                                            G_CALLBACK(+[](GSettings* settings, char* key, gpointer user_data) {
                                                auto self = static_cast<Equalizer*>(user_data);

                                                const uint nbands = g_settings_get_int(settings, key);

                                                const bool split = g_settings_get_boolean(settings, "split-channels") != 0;

                                                using namespace tags::equalizer;

                                                for (uint n = 0U; n < self->max_bands; n++) {
                                                    if (n < nbands) {
                                                        g_settings_set_enum(self->settings_left, band_type[n].data(), 1);

                                                        if (split) {
                                                            g_settings_set_enum(self->settings_right, band_type[n].data(), 1);
                                                        }
                                                    } else {
                                                        // turn off unused bands
                                                        g_settings_set_enum(self->settings_left, band_type[n].data(), 0);

                                                        if (split) {
                                                            g_settings_set_enum(self->settings_right, band_type[n].data(), 0);
                                                        }
                                                    }
                                                }
                                            }),
                           this));

    gconnections.push_back(g_signal_connect(
                               settings, "changed::split-channels",
                               G_CALLBACK(+[](GSettings* settings, char* key, Equalizer* self) { self->on_split_channels(); }), this));

    setup_input_output_gain();
}

Equalizer::~Equalizer() {
    for (auto& handler_id : this->gconnections_unified) {
        g_signal_handler_disconnect(this->settings_left, handler_id);
    }

    this->gconnections_unified.clear();
}

void Equalizer::on_split_channels() {
    if (g_settings_get_boolean(settings, "split-channels") != 0) {
        for (auto& handler_id : gconnections_unified) {
            g_signal_handler_disconnect(settings_left, handler_id);
        }

        gconnections_unified.clear();

        return;
    }

    using namespace tags::equalizer;

    for (uint n = 0U; n < max_bands; n++) {
        g_settings_set_enum(settings_right, band_type[n].data(), g_settings_get_enum(settings_left, band_type[n].data()));

        g_settings_set_enum(settings_right, band_mode[n].data(), g_settings_get_enum(settings_left, band_mode[n].data()));

        g_settings_set_enum(settings_right, band_slope[n].data(), g_settings_get_enum(settings_left, band_slope[n].data()));

        g_settings_set_boolean(settings_right, band_solo[n].data(),
                               g_settings_get_boolean(settings_left, band_solo[n].data()));

        g_settings_set_boolean(settings_right, band_mute[n].data(),
                               g_settings_get_boolean(settings_left, band_mute[n].data()));

        g_settings_set_double(settings_right, band_frequency[n].data(),
                              g_settings_get_double(settings_left, band_frequency[n].data()));

        g_settings_set_double(settings_right, band_gain[n].data(),
                              g_settings_get_double(settings_left, band_gain[n].data()));

        g_settings_set_double(settings_right, band_q[n].data(), g_settings_get_double(settings_left, band_q[n].data()));

        /*
      When in unified mode we want settings applied to the left channel to be propagated to the right channel
      database
    */

        gconnections_unified.push_back(g_signal_connect(settings_left, ("changed::"s + band_gain[n].data()).c_str(),
                                                        G_CALLBACK(+[](GSettings* settings, char* key, Equalizer* self) {
                                                            g_settings_set_double(self->settings_right, key,
                                                            g_settings_get_double(settings, key));
                                                        }),
                                       this));

        gconnections_unified.push_back(g_signal_connect(settings_left, ("changed::"s + band_frequency[n].data()).c_str(),
                                                        G_CALLBACK(+[](GSettings* settings, char* key, Equalizer* self) {
                                                            g_settings_set_double(self->settings_right, key,
                                                            g_settings_get_double(settings, key));
                                                        }),
                                       this));

        gconnections_unified.push_back(g_signal_connect(settings_left, ("changed::"s + band_q[n].data()).c_str(),
                                                        G_CALLBACK(+[](GSettings* settings, char* key, Equalizer* self) {
                                                            g_settings_set_double(self->settings_right, key,
                                                            g_settings_get_double(settings, key));
                                                        }),
                                       this));

        gconnections_unified.push_back(g_signal_connect(settings_left, ("changed::"s + band_type[n].data()).c_str(),
                                                        G_CALLBACK(+[](GSettings* settings, char* key, Equalizer* self) {
                                                            g_settings_set_enum(self->settings_right, key,
                                                            g_settings_get_enum(settings, key));
                                                        }),
                                       this));

        gconnections_unified.push_back(g_signal_connect(settings_left, ("changed::"s + band_mode[n].data()).c_str(),
                                                        G_CALLBACK(+[](GSettings* settings, char* key, Equalizer* self) {
                                                            g_settings_set_enum(self->settings_right, key,
                                                            g_settings_get_enum(settings, key));
                                                        }),
                                       this));

        gconnections_unified.push_back(g_signal_connect(settings_left, ("changed::"s + band_slope[n].data()).c_str(),
                                                        G_CALLBACK(+[](GSettings* settings, char* key, Equalizer* self) {
                                                            g_settings_set_enum(self->settings_right, key,
                                                            g_settings_get_enum(settings, key));
                                                        }),
                                       this));

        gconnections_unified.push_back(g_signal_connect(settings_left, ("changed::"s + band_mute[n].data()).c_str(),
                                                        G_CALLBACK(+[](GSettings* settings, char* key, Equalizer* self) {
                                                            g_settings_set_boolean(self->settings_right, key,
                                                            g_settings_get_boolean(settings, key));
                                                        }),
                                       this));
    }
}
