#include "R19Frame.hh"
#include "XR25Frame.hh"   // must provide getters used below
#include <sstream>
#include <iomanip>

R19Frame::R19Frame(const XR25Frame& data)
{
    FrameNumber = data.getFrameCounter();

    {
        int idx_add = (data.getByteByIndex(6) == 0xff ? 0 : -2) + 1;
        {
            uint32_t num = (data.getIntByIndex(11 + idx_add) |
                            (data.getIntByIndex(12 + idx_add) << 8)) & 0xffff;
            EngineSpeed_RPM = (num == 0) ? 0 : static_cast<int>(30000000L / num);
        }
        MAP_mBar      = static_cast<int>(data.getIntByIndex(6 + idx_add) * 3.697f + 103.0f);
        IAT_Celsius   = static_cast<int>(data.getIntByIndex(8 + idx_add) * 0.625f - 40.0f);
        ECT_Celsius   = static_cast<int>(data.getIntByIndex(7 + idx_add) * 0.625f - 40.0f);
        O2_Sensor_mV  = static_cast<int>(data.getIntByIndex(10 + idx_add) * 4);
        AP_mBar       = 1090 - data.getIntByIndex(21 + idx_add);
        BatteryVoltage_V =
            data.getIntByIndex(9 + idx_add) * 0.0312f + 8.0f;
        {
            uint32_t num = (data.getIntByIndex(13 + idx_add) |
                            (data.getIntByIndex(14 + idx_add) << 8)) & 0xffff;
            ID_msec       = (num == 0) ? 0.0f : (num * 0.002f) - 0.5f;
        }
        isThrottleOpen  = (data.getByteByIndex(5 - 1) & 0x10) == 0;
        isThrottleClosed= (data.getByteByIndex(5 - 1) & 0x08) == 0;
        EngineKnocking  = data.getIntByIndex(13 + idx_add);
        IdleSpeedCorr   = data.getIntByIndex(14 + idx_add);
    }

    {
        int idx_add = (EngineSpeed_RPM == 0 ? -1 : -3);
        isAGR_AKF = (data.getByteByIndex(20 + idx_add) & 0x20) != 0;
    }
}

std::string R19Frame::getDataAsText() const
{
    std::ostringstream oss;

    if (EngineSpeed_RPM == 0) {
        oss << "Motor aus. Frames: " << FrameNumber << "\n\n\n";
    }

    oss << "DK-Leerlauf: " << (isThrottleClosed ? "Y" : "N") << "\n"
        << "DK-Vollgas: "  << (isThrottleOpen  ? "Y" : "N") << "\n"
        << "AKF/AGR: "    << (isAGR_AKF       ? "Y" : "N") << "\n\n";

    oss << "MAP: " << MAP_mBar << " mBar\n"
        << "Kühlwasser: " << ECT_Celsius << " °C\n"
        << "Ansauggemisch: " << IAT_Celsius << " °C\n";

    oss << std::fixed << std::setprecision(1);
    oss << "Batterie: " << BatteryVoltage_V << " V\n";
    oss << std::setprecision(0);
    oss << "Lambda: " << O2_Sensor_mV << " mV ("
        << (O2_Sensor_mV < 100 ? "mager"
            : (O2_Sensor_mV > 600 ? "fett" : "1"))
        << ")\n"
        << "Drehzahl: " << EngineSpeed_RPM << " rpm\n"
        << "Außendruck: " << AP_mBar << " mBar\n";

    oss << std::fixed << std::setprecision(2);
    oss << "Spritzdauer: " << ID_msec << " ms\n\n";

    oss << "Motorklopfen: " << EngineKnocking << "\n"
        << "Leerlaufsteller: " << IdleSpeedCorr << "\n";

    return oss.str();
}