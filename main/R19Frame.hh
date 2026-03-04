#ifndef R19FRAME_H
#define R19FRAME_H

#include <string>

class XR25Frame;   // forward declaration

class R19Frame {
public:
    int FrameNumber;
    int EngineSpeed_RPM;
    int MAP_mBar;
    int IAT_Celsius;
    int ECT_Celsius;
    int O2_Sensor_mV;
    int AP_mBar;
    float BatteryVoltage_V;
    float ID_msec;
    bool isThrottleOpen;
    bool isThrottleClosed;
    bool isAGR_AKF;
    int IdleSpeedCorr;
    int EngineKnocking;

    explicit R19Frame(const XR25Frame& data);

    std::string getDataAsText() const;
};

#endif // R19FRAME_H