#pragma once

#include <array>
#include <string>

#include "XR25Frame.hh"
#include "models/settings.hh"

/// @brief
class X53b740Frame : CarModelBase {
 public:
  // virtual void get_defaults(settings_model_specific_t& dst) const override {
  // }

 public:
#define OLD_FORMULAS 0
  static constexpr int FRAME_SIZE = 29;
  using frame_data_t = std::array<uint8_t, FRAME_SIZE>;

 public:
  enum idx_t : uint8_t {
    program_version,        // 0
    calibration_version,    // 1
    flags0,                 // 2, input (on/off-switches)
    MAP,                    // 3, #1
    ECT,                    // 4, #2
    IAT,                    // 5, #3
    battery,                // 6, #4
    oxygen,                 // 7, #5
    engine_speed_lb,        // 8, #6 low-byte
    engine_speed_hb,        // 9, #6 high-byte
    injection_duration_lb,  // 12, #50 low-byte
    injection_duration_hb,  // 11, #50 high-byte
    engine_knocking,        // 10, #13
    ignition_advance,       // 13, #51
    detonation_correction,  // 14, #15 (knocking retardation)
    o2_integrator,          // 15,
    flags1,                 // 16 static 0x88
    flags2,         // 17   Flags2	Idle Control	Low-bit jitter (stepper), // high-bit spike at end.
    flags3,         // 18 	Enrichment/Purge	Wake-up at 10m, heavy // spiking/toggling.
    flags4,         // 19 static 0x00  Error memory
    flags5,         // 20 static 0x0a
    flags6,         // 21   fuel-pump...
    flags7,         // 22
    idle_adaption,  // 23, #21
    richness_adaption_idle2low,  // 24, #31
    richness_regulation,         // 25, #35
    richness_adaption_avg2high,  // 26, #30
    idle_regulation,             // 27  #12
    id,                          // 28
  };

 public:
  int get_frame_count() const { return FrameNumber; }
  uint8_t fb(uint8_t data_idx_plus2) const { return data[data_idx_plus2 - 2]; }
  uint8_t f3x(uint8_t data_idx_plus3) const { return data[data_idx_plus3 - 3]; }
  uint8_t X(uint8_t data_idx_plus3) const { return data[data_idx_plus3 - 3]; }
  uint8_t X(idx_t col) const { return data[col]; }
  // unknown indexes
  // 15, 17, 18, 19, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
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
  int get_detonation_correction_deg() const {
    return X(idx_t::detonation_correction);
  }
  int get_idle_regulation() const { return int(X(idx_t::idle_regulation)) - 128; }
  int get_idle_adaption() const { return int(X(idx_t::idle_adaption)); }
  int get_richness_regulation() const { return int(X(idx_t::richness_regulation)) - 128; }
  int get_richness_adaption_idle_and_low() const { return int(X(idx_t::richness_adaption_idle2low)) - 128; }
  int get_richness_adaption_moderate_and_high() const { return int(X(idx_t::richness_adaption_avg2high)) - 128; }

  int get_atmospheric_pressure_mBar() const {
#if OLD_FORMULAS
    return 1090 - X(21);
#else
    return (~X(21) & 0xff) * 4;
#endif
  }
  int get_idle_period() const { return X(22); }  // TODO

  bool is_evap_canister_open_to_intake() const {
    return is_vacuum_provided_to_egr_valve();
  }

  int get_id() const { return X(idx_t::id); }

  ///////////////////////// Flags ////////////////////
  // flags0

  bool is_aircon_select() const { return X(idx_t::flags0) & 0x20; }
  bool is_aircon_demand() const { return X(idx_t::flags0) & 0x04; }
  bool is_throttle_fully_open() const { return (X(idx_t::flags0) & 0x10) == 0; }
  bool is_throttle_fully_closed() const {
    return (X(idx_t::flags0) & 0x08) == 0;
  }

  // flags2

  bool is_flag2_bit0() const { return X(idx_t::flags2) & 0x01; }

  // flags3

  /**
   * @brief Manifold air pressure continuous defect
   *
   * @return  true means fault
   */
  bool is_fpp_map() const { return X(idx_t::flags3 & 0x01); }

  bool is_flags3_bit2() const { return X(idx_t::flags3 & 0x02); }

  // flags4

  bool is_oxygen_sensor_loop_closed() const { return X(idx_t::flags4) & 0x08; }
  bool is_vacuum_provided_to_egr_valve() const {
    return !(X(idx_t::flags7) & 0x20);
  }

  /**
   * @brief for f3n-741. is automatic in park/neutral
   *
   * @return true if auto gearbox is in neutral/park
   */
  bool is_auto_park_neutral() const { return X(idx_t::flags4) & 0x80; }

  // flags6

  // flags7

  bool is_fuel_pump_on() const { return X(idx_t::flags7) & 0x10; }

 public:
  const frame_data_t& get_frame() const { return data; }

 private:
  frame_data_t data = {};  ///<  XR25 frame without header ff,00
  int FrameNumber = 0;

 public:
  X53b740Frame(const XR25Frame::voc_t& frame) : FrameNumber(frame.counter) {
    for (int i = 0; i < FRAME_SIZE; ++i) {
      data[i] = frame.frame[i];
    }
  };
  X53b740Frame() = default;

  bool operator==(const X53b740Frame&) const = default;
  bool operator!=(const X53b740Frame&) const = default;
  uint8_t operator[](int idx) const { return data[idx - 2]; }
  uint8_t operator[](idx_t idx) const { return X(idx); }

  std::string getDataAsText() const;
};