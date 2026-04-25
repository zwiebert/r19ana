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
  import { x53b_740_chart_factory } from "../cardiag/charts/x53b-740.svelte";
  import { raw_chart_factory } from "../cardiag/charts/raw.svelte";
  import type { Icar_chart, ILabel } from "../cardiag/charts/iface";
  //import { byte_unstuffing } from "../cardiag/byte_unstuffing";
  import { RenixDestuffer } from "../cardiag/renix_destuffer";
  import { EnableGitHubSamples } from "../store/app_state";
  import { getGithubSamples } from "../sample_data/github_samples";
  //import { DiagDataBuffer } from "../store/diag-data.js";

  let { chart_index = 0, chart_index_viewed = 0 } = $props();

  interface IchartData {
    car_chart: Icar_chart; //
    nmbGraphs: number;
    yn_arr: number[][];
    x_arr: number[];
    x_arr_modifed: boolean;
    yn_arr_modified: boolean;
    id: number;
  }
  let diag_data: Uint8Array = $state.raw(new Uint8Array(0));
  let diag_data_name = $state("");
  let force_all_version = $state(0);
  let input_data_version = $state(0); // version counter for input data changes
  let car_chart_version = $state(0); // index of  current parser/chart generator
  let input_data_live_begin = $state(0);
  let input_data_live_end = $state(0);
  const input_data = $derived(!live_simu ? diag_data : diag_data.subarray(input_data_live_begin, input_data_live_end));
  const input_data_len = $derived(input_data.length);
  const yn_arr_version = $derived({ cc: car_chart_version, len: yn_arr.length, len2: yn_arr[0].length, id: chart_data.id, ls: live_simu }); //  version of y data
  const x_arr_version = $derived({ cc: car_chart_version, len: x_arr_len, x_arr }); //  version of x data

  const n1 = 0;
  const n2 = 200000;
  let x_arr_live = $state(Array.from({ length: n2 - n1 + 1 }, (_, i) => n1 + i));
  const yn_arr = $derived(chart_data?.yn_arr ?? [[]]);
  const x_arr = $derived((!live_simu ? chart_data?.x_arr : x_arr_live) ?? []);
  const x_arr_len = $derived(x_arr.length);
  const nmbGraphs = $derived(chart_data.nmbGraphs);

  let error = $state(null);
  let car_charts: Icar_chart[] = $state([x53b_740_chart_factory(), raw_chart_factory()]);
  let car_chart: Icar_chart = $derived(car_charts[car_chart_version]);
  const car_metrics = $derived(car_chart.get_car_metrics());
  const processData_trigger = $derived({ ver: car_chart_version, len: input_data_len, ipd: input_data_version, fa: force_all_version });

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

  const chart_data: IchartData = $derived.by(() => {
    //console.log("create chart_data");
    const data = input_data;
    const chart = car_chart;
    const trigger = processData_trigger;
    const getID = (() => {
      let id = 0; // This acts as your 'static' variable
      return () => ++id;
    })();
    untrack(() => {
      process_data(data, chart, live_simu);
    });
    const x_arr = chart.get_chart_data()[0].map((_, i) => i);
    //console.log(chart.get_info(), chart.get_chart_data()[1].length);

    return {
      car_chart: chart, //
      nmbGraphs: chart.get_nmb_of_graphs(),
      yn_arr: chart.get_chart_data(),
      x_arr: x_arr,
      id: getID(),
    };
  });

  $effect(() => {
    console.log("simu effect");
    const trigger = live_simu;
    live_simu_process_data();
  });

  $effect(() => {
    //car_chart.clear_chart_data();
  });

  // svelte-ignore state_referenced_locally
  const syncKey = uPlot.sync("zoom_group" + chart_index);

  let live_simu = $state(false);

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
      diag_data = new Uint8Array(buffer);
      diag_data_name = url.split("\\").pop().split("/").pop();

      error = false;
    } catch (e) {
      // Note: Do NOT try to access 'response' here if fetch failed
      error = e.message;
    }
  }

  let timeoutId = 0;

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

  function startTimer(chunk_size: number, chunk_nmb: number) {
    timeoutId = setTimeout(() => {
      const data = diag_data;
      const start = chunk_size * chunk_nmb;
      const end = start + chunk_size;
      if (end >= data.length) return;

      input_data_live_end = end;
      input_data_live_begin = start;
      ++input_data_version; //trigger processing

      //console.log("live simu tick", start, end, input_data.length);

      // Schedule the next run
      if (live_simu) startTimer(chunk_size, chunk_nmb + 1);
    }, 20);
  }

  function live_simu_process_data() {
    clearTimeout(timeoutId);
    if (live_simu) {
      car_chart.clear_chart_data();
      x_arr_live = generate_live_x_arr(n2);
      startTimer(100, 0);
    }
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

  let showChartEditor = $state(true);
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
          {#if !car_charts[0].order.includes(i)}
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
          [car_charts[0].order[selectOrderIdx], car_charts[0].order[selectOrderIdx - 1]] = [
            car_charts[0].order[selectOrderIdx - 1],
            car_charts[0].order[selectOrderIdx],
          ];
          --selectOrderIdx;
          console.log("car_charts[0].order", car_charts[0].order);
        }}>move up</button
      >
      <button
        class=""
        onclick={() => {
          if (selectOrderIdx + 1 >= car_chart.order.length) return;
          [car_charts[0].order[selectOrderIdx], car_charts[0].order[selectOrderIdx + 1]] = [
            car_charts[0].order[selectOrderIdx + 1],
            car_charts[0].order[selectOrderIdx],
          ];
          ++selectOrderIdx;
          console.log("car_charts[0].order", car_charts[0].order);
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
        {#each car_charts[0].order as cmi, i}
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
                <DropFile onDataLoaded={(data_array) => (diag_data = data_array)} mode="button" />
              {/if}

              <label><input type="checkbox" bind:checked={live_simu} />Live-Simulation</label>
            </div>
            <div class="flex flex-col">
              <div>
                <p>Type</p>
                <select bind:value={car_chart_version} size={3}>
                  {#each car_charts as cc, i}
                    <option value={i}>{cc.get_info().name}</option>
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
                  is_live={live_simu}
                  }
                />
                <MyBitsPlot
                  chartData={[x_arr, yn_arr[i + 1]]}
                  chartDataVersions={[x_arr_version, yn_arr_version]}
                  labels={[x_labels, car_chart.labels[i + 1]]}
                  {syncKey}
                  {width}
                  {height}
                  is_live={live_simu}
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
                  is_live={live_simu}
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
