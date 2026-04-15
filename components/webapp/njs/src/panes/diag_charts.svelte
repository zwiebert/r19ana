<script>
  import uPlot from "uplot";
  import "uplot/dist/uPlot.min.css";
  import { onMount, untrack } from "svelte";
  import MyPlot from "./plot_test.svelte";
  import DropFile from "../components/request-or-drop-file.svelte";
  import x53b_740_chart from "../cardiag/charts/x53b-740.js";
  import raw_chart from "../cardiag/charts/raw.js";
  import { EnableGitHubSamples } from "../store/app_state";
  //import { DiagDataBuffer } from "../store/diag-data.js";

  let { diag_data = [], chart_index = 0 } = $props();
  let error = $state(null);
  let car_chart = $state.raw(x53b_740_chart);
  let car_charts = [x53b_740_chart, raw_chart];
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

  let xr25 = {
    m_last_byte_was_ff: false,
    m_header_found: false,
    m_invalid_frame_ct: 0,
    m_rbuf: [],
    FRAME_MIN_SIZE: 29,
    FRAME_MAX_SIZE: 64,

    rbuf_clear: function () {
      this.m_rbuf = [];
      this.m_last_byte_was_ff = false;
      this.m_header_found = false;
    },
    add: function (b) {
      if (b == 0xff && !this.m_last_byte_was_ff) {
        this.m_last_byte_was_ff = true;
        return false;
      }

      if (b == 0x00 && this.m_last_byte_was_ff) {
        if (!this.m_header_found) {
          // this is the first frame.
          this.rbuf_clear();
          this.m_header_found = true;
          return false;
        }

        let frame_length = this.m_rbuf.length;

        if (frame_length < this.FRAME_MIN_SIZE) {
          // discard too short frames
          if (frame_length > 0) ++this.m_invalid_frame_ct;
          this.rbuf_clear();
          return false;
        }

        // we have a valid frame, and a new frame header
        this.m_complete_frame_length = frame_length;
        this.m_header_found = true;
        return true;
      }

      // now all special bytes are handled.
      this.m_last_byte_was_ff = false;

      if (this.m_rbuf.length >= this.FRAME_MAX_SIZE) {
        // drop all data, frame is too long
        this.rbuf_clear();
        return false;
      }

      // store data byte

      this.m_rbuf.push(b);
      return false;
    },
  };

  /**
   *  @brief remove header and bytestuffing
   *  @param arr   input raw data u8 array (with hdr and byte stuffing)
   *
   */
  function process_data(arr) {
    let blockCounter = 0;

    for (let b of arr) {
      if (!xr25.add(b)) continue;
      if (blockCounter++ > 0) {
        car_chart.process_frame(xr25.m_rbuf, blockCounter - 2);
      }
    }
    console.log("nBlocks:", blockCounter);
    redraw_charts();
  }

  // svelte-ignore state_referenced_locally
  const syncKey = uPlot.sync("zoom_group" + chart_index);
  let yn_arr = $state.raw(car_chart.get_parsed_data());
  let x_arr = $state.raw([]);
  function redraw_charts() {
    yn_arr = car_chart.get_parsed_data();
    x_arr = yn_arr[0].map((_, i) => i);
    yn_labels = car_chart.get_labels();
  }

  let yn_show = $state(
    Array(64)
      .fill()
      .map((e) => true),
  );
  let x_labels = { series_label: "Blk", axis_label: "x" };
  let yn_labels = $state(car_chart.get_labels());
  let width = $state(typeof window !== "undefined" ? window.innerWidth : 1000);
  let height = $state(300);
  let win_innerWidth = $state(typeof window !== "undefined" ? window.innerWidth : 1000);
</script>

<svelte:window bind:innerWidth={win_innerWidth} onresize={() => (width = window.innerWidth)} />

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
      <DropFile onDataLoaded={(data_array) => (diag_data = data_array)} mode="button" />
    </div>
    <div class="flex flex-col">
      <div>
        <p>Type</p>
        <select bind:value={car_chart} size={3}>
          {#each car_charts as cc}
            <option value={cc}>{cc.get_info().name}</option>
          {/each}
        </select>
      </div>
      <label>Width: <input type="number" bind:value={width} min={400} max={5000} step={100} /></label>
      <label>Height: <input type="number" bind:value={height} min={100} max={1000} step={25} /></label>
      <button
        onclick={() => {
          process_data(diag_data);
        }}>re-plot</button
      >
    </div>
    <div class="flex flex-col text-left">
      {#each Array.from({ length: Math.floor(car_chart.get_nmb_of_graphs() / 2) }, (_, index) => index * 2) as i}
        <div>
          <label> <input type="checkbox" bind:checked={yn_show[i]} />{yn_labels[i].series_label}, {yn_labels[i + 1].series_label}</label>
        </div>
      {/each}
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
      {#if yn_arr[0].length > 0}
        {#each Array.from({ length: Math.floor(car_chart.get_nmb_of_graphs() / 2) }, (_, index) => index * 2) as i}
          <div class="text-left">
            <div class="text-center" style="display:{yn_show[i] ? 'block' : 'none'};touch-action: pan-y; width: 100%;">
              <MyPlot chartData={[x_arr, yn_arr[i], yn_arr[i + 1]]} labels={[x_labels, yn_labels[i], yn_labels[i + 1]]} {syncKey} {width} {height} } />
            </div>
          </div>
        {/each}
      {/if}
    </div>
  </div>
</div>
