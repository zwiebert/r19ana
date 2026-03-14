#pragma once

#include <string>

class XR25Frame;  // forward declaration

class R19Frame {
  public:
  int get_engine_speed_RPM()const { return EngineSpeed_RPM; }
  int get_manifold_absolute_pressure_mBar()const { return MAP_mBar; }
  int get_intake_air_temperature_Celsius()const { return IAT_Celsius; }
  int get_engine_coolant_temperature_Celsius()const { return ECT_Celsius; }
  int get_oxygen_sensor_voltage_mV () const { return O2_Sensor_mV; }
  int get_atmospheric_pressure_mBar() const { return AP_mBar; }
  int get_battery_voltage_mV() const { return BatteryVoltage_mV; }
  float get_battery_voltage_V() const { return BatteryVoltage_mV / 1000.0f; }
  float get_injection_duration_ms() const { return ID_usec * 0.001f; }
  int get_injection_duration_us() const { return int(ID_usec * 1000.0f); }
  int get_idle_speed_correction() const { return IdleSpeedCorr; }
  int get_engine_knocking() const { return EngineKnocking; }

  bool is_throttle_fully_open() const { return isThrottleOpen; }
  bool is_throttle_fully_closed() const { return isThrottleClosed; }
  bool is_vacuum_provided_to_egr_valve() const { return isAGR_AKF; }
  bool is_evap_canister_open_to_intake() const { return isAGR_AKF; }
  bool is_o2_sensor_loop_closed() const { return isO2_sensor_closed_loop; }

 public:
  int FrameNumber = 0;

  public:
  int EngineSpeed_RPM = 0;
  int MAP_mBar = 0;
  int IAT_Celsius = 0;
  int ECT_Celsius = 0;
  int O2_Sensor_mV = 0;
  int AP_mBar = 0;
  int BatteryVoltage_mV = 0;
  int ID_usec = 0;
  bool isThrottleOpen = false;
  bool isThrottleClosed = false;
  bool isAGR_AKF = false;
  bool isO2_sensor_closed_loop = false;
  int IdleSpeedCorr = 0;
  int EngineKnocking = 0;
  int EngineKnockingDelay = 0;
  

  explicit R19Frame(const XR25Frame& data);
  R19Frame() = default;
  bool operator==(const R19Frame&) const = default;
  bool operator!=(const R19Frame&) const = default;

  std::string getDataAsText() const;
};