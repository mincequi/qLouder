#pragma once

#include <spa/param/latency-utils.h>
#include <mutex>
#include <ranges>
#include <span>
#include "tags_plugin_name.hpp"

class PluginBase {
 public:
  PluginBase(std::string plugin_name,
             std::string plugin_package,
             const std::string& schema,
             const std::string& schema_path);
  virtual ~PluginBase();

  struct data;

  struct port {
    struct data* data;
  };

  struct data {
    struct port* in_left = nullptr;
    struct port* in_right = nullptr;

    struct port* out_left = nullptr;
    struct port* out_right = nullptr;

    struct port* probe_left = nullptr;
    struct port* probe_right = nullptr;

    PluginBase* pb = nullptr;
  };

  const std::string log_tag;

  std::string name, package;

  pw_filter* filter = nullptr;

  uint n_samples = 0U;

  uint rate = 0U;

  float buffer_duration = 0.0F;

  bool package_installed = true;

  bool bypass = false;

  std::vector<float> dummy_left, dummy_right;

  void set_post_messages(const bool& state);

  void reset_settings();

  virtual void update_probe_links();

  virtual auto get_latency_seconds() -> float;

  sigc::signal<void(const float, const float)> input_level;
  sigc::signal<void(const float, const float)> output_level;
  sigc::signal<void()> latency;

 protected:
  std::mutex data_mutex;

  GSettings* settings = nullptr;

  spa_hook listener{};

  data pf_data = {};

  bool post_messages = false;

  uint n_ports = 4;

  float input_gain = 1.0F;
  float output_gain = 1.0F;

  float notification_time_window = 1.0F / 20.0F;  // seconds
  float notification_dt = 0.0F;

  std::vector<gulong> gconnections;

  void setup_input_output_gain();

  void notify();

  static void apply_gain(std::span<float>& left, std::span<float>& right, const float& gain);

 private:
  uint node_id = 0U;

  float input_peak_left = util::minimum_linear_level, input_peak_right = util::minimum_linear_level;
  float output_peak_left = util::minimum_linear_level, output_peak_right = util::minimum_linear_level;
};
