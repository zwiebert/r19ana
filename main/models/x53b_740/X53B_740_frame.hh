#pragma once

#include <array>
#include <string>

#include "XR25Frame.hh"

/// @brief
class X53b740Frame {
  static constexpr bool OLD_FORMULAS = false;

 public:
  static constexpr int FRAME_SIZE = 30;
  using frame_data_t = std::array<uint8_t, FRAME_SIZE>;

 public:
  int get_frame_count() const { return FrameNumber; }
  uint8_t fb(uint8_t data_idx_plus2) const { return data[data_idx_plus2 - 2]; }
  uint8_t f3x(uint8_t data_idx_plus2) const { return data[data_idx_plus2 - 3]; }
  uint8_t X(uint8_t data_idx_plus2) const { return data[data_idx_plus2 - 3]; }
  // unknown indexes
  // 15, 17, 18, 19, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
  bool is_throttle_fully_open() const { return (X(5) & 0x10) == 0; }
  bool is_throttle_fully_closed() const { return (X(5) & 0x08) == 0; }
  int get_manifold_absolute_pressure_mBar() const {
#if OLD_FORMULAS
    return int(X(6) * 3.697f + 103.0f);
#else
    return int(X(6) * 4);
#endif
  }
  int get_engine_coolant_temperature_Celsius() const {
    return int(X(7) * 0.625f - 40.0f);
  }
  int get_intake_air_temperature_Celsius() const {
    return int(X(8) * 0.625f - 40.0f);
  }
  int get_battery_voltage_mV() const { return (X(9) * 1000) / 32 + 8000; }
  float get_battery_voltage_V() const { return X(9) * 0.0312f + 8.0f; }
  int get_oxygen_sensor_voltage_mV() const {
#if OLD_FORMULAS
    return int(X(10) * 4);
#else
    return int(X(10) * 6);
#endif
  }
  int get_engine_speed_RPM() const {
    uint32_t num = (X(11)) | (X(12) << 8);
    return (num == 0) ? 0 : int(30000000L / num);
  }
  int get_engine_knocking() const { return X(13); }
  int get_injection_duration_us() const { return 2 * ((X(14) << 8) | X(15)); }
  float get_injection_duration_ms() const {
    return get_injection_duration_us() * 0.001f;
  }
  int get_idle_regulation() const { return X(17); }  // TODO
  int get_atmospheric_pressure_mBar() const {
#if OLD_FORMULAS
    return 1090 - X(21);
#else
    return (~X(21) & 0xff) * 4;
#endif
  }
  int get_idle_period() const { return X(22); }  // TODO

  bool is_vacuum_provided_to_egr_valve() const { return !!(X(21) & 0x20); }
  bool is_evap_canister_open_to_intake() const {
    return is_vacuum_provided_to_egr_valve();
  }
  bool is_oxygen_sensor_loop_closed() const { return !!(X(21) & 0x08); }
  bool is_fuel_pump_on() const { return !!(X(23) & 0x10); }

  int get_id() const { return X(31); }

  const frame_data_t& get_frame() const { return data; }

 private:
  frame_data_t data = {};  ///<  XR25 frame without header ff,00
  int FrameNumber = 0;

 public:
  X53b740Frame(const XR25Frame::frame_data_t& xr25_frame, int frame_count)
      : FrameNumber(frame_count) {
    for (int i = 0; i < data.size(); ++i) {
      data[i] = xr25_frame[i];
    }
  };
  X53b740Frame() = default;

  bool operator==(const X53b740Frame&) const = default;
  bool operator!=(const X53b740Frame&) const = default;
  uint8_t operator[](int idx) const { return data[idx - 2]; }

  std::string getDataAsText() const;
};