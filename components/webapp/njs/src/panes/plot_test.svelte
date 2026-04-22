<script lang="ts">
  import uPlot from "uplot";
  import { untrack } from "svelte";
  import "uplot/dist/uPlot.min.css";
  import type { ILabel } from "../cardiag/charts/iface";
  import { touchZoomPanPlugin } from "../plotting/uplot/zoom_plugin";

  interface Iprops {
    chartData: number[][];
    chartDataVersions: number[];
    labels: ILabel[3];
    syncKey: objectst;
    width: number;
    height: number;
    live: boolean;
  }
  // Props or state
  let { chartData, chartDataVersions, labels = [{}, {}, {}], syncKey = null, width = 1600, height = 30, is_live = false }: Iprops = $props();

  let chart;
  let chartContainer;
  const create_trigger = $derived(is_live ? 1 : chartDataVersions[0]);
  const setData_trigger = $derived(chartDataVersions[1] + chartDataVersions[2]);
  const setScaleX_trigger = $derived(is_live ? chartDataVersions[0] : 1);

  let options = $state({});
  options = {
    width: width,
    height: height,
    // plugins: [touchZoomPanPlugin(chartData[0].length)],
    scales: {
      x: { time: false },
      y: {}, // Default left scale
      y2: {}, // New independent right scale
    },
    series: [
      { label: labels[0]?.series_label ?? "x" }, // X-axis
      {
        label: labels[1]?.series_label ?? "y",
        stroke: "red",
        scale: "y", // Uses default left scale
      },
      {
        label: labels[2]?.series_label ?? "y2",
        stroke: "blue",
        scale: "y2", // Uses independent right scale
      },
    ],
    axes: [
      {}, // Bottom X-axis
      {
        scale: "y",
        side: 3, // Left side (standard)
        label: labels[1]?.axis_label ?? "",
      },
      {
        scale: "y2",
        side: 1, // Right side (standard for secondary axes)
        label: labels[2]?.axis_label ?? "",
        grid: { show: false }, // Optional: hide grid to avoid clutter
      },
    ],
  };

  if (labels[1]?.axis_label == "boolean") {
    options.scales.y = { auto: false, range: (u, min, max) => [-3.6, 2.5] };
  } else if (labels[1]?.range) {
    options.scales.y = { auto: false, range: (u, min, max) => labels[1].range };
  }
  if (labels[2]?.axis_label == "boolean") {
    options.scales.y2 = { auto: false, range: (u, min, max) => [-4.1, 2.1] };
  } else if (labels[2].range) {
    options.scales.y2 = { auto: false, range: (u, min, max) => labels[2].range };
  }

  if (syncKey)
    options.cursor = {
      // 2. Link the charts using the sync key
      sync: {
        key: syncKey.key,
        setSeries: true, // Optional: syncs series toggling/highlighting
        setScale: [true, false], // Syncs X scale (index 0), ignores Y scale (index 1)
      },
    };

  $effect(() => {
    console.log("create new uplot chart");
    const trigger = create_trigger;
    chart?.destroy();
    chart = new uPlot(
      options,
      untrack(() => chartData),
      chartContainer,
    );
    // Cleanup
    return () => {
      chart?.destroy();
      chart = null;
    };
  });
  $effect(() => {
    //console.log("setData");
    if (is_live) {
      const trigger = setData_trigger;
      chart?.setData(chartData, false);
      requestAnimationFrame(() => {
        chart?.redraw(true);
      });
    }
  });

  $effect(() => {
    console.log("setScale-x");
    const trigger = setScaleX_trigger;
    if (is_live) {
      // 1. Calculate how much the data moved (e.g., how many seconds/indices)
      // Assuming your X-array is sorted, compare the new first element to the old one
      const newDataStart = chartData[0][0];
      const oldDataStart = chart.data[0][0];
      const delta = newDataStart - oldDataStart;

      requestAnimationFrame(() => {
        if (chart) {
          // 3. Shift the current view by the same delta
          // This keeps the "zoom level" (the width) the same,
          // but slides the "window" along with the data.
          chart.setScale("x", {
            min: chart.scales.x.min + delta,
            max: chart.scales.x.max + delta,
          });
        }
      });
    }
  });

  $effect(() => {
    console.log("setSize");
    if (width && height) chart?.setSize({ width, height });
  });
</script>

<!-- This container stays the size of the screen -->
<div class="w-full overflow-x-auto">
  <!-- This container holds the actual uPlot and resets alignment -->
  <div class="text-left flex justify-start items-start">
    <div bind:this={chartContainer}></div>
  </div>
</div>
