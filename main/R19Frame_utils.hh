#pragma once
#include "R19Frame.hh"
#include <bitset>


using r19frame_mask_t = std::bitset<32>;

inline const char* btoa(bool v) { return v ? "Y" : "N"; }

/// @brief Conditional print members of R19Frame object
/// @param dst       write buffer
/// @param dst_siz   write buffer size
/// @param d         object
/// @param view_mask bitmask representing members. Only members with 1 bits are
/// printed.
/// @return  bytes written or if greater than dst_siz, the required buffer size
/// (man 3 snprintf)
inline int r19_frame_print(char* dst, size_t dst_siz, const R19Frame& d,
                    r19frame_mask_t view_mask) {
  ssize_t dst_size = ssize_t(dst_siz);
  int ct = 0;

  if (view_mask.any()) {
    unsigned bit = 0;
    auto dst_max = dst + dst_size - 1;

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Engine Speed (RPM)=%d\r\n", bit, d.get_engine_speed_RPM());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Manifold Absolute Pressure (mBar)=%d\r\n", bit, d.get_manifold_absolute_pressure_mBar());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Intake Air Temperature (°C)=%d\r\n", bit, d.get_intake_air_temperature_Celsius());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Engine Coolant Temperature (°C)=%d\r\n", bit, d.get_engine_coolant_temperature_Celsius());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: O2 Sensor (mV)=%d\r\n", bit, d.get_oxygen_sensor_voltage_mV());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Atmospheric Pressure (mBar)=%d\r\n", bit, d.get_atmospheric_pressure_mBar());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Battery Voltage (V)=%0.1f\r\n", bit, d.get_battery_voltage_V());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Injection Duration (ms)=%0.2f\r\n", bit, d.get_injection_duration_ms());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Throttle Fully Open=%s\r\n", bit, btoa(d.is_throttle_fully_open()));
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Throttle Fully Closed=%s\r\n", bit, btoa(d.is_throttle_fully_closed()));
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: EGR/EVap active=%s\r\n", bit, btoa(d.is_vacuum_provided_to_egr_valve()));
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Idle Speed Correction=%d\r\n", bit, d.get_idle_speed_correction());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Engine Knocking=%d\r\n", bit, d.get_engine_knocking());
    }

    if (ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "\r\n");
    }

    if (ct >= dst_size || ct < 0)
      return ct;  // ERROR: data was not fully written (e.g. buffer too small)
  }

  return ct;
}

/// @brief      compare all members of 2 R19Frame objects
/// @param c    object
/// @param d    object
/// @return     bitset where none differing members are represented by zero
/// bits. order of bits is order of member definition.
inline r19frame_mask_t r19_frame_members_cmp(const R19Frame& c, const R19Frame& d) {
  r19frame_mask_t changed_mask = 0;
  {
    unsigned bit = 0;
    changed_mask.set(bit, (c.EngineSpeed_RPM != d.EngineSpeed_RPM));
    ++bit, changed_mask.set(bit, (c.MAP_mBar != d.MAP_mBar));
    ++bit, changed_mask.set(bit, (c.IAT_Celsius != d.IAT_Celsius));
    ++bit, changed_mask.set(bit, (c.ECT_Celsius != d.ECT_Celsius));
    ++bit, changed_mask.set(bit, (c.O2_Sensor_mV != d.O2_Sensor_mV));
    ++bit, changed_mask.set(bit, (c.AP_mBar != d.AP_mBar));
    ++bit, changed_mask.set(bit, (c.BatteryVoltage_mV != d.BatteryVoltage_mV));
    ++bit, changed_mask.set(bit, (c.ID_usec != d.ID_usec));
    ++bit, changed_mask.set(bit, (c.isThrottleOpen != d.isThrottleOpen));
    ++bit, changed_mask.set(bit, (c.isThrottleClosed != d.isThrottleClosed));
    ++bit, changed_mask.set(bit, (c.isAGR_AKF != d.isAGR_AKF));
    ++bit, changed_mask.set(bit, (c.IdleSpeedCorr != d.IdleSpeedCorr));
    ++bit, changed_mask.set(bit, (c.EngineKnocking != d.EngineKnocking));
  }
  return changed_mask;
}