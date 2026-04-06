<script>
  import uPlot from 'uplot';
  import 'uplot/dist/uPlot.min.css';

  // Props or state
  let { array1, array2 } = $props();

  let chart;
  let chartContainer;

  // Process data for uPlot format
  const chartData = $derived([
    array1.map((_, i) => i),
    array1,
    array2
  ]);
/*
  const options = {
    width: 600,
    height: 400,
    scales: { x: { time: false } },
    series: [{}, { stroke: "red" }, { stroke: "blue" }]
  };
  */
 const options = {
  width: 600,
  height: 400,
  scales: {
    x: { time: false },
    y: {},        // Default left scale
    y2: {}       // New independent right scale
  },
  series: [
    {}, // X-axis
    {
      label: "Dataset 1",
      stroke: "red",
      scale: "y", // Uses default left scale
    },
    {
      label: "Dataset 2",
      stroke: "blue",
      scale: "y2", // Uses independent right scale
    }
  ],
  axes: [
    {}, // Bottom X-axis
    {
      scale: "y",
      side: 3, // Left side (standard)
      label: "Left Axis"
    },
    {
      scale: "y2",
      side: 1, // Right side (standard for secondary axes)
      label: "Right Axis",
      grid: { show: false } // Optional: hide grid to avoid clutter
    }
  ]
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
