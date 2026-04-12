import { defineConfig } from "vite";
import { svelte } from "@sveltejs/vite-plugin-svelte";
import tailwindcss from "@tailwindcss/vite";

// --- CHANGE THIS IP AS NEEDED ---
const ESP_IP = "192.168.1.69";
const build_directory = process.env.BUILD_DIR || "/tmp/rv/njs/build";
const isProduction = process.env.NODE_ENV === "production";

const proxyTarget = `http://${ESP_IP}`;
const wsTarget = `ws://${ESP_IP}`;

export default defineConfig(({ mode }) => {
  const isGithub = mode === "github";
  return {
    define: {
      isProduction: isProduction,
      isDistro: process.env.DISTRO === "1",
    },
    plugins: [svelte(), tailwindcss()],
    base: isGithub ? "/r19xr25-esp32/" : isProduction ? "/f/" : "/",

    build: {
      outDir: isGithub ? "dist-gh" : build_directory,
      sourcemap: true,
      cssCodeSplit: isGithub, // Allow splitting on GitHub for performance

      rollupOptions: {
        output: isGithub
          ? {
              // Standard GitHub Pages settings (hashed)
              entryFileNames: "assets/[name]-[hash].js",
              chunkFileNames: "assets/[name]-[hash].js",
              assetFileNames: "assets/[name]-[hash][extname]",
            }
          : {
              // ESP32 settings (fixed names, no subfolders)
              entryFileNames: "wapp.js",
              assetFileNames: "wapp.[ext]",
              format: "iife",
              name: "wapp",
            },
      },
    },
    server: {
      port: 3003,
      watch: {
      },
      proxy: {
        // JSON files and /f/ paths
        "^(.*)\\.json$": {
          target: proxyTarget,
          changeOrigin: true,
          agent: false,
          timeout: 5000, // 5 seconds to wait for a response
          proxyTimeout: 5000, // 5 seconds for the proxy itself
          headers: {
            // Tell the ESP32 to close the socket immediately after sending data
            Connection: "close",
          },
        },
        "/f": {
          target: proxyTarget,
          changeOrigin: true,
          timeout: 5000, // 5 seconds to wait for a response
          proxyTimeout: 5000, // 5 seconds for the proxy itself
          agent: false,
          headers: {
            // Tell the ESP32 to close the socket immediately after sending data
            Connection: "close",
          },
        },

        // WebSockets
        "/ws": {
          target: wsTarget,
          ws: true,
          rewrite: (path) => path, // ensures /ws stays /ws
        },
      },
    },
  };
});
