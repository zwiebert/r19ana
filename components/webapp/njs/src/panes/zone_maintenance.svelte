<script>
  import { onMount } from "svelte";
  import MyPlot from "./plot_test.svelte";

  let dataArray = [];
  let error = null;

  async function fetchBinaryData() {
    // 1. Declare variables at the top of the function scope
    let response;
    let buffer;

    try {
      // 2. Assign the result to your lowercase 'response' variable
      response = await fetch("/f/r19data.bin");

      if (!response.ok) throw new Error("Network response was not ok");

      // 3. Use the lowercase 'response' variable here
      buffer = await response.arrayBuffer();

      // Convert to a typed array
      dataArray = Array.from(new Uint8Array(buffer));

      console.log("Binary data loaded into array:", dataArray);
    } catch (e) {
      // Note: Do NOT try to access 'response' here if fetch failed
      error = e.message;
    }
  }

  const processed = $derived.by(() => {
    process_data(dataArray);
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

  function is_evap_canister_open_to_intake() {
    return is_vacuum_provided_to_egr_valve();
  }
  function getbit(n, pos) {
    return n & (1 << pos);
  }

  function is_throttle_fully_open() {
    return (X(idx_t.flags0) & 0x10) == 0;
  }
  function is_throttle_fully_closed() {
    return !getbit(X(idx_t.flags0), 3);
  }
  function get_id() {
    return X(idx_t.id);
  }

  let data_frame = [];
  function X(idx) {
    return data_frame[idx];
  }

  let yn_arr = $state([[], [], [], [], [], []]);

  let y_arr = $state([1, 2, 3, 4, 5]);
  let y2_arr = $state([5, 4, 3, 2, 1]);

  let y3_arr = $state([1, 2, 3, 4, 5]);
  let y4_arr = $state([5, 4, 3, 2, 1]);

  function process_frame(arr, ct) {
    if (ct == 0) {
      yn_arr = [[], [], [], [], [], []];
      y_arr = [];
      y2_arr = [];
    }
    data_frame = arr;
    yn_arr[0].push(get_engine_speed_RPM());
    yn_arr[1].push(get_manifold_absolute_pressure_mBar());
    yn_arr[2].push(get_ignition_advance_deg());
    yn_arr[3].push(get_oxygen_sensor_voltage_mV());
    yn_arr[4].push(get_injection_duration_ms());
    yn_arr[5].push(is_throttle_fully_closed());

  }

  onMount(() => {
    fetchBinaryData();
  });
</script>

{#if error}
  <p style="color: red;">Error: {error}</p>
{:else if dataArray.length > 0}
  <ul>
    {#each dataArray.slice(0, 10) as byte}
      <li>Byte value: {byte}</li>
    {/each}
    {#if dataArray.length > 10}
      <li>...and {dataArray.length - 10} more bytes</li>
    {/if}
  </ul>
{:else}
  <p>Loading binary data...</p>
{/if}

<button
  on:click={() => {
    process_data(dataArray);
  }}>run</button
>

<MyPlot array1={yn_arr[0]} array2={yn_arr[1]} />
<MyPlot array1={yn_arr[2]} array2={yn_arr[3]} />
<MyPlot array1={yn_arr[4]} array2={yn_arr[5]} />
