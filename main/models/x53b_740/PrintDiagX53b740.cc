#include "PrintDiagX53b740.hh"

#include <bitset>

#include "X53B_740_frame.hh"
#include "i18n.hh"

static void tohex(const uint8_t* in, size_t insz, char* out, size_t outsz) {
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

static int snprinthex(char* dst, size_t dst_size,
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

/// @brief Conditional print members of X53b740Frame object
/// @param dst       write buffer
/// @param dst_siz   write buffer size
/// @param d         object
/// @param view_mask bitmask representing members. Only members with 1 bits are
/// printed.
/// @return  bytes written or if greater than dst_siz, the required buffer size
/// (man 3 snprintf)
static int frame_print(char* dst, size_t dst_siz, const X53b740Frame& d,
                       const PrintCarDiag::line_view_mask_t& view_mask) {
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
          snprintf(p, l, "%02u:[26-30] %.*s\r\n", bit, 14, frame_hex + 18 * 4);
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d %s\r\n", bit, d.get_id(), _("ID"));
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

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u:    [%s] %s\r\n", bit, btoa(d.is_fuel_pump_on()),
                     _("Fuel-Pump"));
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
      ct += snprintf(p, l, "%u:    [%s] %s\r\n", bit, btoa(d[23] & 0x08),
                     _("23-0x08"));
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u:    [%s] %s\r\n", bit, btoa(d[23] & 0x20),
                     _("23-0x20"));
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%u:    [%s] %s\r\n", bit, btoa(d[23] & 0x80),
                     _("23-0x80"));
    }
    /////////////////////////////////////////////////////
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct +=
          snprintf(p, l, "%02u: %6d °D %s (15)\r\n", bit, d[15], _("Advance"));
    }
#if 0
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
      ct += snprintf(p, l, "%02u:  %4.2f %s (22)\r\n", bit, d[22] / 2.25,
                     _("Throttle"));
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d mBar %s\r\n", bit, (4 * (~d[29] & 0xff)),
                     _("Atmosphere"));
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %02x%02x%02x %s (27,19,18)\r\n", bit, d[27],
                     d[19], d[18], _("Fault-Flags"));
    }
#endif

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d °D %s (28)\r\n", bit, d[28],
                     _("Knock-Delay"));
    }

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u:     %02x %s (26)\r\n", bit, d[26],
                     _("Fault-Fugitive"));
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d ??? %s\r\n", bit, int(d[27]) - 0x82,
                     _("(27) - 0x82"));
    }
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d ??? %s\r\n", bit,
                     int(d[27] | (d[28] << 8)) - 0x8182, _("(28,27) - 0x8182"));
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
      ct += snprintf(p, l, "%02u: ???    28=%02x,29=%02x,30=%02x,4=%02x\r\n",
                     bit, d[28], d[29], d[30], d[4]);
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

int PrintDiagX53b740::snprint_diag(char* dst, size_t dst_size,
                                   line_view_mask_t show_lines) const {
  return frame_print(dst, dst_size, m_frame, show_lines);
}