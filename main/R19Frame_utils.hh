#pragma once
#include <bitset>

#include "R19Frame.hh"

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

    if (ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "Frame-Number: %d\r\n", d.get_frame_count());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Crankshaft (RPM)=%d\r\n", bit,
                     d.get_engine_speed_RPM());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Manifold (mBar)=%d\r\n", bit,
                     d.get_manifold_absolute_pressure_mBar());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Intake Air (°C)=%d\r\n", bit,
                     d.get_intake_air_temperature_Celsius());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Engine Coolant (°C)=%d\r\n", bit,
                     d.get_engine_coolant_temperature_Celsius());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: O2 Sensor (mV)=%d\r\n", bit,
                     d.get_oxygen_sensor_voltage_mV());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Atmosphere (mBar)=%d\r\n", bit,
                     d.get_atmospheric_pressure_mBar());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Battery (V)=%0.1f\r\n", bit,
                     d.get_battery_voltage_V());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Injection Duration (ms)=%0.2f\r\n", bit,
                     d.get_injection_duration_ms());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Throttle-Pos Full-Power=%s\r\n", bit,
                     btoa(d.is_throttle_fully_open()));
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Throttle-Pos Idle=%s\r\n", bit,
                     btoa(d.is_throttle_fully_closed()));
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: EGR/EVap=%s\r\n", bit,
                     btoa(d.is_vacuum_provided_to_egr_valve()));
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: O2 sensor loop %s\r\n", bit,
                     d.is_oxygen_sensor_loop_closed() ? "closed" : "open");
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Idle Speed Correction=%d\r\n", bit,
                     d.get_idle_speed_correction());
    }

    if (ct >= 0 && view_mask.test(bit++)) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u: Engine Knocking=%d\r\n", bit,
                     d.get_engine_knocking());
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
inline r19frame_mask_t r19_frame_members_cmp(const R19Frame& c,
                                             const R19Frame& d) {
  r19frame_mask_t changed_mask = 0;
  {
    unsigned bit = 0;
    changed_mask.set(bit,
                     (c.get_engine_speed_RPM() != d.get_engine_speed_RPM()));
    ++bit, changed_mask.set(bit, (c.get_manifold_absolute_pressure_mBar() !=
                                  d.get_manifold_absolute_pressure_mBar()));
    ++bit, changed_mask.set(bit, (c.get_intake_air_temperature_Celsius() !=
                                  d.get_intake_air_temperature_Celsius()));
    ++bit, changed_mask.set(bit, (c.get_engine_coolant_temperature_Celsius() !=
                                  d.get_engine_coolant_temperature_Celsius()));
    ++bit, changed_mask.set(bit, (c.get_oxygen_sensor_voltage_mV() !=
                                  d.get_oxygen_sensor_voltage_mV()));
    ++bit, changed_mask.set(bit, (c.get_atmospheric_pressure_mBar() !=
                                  d.get_atmospheric_pressure_mBar()));
    ++bit, changed_mask.set(
               bit, (c.get_battery_voltage_mV() != d.get_battery_voltage_mV()));
    ++bit, changed_mask.set(bit, (c.get_injection_duration_us() !=
                                  d.get_injection_duration_us()));
    ++bit, changed_mask.set(
               bit, (c.is_throttle_fully_open() != d.is_throttle_fully_open()));
    ++bit, changed_mask.set(bit, (c.is_throttle_fully_closed() !=
                                  d.is_throttle_fully_closed()));
    ++bit, changed_mask.set(bit, (c.is_vacuum_provided_to_egr_valve() !=
                                  d.is_vacuum_provided_to_egr_valve()));
    ++bit, changed_mask.set(bit, (c.is_oxygen_sensor_loop_closed() !=
                                  d.is_oxygen_sensor_loop_closed()));
    ++bit, changed_mask.set(bit, (c.get_idle_speed_correction() !=
                                  d.get_idle_speed_correction()));
    ++bit, changed_mask.set(
               bit, (c.get_engine_knocking() != d.get_engine_knocking()));
  }
  return changed_mask;
}

int write_r19_frame(char* dst, size_t dst_siz, const R19Frame& d,
                    r19frame_mask_t mask = ~0UL, bool force = false) {
  ssize_t dst_size = ssize_t(dst_siz);
  static R19Frame c;  // copy of last written frame
  int ct = 0;

  const bool hide_unchanged = !force && true;
  const r19frame_mask_t view_mask =
      hide_unchanged ? (r19_frame_members_cmp(c, d) & mask) : mask;

  if (view_mask.any()) {
    ct = r19_frame_print(dst, dst_size, d, view_mask);

    if (ct >= dst_size || ct < 0)
      return ct;  // ERROR: data was not fully written (e.g. buffer too small)

    // all done. update our local state variables
    c = d;
  }

  return ct;
}