#include "PrintDiag.hh"

#include <bitset>

#include "frame.hh"
#include "i18n.hh"
#include "main.hh"

using OurFrame = X53b740Frame;

#define diag_printf(fmt, ...)                              \
  do {                                                     \
    if (view_mask.test(bit++) && ct >= 0) {                \
      auto p = std::min(dst_max, dst + ct);                \
      auto l = std::max(ssize_t(0), dst_size - ct);        \
      ct += snprintf(p, l, "%02u " fmt, bit, __VA_ARGS__); \
    }                                                      \
  } while (0)

static char frame_hex[OurFrame::FRAME_SIZE * 3];
static void frame_hex_fill(const OurFrame::frame_data_t& frame,
                           const char* sep = ",") {
  bin2hex(&frame[0], frame.size(), frame_hex, sizeof frame_hex, sep);
}

int PrintDiagX53b740::snprint_diag(char* dst, size_t dst_siz,
                                   const line_view_mask_t& view_mask) const {
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

    // analog-values
    diag_printf("%6d N    xx  \"Frame-Count\"\n", d.get_frame_count());
    diag_printf("%6d mBar #01 \"%s\"\n",
                d.get_manifold_absolute_pressure_mBar(), _("Manifold"));
    diag_printf("%6d °C   #02 \"%s\"\n",
                d.get_engine_coolant_temperature_Celsius(), _("Coolant"));
    diag_printf("%6d °C   #03 \"%s\"\n", d.get_intake_air_temperature_Celsius(),
                _("Intake Air"));
    diag_printf("%6.02f V    #04 \"%s\"\n", d.get_battery_voltage_V(),
                _("Battery"));
    diag_printf("%6d mV   #05 \"%s\"\n", d.get_oxygen_sensor_voltage_mV(),
                _("O2 Sensor"));
    diag_printf("%6d rpm  #06 \"%s\"\n", d.get_engine_speed_RPM(),
                _("Crankshaft"));

    diag_printf("%6d N    #12 \"%s\"\n", d.get_idle_regulation(),
                _("Idle Regulation"));
    diag_printf("%6d N    #13 \"%s\"\n", d.get_engine_knocking(),
                _("Engine-Knock"));
    diag_printf("%6d °D   #15 \"%s\"\n", d.get_detonation_correction_deg(),
                _("Detonation Correction"));

    diag_printf("%6d N    #21 \"%s\"\n", d.get_idle_adaption(),
                _("Idle Adaption"));


    diag_printf("%6d N    #30 \"%s\"\n",
                int(d[idx_t::richness_adaption_idle2low]) - 128,
                _("Richness-Adaption idle and low load (LTFT)"));
    diag_printf("%6d N    #31 \"%s\"\n",
                int(d[idx_t::richness_adaption_avg2high]) - 128,
                _("Richness-Adaption avg and high load (LTFT)"));
    diag_printf("%6d N    #35 \"%s\"\n",
                int(d[idx_t::richness_regulation]) - 128,
                _("Richness-Regulation (STFT)"));

    diag_printf("%6.02f ms   #50 \"%s\"\n", d.get_injection_duration_ms(),
                _("Injection Duration"));
    diag_printf("%6d °D   #51 \"%s\"\n", d.get_ignition_advance_deg(),
                _("Advance"));


    // static values?
    diag_printf("%6d N    VER \"Version\"\n", d.X(idx_t::program_version));
    diag_printf("%6d N    CAL \"Calibration\"\n",
                d.X(idx_t::calibration_version));
    diag_printf("%6d N    ID  \"Renault-ID\"\n", d.get_id());

    // boolean status bits
    diag_printf("%6d bool PG \"%s\"\n", (d.is_throttle_fully_open()),
                _("Throttle Full-Power"));
    diag_printf("%6d bool PL \"%s\"\n", (d.is_throttle_fully_closed()),
                _("Throttle Idle"));
    diag_printf("%6d bool FUEL \"%s\"\n", d.is_fuel_pump_on(), _("Fuel-Pump"));

#if 1  // no idea which index, if any
    diag_printf("%6d bool XXX \"%s\"\n", (d.is_vacuum_provided_to_egr_valve()),
                _("EGR+EVap enabled"));
    diag_printf("%6d bool XXX \"%s\"\n", (d.is_oxygen_sensor_loop_closed()),
                _("O2 sensor loop"));
#endif

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
