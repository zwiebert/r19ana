#pragma once

#include <array>
#include <string>

#include "XR25Frame.hh"

/// @brief
class R19Frame {
  static constexpr bool OLD_FORMULAS = false;

 public:
  int get_frame_count() const { return FrameNumber; }
  uint8_t fb(uint8_t data_idx_plus2) const { return data[data_idx_plus2 - 2]; }
  uint8_t f3x(uint8_t data_idx_plus2) const { return data[data_idx_plus2 - 3]; }
// unknown indexes
// 15, 17, 18, 19, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
  bool is_throttle_fully_open() const { return (fb(4) & 0x10) == 0; }
  bool is_throttle_fully_closed() const { return (fb(4) & 0x08) == 0; }
  int get_manifold_absolute_pressure_mBar() const {
#if OLD_FORMULAS
    return int(fb(5) * 3.697f + 103.0f);
#else
    return int(fb(5) * 4);
#endif
  }
  int get_engine_coolant_temperature_Celsius() const {
    return int(fb(6) * 0.625f - 40.0f);
  }
  int get_intake_air_temperature_Celsius() const {
    return int(fb(7) * 0.625f - 40.0f);
  }
  int get_battery_voltage_mV() const { return (fb(8) * 1000) / 32 + 8000; }
  float get_battery_voltage_V() const { return fb(8) * 0.03125f + 8.0f; }
  int get_oxygen_sensor_voltage_mV() const {
#if OLD_FORMULAS
    return int(fb(9) * 4);
#else
    return int(fb(9) * 6);
#endif
  }
  int get_engine_speed_RPM() const {
    uint32_t num = (fb(10)) | (fb(11) << 8);
    return (num == 0) ? 0 : int(30000000L / num);
  }
  int get_engine_knocking() const { return fb(12); }
  int get_injection_duration_us() const { return 2 * ((fb(13) << 8) | fb(14)); }
  float get_injection_duration_ms() const {
    return get_injection_duration_us() * 0.001f;
  }
  int get_idle_regulation() const { return fb(16); }  // TODO
  int get_atmospheric_pressure_mBar() const {
#if OLD_FORMULAS
    return 1090 - fb(20);
#else
    return (~fb(20) & 0xff) * 4;
#endif
  }
  int get_idle_period() const { return fb(21); }  // TODO

  bool is_vacuum_provided_to_egr_valve() const { return !!(fb(20) & 0x20); }
  bool is_evap_canister_open_to_intake() const {
    return is_vacuum_provided_to_egr_valve();
  }

  bool is_oxygen_sensor_loop_closed() const { return !!(fb(20) & 0x08); }

  const XR25Frame::frame_data_t& get_frame() const { return data; }

 private:
  XR25Frame::frame_data_t data = {};  ///<  XR25 frame without header ff,00
  int FrameNumber = 0;

 public:
  R19Frame(const XR25Frame& xr25_frame)
      : data(xr25_frame.get_frame_data()),
        FrameNumber(xr25_frame.getFrameCounter()) {};
  R19Frame() = default;

  bool operator==(const R19Frame&) const = default;
  bool operator!=(const R19Frame&) const = default;
  uint8_t operator[](int idx) const { return data[idx - 2]; }

  std::string getDataAsText() const;
};