#include "R19Frame.hh"

#include <iomanip>
#include <sstream>

#include "XR25Frame.hh"  // must provide getters used below

R19Frame::R19Frame(const XR25Frame& data) {
  FrameNumber = data.getFrameCounter();

  {
    {
      uint32_t num = ((data[10]) | (data[11] << 8)) & 0xffff;
      EngineSpeed_RPM = (num == 0) ? 0 : int(30000000L / num);
    }
    MAP_mBar = int(data[5] * 3.697f + 103.0f);
    IAT_Celsius = int(data[7] * 0.625f - 40.0f);
    ECT_Celsius = int(data[6] * 0.625f - 40.0f);

    O2_Sensor_mV = int(data[9] * 4);
    AP_mBar = 1090 - data[20];
    BatteryVoltage_mV = int(1000.0f * (data[8] * 0.0312f + 8.0f));
    ID_usec = 2 * ((data[12] | (data[13] << 8)) & 0xffff) - 500;
    isThrottleOpen = (data[4] & 0x10) == 0;
    isThrottleClosed = (data[4] & 0x08) == 0;
    EngineKnocking = data[12];
    // EngineKnockingDelay = data.getIntByIndex(27 + idx_add);
    // IdleSpeedCorr = data.getIntByIndex(14 + idx_add);
  }

    isAGR_AKF = !!(data[20] & 0x20);
    isO2_sensor_closed_loop = !!(data[20] & 0x08);
}

std::string R19Frame::getDataAsText() const {
  std::ostringstream oss;

  if (EngineSpeed_RPM == 0) {
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