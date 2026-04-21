<script lang="ts">
  import uPlot from "uplot";
  import "uplot/dist/uPlot.min.css";
  import type { ILabel } from "../cardiag/charts/iface";
  import { touchZoomPanPlugin } from "../plotting/uplot/zoom_plugin";

  interface Iprops {
    chartData: number[][];
    labels: ILabel[3];
    syncKey: objectst;
    width: number;
    height: number;
  }
  // Props or state
  let { chartData, labels = [{}, {}, {}], syncKey = null, width = 1600, height = 300 }: Iprops = $props();
  let chart;

  let chartContainer;

  /*
  const options = {
    width: 600,
    height: 400,
    scales: { x: { time: false } },
    series: [{}, { stroke: "red" }, { stroke: "blue" }]
  };
  */

  let options = {};

  $effect(() => {
    let opts = $state([
      Object.assign({ series_label: "x", axis_label: "x" }, labels[0]),
      Object.assign({ series_label: "Data Set 1", axis_label: "Left Axis" }, labels[1]),
      Object.assign({ series_label: "Data Set 2", axis_label: "Right Axis" }, labels[2]),
    ]);
    options = {
      width: width,
      height: height,
      plugins: [touchZoomPanPlugin(chartData[0].length)],
      scales: {
        x: { time: false },
        y: {}, // Default left scale
        y2: {}, // New independent right scale
      },
      series: [
        { label: opts[0].series_label }, // X-axis
        {
          label: opts[1].series_label,
          stroke: "red",
          scale: "y", // Uses default left scale
        },
        {
          label: opts[2].series_label,
          stroke: "blue",
          scale: "y2", // Uses independent right scale
        },
      ],
      axes: [
        {}, // Bottom X-axis
        {
          scale: "y",
          side: 3, // Left side (standard)
          label: opts[1].axis_label,
        },
        {
          scale: "y2",
          side: 1, // Right side (standard for secondary axes)
          label: opts[2].axis_label,
          grid: { show: false }, // Optional: hide grid to avoid clutter
        },
      ],
    };

    if (opts[1].axis_label == "boolean") {
      options.scales.y = { auto: false, range: (u, min, max) => [-3.6, 2.5] };
    } else if (opts[1].range) {
      options.scales.y = { auto: false, range: (u, min, max) => opts[1].range };
    }
    if (opts[2].axis_label == "boolean") {
      options.scales.y2 = { auto: false, range: (u, min, max) => [-4.1, 2.1] };
    } else if (opts[2].range) {
      options.scales.y2 = { auto: false, range: (u, min, max) => opts[2].range };
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

    if (!chart) {
      chart = new uPlot(options, chartData, chartContainer);
    } else {
      chart.destroy();
      chart = new uPlot(options, chartData, chartContainer);
    }
    // Cleanup
    return () => {
      chart?.destroy();
      chart = null;
    };
  });

  /*
  $effect(() => {
    if (chart) {
      chart.setData(chartData); // XXX: this isnot useful. touchZoomPlugin depends on chartData.length and is part of options
    }
  });
  */
</script>

<!-- This container stays the size of the screen -->
<div class="w-full overflow-x-auto">
  <!-- This container holds the actual uPlot and resets alignment -->
  <div class="text-left flex justify-start items-start">
    <div bind:this={chartContainer}></div>
  </div>
</div>
