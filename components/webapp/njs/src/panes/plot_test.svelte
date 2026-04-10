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
    plugins: [touchZoomPanPlugin(chartData[0].length)],
    scales: {
      x: { time: false },
      y: {}, // Default left scale
      y2: { }, // New independent right scale
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

  if (opts2.y_axis_label == "boolean") {
    options.scales.y2 = { auto: false, range: (u, min, max) => [-.1, 1.1], };
    options.axes[2] = { };
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

  function touchZoomPanPlugin(maxBlocks) {
    return {
      hooks: {
        ready: (u) => {
          let over = u.root.querySelector(".u-over");
          let rect, xMin, xMax, xRange, initialPinchDist, initialX, initialY;

          over.addEventListener(
            "touchstart",
            (e) => {
              rect = over.getBoundingClientRect();
              xMin = u.scales.x.min;
              xMax = u.scales.x.max;
              xRange = xMax - xMin;

              initialX = e.touches[0].clientX;
              initialY = e.touches[0].clientY;

              if (e.touches.length === 2) {
                initialPinchDist = Math.hypot(e.touches[1].clientX - e.touches[0].clientX, e.touches[1].clientY - e.touches[0].clientY);
              }
            },
            { passive: false },
          );

          over.addEventListener(
            "touchmove",
            (e) => {
              let cx = e.touches[0].clientX;
              let cy = e.touches[0].clientY;
              let dx = Math.abs(cx - initialX);
              let dy = Math.abs(cy - initialY);

              // 1-finger panning
              if (e.touches.length === 1 && dx > dy) {
                e.preventDefault();
                let movePct = (cx - initialX) / rect.width;
                let shift = xRange * movePct;

                let newMin = Math.max(0, xMin - shift);
                let newMax = Math.min(maxBlocks, xMax - shift);

                // Adjust if we hit a boundary to maintain zoom level
                if (newMin === 0) newMax = xRange;
                if (newMax === maxBlocks) newMin = maxBlocks - xRange;

                u.setScale("x", { min: newMin, max: newMax });

                // Create a custom event that uPlot's sync logic expects
                let ev = new CustomEvent("uplot-setScale", { bubbles: true });
                // Dispatch it on the chart's interaction layer
                // This "tricks" the sync key into seeing the update
                u.root.querySelector(".u-over").dispatchEvent(ev);

                // Broadcast to all linked charts:
                syncKey.pub("setScale", u, "x", newMin, newMax);
                // This often 'wakes up' the other charts in the sync group
                syncKey.pub("setCursor", u, u.cursor.left, u.cursor.top);
              }
              // 2-finger zooming
              else if (e.touches.length === 2) {
                e.preventDefault();
                let dist = Math.hypot(e.touches[1].clientX - e.touches[0].clientX, e.touches[1].clientY - e.touches[0].clientY);

                let zoomFactor = initialPinchDist / dist;
                let newRange = Math.min(maxBlocks, xRange * zoomFactor);

                let centerX = (e.touches[0].clientX + e.touches[1].clientX) / 2 - rect.left;
                let pctX = centerX / rect.width;

                let newMin = Math.max(0, xMin + (xRange - newRange) * pctX);
                let newMax = Math.min(maxBlocks, newMin + newRange);

                u.setScale("x", { min: newMin, max: newMax });

                // Create a custom event that uPlot's sync logic expects
                let ev = new CustomEvent("uplot-setScale", { bubbles: true });
                // Dispatch it on the chart's interaction layer
                // This "tricks" the sync key into seeing the update
                u.root.querySelector(".u-over").dispatchEvent(ev);
                // Broadcast to all linked charts:
                syncKey.pub("setScale", u, "x", newMin, newMax);
                // This often 'wakes up' the other charts in the sync group
                syncKey.pub("setCursor", u, u.cursor.left, u.cursor.top);
                // ... after u.setScale("x", { min: newMin, max: newMax }) ...

              }
            },
            { passive: false },
          );
        },
      },
    };
  }

  function touchZoomPanPluginOLD() {
    return {
      hooks: {
        ready: (u) => {
          let over = u.root.querySelector(".u-over");
          let rect, xMin, xMax, xRange, initialPinchDist, initialX;

          over.addEventListener(
            "touchstart",
            (e) => {
              rect = over.getBoundingClientRect();
              xMin = u.scales.x.min;
              xMax = u.scales.x.max;
              xRange = xMax - xMin;

              if (e.touches.length === 1) {
                // Setup for Panning
                initialX = e.touches[0].clientX;
              } else if (e.touches.length === 2) {
                // Setup for Pinch Zoom
                initialPinchDist = Math.hypot(e.touches[0].clientX - e.touches[1].clientX, e.touches[0].clientY - e.touches[1].clientY);
              }
            },
            { passive: false },
          );

          over.addEventListener(
            "touchmove",
            (e) => {
              e.preventDefault(); // Stop page scroll

              if (e.touches.length === 1 && initialX != null) {
                // --- PANNING LOGIC ---
                let currentX = e.touches[0].clientX;
                let deltaX = currentX - initialX;
                let movePct = deltaX / rect.width;
                let shift = xRange * movePct;

                u.setScale("x", { min: xMin - shift, max: xMax - shift });
              } else if (e.touches.length === 2 && initialPinchDist > 0) {
                // --- ZOOMING LOGIC ---
                let currentDist = Math.hypot(e.touches[0].clientX - e.touches[1].clientX, e.touches[0].clientY - e.touches[1].clientY);

                let zoomFactor = initialPinchDist / currentDist;
                let newRange = xRange * zoomFactor;

                // Zoom centered on the midpoint between two fingers
                let centerX = (e.touches[0].clientX + e.touches[1].clientX) / 2 - rect.left;
                let pctX = centerX / rect.width;

                let newMin = xMin + (xRange - newRange) * pctX;
                let newMax = newMin + newRange;

                u.setScale("x", { min: newMin, max: newMax });
              }
            },
            { passive: false },
          );

          over.addEventListener("touchend", () => {
            initialPinchDist = 0;
            initialX = null;
          });
        },
      },
    };
  }
</script>

<div bind:this={chartContainer}></div>
