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
  const bool_val_0 = "0";
  const bool_val_1 = "1";

  const bitChartData = Array.from({ length: 9 }, () => []);
  $effect(() => {
    for (let i = 0; i < chartData[0].length; ++i) {
      bitChartData[0].push(chartData[0][i]);
      const val = chartData[1][i];
      for (let k = 0; k < 8; ++k) {
        bitChartData[k + 1].push(((val >>> k) & 1) * 0.8 + k - 0.4);
      }
    }
  });

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
      series: [
        { label: opts[0].series_label }, // X-axis
        {
          label: "Bit 0",
          stroke: "red",
          scale: "bits",
          // 'v' is the raw value (e.g., 0.4 or -0.4)
          value: (self, v) => {
            if (v == null) return "--";
            // If value is greater than the integer 'n', it's a 1
            // For Bit 0, n=0. If v > 0, it's HIGH.
            return v > 0 ? bool_val_1 : bool_val_0;
          },
        },
        {
          label: "Bit 1",
          stroke: "blue",
          scale: "bits",
          value: (self, v) => {
            if (v == null) return "--";
            return v > 1 ? bool_val_1 : bool_val_0;
          },
        },

        {
          label: "Bit 2",
          stroke: "green",
          scale: "bits",
          value: (self, v) => {
            if (v == null) return "--";
            return v > 2 ? bool_val_1 : bool_val_0;
          },
        },
        {
          label: "Bit 3",
          stroke: "black",
          scale: "bits",
          value: (self, v) => {
            if (v == null) return "--";
            return v > 3 ? bool_val_1 : bool_val_0;
          },
        },
        {
          label: "Bit 4",
          stroke: "red",
          scale: "bits",
          value: (self, v) => {
            if (v == null) return "--";
            return v > 4 ? bool_val_1 : bool_val_0;
          },
        },
        {
          label: "Bit 5",
          stroke: "blue",
          scale: "bits",
          value: (self, v) => {
            if (v == null) return "--";
            return v > 5 ? bool_val_1 : bool_val_0;
          },
        },
        {
          label: "Bit 6",
          stroke: "green",
          scale: "bits",
          value: (self, v) => {
            if (v == null) return "--";
            return v > 6 ? bool_val_1 : bool_val_0;
          },
        },
        {
          label: "Bit 7",
          stroke: "black",
          scale: "bits",
          value: (self, v) => {
            if (v == null) return "--";
            return v > 7 ? bool_val_1 : bool_val_0;
          },
        },
      ],
      scales: {
        x: { time: false },
        bits: {
          auto: true,
          range: [-0.5, 7.5], // Adjust based on your offsets
        },
      },
      axes: [
        {}, // Bottom X-axis
        {
          scale: "bits",
          grid: { show: false },
          side: 1, // Left side (standard)
          label: opts[1].series_label,
          incrs: [1],
          space: 0,
          values: (self, ticks) => ticks.filter((v) => Number.isInteger(v)),
        },
        {
          scale: "bits",
          grid: { show: false },
          side: 3, // Left side (standard)
          label: opts[1].series_label,
          incrs: [1],
          space: 0,
          values: (self, ticks) => ticks.filter((v) => Number.isInteger(v)),
        },
      ],
    };

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
      chart = new uPlot(options, bitChartData, chartContainer);
    } else {
      chart.destroy();
      chart = new uPlot(options, bitChartData, chartContainer);
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
