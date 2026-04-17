enum idx_t {
  program_version, // 0
  calibration_version, // 1
  flags0, // 2, input (on/off-switches)
  MAP, // 3, #1
  ECT, // 4, #2
  IAT, // 5, #3
  battery, // 6, #4
  oxygen, // 7, #5
  engine_speed_lb, // 8, #6 low-byte
  engine_speed_hb, // 9, #6 high-byte
  injection_duration_lb, // 12, #50 low-byte
  injection_duration_hb, // 11, #50 high-byte
  engine_knocking, // 10, #13
  ignition_advance, // 13, #51
  detonation_correction, // 14, #15 (knocking retardation)
  o2_integrator, // 15,
  flags1, // 16 static 0x88  (bit-5: Closed Loop Readiness (Condition Correction Lambda)!!!)
  flags2, // 17   Flags2	Idle Control	Low-bit jitter (stepper), //
  // high-bit spike at end.
  flags3, // 18   Enrichment/Purge,	Wakes-up at warm engine, heavy //
  // spiking/toggling.
  flags4, // 19 static 0x00  Error memory   (rich/lean signal at bit-0 !!!)
  flags5, // 20 static 0x0a
  flags6, // 21   fuel-pump...
  flags7, // 22
  idle_adaption, // 23, #21
  richness_adaption_idle2low, // 24, #31
  richness_regulation, // 25, #35
  richness_adaption_avg2high, // 26, #30
  idle_regulation, // 27  #12
  id, // 28
}
/**
 * C-style int cast: Truncates decimals and
 * constrains to a 32-bit signed integer.
 */
const int = (n: number) => n | 0;
const getbit = (n: number, pos: number) => (n >>> pos) & 1;
function get_null() {
  return 0;
}

export class x53b_740_parser {
  private data_frame: Uint8Array;

  constructor(data: Uint8Array) {
    this.data_frame = data;
  }

  set_data_frame(data: Uint8Array) {
    this.data_frame = data;
    return this;
  }

  private X(idx: number) {
    return this.data_frame[idx];
  }

  get_manifold_absolute_pressure_mBar() {
    return int(this.X(idx_t.MAP) * 3.697 + 103.0);
  }

  get_engine_coolant_temperature_Celsius() {
    return int(this.X(idx_t.ECT) * 0.625 - 40.0);
  }
  get_intake_air_temperature_Celsius() {
    return int(this.X(idx_t.IAT) * 0.625 - 40.0);
  }
  get_battery_voltage_mV() {
    return (this.X(idx_t.battery) * 1000) / 32 + 8000;
  }
  get_battery_voltage_V() {
    return this.X(idx_t.battery) * 0.0312 + 8.0;
  }
  get_oxygen_sensor_voltage_mV() {
    return int(this.X(idx_t.oxygen) * 4);
  }
  get_engine_speed_RPM() {
    let num = this.X(idx_t.engine_speed_lb) | (this.X(idx_t.engine_speed_hb) << 8);
    return num == 0 ? 0 : int(30000000 / num);
  }
  get_engine_knocking() {
    return this.X(idx_t.engine_knocking);
  }
  get_injection_duration_us() {
    return 2 * ((this.X(idx_t.injection_duration_hb) << 8) | this.X(idx_t.injection_duration_lb));
  }
  get_injection_duration_ms() {
    return this.get_injection_duration_us() * 0.001;
  }
  get_ignition_advance_deg() {
    return this.X(idx_t.ignition_advance);
  }
  get_detonation_correction_deg() {
    return this.X(idx_t.detonation_correction);
  }
  get_idle_regulation() {
    return int(this.X(idx_t.idle_regulation));
  }
  get_idle_adaption() {
    return int(this.X(idx_t.idle_adaption));
  }
  get_richness_regulation() {
    return int(this.X(idx_t.richness_regulation));
  }
  get_richness_adaption_idle_and_low() {
    return int(this.X(idx_t.richness_adaption_idle2low)) - 128;
  }
  get_richness_adaption_moderate_and_high() {
    return int(this.X(idx_t.richness_adaption_avg2high)) - 128;
  }

  get_idle_period() {
    return this.X(22);
  } // TODO

  get_id() {
    return this.X(idx_t.id);
  }

  is_evap_canister_open_to_intake() {
    return this.is_vacuum_provided_to_egr_valve();
  }
  is_throttle_fully_open() {
    return !getbit(this.X(idx_t.flags0), 4);
    return (this.X(idx_t.flags0) & 0x10) == 0;
  }
  is_throttle_fully_closed() {
    return !getbit(this.X(idx_t.flags0), 3);
  }
  is_fuel_pump_on() {
    return getbit(this.X(idx_t.flags6), 4);
  }
  is_oxygen_sensor_loop_closed() {
    return getbit(this.X(idx_t.flags2), 5);
  } // plot-verified
  is_vacuum_provided_to_egr_valve() {
    return getbit(this.X(idx_t.flags3), 5);
  }
}

export class x53b_740_XR25_parser extends x53b_740_parser {
  readonly xr25_keys: Record<number, (...args: any[]) => any> = {
    1: this.get_manifold_absolute_pressure_mBar.bind(this),
    2: this.get_engine_coolant_temperature_Celsius.bind(this),
    3: this.get_intake_air_temperature_Celsius.bind(this),
    4: this.get_battery_voltage_V.bind(this),
    5: this.get_oxygen_sensor_voltage_mV.bind(this),
    6: this.get_engine_speed_RPM.bind(this),
    12: this.get_idle_regulation.bind(this),
    13: this.get_engine_knocking.bind(this),
    15: this.get_detonation_correction_deg.bind(this),
    21: this.get_idle_adaption.bind(this),
    30: this.get_richness_adaption_moderate_and_high.bind(this),
    31: this.get_richness_adaption_idle_and_low.bind(this),
    35: this.get_richness_regulation.bind(this),
    50: this.get_injection_duration_ms.bind(this),
    51: this.get_ignition_advance_deg.bind(this),
  };
}

export enum x53b_740_metrics_table_pos {
  map_sensor,
  ect_sensor,
  iat_sensor,
  batt_voltage,
  o2_sensor,
  engine_speed,
  idle_regulation,
  engine_pinking,
  detonation_correction,
  idle_adaption,
  richness_adaption_avg2high,
  richness_adaption_idle2low,
  richness_regulation,
  injection_duration,
  ignition_advance,
  fuel_pump_relay,
  idle_switch,
  full_load_switch,
  COUNT,
}

export interface CarMetrics {
  k: number;
  parse: () => any;
  name: string;
  unit: string;
  short_name: string;
}

const P = x53b_740_parser.prototype;
export const x53b_740_metrics_table: Array<CarMetrics> = [
  { k: 1, parse: P.get_manifold_absolute_pressure_mBar, name: "Manifold Absolute Pressure", unit: "mBar", short_name: "MAP" },
  { k: 2, parse: P.get_engine_coolant_temperature_Celsius, name: "Engine Coolant Temperature", unit: "°C", short_name: "ECT" },
  { k: 3, parse: P.get_intake_air_temperature_Celsius, name: "Intake Air Temperature", unit: "°C", short_name: "IAT" },
  { k: 4, parse: P.get_battery_voltage_V, name: "Battery Voltage", unit: "V", short_name: "Batt." },
  { k: 5, parse: P.get_oxygen_sensor_voltage_mV, name: "Oxygen Sensor", unit: "mV", short_name: "O2" },
  { k: 6, parse: P.get_engine_speed_RPM, name: "Engine Speed", unit: "RPM", short_name: "RPM" },
  { k: 12, parse: P.get_idle_regulation, name: "Idle Regulation", unit: "", short_name: "IdleRegu" },
  { k: 13, parse: P.get_engine_knocking, name: "Engine Pinking", unit: "", short_name: "Cliq" },
  { k: 15, parse: P.get_detonation_correction_deg, name: "Detonation Correction", unit: "°D", short_name: "" },
  { k: 21, parse: P.get_idle_adaption, name: "Idle Adaption", unit: "", short_name: "IdleAdpt" },
  { k: 30, parse: P.get_richness_adaption_moderate_and_high, name: "Richness Adaption (avg-high load)", unit: "", short_name: "MixAdptHigh" },
  { k: 31, parse: P.get_richness_adaption_idle_and_low, name: "Richness Adaption (idle-low load)", unit: "", short_name: "MixAdptLow" },
  { k: 35, parse: P.get_richness_regulation, name: "", unit: "", short_name: "MixRegu" },
  { k: 50, parse: P.get_injection_duration_ms, name: "Injection Duration", unit: "ms", short_name: "InjDur" },
  { k: 51, parse: P.get_ignition_advance_deg, name: "Ignition Advance", unit: "°D", short_name: "Adv" },
  { k: 0, parse: P.is_fuel_pump_on, name: "Fuel Pump Relay", unit: "boolean", short_name: "FuelPump" },
  { k: 0, parse: P.is_throttle_fully_closed, name: "Idle Switch", unit: "boolean", short_name: "IdleSw" },
  { k: 0, parse: P.is_throttle_fully_open, name: "Full Load Switch", unit: "boolean", short_name: "FullLdSw" },
];

export default x53b_740_parser;
