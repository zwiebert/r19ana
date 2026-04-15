import { get_x53b_740 } from "../parser/x53b-740.js";

export default {
  nmbGraphs: 18,
  yn_arr: null,

  clear_parsed_data: function () {
    this.yn_arr = Array(this.nmbGraphs)
      .fill()
      .map((e) => []);
  },

  get_parsed_data: function () {
    if (!this.yn_arr)
        this.clear_parsed_data();
    return this.yn_arr; 
  },

  process_frame: function (arr, ct) {
    if (ct == 0) {
      this.clear_parsed_data();
    }
    let yn_arr = this.yn_arr;
    let m = get_x53b_740(arr);
    let idx = 0;
    yn_arr[idx++].push(m.get_engine_speed_RPM());
    yn_arr[idx++].push(m.get_manifold_absolute_pressure_mBar());
    yn_arr[idx++].push(m.get_ignition_advance_deg());
    yn_arr[idx++].push(m.get_oxygen_sensor_voltage_mV());
    yn_arr[idx++].push(m.get_injection_duration_ms());
    yn_arr[idx++].push(m.is_throttle_fully_closed());
    ////////////////////////
    yn_arr[idx++].push(m.get_battery_voltage_V());
    yn_arr[idx++].push(m.is_fuel_pump_on());
    yn_arr[idx++].push(m.get_engine_coolant_temperature_Celsius());
    yn_arr[idx++].push(m.get_intake_air_temperature_Celsius());
    yn_arr[idx++].push(m.get_engine_knocking());
    yn_arr[idx++].push(m.get_detonation_correction_deg());
    ////////////////////////
    yn_arr[idx++].push(m.get_richness_regulation());
    yn_arr[idx++].push(m.get_engine_speed_RPM());
    yn_arr[idx++].push(m.get_richness_adaption_idle_and_low());
    yn_arr[idx++].push(m.get_richness_adaption_moderate_and_high());
    yn_arr[idx++].push(m.get_idle_regulation());
    yn_arr[idx++].push(m.get_idle_adaption());
  },

  labels: [
    { y_series_label: "Engine-Speed", y_axis_label: "RPM" },
    { y_series_label: "MAP", y_axis_label: "mBar" },
    { y_series_label: "Spark Advance", y_axis_label: "Degree" },
    { y_series_label: "Oxygen Sensor", y_axis_label: "mV" },
    { y_series_label: "Injection Duration", y_axis_label: "ms" },
    { y_series_label: "Throttle Closed", y_axis_label: "boolean" },

    { y_series_label: "Battery Voltage", y_axis_label: "V" },
    { y_series_label: "Fuel Pump", y_axis_label: "boolean" },
    { y_series_label: "ECT", y_axis_label: "Celsius" },
    { y_series_label: "IAT", y_axis_label: "Celsius" },
    { y_series_label: "Engine Pinking", y_axis_label: "N" },
    { y_series_label: "Detonation Correction", y_axis_label: "Degree" },

    { y_series_label: "Mix regulation", y_axis_label: "N" },
    { y_series_label: "Engine Speed", y_axis_label: "RPM" },
    { y_series_label: "Mix adapt low", y_axis_label: "N" },
    { y_series_label: "Mix adapt high", y_axis_label: "N" },
    { y_series_label: "Idle regulation", y_axis_label: "" },
    { y_series_label: "Idle adapt", y_axis_label: "" },
  ],
};
