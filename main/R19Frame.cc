#include "R19Frame.hh"

#include <iomanip>
#include <sstream>

#include "XR25Frame.hh"  // must provide getters used below

std::string R19Frame::getDataAsText() const {
  std::ostringstream oss;

  if (get_engine_speed_RPM() == 0) {
    oss << "Motor aus. Frames: " << FrameNumber << "\n\n\n";
  }

  oss << "DK-Leerlauf: " << (is_throttle_fully_closed() ? "Y" : "N") << "\n"
      << "DK-Vollgas: " << (is_throttle_fully_open() ? "Y" : "N") << "\n"
      << "AKF/AGR: " << (is_vacuum_provided_to_egr_valve() ? "Y" : "N")
      << "\n\n";

  oss << "MAP: " << get_manifold_absolute_pressure_mBar() << " mBar\n"
      << "Kühlwasser: " << get_engine_coolant_temperature_Celsius() << " °C\n"
      << "Ansauggemisch: " << get_intake_air_temperature_Celsius() << " °C\n";

  oss << std::fixed << std::setprecision(1);
  oss << "Batterie: " << get_battery_voltage_mV() << " V\n";
  oss << std::setprecision(0);
  oss << "Lambda: " << get_oxygen_sensor_voltage_mV() << " mV ("
      << (get_oxygen_sensor_voltage_mV() < 100
              ? "mager"
              : (get_oxygen_sensor_voltage_mV() > 600 ? "fett" : "1"))
      << ")\n"
      << "Drehzahl: " << get_engine_speed_RPM() << " rpm\n"
      << "Außendruck: " << get_atmospheric_pressure_mBar() << " mBar\n";

  oss << std::fixed << std::setprecision(2);
  oss << "Spritzdauer: " << get_injection_duration_ms() << " ms\n\n";

  oss << "Motorklopfen: " << get_engine_knocking() << "\n"
      << "Leerlaufsteller: " << get_idle_speed_correction() << "\n";

  return oss.str();
}