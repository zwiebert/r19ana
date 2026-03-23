#pragma once

#include <array>
#include <string>

#include "XR25Frame.hh"

/// @brief
class X53b740Frame {
  static constexpr bool OLD_FORMULAS = false;

 public:
  static constexpr int FRAME_SIZE = 29;
  using frame_data_t = std::array<uint8_t, FRAME_SIZE>;

 public:
  enum idx_t : uint8_t {
    c0,
    c1,
    flags_in,
    MAP,
    ECT,
    IAT,
    battery,
    oxygen,
    engine_speed_lb,
    engine_speed_hb,
    engine_knocking,
    injection_duration_hb,
    injection_duration_lb,
    ignition_advance,
    c14,
    c15,
    c16,
    c17,
    c18,
    c19,
    flags_out,
    c21,
    c23,
    c24,
    c25,
    c26,
    c27,
    id,
    c29,
  };

 public:
  int get_frame_count() const { return FrameNumber; }
  uint8_t fb(uint8_t data_idx_plus2) const { return data[data_idx_plus2 - 2]; }
  uint8_t f3x(uint8_t data_idx_plus3) const { return data[data_idx_plus3 - 3]; }
  uint8_t X(uint8_t data_idx_plus3) const { return data[data_idx_plus3 - 3]; }
  uint8_t X(idx_t col) const { return data[col]; }
  // unknown indexes
  // 15, 17, 18, 19, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
  bool is_throttle_fully_open() const {
    return (X(idx_t::flags_in) & 0x10) == 0;
  }
  bool is_throttle_fully_closed() const {
    return (X(idx_t::flags_in) & 0x08) == 0;
  }
  int get_manifold_absolute_pressure_mBar() const {
#if OLD_FORMULAS
    return int(X(idx_t::MAP) * 3.697f + 103.0f);
#else
    return int(X(idx_t::MAP) * 4);
#endif
  }
  int get_engine_coolant_temperature_Celsius() const {
    return int(X(idx_t::ECT) * 0.625f - 40.0f);
  }
  int get_intake_air_temperature_Celsius() const {
    return int(X(idx_t::IAT) * 0.625f - 40.0f);
  }
  int get_battery_voltage_mV() const {
    return (X(idx_t::battery) * 1000) / 32 + 8000;
  }
  float get_battery_voltage_V() const {
    return X(idx_t::battery) * 0.0312f + 8.0f;
  }
  int get_oxygen_sensor_voltage_mV() const {
#if OLD_FORMULAS
    return int(X(idx_t::oxygen) * 4);
#else
    return int(X(idx_t::oxygen) * 6);
#endif
  }
  int get_engine_speed_RPM() const {
    uint32_t num =
        (X(idx_t::engine_speed_lb)) | (X(idx_t::engine_speed_hb) << 8);
    return (num == 0) ? 0 : int(30000000L / num);
  }
  int get_engine_knocking() const { return X(idx_t::engine_knocking); }
  int get_injection_duration_us() const {
    return 2 * ((X(idx_t::injection_duration_hb) << 8) |
                X(idx_t::injection_duration_lb));
  }
  float get_injection_duration_ms() const {
    return get_injection_duration_us() * 0.001f;
  }
  int get_ignition_advance_deg() const { return X(idx_t::ignition_advance); }
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
  bool is_fuel_pump_on() const { return !!(X(idx_t::flags_out) & 0x10); }

  int get_id() const { return X(idx_t::id); }

  const frame_data_t& get_frame() const { return data; }

 private:
  frame_data_t data = {};  ///<  XR25 frame without header ff,00
  int FrameNumber = 0;

 public:
  X53b740Frame(const XR25Frame& frame)
      : FrameNumber(frame.get_frame_counter()) {
    for (int i = 0; i < FRAME_SIZE; ++i) {
      data[i] = frame.get_frame_data()[i];
    }
  };
  X53b740Frame() = default;

  bool operator==(const X53b740Frame&) const = default;
  bool operator!=(const X53b740Frame&) const = default;
  uint8_t operator[](int idx) const { return data[idx - 2]; }
  uint8_t operator[](idx_t idx) const { return X(idx); }

  std::string getDataAsText() const;
};