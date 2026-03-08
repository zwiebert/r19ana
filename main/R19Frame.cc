#include "R19Frame.hh"

#include <iomanip>
#include <sstream>

#include "XR25Frame.hh"  // must provide getters used below

R19Frame::R19Frame(const XR25Frame& data) {
  FrameNumber = data.getFrameCounter();

  {
    int idx_add = (data.getByteByIndex(6) == 0xff ? 0 : -2) + 1;
    {
      uint32_t num = (data.getIntByIndex(11 + idx_add) |
                      (data.getIntByIndex(12 + idx_add) << 8)) &
                     0xffff;
      EngineSpeed_RPM = (num == 0) ? 0 : static_cast<int>(30000000L / num);
    }
    MAP_mBar =
        static_cast<int>(data.getIntByIndex(6 + idx_add) * 3.697f + 103.0f);
    IAT_Celsius =
        static_cast<int>(data.getIntByIndex(8 + idx_add) * 0.625f - 40.0f);
    ECT_Celsius =
        static_cast<int>(data.getIntByIndex(7 + idx_add) * 0.625f - 40.0f);
    O2_Sensor_mV = static_cast<int>(data.getIntByIndex(10 + idx_add) * 4);
    AP_mBar = 1090 - data.getIntByIndex(21 + idx_add);
    BatteryVoltage_mV = int(1000.0f * (data.getIntByIndex(9 + idx_add) * 0.0312f + 8.0f));
    ID_usec = 2 * ((data.getIntByIndex(13 + idx_add) |
                    (data.getIntByIndex(14 + idx_add) << 8)) &
                   0xffff) - 500;
    isThrottleOpen = (data.getByteByIndex(5 - 1) & 0x10) == 0;
    isThrottleClosed = (data.getByteByIndex(5 - 1) & 0x08) == 0;
    EngineKnocking = data.getIntByIndex(13 + idx_add);
    IdleSpeedCorr = data.getIntByIndex(14 + idx_add);
  }

  {
    int idx_add = (EngineSpeed_RPM == 0 ? -1 : -3);
    isAGR_AKF = (data.getByteByIndex(20 + idx_add) & 0x20) != 0;
  }
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