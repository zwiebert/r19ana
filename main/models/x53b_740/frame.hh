#pragma once

#include <array>
#include <string>

#include "XR25Frame.hh"
#include "main.hh"
#include "models/settings.hh"

/// @brief
class X53b740Frame : public CarModelBase {
 public:
  // virtual void get_defaults(settings_model_specific_t& dst) const override {
  // }

 public:
#define OLD_FORMULAS 0
  static constexpr int FRAME_SIZE = 30;
  using frame_data_t = std::array<uint8_t, FRAME_SIZE>;

 public:
  enum idx_t : uint8_t {
    start_byte,             // always zero
    program_version,        //
    calibration_version,    //
    status0,                // input-status (on/off-switches)
    MAP,                    //  #1
    ECT,                    //  #2
    IAT,                    //  #3
    battery,                //  #4
    oxygen,                 //  #5
    engine_speed_lb,        // #6 low-byte
    engine_speed_hb,        // #6 high-byte
    injection_duration_lb,  //  #50 low-byte
    injection_duration_hb,  //  #50 high-byte
    engine_knocking,        //  #13
    ignition_advance,       //  #51
    detonation_correction,  // , #15 (knocking retardation)
    idle_regulation,        //  #12
    status1,  // static 0x88  (bit-5: Closed Loop Readiness (Condition
              // Correction Lambda)!!!)
    status2,  // Flags2	Idle Control	Low-bit jitter (stepper), //
    // high-bit spike at end.
    status3,  // Enrichment/Purge,	Wakes-up at warm engine, heavy //
    // spiking/toggling.
    status4,  // static 0x00  Error memory   (rich/lean signal at bit-0 !!!)
    status5,  // static 0x0a
    status6,  // fuel-pump...
    status7,  //
    unknown_x,
    richness_regulation,  // #35
    unknown_xx,
    richness_adaption_avg2high,  // #30
    idle_adaption,               // #21
    richness_adaption_idle2low,  // #31
    COUNT,
  };

 public:
  int get_frame_count() const { return FrameNumber; }
  uint8_t fb(uint8_t data_idx_plus2) const { return data[data_idx_plus2 - 2]; }
  uint8_t f3x(uint8_t data_idx_plus3) const { return data[data_idx_plus3 - 3]; }
  uint8_t X(uint8_t data_idx_plus3) const { return data[data_idx_plus3 - 3]; }
  uint8_t X(idx_t col) const { return data[col]; }
  uint8_t R(uint8_t data_idx) const { return data[data_idx]; }
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
  int get_idle_regulation() const {
    return int(X(idx_t::idle_regulation)) - 128;
  }
  int get_idle_adaption() const { return int(X(idx_t::idle_adaption)) - 128; }
  int get_richness_regulation() const {
    return int(X(idx_t::richness_regulation) - 128);
  }
  int get_richness_adaption_idle_and_low() const {
    return int(X(idx_t::richness_adaption_idle2low)) - 128;
  }
  int get_richness_adaption_moderate_and_high() const {
    return int(X(idx_t::richness_adaption_avg2high)) - 128;
  }

  int get_idle_period() const { return X(22); }  // TODO

  bool is_evap_canister_open_to_intake() const {
    return is_vacuum_provided_to_egr_valve();
  }

  ///////////////////////// Flags ////////////////////
  // flags0

  bool is_aircon_select() const { return X(idx_t::status0) & 0x20; }
  bool is_aircon_demand() const { return X(idx_t::status0) & 0x04; }
  bool is_throttle_fully_open() const { return (X(idx_t::status0) & 0x10) == 0; }
  bool is_throttle_fully_closed() const { return !getbit(X(idx_t::status0), 3); }

  // flags2

  bool is_flag2_bit0() const { return X(idx_t::status2) & 0x01; }

  // flags3

  /**
   * @brief Manifold air pressure continuous defect
   *
   * @return  true means fault
   */
  bool is_fpp_map() const { return X(idx_t::status3 & 0x01); }

  bool is_flags3_bit2() const { return X(idx_t::status3 & 0x02); }

  bool is_oxygen_sensor_loop_closed() const {
    return getbit(X(idx_t::status2), 5);
  }  // plot-verified
  bool is_vacuum_provided_to_egr_valve() const {
    return getbit(X(idx_t::status3), 5);
  }
  // flags4

  /**
   * @brief for f3n-741. is automatic in park/neutral
   *
   * @return true if auto gearbox is in neutral/park
   */
  bool is_auto_park_neutral() const { return X(idx_t::status4) & 0x80; }

  // flags6

  // flags7
  bool is_fuel_pump_on() const { return getbit(X(idx_t::status6), 4); }

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