#pragma once

#include <array>
#include <string>

class XR25Frame;  // forward declaration

class R19Frame {
 public:
  int get_frame_count() const { return FrameNumber; }

  int get_engine_speed_RPM() const { return m_ia[cs_RPM]; }
  int get_manifold_absolute_pressure_mBar() const { return m_ia[map_mBar]; }
  int get_intake_air_temperature_Celsius() const { return m_ia[iat_dC]; }
  int get_engine_coolant_temperature_Celsius() const { return m_ia[ect_dC]; }
  int get_oxygen_sensor_voltage_mV() const { return m_ia[o2_mV]; }
  int get_atmospheric_pressure_mBar() const { return m_ia[ap_mBar]; }
  int get_battery_voltage_mV() const { return m_ia[batt_mV]; }
  int get_injection_duration_us() const { return int(m_ia[inj_dur_us]); }
  int get_idle_speed_correction() const { return m_ia[idle_speed_corr]; }
  int get_engine_knocking() const { return m_ia[engine_knock]; }

  float get_battery_voltage_V() const { return m_ia[batt_mV] * 0.001f; }
  float get_injection_duration_ms() const { return m_ia[inj_dur_us] * 0.001f; }

  bool is_throttle_fully_open() const { return m_ba[thro_idle]; }
  bool is_throttle_fully_closed() const { return m_ba[thro_full_pow]; }
  bool is_vacuum_provided_to_egr_valve() const { return m_ba[egr_enabled]; }
  bool is_evap_canister_open_to_intake() const { return m_ba[egr_enabled]; }
  bool is_oxygen_sensor_loop_closed() const { return m_ba[o2_closed_loop]; }

 private:
  int FrameNumber = 0;

 private:
  enum : uint8_t {
    cs_RPM,
    map_mBar,
    iat_dC,
    ect_dC,
    o2_mV,
    ap_mBar,
    batt_mV,
    inj_dur_us,
    idle_speed_corr,
    engine_knock,
    //
    int_arr_size
  };
  enum : uint8_t {
    thro_idle,
    thro_full_pow,
    egr_enabled,
    o2_closed_loop,
    //
    bool_arr_size
  };

 private:
  std::array<int, int_arr_size> m_ia = {{}};
  std::array<bool, bool_arr_size> m_ba = {{}};

 public:
  explicit R19Frame(const XR25Frame& data);
  R19Frame() = default;
  bool operator==(const R19Frame&) const = default;
  bool operator!=(const R19Frame&) const = default;

  std::string getDataAsText() const;
};