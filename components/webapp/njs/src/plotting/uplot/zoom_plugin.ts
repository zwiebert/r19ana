/* eslint-disable @typescript-eslint/no-unsafe-assignment */
/* eslint-disable @typescript-eslint/no-unsafe-member-access */
/* eslint-disable @typescript-eslint/no-unsafe-call */

export function touchZoomPanPlugin(maxBlocks: number) {
    return {
      hooks: {
        ready: (u) => {
          const over = u.root.querySelector(".u-over");
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
              const cx = e.touches[0].clientX;
              const cy = e.touches[0].clientY;
              const dx = Math.abs(cx - initialX);
              const dy = Math.abs(cy - initialY);

              // 1-finger panning
              if (e.touches.length === 1 && dx > dy) {
                e.preventDefault();
                const movePct = (cx - initialX) / rect.width;
                const shift = xRange * movePct;

                let newMin = Math.max(0, xMin - shift);
                let newMax = Math.min(maxBlocks, xMax - shift);

                // Adjust if we hit a boundary to maintain zoom level
                if (newMin === 0) newMax = xRange;
                if (newMax === maxBlocks) newMin = maxBlocks - xRange;

                u.setScale("x", { min: newMin, max: newMax });

                // Create a custom event that uPlot's sync logic expects
                const ev = new CustomEvent("uplot-setScale", { bubbles: true });
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
                const dist = Math.hypot(e.touches[1].clientX - e.touches[0].clientX, e.touches[1].clientY - e.touches[0].clientY);

                const zoomFactor = initialPinchDist / dist;
                const newRange = Math.min(maxBlocks, xRange * zoomFactor);

                const centerX = (e.touches[0].clientX + e.touches[1].clientX) / 2 - rect.left;
                const pctX = centerX / rect.width;

                const newMin = Math.max(0, xMin + (xRange - newRange) * pctX);
                const newMax = Math.min(maxBlocks, newMin + newRange);

                u.setScale("x", { min: newMin, max: newMax });

                // Create a custom event that uPlot's sync logic expects
                const ev = new CustomEvent("uplot-setScale", { bubbles: true });
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