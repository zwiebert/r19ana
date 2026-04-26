<script lang="ts">
  /* eslint-disable svelte/require-each-key */
  import tippy from "sveltejs-tippy";
  import uPlot from "uplot";
  import { stickChartControls } from "../store/app_state";
  import "uplot/dist/uPlot.min.css";
  import { untrack } from "svelte";
  import MyPlot from "./plot_test.svelte";
  import MyBitsPlot from "./plot_status_bits.svelte";
  import DropFile from "../components/request-or-drop-file.svelte";
  import { x53b_740_chart } from "../cardiag/charts/x53b-740.svelte";
  import { raw_chart } from "../cardiag/charts/raw.svelte";
  import type { Icar_chart, Icar_chart_static, ILabel } from "../cardiag/charts/iface";
  //import { byte_unstuffing } from "../cardiag/byte_unstuffing";
  import { RenixDestuffer } from "../cardiag/renix_destuffer";
  import { EnableGitHubSamples } from "../store/app_state";
  import { getGithubSamples } from "../sample_data/github_samples";
  //import { DiagDataBuffer } from "../store/diag-data.js";

  let { chart_index = 0, chart_index_viewed = 0 } = $props();

  let diag_data = new Uint8Array(0);
  let diag_data_name = $state("");
  let car_charts = [x53b_740_chart, raw_chart];
  let car_chart_class = x53b_740_chart;
  let car_chart: Icar_chart = $state(new car_chart_class());
  let live_simu = $state(false);
  let live = $state(false);

  let yn_arr_version_counter = $state(0);
  let x_arr_version_counter = $state(0);
  const yn_arr_version = $derived({ yn_arr_version_counter }); //  version of y data
  const x_arr_version = $derived({ x_arr_version_counter, len: x_arr_len, x_arr }); //  version of x data

  const n1 = 0;
  const n2 = 200000;
  let x_arr_live = $state(Array.from({ length: n2 - n1 + 1 }, (_, i) => n1 + i));

  const yn_arr = $derived.by(() => {
    //$inspect.trace("input-data");
    //console.log("yn_arr-derived");
    yn_arr_version_counter;
    if (!car_chart) return [[]];
    return car_chart.get_chart_data();
  });

  //$effect(()=>{yn_arr;});

  const x_arr = $derived((!live ? car_chart?.get_chart_data()[0].map((_, i) => i) : x_arr_live) ?? []);
  const x_arr_len = $derived(x_arr.length);
  const nmbGraphs = $derived(car_chart.nmbGraphs);

  let error = $state(null);

  const car_metrics = $derived(car_chart.get_car_metrics());

  let yn_show = $state(
    Array(64)
      .fill()
      .map((e) => true),
  );
  let yn_show_as_bits = $state(
    Array(64)
      .fill()
      .map((e) => false),
  );
  let x_labels: ILabel = $derived({ series_label: "Blk", axis_label: "x", vmin: 0, vmax: x_arr.length });
  let width = $state(typeof window !== "undefined" ? window.innerWidth - 10 : 1000);
  let height = $state(300);
  let win_innerWidth = $state(typeof window !== "undefined" ? window.innerWidth : 1000);

  $effect(() => {
    //car_chart.clear_chart_data();
  });

  // svelte-ignore state_referenced_locally
  const syncKey = uPlot.sync("zoom_group" + chart_index);

  function load_data(data: Uint8Array, name = "") {
    live_simulation(false); 
    diag_data = data;
    diag_data_name = name;
    process_data(data, car_chart, false);
    ++yn_arr_version_counter;
    ++x_arr_version_counter;
  }
  function change_car_chart(car_chart_class: Icar_chart_static) {
    live_simulation(false); 
    car_chart = new car_chart_class();
    process_data(diag_data, car_chart, false);
    ++yn_arr_version_counter;
  }

  function live_simulation(simu: boolean) {
    if (!simu) {
      live_simu = false;
      live = false;
      if (live_simu_running) {
        clearTimeout(timeoutId);
        live_simu_running = false;
      }
      return;
    }

    car_chart.clear_chart_data();
    x_arr_live = generate_live_x_arr(n2);
    live_simu = true;
    live = true;
    live_simu_running = true;
    startTimer(100, 0);
  }

  async function fetchBinaryData(url: string) {
    // 1. Declare variables at the top of the function scope
    let response;
    let buffer;

    try {
      // 2. Assign the result to your lowercase 'response' variable
      response = await fetch(url);

      if (!response.ok) throw new Error("Network response was not ok");

      // 3. Use the lowercase 'response' variable here
      buffer = await response.arrayBuffer();
      load_data(new Uint8Array(buffer), url.split("\\").pop().split("/").pop());

      error = false;
    } catch (e) {
      // Note: Do NOT try to access 'response' here if fetch failed
      error = e.message;
    }
  }

  function generate_live_x_arr(count: number, step: number = 0.015): Float64Array {
    const start = Date.now() / 1000;
    const x = new Float64Array(count);
    console.log("start xarray");
    for (let i = 0; i < count; i++) {
      x[i] = start + i * step;
    }
    console.log("end xarray");
    return x;
  }

  let timeoutId = 0;
  let live_simu_running = false;
  function startTimer(chunk_size: number, chunk_nmb: number) {
    timeoutId = setTimeout(() => {
      live_simu_running = true;
      const start = chunk_size * chunk_nmb;
      const end = start + chunk_size;
      if (end >= diag_data.length) return;
      const data = diag_data.subarray(start, end);
      process_data(data, car_chart, true);
      ++yn_arr_version_counter;
      // ++input_data_version; //trigger processing

      //console.log("live simu tick", start, end, data.length);

      // Schedule the next run
      if (live_simu) startTimer(chunk_size, chunk_nmb + 1);
      else {
        live_simu_running = false;
        timeoutId = 0;
      }
    }, 25);
  }

  //startTimer();

  /**
   *  @brief generate chart data from binary input
   *  @param data   input raw data u8 array (with hdr and byte stuffing)
   *  @param append  if true, append to the existing chart data instead of replacing it
   *
   */
  function process_data(data: Uint8Array, car_chart: Icar_chart, append: boolean = false) {
    const unstuffing = new RenixDestuffer((arr: Uint8Array, ct: number) => car_chart.process_data_packet(arr, ct));
    if (!append) car_chart.clear_chart_data();
    unstuffing.process_chunk(data);
  }

  const diag_charts = {
    tt: {
      h3: "Hotkeys: 1,2,3,4,Tab: Change chart page." + "c: make chart controls visible when scrolling",
    },
  };

  let showChartEditor = $state(false);
  let selectOrderIdx = $state(0);
  let selectUnusedMetricsIdx = $state(0);
</script>

{#snippet header()}
  <h3 class="w-screen mx-0 pointer-events-auto" use:tippy={{ content: diag_charts.tt.h3 }}>{chart_index + 1} : {diag_data_name}</h3>
{/snippet}

<svelte:window bind:innerWidth={win_innerWidth} onresize={() => (width = window.innerWidth - 10)} />

<!-- begin of experimental -->
<label><input type="checkbox" bind:checked={showChartEditor} />Show Chart Editor</label>
{#if showChartEditor}
  <div class="flex flex-row mx-auto w-fit">
    <div>
      <h5 class="p-0 m-0">Unused Metrics</h5>
      <select size={car_metrics.length} bind:value={selectUnusedMetricsIdx}>
        {#each car_metrics as cm, i}
          {#if !car_chart.order.includes(i)}
            <option value={i}>
              {i}
              {cm.name}
            </option>
          {/if}
        {/each}
      </select>
    </div>
    <div class="flex flex-col mx-auto w-fit">
      <button
        class=""
        onclick={() => {
          if (selectOrderIdx === 0) return;
          [car_chart.order[selectOrderIdx], car_chart.order[selectOrderIdx - 1]] = [car_chart.order[selectOrderIdx - 1], car_chart.order[selectOrderIdx]];
          --selectOrderIdx;
          console.log("car_chart.order", car_chart.order);
        }}>move up</button
      >
      <button
        class=""
        onclick={() => {
          if (selectOrderIdx + 1 >= car_chart.order.length) return;
          [car_chart.order[selectOrderIdx], car_chart.order[selectOrderIdx + 1]] = [car_chart.order[selectOrderIdx + 1], car_chart.order[selectOrderIdx]];
          ++selectOrderIdx;
          console.log("car_chart.order", car_chart.order);
        }}>move down</button
      >
      <button
        class=""
        onclick={() => {
          console.log("push:", selectUnusedMetricsIdx);
          car_chart.order.push(selectUnusedMetricsIdx);
        }}>use this</button
      >
      <button
        class=""
        onclick={() => {
          const remIdx = selectOrderIdx;
          car_chart.order.splice(remIdx, 1);
          if (selectOrderIdx >= car_chart.order.length) selectOrderIdx = car_chart.order.length - 1;
        }}>not use this</button
      >
      <button
        class=""
        onclick={() => {
          car_chart.order.length = 0;
        }}>use none</button
      >
    </div>
    <div>
      <h5 class="p-0 m-0">Used Metrics</h5>
      <select size={car_metrics.length} bind:value={selectOrderIdx}>
        {#each car_chart.order as cmi, i}
          <option value={i}>
            {cmi}
            {car_metrics[cmi].name}
          </option>
        {/each}
      </select>
    </div>
  </div>
{/if}
<!-- end of experimental -->

<div class="min-w-full text-center">
  <div class="min-w-full mx-auto">
    <!-- 2. The Wide Container (The "Giant Canvas") -->
    <!-- 'w-max' tells it to grow with the charts. 'min-w-full' keeps it at least screen-wide. -->
    <div class="block w-max min-w-full text-left p-4">
      {#if !$stickChartControls}
        <div class="sticky top-0 left-0 z-60 w-screen">
          {@render header()}
        </div>
      {/if}

      <div class="sticky left-0 z-50 w-screen pointer-events-none text-center" class:top-0={$stickChartControls}>
        {#if $stickChartControls}
          {@render header()}
        {/if}
        <div class="mx-auto w-fit pointer-events-auto bg-amber-50 p-2" class:shadow-xl={$stickChartControls}>
          <div class="flex flex-row items-center">
            <div class="flex flex-col">
              <label><input type="checkbox" bind:checked={$EnableGitHubSamples} />Allow Samples from GitHub</label>
              {#if $EnableGitHubSamples}
                {#await getGithubSamples()}
                  <p>Loading samples URLs...</p>
                {:then list: {url:string, name:string}[]}
                  <select
                    size={6}
                    onchange={(event) => {
                      let url = event.target.value as string;
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
              {#if chart_index === chart_index_viewed}
                <DropFile onDataLoaded={load_data} mode="button" />
              {/if}

              <label
                ><input
                  type="checkbox"
                  bind:checked={live_simu}
                  onclick={(e) => {
                    live_simulation(e.target.value);
                  }}
                />Live-Simulation</label
              >
            </div>
            <div class="flex flex-col">
              <div>
                <p>Type</p>
                <select
                  bind:value={car_chart_class}
                  size={3}
                  onchange={() => {
                    change_car_chart(car_chart_class);
                  }}
                >
                  {#each car_charts as cc}
                    <option value={cc}>{cc.get_info().name}</option>
                  {/each}
                </select>
              </div>
              <label>Width: <input type="number" bind:value={width} min={400} max={5000} step={100} /></label>
              <label>Height: <input type="number" bind:value={height} min={100} max={1000} step={25} /></label>
              <button
                onclick={() => {
                  ++force_all_version;
                }}>re-plot</button
              >
            </div>
            <div class="flex flex-col text-left">
              {#each Array.from({ length: Math.floor(nmbGraphs / 2) }, (_, index) => index * 2) as i}
                <div>
                  <label><input type="checkbox" bind:checked={yn_show[i]} />{car_chart.labels[i]?.series_label}, {car_chart.labels[i + 1]?.series_label}</label>
                  {#if car_chart.labels[i]?.axis_label === "bits" || car_chart.labels[i]?.axis_label === "raw"}
                    <label><input type="checkbox" bind:checked={yn_show_as_bits[i]} />bits</label>
                  {/if}
                </div>
              {/each}
            </div>
          </div>
          {#if error}
            <p style="color: red;">Error: {error}</p>
          {/if}
        </div>
      </div>
      {#if x_arr.length > 0}
        {#each Array.from({ length: Math.floor(nmbGraphs / 2) }, (_, index) => index * 2) as i}
          <div class="text-left">
            <div class="text-center" style="display:{yn_show[i] ? 'block' : 'none'};touch-action: pan-y; width: 100%;">
              {#if yn_show_as_bits[i] && (car_chart.labels[i]?.axis_label === "bits" || car_chart.labels[i]?.axis_label === "raw")}
                <MyBitsPlot
                  chartData={[x_arr, yn_arr[i]]}
                  chartDataVersions={[x_arr_version, yn_arr_version]}
                  labels={[x_labels, car_chart.labels[i]]}
                  {syncKey}
                  {width}
                  {height}
                  is_live={live}
                  }
                />
                <MyBitsPlot
                  chartData={[x_arr, yn_arr[i + 1]]}
                  chartDataVersions={[x_arr_version, yn_arr_version]}
                  labels={[x_labels, car_chart.labels[i + 1]]}
                  {syncKey}
                  {width}
                  {height}
                  is_live={live}
                  }
                />
              {:else}
                <MyPlot
                  xData={x_arr}
                  xDataVer={x_arr_version}
                  yData={yn_arr[i]}
                  yDataVer={yn_arr_version}
                  y2Data={yn_arr[i + 1]}
                  y2DataVer={yn_arr_version}
                  labels={[x_labels, car_chart.labels[i], car_chart.labels[i + 1]]}
                  {syncKey}
                  {width}
                  {height}
                  is_live={live}
                  }
                />
              {/if}
            </div>
          </div>
        {/each}
      {/if}
    </div>
  </div>
</div>
