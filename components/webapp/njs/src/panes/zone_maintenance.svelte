<script>
  import uPlot from "uplot";
  import "uplot/dist/uPlot.min.css";
  import { onMount, untrack } from "svelte";
  import MyPlot from "./plot_test.svelte";
  import DropFile from "../components/drop-file.svelte";
  import { DiagDataBuffer } from "../store/diag-data.js";

  let error = $state(null);

  async function fetchBinaryData() {
    // 1. Declare variables at the top of the function scope
    let response;
    let buffer;

    try {
      // 2. Assign the result to your lowercase 'response' variable
      response = await fetch("/f/mnt/sdcard/xr25.bin");

      if (!response.ok) throw new Error("Network response was not ok");

      // 3. Use the lowercase 'response' variable here
      buffer = await response.arrayBuffer();
      DiagDataBuffer.set(new Uint8Array(buffer));

      console.log("Binary data loaded into array:", $DiagDataBuffer);
    } catch (e) {
      // Note: Do NOT try to access 'response' here if fetch failed
      error = e.message;
    }
  }

  $effect(() => {
    let data = $DiagDataBuffer;
    if (data && data.length > 0) {
      untrack (() => {
        process_data(data);
      });
    }
  });

  function process_data(arr) {
    let ct = 0;
    let blockCounter = 0;
    let dataFrame = [];
    let last_xff = false;
    let fidx = 0;

    for (let b of arr) {
      if (last_xff) {
        if (b == 0x00) {
          last_xff = false;
          if (blockCounter++ > 0) {
            process_frame(dataFrame, blockCounter - 2);
          }
          dataFrame = [];
          continue;
        } else {
          last_xff = false;
        }
      } else {
        if (b == 0xff) {
          last_xff = true;
          continue;
        }
      }
      dataFrame.push(b);
      ct++;
    }
    redraw_charts();
    console.log("ct:", ct, "blocks:", blockCounter);
  }

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

  // Usage (Commas and comments now allowed!)
  let Status = Enum`
  PENDING,    // Just started
  RUNNING,    /* In progress */
  COMPLETED,
  FAILED      // Something went wrong
`;

  console.log(Status.PENDING); // 0
  console.log(Status.RUNNING); // 1
  console.log(Status.COMPLETED); // 2

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

  function get_manifold_absolute_pressure_mBar() {
    return int(X(idx_t.MAP) * 3.697 + 103.0);
  }

  function get_engine_coolant_temperature_Celsius() {
    return int(X(idx_t.ECT) * 0.625 - 40.0);
  }
  function get_intake_air_temperature_Celsius() {
    return int(X(idx_t.IAT) * 0.625 - 40.0);
  }
  function get_battery_voltage_mV() {
    return (X(idx_t.battery) * 1000) / 32 + 8000;
  }
  function get_battery_voltage_V() {
    return X(idx_t.battery) * 0.0312 + 8.0;
  }
  function get_oxygen_sensor_voltage_mV() {
    return int(X(idx_t.oxygen) * 4);
  }
  function get_engine_speed_RPM() {
    let num = X(idx_t.engine_speed_lb) | (X(idx_t.engine_speed_hb) << 8);
    return num == 0 ? 0 : int(30000000 / num);
  }
  function get_engine_knocking() {
    return X(idx_t.engine_knocking);
  }
  function get_injection_duration_us() {
    return 2 * ((X(idx_t.injection_duration_hb) << 8) | X(idx_t.injection_duration_lb));
  }
  function get_injection_duration_ms() {
    return get_injection_duration_us() * 0.001;
  }
  function get_ignition_advance_deg() {
    return X(idx_t.ignition_advance);
  }
  function get_detonation_correction_deg() {
    return X(idx_t.detonation_correction);
  }
  function get_idle_regulation() {
    return int(X(idx_t.idle_regulation));
  }
  function get_idle_adaption() {
    return int(X(idx_t.idle_adaption));
  }
  function get_richness_regulation() {
    return int(X(idx_t.richness_regulation));
  }
  function get_richness_adaption_idle_and_low() {
    return int(X(idx_t.richness_adaption_idle2low)) - 128;
  }
  function get_richness_adaption_moderate_and_high() {
    return int(X(idx_t.richness_adaption_avg2high)) - 128;
  }

  function get_idle_period() {
    return X(22);
  } // TODO

  function get_id() {
    return X(idx_t.id);
  }

  function is_evap_canister_open_to_intake() {
    return is_vacuum_provided_to_egr_valve();
  }
  function getbit(n, pos) {
    return (n & (1 << pos)) != 0;
  }

  function is_throttle_fully_open() {
    return (X(idx_t.flags0) & 0x10) == 0;
  }
  function is_throttle_fully_closed() {
    return !getbit(X(idx_t.flags0), 3);
  }
  function is_fuel_pump_on() {
    return getbit(X(idx_t.flags6), 4);
  }
  function is_oxygen_sensor_loop_closed() {
    return getbit(X(idx_t.flags2), 5);
  } // plot-verified
  function is_vacuum_provided_to_egr_valve() {
    return getbit(X(idx_t.flags3), 5);
  }

  let data_frame = [];
  function X(idx) {
    return data_frame[idx];
  }

  const syncKey = uPlot.sync("zoom_group");
  const nmbGraphs = 18;
  let yn_arr = $state.raw(
    Array(nmbGraphs)
      .fill()
      .map((e) => []),
  );
  function redraw_charts() {
    yn_arr = yn_arr;
  }
  let yn_show = $state(
    Array(nmbGraphs)
      .fill()
      .map((e) => true),
  );

  let yn_labels = $state([
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
  ]);

  function process_frame(arr, ct) {
    if (ct == 0) {
      yn_arr = Array(nmbGraphs)
        .fill()
        .map((e) => []);
    }
    data_frame = arr;
    let idx = 0;
    yn_arr[idx++].push(get_engine_speed_RPM());
    yn_arr[idx++].push(get_manifold_absolute_pressure_mBar());
    yn_arr[idx++].push(get_ignition_advance_deg());
    yn_arr[idx++].push(get_oxygen_sensor_voltage_mV());
    yn_arr[idx++].push(get_injection_duration_ms());
    yn_arr[idx++].push(is_throttle_fully_closed());
    ////////////////////////
    yn_arr[idx++].push(get_battery_voltage_V());
    yn_arr[idx++].push(is_fuel_pump_on());
    yn_arr[idx++].push(get_engine_coolant_temperature_Celsius());
    yn_arr[idx++].push(get_intake_air_temperature_Celsius());
    yn_arr[idx++].push(get_engine_knocking());
    yn_arr[idx++].push(get_detonation_correction_deg());
    ////////////////////////
    yn_arr[idx++].push(get_richness_regulation());
    yn_arr[idx++].push(get_engine_speed_RPM());
    yn_arr[idx++].push(get_richness_adaption_idle_and_low());
    yn_arr[idx++].push(get_richness_adaption_moderate_and_high());
    yn_arr[idx++].push(get_idle_regulation());
    yn_arr[idx++].push(get_idle_adaption());
  }

  onMount(() => {
   // fetchBinaryData();
  });
</script>

{#if error}
  <p style="color: red;">Error: {error}</p>
{:else if $DiagDataBuffer.length > 0}
  <ul>
    {#each $DiagDataBuffer.slice(0, 10) as byte}
      <li>Byte value: {byte}</li>
    {/each}
    {#if $DiagDataBuffer.length > 10}
      <li>...and {$DiagDataBuffer.length - 10} more bytes</li>
    {/if}
  </ul>
{:else}
  <p>Loading binary data...</p>
{/if}
<DropFile />
<button
  on:click={() => {
    fetchBinaryData();
  }}>fetch</button
>
<button
  on:click={() => {
    process_data($DiagDataBuffer);
  }}>run</button
>

{#each [0, 2, 4, 6, 8, 10, 12, 14, 16] as i}
  <div class="text-left">
    <label class="text-left">
      <input type="checkbox" bind:checked={yn_show[i]} />Chart for {yn_labels[i].y_series_label} and {yn_labels[i + 1].y_series_label}</label
    >
    <div style="display:{yn_show[i] ? 'block' : 'none'};touch-action: pan-y; width: 100%;">
      <MyPlot array1={yn_arr[i]} array2={yn_arr[i + 1]} labels1={yn_labels[i]} labels2={yn_labels[i + 1]} {syncKey} } />
    </div>
  </div>
{/each}
