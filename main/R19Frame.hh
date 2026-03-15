#pragma once

#include <array>
#include <string>

#include "XR25Frame.hh"

/// @brief
class R19Frame {
  static constexpr bool OLD_FORMULAS = false;

 public:
  int get_frame_count() const { return FrameNumber; }

  int get_engine_speed_RPM() const {
    uint32_t num = ((data[8]) | (data[9] << 8)) & 0xffff;
    return (num == 0) ? 0 : int(30000000L / num);
  }

  int get_manifold_absolute_pressure_mBar() const {
#if OLD_FORMULAS
    return int(data[3] * 3.697f + 103.0f);
#else
    return int(data[3] * 4);
#endif
  }

  int get_intake_air_temperature_Celsius() const {
    return int(data[5] * 0.625f - 40.0f);
  }

  int get_engine_coolant_temperature_Celsius() const {
    return int(data[4] * 0.625f - 40.0f);
  }

  int get_oxygen_sensor_voltage_mV() const {
#if OLD_FORMULAS
    return int(data[7] * 4);
#else
    return int(data[7] * 6);
#endif
  }

  int get_atmospheric_pressure_mBar() const {
#if OLD_FORMULAS
    return 1090 - data[18];
#else
    return (~data[18] & 0xff) * 4;
#endif
  }

  float get_battery_voltage_V() const {
    return float((data[6] * 0.0312f + 8.0f));
  }

  int get_battery_voltage_mV() const {
    return int(1000.0f * get_battery_voltage_V());
  }

  int get_injection_duration_us() const {
    return 2 * ((data[12] | (data[11] << 8)) & 0xffff);
  }

  float get_injection_duration_ms() const {
    return get_injection_duration_us() * 0.001f;
  }

  int get_idle_speed_correction() const { return 0; }  // TODO

  int get_engine_knocking() const { return data[10]; }

  bool is_throttle_fully_closed() const { return (data[2] & 0x10) == 0; }

  bool is_throttle_fully_open() const { return (data[2] & 0x08) == 0; }

  bool is_vacuum_provided_to_egr_valve() const { return !!(data[18] & 0x20); }

  bool is_evap_canister_open_to_intake() const {
    return is_vacuum_provided_to_egr_valve();
  }

  bool is_oxygen_sensor_loop_closed() const { return !!(data[18] & 0x08); }

 private:
  XR25Frame::frame_data_t data = {};  ///<  XR25 frame without header ff,00
  int FrameNumber = 0;

 public:
  explicit R19Frame(const XR25Frame& xr25_frame)
      : data(xr25_frame.get_frame_data()),
        FrameNumber(xr25_frame.getFrameCounter()) {};
  R19Frame() = default;
  bool operator==(const R19Frame&) const = default;
  bool operator!=(const R19Frame&) const = default;

  std::string getDataAsText() const;
};