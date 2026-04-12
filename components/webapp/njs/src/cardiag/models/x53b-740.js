function Enum(strings) {
  // 1. Clean the string: remove // comments and /* */ comments
  let cleaned = strings[0]
    .replace(/\/\/.*/g, "") // Remove // comments
    .replace(/\/\*[\s\S]*?\*\//g, ""); // Remove /* */ comments

  // 2. Split by commas OR any whitespace
  let keys = cleaned.split(/[,\s]+/).filter(Boolean);

  let obj = {};
  keys.forEach((key, i) => (obj[key] = i));
  return Object.freeze(obj);
}

let idx_t = Enum`
    program_version,        // 0
    calibration_version,    // 1
    flags0,                 // 2, input (on/off-switches)
    MAP,                    // 3, #1
    ECT,                    // 4, #2
    IAT,                    // 5, #3
    battery,                // 6, #4
    oxygen,                 // 7, #5
    engine_speed_lb,        // 8, #6 low-byte
    engine_speed_hb,        // 9, #6 high-byte
    injection_duration_lb,  // 12, #50 low-byte
    injection_duration_hb,  // 11, #50 high-byte
    engine_knocking,        // 10, #13
    ignition_advance,       // 13, #51
    detonation_correction,  // 14, #15 (knocking retardation)
    o2_integrator,          // 15,
    flags1,                 // 16 static 0x88  (bit-5: Closed Loop Readiness (Condition Correction Lambda)!!!)
    flags2,  // 17   Flags2	Idle Control	Low-bit jitter (stepper), //
             // high-bit spike at end.
    flags3,  // 18   Enrichment/Purge,	Wakes-up at warm engine, heavy //
             // spiking/toggling.
    flags4,  // 19 static 0x00  Error memory   (rich/lean signal at bit-0 !!!)
    flags5,  // 20 static 0x0a
    flags6,  // 21   fuel-pump...
    flags7,  // 22
    idle_adaption,               // 23, #21
    richness_adaption_idle2low,  // 24, #31
    richness_regulation,         // 25, #35
    richness_adaption_avg2high,  // 26, #30
    idle_regulation,             // 27  #12
    id,                          // 28
`;
/**
 * C-style int cast: Truncates decimals and
 * constrains to a 32-bit signed integer.
 */
const int = (n) => n | 0;
const getbit = (n, pos) => (n >>> pos) & 1;

export function get_x53b_740(data_frame = []) {
  return x53b_740.set_data_frame(data_frame);
}

function get_null() {
  return 0;
}

let x53b_740 = {
  data_frame: [],
  set_data_frame: function (data) {
    this.data_frame = data;
    return this;
  },
  X: function (idx) {
    return this.data_frame[idx];
  },

  get_manifold_absolute_pressure_mBar: function () {
    return int(this.X(idx_t.MAP) * 3.697 + 103.0);
  },

  get_engine_coolant_temperature_Celsius: function () {
    return int(this.X(idx_t.ECT) * 0.625 - 40.0);
  },
  get_intake_air_temperature_Celsius: function () {
    return int(this.X(idx_t.IAT) * 0.625 - 40.0);
  },
  get_battery_voltage_mV: function () {
    return (this.X(idx_t.battery) * 1000) / 32 + 8000;
  },
  get_battery_voltage_V: function () {
    return this.X(idx_t.battery) * 0.0312 + 8.0;
  },
  get_oxygen_sensor_voltage_mV: function () {
    return int(this.X(idx_t.oxygen) * 4);
  },
  get_engine_speed_RPM: function () {
    let num = this.X(idx_t.engine_speed_lb) | (this.X(idx_t.engine_speed_hb) << 8);
    return num == 0 ? 0 : int(30000000 / num);
  },
  get_engine_knocking: function () {
    return this.X(idx_t.engine_knocking);
  },
  get_injection_duration_us: function () {
    return 2 * ((this.X(idx_t.injection_duration_hb) << 8) | this.X(idx_t.injection_duration_lb));
  },
  get_injection_duration_ms: function () {
    return this.get_injection_duration_us() * 0.001;
  },
  get_ignition_advance_deg: function () {
    return this.X(idx_t.ignition_advance);
  },
  get_detonation_correction_deg: function () {
    return this.X(idx_t.detonation_correction);
  },
  get_idle_regulation: function () {
    return int(this.X(idx_t.idle_regulation));
  },
  get_idle_adaption: function () {
    return int(this.X(idx_t.idle_adaption));
  },
  get_richness_regulation: function () {
    return int(this.X(idx_t.richness_regulation));
  },
  get_richness_adaption_idle_and_low: function () {
    return int(this.X(idx_t.richness_adaption_idle2low)) - 128;
  },
  get_richness_adaption_moderate_and_high: function () {
    return int(this.X(idx_t.richness_adaption_avg2high)) - 128;
  },

  get_idle_period: function () {
    return this.X(22);
  }, // TODO

  get_id: function () {
    return this.X(idx_t.id);
  },

  is_evap_canister_open_to_intake: function () {
    return this.is_vacuum_provided_to_egr_valve();
  },
  is_throttle_fully_open: function () {
    return !getbit(this.X(idx_t.flags0), 4);
    return (this.X(idx_t.flags0) & 0x10) == 0;
  },
  is_throttle_fully_closed: function () {
    return !getbit(this.X(idx_t.flags0), 3);
  },
  is_fuel_pump_on: function () {
    return getbit(this.X(idx_t.flags6), 4);
  },
  is_oxygen_sensor_loop_closed: function () {
    return getbit(this.X(idx_t.flags2), 5);
  }, // plot-verified
  is_vacuum_provided_to_egr_valve: function () {
    return getbit(this.X(idx_t.flags3), 5);
  },
};

  x53b_740.xr25_keys =  [
    get_null,
    // #1..#6
    x53b_740.get_manifold_absolute_pressure_mBar.bind(x53b_740),
    x53b_740.get_engine_coolant_temperature_Celsius.bind(x53b_740),
    x53b_740.get_intake_air_temperature_Celsius.bind(x53b_740),
    x53b_740.get_battery_voltage_V.bind(x53b_740),
    x53b_740.get_oxygen_sensor_voltage_mV.bind(x53b_740),
    x53b_740.get_engine_speed_RPM.bind(x53b_740),
    // #7..#11 are empty
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    // #12..#15
    x53b_740.get_idle_regulation.bind(x53b_740),
    x53b_740.get_engine_knocking.bind(x53b_740),
    get_null,
    x53b_740.get_detonation_correction_deg.bind(x53b_740),
    // #16..#20
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    //#21..#29
    x53b_740.get_idle_adaption.bind(x53b_740),
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    //#30..
    x53b_740.get_richness_adaption_moderate_and_high.bind(x53b_740),
    x53b_740.get_richness_adaption_idle_and_low.bind(x53b_740),
    get_null,
    get_null,
    get_null,
    x53b_740.get_richness_regulation.bind(x53b_740),
    //#36..#49
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    get_null,
    //#50..
    x53b_740.get_injection_duration_ms.bind(x53b_740),
    x53b_740.get_ignition_advance_deg.bind(x53b_740),
  ];
