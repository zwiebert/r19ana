import type { CarMetrics } from "../charts/iface";
enum idx_t {
  start_byte, // always zero
  program_version, //
  calibration_version, //
  flags0, // input-flags (on/off-switches)
  MAP, //  #1
  ECT, //  #2
  IAT, //  #3
  battery, //  #4
  oxygen, //  #5
  engine_speed_lb, // #6 low-byte
  engine_speed_hb, // #6 high-byte
  injection_duration_lb, //  #50 low-byte
  injection_duration_hb, //  #50 high-byte
  engine_knocking, //  #13
  ignition_advance, //  #51
  detonation_correction, // , #15 (knocking retardation)
  idle_regulation, //  #12
  flags1, // static 0x88  (bit-5: Closed Loop Readiness (Condition Correction Lambda)!!!)
  flags2, // Flags2	Idle Control	Low-bit jitter (stepper), //
  // high-bit spike at end.
  flags3, // Enrichment/Purge,	Wakes-up at warm engine, heavy //
  // spiking/toggling.
  flags4, // static 0x00  Error memory   (rich/lean signal at bit-0 !!!)
  flags5, // static 0x0a
  flags6, // fuel-pump...
  flags7, // 
  unknown_byte24,
  richness_regulation, // #35
  richness_adaption_avg2high, // #30
  idle_adaption, // #21
  unknown_byte28, 
  richness_adaption_idle2low, // #31
  COUNT
}
/**
 * C-style int cast: Truncates decimals and
 * constrains to a 32-bit signed integer.
 */
const int = (n: number) => n | 0;
const getbit = (n: number, pos: number) => (n >>> pos) & 1;

export class x53b_740_parser {
  private data_frame: Uint8Array;

  constructor(data: Uint8Array) {
    this.data_frame = data;
  }

  set_data_packet(data: Uint8Array) {
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
    const num = this.X(idx_t.engine_speed_lb) | (this.X(idx_t.engine_speed_hb) << 8);
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
    return (int(this.X(idx_t.richness_regulation)) - 128) * 0.78125;
  }
  get_richness_adaption_idle_and_low_percent() {
    return (int(this.X(idx_t.richness_adaption_idle2low)) - 128) * 0.78125;
  }
  get_richness_adaption_moderate_and_high_percent() {
    return (int(this.X(idx_t.richness_adaption_avg2high)) - 128)   * 0.78125;
  }
  get_unknown_byte24() {
    return int(this.X(idx_t.unknown_byte24));
  }
  get_unknown_byte28() {
    return int(this.X(idx_t.unknown_byte28));
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
  unknown_byte24,
  unknown_byte28,
  COUNT,
}

/* eslint-disable @typescript-eslint/unbound-method */
const P = x53b_740_parser.prototype;
export const x53b_740_metrics_table: Array<CarMetrics> = [
  { k: 1, parse: P.get_manifold_absolute_pressure_mBar, name: "Manifold Absolute Pressure", unit: "mBar", range:[103,1045], short_name: "MAP" },
  { k: 2, parse: P.get_engine_coolant_temperature_Celsius, name: "Engine Coolant Temperature", unit: "°C", range:[-40,120],  short_name: "ECT" },
  { k: 3, parse: P.get_intake_air_temperature_Celsius, name: "Intake Air Temperature", unit: "°C", range:[-40,120], short_name: "IAT" },
  { k: 4, parse: P.get_battery_voltage_V, name: "Battery Voltage", unit: "V", range:[8,16], short_name: "Batt." },
  { k: 5, parse: P.get_oxygen_sensor_voltage_mV, name: "Oxygen Sensor", unit: "mV", range:[ 0,1200], short_name: "O2" },
  { k: 6, parse: P.get_engine_speed_RPM, name: "Engine Speed", unit: "RPM", range:[0,6000], short_name: "RPM" },
  { k: 12, parse: P.get_idle_regulation, name: "Idle Regulation", unit: "", range:[0,255], short_name: "IdleRegu" },
  { k: 13, parse: P.get_engine_knocking, name: "Engine Pinking", unit: "", range:[0,255], short_name: "Knock" },
  { k: 15, parse: P.get_detonation_correction_deg, name: "Knock-Retard", unit: "°D", range:[0,255], short_name: "KnockRtrd" },
  { k: 21, parse: P.get_idle_adaption, name: "Idle Adaption", unit: "", range:[0,255], short_name: "IdleAdpt" },
  { k: 30, parse: P.get_richness_adaption_moderate_and_high_percent, name: "Richness Adaption (avg-high load)", unit: "%",range:[-100,100], short_name: "LTFT-Cruise" },
  { k: 31, parse: P.get_richness_adaption_idle_and_low_percent, name: "Richness Adaption (idle-low load)", unit: "%",range:[-100,100], short_name: "LTFT-Idle" },
  { k: 35, parse: P.get_richness_regulation, name: "", unit: "%", range:[-100,100], short_name: "STFT" },
  { k: 50, parse: P.get_injection_duration_ms, name: "Injection Duration", unit: "ms", range:[0,66], short_name: "InjDur" },
  { k: 51, parse: P.get_ignition_advance_deg, name: "Ignition Advance", unit: "°BDC", range:[0,255], short_name: "Adv" },
  { k: 0, parse: P.is_fuel_pump_on, name: "Fuel Pump Relay", unit: "boolean",range:[0,1], short_name: "FuelPump" },
  { k: 0, parse: P.is_throttle_fully_closed, name: "Idle Switch", unit: "boolean", range:[0,1], short_name: "IdleSw" },
  { k: 0, parse: P.is_throttle_fully_open, name: "Full Load Switch", unit: "boolean", range:[0,1], short_name: "WOT-Sw" },
  { k: 0, parse: P.get_unknown_byte24, name: "unknown byte 24", unit: "???", range:[0,255], short_name: "byte24" },
  { k: 0, parse: P.get_unknown_byte28, name: "unknown byte 28", unit: "???", range:[0,255], short_name: "byte28" },
];
/* eslint-enable @typescript-eslint/unbound-method */
export default x53b_740_parser;
