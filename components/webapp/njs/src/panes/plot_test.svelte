<script>
  import uPlot from "uplot";
  import "uplot/dist/uPlot.min.css";

  // Props or state
  let { array1, array2, labels1 = {}, labels2 = {}, syncKey = null } = $props();
  let chart;
  let opts1 = Object.assign({ y_series_label: "Data Set 1", y_axis_label: "Left Axis" }, labels1);
  let opts2 = Object.assign({ y_series_label: "Data Set 2", y_axis_label: "Right Axis" }, labels2);
  let chartContainer;

  // Process data for uPlot format
  const chartData = $derived([array1.map((_, i) => i), array1, array2]);
  /*
  const options = {
    width: 600,
    height: 400,
    scales: { x: { time: false } },
    series: [{}, { stroke: "red" }, { stroke: "blue" }]
  };
  */
  let options = {
    width: 600,
    height: 400,
    scales: {
      x: { time: false },
      y: {}, // Default left scale
      y2: {}, // New independent right scale
    },
    series: [
      {}, // X-axis
      {
        label: opts1.y_series_label,
        stroke: "red",
        scale: "y", // Uses default left scale
      },
      {
        label: opts2.y_series_label,
        stroke: "blue",
        scale: "y2", // Uses independent right scale
      },
    ],
    axes: [
      {}, // Bottom X-axis
      {
        scale: "y",
        side: 3, // Left side (standard)
        label: opts1.y_axis_label,
      },
      {
        scale: "y2",
        side: 1, // Right side (standard for secondary axes)
        label: opts2.y_axis_label,
        grid: { show: false }, // Optional: hide grid to avoid clutter
      },
    ],
  };

  if (syncKey)
    options.cursor = {
      // 2. Link the charts using the sync key
      sync: {
        key: syncKey.key,
        setSeries: true, // Optional: syncs series toggling/highlighting
      },
    };

  $effect(() => {
    // Initialize
    if (!chart) {
      chart = new uPlot(options, chartData, chartContainer);
    } else {
      // Efficiently update data without destroying the chart
      chart.setData(chartData);
    }

    // Cleanup
    return () => {
      chart?.destroy();
      chart = null;
    };
  });
</script>

<div bind:this={chartContainer}></div>
