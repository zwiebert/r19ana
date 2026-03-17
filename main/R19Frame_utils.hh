#pragma once
#include <bitset>

#include "R19Frame.hh"
#include "i18n.hh"

using r19frame_mask_t = std::bitset<32>;

namespace {
inline void tohex(const uint8_t* in, size_t insz, char* out, size_t outsz) {
  const unsigned char* pin = in;
  const char* hex = "0123456789ABCDEF";
  char* pout = out;
  for (; pin < in + insz; pout += 3, pin++) {
    pout[0] = hex[(*pin >> 4) & 0xF];
    pout[1] = hex[*pin & 0xF];
    pout[2] = ' ';
    if (pout + 3 - out > outsz) {
      /* Better to truncate output string than overflow buffer */
      /* it would be still better to either return a status */
      /* or ensure the target buffer is large enough and it never happen */
      break;
    }
  }
  pout[-1] = '\0';
}
static char frame_hex[XR25Frame::FRAME_SIZE * 3];
void frame_hex_fill(const XR25Frame::frame_data_t& frame) {
  tohex(&frame[0], frame.size(), frame_hex, sizeof frame_hex);
}

};  // namespace

inline int snprinthex(char* dst, size_t dst_size,
                      const XR25Frame::frame_data_t& frame) {
  const int req_len = frame.size() * 2;
  if (dst_size <= req_len) {
    *dst = '\0';
    return req_len;
  }

  tohex(&frame[0], frame.size(), dst, dst_size);

  return req_len;
}

inline const char* btoa(bool v) { return v ? "X" : " "; }

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

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d Frame-Count #####\r\n", bit,
                     d.get_frame_count());
    }

    frame_hex_fill(d.get_frame());
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct +=
          snprintf(p, l, "%02u:[02-07] %.*s\r\n", bit, 17, frame_hex + 18 * 0);
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct +=
          snprintf(p, l, "%02u:[08-13] %.*s \r\n", bit, 17, frame_hex + 18 * 1);
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct +=
          snprintf(p, l, "%02u:[14-19] %.*s\r\n", bit, 17, frame_hex + 18 * 2);
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct +=
          snprintf(p, l, "%02u:[20-25] %.*s\r\n", bit, 17, frame_hex + 18 * 3);
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct +=
          snprintf(p, l, "%02u:[26-31] %.*s\r\n", bit, 17, frame_hex + 18 * 4);
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d rpm %s\r\n", bit,
                     d.get_engine_speed_RPM(), _("Crankshaft"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u:   %2.1f V %s\r\n", bit,
                     d.get_battery_voltage_V(), _("Battery"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d mBar %s\r\n", bit,
                     d.get_atmospheric_pressure_mBar(), _("Atmosphere"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d mBar %s\r\n", bit,
                     d.get_manifold_absolute_pressure_mBar(), _("Manifold"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d °C %s\r\n", bit,
                     d.get_intake_air_temperature_Celsius(), _("Intake Air"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d °C %s\r\n", bit,
                     d.get_engine_coolant_temperature_Celsius(), _("Coolant"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u:  %3.2f ms %s\r\n", bit,
                     d.get_injection_duration_ms(), _("Injection Duration"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d mV %s\r\n", bit,
                     d.get_oxygen_sensor_voltage_mV(), _("O2 Sensor"));
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d %s\r\n", bit, d.get_engine_knocking(),
                     _("Engine-Knock"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct +=
          snprintf(p, l, "%u:    [%s] %s\r\n", bit,
                   btoa(d.is_throttle_fully_open()), _("Throttle Full-Power"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u:    [%s] %s\r\n", bit,
                     btoa(d.is_throttle_fully_closed()), _("Throttle Idle"));
    }

#if 0  // no idea which index, if any
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u:    [%s] %s\r\n", bit,
                     btoa(d.is_vacuum_provided_to_egr_valve()), _("EGR+EVap enabled"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u:    [%s] %s\r\n", bit,
                     btoa(d.is_oxygen_sensor_loop_closed()), _("O2 sensor loop"));
    }
#endif
    /////////////////////////// experimental
    /////////////// try and error confirmed /////////////
    // fuel pump (can be heard when ignition turns on)
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u:    [%s] %s\r\n", bit, btoa(d[23] & 0x10),
                     _("Fuel-Pump"));
    }

    /////////////////////////////////////////////////////
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d %s (15)\r\n", bit, d[15], _("Advance"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d %s\r\n", bit, d.get_idle_regulation(),
                     _("Idle-Regulation"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d %s\r\n", bit, d.get_idle_period(),
                     _("Idle-Period"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct +=
          snprintf(p, l, "%02u: %6d %s (28)\r\n", bit, d[28], _("Knock-Delay"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u:  %4.2f %s (22)\r\n", bit, d[22] / 2.25,
                     _("Throttle"));
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %02x%02x%02x %s (27,19,18)\r\n", bit, d[27],
                     d[19], d[18], _("Fault-Flags"));
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u:     %02x %s (26)\r\n", bit, d[26],
                     _("Fault-Fugitive"));
    }
    //  unknown: 17, 23, 24, 25, 29, 30, 31
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: ???    17=%02x,23=%02x,24=%02x,25=%02x\r\n",
                     bit, d[17], d[23], d[24], d[25]);
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: ???    28=%02x,29=%02x,30=%02x,31=%02x\r\n",
                     bit, d[28], d[29], d[30], d[31]);
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d mBar %s\r\n", bit, (4 * (~d[29] & 0xff)),
                     _("Atmosphere"));
    }

    /////////////////////// end experimental ///////////////////////////

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
    ++bit, changed_mask.set(
               bit, (c.get_idle_regulation() != d.get_idle_regulation()));
    ++bit, changed_mask.set(bit, (c.get_idle_period() != d.get_idle_period()));
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