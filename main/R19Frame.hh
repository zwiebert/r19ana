#ifndef R19FRAME_H
#define R19FRAME_H

#include <string>

class XR25Frame;   // forward declaration

class R19Frame {
public:
    int FrameNumber = 0;
    int EngineSpeed_RPM = 0;
    int MAP_mBar = 0;
    int IAT_Celsius = 0;
    int ECT_Celsius = 0;
    int O2_Sensor_mV = 0;
    int AP_mBar = 0;
    float BatteryVoltage_V = 0.0f;
    float ID_msec = 0.0f;
    bool isThrottleOpen = false;
    bool isThrottleClosed = false;
    bool isAGR_AKF = false;
    int IdleSpeedCorr = 0;
    int EngineKnocking = 0;

    explicit R19Frame(const XR25Frame& data);
    R19Frame() = default;
    bool operator==(const R19Frame&) const = default; 
    bool operator!=(const R19Frame&) const = default; 


    std::string getDataAsText() const;
};

#endif // R19FRAME_H