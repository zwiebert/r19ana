<script lang="ts">
  import uPlot from "uplot";
  import { untrack } from "svelte";
  import "uplot/dist/uPlot.min.css";
  import type { ILabel } from "../cardiag/charts/iface";
  import { touchZoomPanPlugin } from "../plotting/uplot/zoom_plugin";

  interface Iprops {
    chartData: number[][];
    chartDataVersions: objects[];
    labels: ILabel[3];
    syncKey: objectst;
    width: number;
    height: number;
    is_live: boolean;
  }
  // Props or state
  const { chartData, chartDataVersions, labels = [{}, {}, {}], syncKey = null, width = 1600, height = 30, is_live = false }: Iprops = $props();

  let chart;

  let chartContainer;
  const bool_val_0 = "0";
  const bool_val_1 = "1";
  const x_arr_version = $derived(chartDataVersions[0]);
  const y_arr_version = $derived(chartDataVersions[1]);
  const create_trigger = $derived(is_live ? {} : x_arr_version);
  const setData_trigger = $derived(y_arr_version);
  const setScaleX_trigger = $derived(is_live ? x_arr_version : {});

  const getbit = (n: number, pos: number) => (n >>> pos) & 1;

  // create stacked bit values
  let bitChartData = Array.from({ length: 9 }, () => []);
  $effect(() => {
    const trigger = setData_trigger;
    bitChartData = Array.from({ length: 9 }, () => []);
    bitChartData[0] = chartData[0];
    for (let i = 0; i < chartData[1].length; ++i) {
      const val = chartData[1][i];
      for (let k = 0; k < 8; ++k) {
        bitChartData[k + 1].push(getbit(val, k) * 0.8 + k - 0.4);
      }
    }
  });

  let options = $derived({
    width: width,
    height: height,
    plugins: [touchZoomPanPlugin(bitChartData[0].length)],
    series: [
      {
        label: labels[0]?.series_label ?? "x",

        value: !is_live
          ? (u, v) => {
              if (v == null) return "-";
              const s = v * 0.015;
              const min = Math.floor(s / 60)
                .toString()
                .padStart(2, "0");
              const sec = Math.floor(s % 60)
                .toString()
                .padStart(2, "0");
              const ms = Math.floor((s % 1) * 1000)
                .toString()
                .padStart(3, "0");
              return `${v}, T: ${min}:${sec}.${ms}`;
            }
          : (u, v) => {
              if (v == null) return "-";
              const d = new Date(v * 1000);
              const timeStr = d.toLocaleTimeString("en-GB", { hour12: false });
              const ms = d.getMilliseconds().toString().padStart(3, "0");
              return `${timeStr}.${ms}`;
            },
      }, // X-axis
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
        points: { show: false },
        pxAlign: 0,
      },
      {
        label: "Bit 1",
        stroke: "blue",
        scale: "bits",
        value: (self, v) => {
          if (v == null) return "--";
          return v > 1 ? bool_val_1 : bool_val_0;
        },
        points: { show: false },
        pxAlign: 0,
      },

      {
        label: "Bit 2",
        stroke: "green",
        scale: "bits",
        value: (self, v) => {
          if (v == null) return "--";
          return v > 2 ? bool_val_1 : bool_val_0;
        },
        points: { show: false },
        pxAlign: 0,
      },
      {
        label: "Bit 3",
        stroke: "black",
        scale: "bits",
        value: (self, v) => {
          if (v == null) return "--";
          return v > 3 ? bool_val_1 : bool_val_0;
        },
        points: { show: false },
        pxAlign: 0,
      },
      {
        label: "Bit 4",
        stroke: "red",
        scale: "bits",
        value: (self, v) => {
          if (v == null) return "--";
          return v > 4 ? bool_val_1 : bool_val_0;
        },
        points: { show: false },
        pxAlign: 0,
      },
      {
        label: "Bit 5",
        stroke: "blue",
        scale: "bits",
        value: (self, v) => {
          if (v == null) return "--";
          return v > 5 ? bool_val_1 : bool_val_0;
        },
        points: { show: false },
        pxAlign: 0,
      },
      {
        label: "Bit 6",
        stroke: "green",
        scale: "bits",
        value: (self, v) => {
          if (v == null) return "--";
          return v > 6 ? bool_val_1 : bool_val_0;
        },
        points: { show: false },
        pxAlign: 0,
      },
      {
        label: "Bit 7",
        stroke: "black",
        scale: "bits",
        value: (self, v) => {
          if (v == null) return "--";
          return v > 7 ? bool_val_1 : bool_val_0;
        },
        points: { show: false },
        pxAlign: 0,
      },
    ],
    scales: {
      x: { time: is_live },
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
        label: labels[1]?.series_label ?? "",
        incrs: [1],
        space: 0,
        values: (self, ticks) => ticks.filter((v) => Number.isInteger(v)),
      },
      {
        scale: "bits",
        grid: { show: false },
        side: 3, // Left side (standard)
        label: labels[1]?.series_label ?? "",
        incrs: [1],
        space: 0,
        values: (self, ticks) => ticks.filter((v) => Number.isInteger(v)),
      },
    ],
    cursor: {
      sync: syncKey
        ? {
            key: syncKey.key,
            setSeries: true, // Optional: syncs series toggling/highlighting
            setScale: [true, false], // Syncs X scale (index 0), ignores Y scale (index 1)
          }
        : {},
    },
  });

  $effect(() => {
    console.log("create new uplot chart");
    const trigger = create_trigger;
    chart?.destroy();
    chart = new uPlot(
      options,
      untrack(() => bitChartData),
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
      chart?.setData(bitChartData, false);
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
      const newDataStart = bitChartData[0][0];
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
