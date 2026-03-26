#include "PrintDiag.hh"

#include <bitset>

#include "frame.hh"
#include "i18n.hh"

using OurFrame = X53b740Frame;

#define diag_printf(fmt, ...)                               \
  do {                                                      \
    if (view_mask.test(bit++) && ct >= 0) {                 \
      auto p = std::min(dst_max, dst + ct);                 \
      auto l = std::max(ssize_t(0), dst_size - ct);         \
      ct += snprintf(p, l, "%02u " fmt, bit, __VA_ARGS__); \
    }                                                       \
  } while (0)

static int tohex(const uint8_t* in, size_t insz, char* out, size_t outsz,
                 const char* sep = "") {
  const auto sep_len = strlen(sep);
  const unsigned char* pin = in;
  const char* hex = "0123456789ABCDEF";
  const int req_size = insz * (2 + sep_len) - sep_len;

  char* pout = out;
  for (; pin < in + insz; pout += 2 + sep_len, pin++) {
    pout[0] = hex[(*pin >> 4) & 0xF];
    pout[1] = hex[*pin & 0xF];
    memcpy(&pout[2], sep, sep_len);
    if (pout + 2 + sep_len - out > outsz) {
      /* Better to truncate output string than overflow buffer */
      /* it would be still better to either return a status */
      /* or ensure the target buffer is large enough and it never happen */
      break;
    }
  }
  pout[-sep_len] = '\0';
  return req_size;
}

static char frame_hex[OurFrame::FRAME_SIZE * 3];
static void frame_hex_fill(const OurFrame::frame_data_t& frame,
                           const char* sep = ",") {
  tohex(&frame[0], frame.size(), frame_hex, sizeof frame_hex, sep);
}

int PrintDiagX53b740::snprint_diag(char* dst, size_t dst_siz,
                                   line_view_mask_t view_mask) const {
  const OurFrame& d = m_frame;
  using idx_t = OurFrame::idx_t;

  ssize_t dst_size = ssize_t(dst_siz);
  int ct = 0;

  if (view_mask.any()) {
    unsigned bit = 0;
    auto dst_max = dst + dst_size - 1;

    ///////////////////////////////////////////////
    frame_hex_fill(d.get_frame(), "");
    diag_printf("%s\n", frame_hex);
    ///////////////////////////////////////////////

    diag_printf("%6d Frame-Count, ID=%d\n", d.get_frame_count(), d.get_id());
    diag_printf("%6.02f V %s\n", d.get_battery_voltage_V(), _("Battery"));
    diag_printf("%6d °C %s\n", d.get_intake_air_temperature_Celsius(),
                _("Intake Air"));
    diag_printf("%6d °C %s\n", d.get_engine_coolant_temperature_Celsius(),
                _("Coolant"));
    diag_printf("%6d rpm %s\n", d.get_engine_speed_RPM(), _("Crankshaft"));
    diag_printf("%6d mBar %s\n", d.get_manifold_absolute_pressure_mBar(),
                _("Manifold"));
    diag_printf("%6d °D %s\n", d.get_ignition_advance_deg(), _("Advance"));
    diag_printf("%6d °D %s\n", d.get_detonation_correction_deg(), _("Detonation Correction"));
    diag_printf("%6.02f ms %s\n", d.get_injection_duration_ms(),
                _("Injection Duration"));
    diag_printf("%6d %s\n", d[idx_t::adaption_AFR], _("Adaption Air/Fuel"));
    diag_printf("%6d %s\n", d[idx_t::adaption_running],
                _("Adaption Running (Driving)"));
    diag_printf("%6d %s\n", d[idx_t::adaption_stationary],
                _("Adaption Stationary (Idle)"));
    diag_printf("%6d mV %s\n", d.get_oxygen_sensor_voltage_mV(),
                _("O2 Sensor"));
    diag_printf("%6d %s\n", d.get_engine_knocking(), _("Engine-Knock"));
    diag_printf("%6d mBar %s\n", d.get_atmospheric_pressure_mBar(),
                _("Atmosphere"));
    diag_printf("%6d %s\n", (d.is_throttle_fully_open()),
                _("Throttle Full-Power"));
    diag_printf("%6d %s\n", (d.is_throttle_fully_closed()),
                _("Throttle Idle"));
    diag_printf("%6d %s\n", d.is_fuel_pump_on(), _("Fuel-Pump"));

#if 0  // no idea which index, if any
    diag_printf("%6d %s\n", (d.is_vacuum_provided_to_egr_valve()),
                _("EGR+EVap enabled"));
    diag_printf("%6d %s\n", (d.is_oxygen_sensor_loop_closed()),
                _("O2 sensor loop"));
#endif
    diag_printf("%6d °D %s (26)\n", d[28], _("Knock-Delay"));



    if (ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "\n");
    }

    if (ct >= dst_size || ct < 0)
      return ct;  // ERROR: data was not fully written (e.g. buffer too small)
  }

  return ct;
}
