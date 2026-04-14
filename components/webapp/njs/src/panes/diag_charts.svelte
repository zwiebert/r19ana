<script>
  import uPlot from "uplot";
  import "uplot/dist/uPlot.min.css";
  import { onMount, untrack } from "svelte";
  import MyPlot from "./plot_test.svelte";
  import DropFile from "../components/drop-file.svelte";
  import { get_x53b_740 } from "../cardiag/models/x53b-740.js";
  import {EnableGitHubSamples} from "../store/app_state";
  //import { DiagDataBuffer } from "../store/diag-data.js";

  let { diag_data = [], chart_index = 0 } = $props();
  let error = $state(null);

  onMount(() => {});

  async function getGithubSamples() {
    const user = "zwiebert";
    const repo = "r19ana";
    const folder = "main/data"; // the folder where your .bin files sit

    const url = `https://api.github.com/repos/${user}/${repo}/contents/${folder}`;

    try {
      const response = await fetch(url);
      const files = await response.json();
      console.log("files: ", files);
      // This returns an array of objects with 'name' and 'download_url'
      return files
        .filter((file) => file.name.endsWith(".bin"))
        .map((file) => ({
          name: file.name,
          url: file.download_url, // This is the raw URL to the file
        }));
    } catch (e) {
      console.error("Could not scan GitHub samples", e);
      return [];
    }
  }

  async function fetchBinaryData(url) {
    // 1. Declare variables at the top of the function scope
    let response;
    let buffer;

    try {
      // 2. Assign the result to your lowercase 'response' variable
      response = await fetch(url);

      if (!response.ok) throw new Error("Network response was not ok");

      // 3. Use the lowercase 'response' variable here
      buffer = await response.arrayBuffer();
      diag_data = new Uint8Array(buffer);

      console.log("Binary data loaded into array:", diag_data);
      error = false;
    } catch (e) {
      // Note: Do NOT try to access 'response' here if fetch failed
      error = e.message;
    }
  }

  $effect(() => {
    let data = diag_data;
    if (data && data.length > 0) {
      untrack(() => {
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

  // svelte-ignore state_referenced_locally
  const syncKey = uPlot.sync("zoom_group" + chart_index);
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

  function clear_parsed_data() {
    yn_arr = Array(nmbGraphs)
      .fill()
      .map((e) => []);
  }

  function process_frame(arr, ct) {
    if (ct == 0) {
      clear_parsed_data();
    }
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
  }

  let width = $state(typeof window !== "undefined" ? window.innerWidth : 1000);
  let height = $state(300);
</script>

<h3>{chart_index + 1}</h3>

<div class="w-fit mx-auto">
  <div class="flex flex-row items-center">

    <div class="flex flex-col">
      <label><input type="checkbox" bind:checked={$EnableGitHubSamples} />Allow Samples from GitHub</label>
      {#if $EnableGitHubSamples}
        {#await getGithubSamples()}
          <p>Loading samples URLs...</p>
        {:then list}
          <select
            size={6}
            onchange={(event) => {
              let url = event.target.value;
              if (url) fetchBinaryData(url);
              else clear_parsed_data();
            }}
          >
            {#each list as sample}
              <option value={sample.url}>{sample.name}</option>
            {/each}
          </select>
        {/await}
      {/if}

      {#if import.meta.env.MODE === "mcu"}
        <button
          onclick={() => {
            fetchBinaryData("/f/mnt/sdcard/xr25.bin");
          }}>Fetch Data File From MCU</button
        >
      {/if}
    </div>
    <DropFile diag_data={(data_array) => (diag_data = data_array)} />
    <div class="flex flex-col">
      <label>Chart-Width: <input type="number" bind:value={width} min={400} max={5000} step={100} /></label>
      <label>Chart-Height: <input type="number" bind:value={height} min={100} max={1000} step={25} /></label>
      <button
        onclick={() => {
          process_data(diag_data);
        }}>re-plot</button
      >
    </div>
  </div>
</div>

{#if error}
  <p style="color: red;">Error: {error}</p>
{:else if diag_data.length > 0}
  <p>...{diag_data.length} bytes</p>
{/if}

<div class="w-full text-center">
  <div class="w-fit mx-auto">
    <!-- 2. The Wide Container (The "Giant Canvas") -->
    <!-- 'w-max' tells it to grow with the charts. 'min-w-full' keeps it at least screen-wide. -->
    <div class="block w-max min-w-full text-left p-4">
      {#each [0, 2, 4, 6, 8, 10, 12, 14, 16] as i}
        <div class="text-left">
          <label> <input type="checkbox" bind:checked={yn_show[i]} />Chart for {yn_labels[i].y_series_label} and {yn_labels[i + 1].y_series_label}</label>
          <div class="text-center" style="display:{yn_show[i] ? 'block' : 'none'};touch-action: pan-y; width: 100%;">
            <MyPlot array1={yn_arr[i]} array2={yn_arr[i + 1]} labels1={yn_labels[i]} labels2={yn_labels[i + 1]} {syncKey} {width} {height} } />
          </div>
        </div>
      {/each}
    </div>
  </div>
</div>
