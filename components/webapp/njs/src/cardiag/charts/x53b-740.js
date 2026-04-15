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
    { series_label: "RPM", axis_label: "RPM" },
    { series_label: "MAP", axis_label: "mBar" },
    { series_label: "Advance", axis_label: "Degree" },
    { series_label: "O2", axis_label: "mV" },
    { series_label: "Inj-Duration", axis_label: "ms" },
    { series_label: "Sw-Idle", axis_label: "boolean" },

    { series_label: "Battery", axis_label: "V" },
    { series_label: "Fuel-Pump", axis_label: "boolean" },
    { series_label: "ECT", axis_label: "Celsius" },
    { series_label: "IAT", axis_label: "Celsius" },
    { series_label: "Pinking", axis_label: "N" },
    { series_label: "Det-Correction", axis_label: "Degree" },

    { series_label: "Mix-Regu", axis_label: "N" },
    { series_label: "RPM", axis_label: "RPM" },
    { series_label: "Mix-Adpt-Low", axis_label: "N" },
    { series_label: "Mix-Adpt-High", axis_label: "N" },
    { series_label: "Idle-Regu", axis_label: "" },
    { series_label: "Idle-Adpt", axis_label: "" },
  ],
};
