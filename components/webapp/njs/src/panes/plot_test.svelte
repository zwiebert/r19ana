<script lang="ts">
  import uPlot from "uplot";
  import { untrack } from "svelte";
  import "uplot/dist/uPlot.min.css";
  import type { ILabel } from "../cardiag/charts/iface";
  import { touchZoomPanPlugin } from "../plotting/uplot/zoom_plugin";

  interface Iprops {
    xData: number[];
    yData: number[];
    y2Data: number[];
    xDataVer: {};
    yDataVer: {};
    y2DataVer: {};
    labels: ILabel[3];
    syncKey: {};
    width: number;
    height: number;
    is_live: boolean;
  }
  // Props or state
  const {
    xData,
    xDataVer,
    yData,
    yDataVer,
    y2Data,
    y2DataVer,
    labels = [{}, {}, {}],
    syncKey = null,
    width = 1600,
    height = 30,
    is_live = false,
  }: Iprops = $props();

  let chart;
  let chartContainer;
  const create_trigger = $derived(is_live ? {} : xDataVer);
  const setData_trigger = $derived({ yDataVer, y2DataVer });
  const setScaleX_trigger = $derived(is_live ? xDataVer : {});

  let options = $derived({
    width: width,
    height: height,
    // plugins: [touchZoomPanPlugin(xData.length)],
    scales: {
      x: { time: is_live },
      y:
        labels[1]?.axis_label == "boolean"
          ? { auto: false, range: (u, min, max) => [-3.6, 2.5] }
          : labels[1]?.range
            ? { auto: false, range: (u, min, max) => labels[1].range }
            : {}, // Default left scale
      y2:
        labels[2]?.axis_label == "boolean"
          ? { auto: false, range: (u, min, max) => [-4.1, 2.1] }
          : labels[2]?.range
            ? { auto: false, range: (u, min, max) => labels[2].range }
            : {}, // Default right scale
    },
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
        label: labels[1]?.series_label ?? "y",
        stroke: "red",
        scale: "y", // Uses default left scale

        // 1. POINTS.SHOW
        // Set to false to stop uPlot from drawing 200k circles
        points: { show: false },

        // 2. PATHBUILDER
        // Usually left alone, but you can provide custom line logic here.
        // For performance, uPlot's default is already highly optimized.
        // pathBuilder: uPlot.paths.stepped({align: 1}),

        // 3. PXALIGN
        // 1 (default) = align to physical screen pixels (crisp but slower)
        // 0 = no alignment (much faster for zooming 200k points)
        pxAlign: 0,
      },
      {
        label: labels[2]?.series_label ?? "y2",
        stroke: "blue",
        scale: "y2", // Uses independent right scale
        // 1. POINTS.SHOW
        // Set to false to stop uPlot from drawing 200k circles
        points: { show: false },

        // 2. PATHBUILDER
        // Usually left alone, but you can provide custom line logic here.
        // For performance, uPlot's default is already highly optimized.
        // pathBuilder: uPlot.paths.stepped({align: 1}),

        // 3. PXALIGN
        // 1 (default) = align to physical screen pixels (crisp but slower)
        // 0 = no alignment (much faster for zooming 200k points)
        pxAlign: 0,
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
    cursor: {
      sync: syncKey
        ? {
            key: syncKey.key,
            setSeries: false, // Optional: syncs series toggling/highlighting
            setScale: [true, false], // Syncs X scale (index 0), ignores Y scale (index 1)
          }
        : undefined,
    },
  });

  $effect(() => {
    console.log("create new uplot chart");
    const trigger = create_trigger;
    chart?.destroy();
    chart = new uPlot(
      options,
      untrack(() => [xData, yData, y2Data]),
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
      chart?.setData(
        untrack(() => [xData, yData, y2Data]),
        false,
      );
      requestAnimationFrame(() => {
        chart?.redraw(true);
      });
    }
  });
  $effect(() => {
    console.log("setScale-x");
    const trigger = setScaleX_trigger;
    untrack(() => {
      if (is_live) {
        // 1. Calculate how much the data moved (e.g., how many seconds/indices)
        // Assuming your X-array is sorted, compare the new first element to the old one
        const newDataStart = xData[0];
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
