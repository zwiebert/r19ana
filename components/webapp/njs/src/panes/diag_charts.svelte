<script lang="ts">
  /* eslint-disable svelte/require-each-key */
  import uPlot from "uplot";
  import "uplot/dist/uPlot.min.css";
  import { onMount, untrack } from "svelte";
  import MyPlot from "./plot_test.svelte";
  import MyBitsPlot from "./plot_status_bits.svelte";
  import DropFile from "../components/request-or-drop-file.svelte";
  import { x53b_740_chart_factory } from "../cardiag/charts/x53b-740";
  import { raw_chart_factory } from "../cardiag/charts/raw";
  import type { Icar_chart, ILabel } from "../cardiag/charts/iface";
  import { byte_unstuffing } from "../cardiag/byte_unstuffing";
  import { RenixDestuffer } from "../cardiag/renix_destuffer";
  import { EnableGitHubSamples } from "../store/app_state";
  import { getGithubSamples, fetchBinaryData } from "../sample_data/github_samples";
  //import { DiagDataBuffer } from "../store/diag-data.js";

  let { chart_index = 0, chart_index_viewed = 0 } = $props();
  let error = $state(null);
  let car_charts: Icar_chart[] = [x53b_740_chart_factory(), raw_chart_factory()];
  let car_chart: Icar_chart = $state.raw(car_charts[0]);
  let diag_data: Uint8Array = $state.raw(new Uint8Array(0));
  let nmbGraphs: number = $state(0);

  onMount(() => {});

  $effect(() => {
    let data = diag_data;
    let chart = car_chart;

    if (data && data.length > 0) {
      untrack(() => {
        process_data(data, chart);
      });
      redraw_charts();
    }
  });

  /**
   *  @brief generate chart data from binary input
   *  @param data   input raw data u8 array (with hdr and byte stuffing)
   *  @param append  if true, append to the existing chart data instead of replacing it
   *
   */
  function process_data(data: Uint8Array, car_chart: Icar_chart, append: boolean = false) {
    console.log("process_data:", car_chart.get_info().name);
    //const unstuffing = new byte_unstuffing((arr: Uint8Array, ct: number) => car_chart.process_data_packet(arr, ct));
    const unstuffing = new RenixDestuffer((arr: Uint8Array, ct: number) => car_chart.process_data_packet(arr, ct));
    car_chart.clear_chart_data();
    unstuffing.process_chunk(data);
  }

  // svelte-ignore state_referenced_locally
  const syncKey = uPlot.sync("zoom_group" + chart_index);
  let yn_arr = $state.raw(car_chart.get_chart_data());
  let x_arr = $state.raw([]);
  function redraw_charts() {
    car_chart = car_chart;
    nmbGraphs = car_chart.get_nmb_of_graphs();
    yn_arr = car_chart.get_chart_data();
    x_arr = yn_arr[0].map((_, i) => i);
    console.log("redraw_charts: yn_arr.len", yn_arr[0].length);
  }

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
    </div>
    <div class="flex flex-col">
      <div>
        <p>Type</p>
        <select bind:value={car_chart} onchange={() => (yn_arr = car_chart.get_chart_data())} size={3}>
          {#each car_charts as cc}
            <option value={cc}>{cc.get_info().name}</option>
          {/each}
        </select>
      </div>
      <label>Width: <input type="number" bind:value={width} min={400} max={5000} step={100} /></label>
      <label>Height: <input type="number" bind:value={height} min={100} max={1000} step={25} /></label>
      <button
        onclick={() => {
          process_data(diag_data, car_chart);
        }}>re-plot</button
      >
    </div>
    <div class="flex flex-col text-left">
      {#each Array.from({ length: Math.floor(nmbGraphs / 2) }, (_, index) => index * 2) as i}
        <div>
          <label><input type="checkbox" bind:checked={yn_show[i]} />{car_chart.get_label(i).series_label}, {car_chart.get_label(i + 1).series_label}</label>
          {#if car_chart.get_label(i).axis_label === "bits" || car_chart.get_label(i).axis_label === "raw"}
            <label><input type="checkbox" bind:checked={yn_show_as_bits[i]} />bits</label>
          {/if}
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
      {#if x_arr.length > 0}
        {#each Array.from({ length: Math.floor(nmbGraphs / 2) }, (_, index) => index * 2) as i}
          <div class="text-left">
            <div class="text-center" style="display:{yn_show[i] ? 'block' : 'none'};touch-action: pan-y; width: 100%;">
              {#if yn_show_as_bits[i]}
                <MyBitsPlot chartData={[x_arr, yn_arr[i]]} labels={[x_labels, car_chart.get_label(i)]} {syncKey} {width} {height} } />
                <MyBitsPlot chartData={[x_arr, yn_arr[i + 1]]} labels={[x_labels, car_chart.get_label(i + 1)]} {syncKey} {width} {height} } />
              {:else}
                <MyPlot
                  chartData={[x_arr, yn_arr[i], yn_arr[i + 1]]}
                  labels={[x_labels, car_chart.get_label(i), car_chart.get_label(i + 1)]}
                  {syncKey}
                  {width}
                  {height}
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
