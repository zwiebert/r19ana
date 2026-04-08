import { defineConfig } from 'vite';
import { svelte } from '@sveltejs/vite-plugin-svelte';

const build_directory = process.env.BUILD_DIR || "/tmp/rv/njs/build";


// --- CHANGE THIS IP AS NEEDED ---
const ESP_IP = '192.168.1.96'; 

const proxyTarget = `http://${ESP_IP}`;
const wsTarget = `ws://${ESP_IP}`;
export default defineConfig(({ mode }) => { 
  const isGithub = mode === 'github';
 return {
   define: {
    isProduction : process.env.NODE_ENV === "production",
    isDistro : process.env.DISTRO === "1",
  },
   plugins: [svelte()],
   base: isGithub ? '/your-repo-name/' : './', 
    
    build: {
      outDir: isGithub ? 'dist-gh' : build_directory,
      sourcemap: true,
      cssCodeSplit: isGithub, // Allow splitting on GitHub for performance
      
      rollupOptions: {
        output: isGithub 
          ? {
              // Standard GitHub Pages settings (hashed)
              entryFileNames: 'assets/[name]-[hash].js',
              chunkFileNames: 'assets/[name]-[hash].js',
              assetFileNames: 'assets/[name]-[hash][extname]',
            }
          : {
              // ESP32 settings (fixed names, no subfolders)
              entryFileNames: 'wapp.js',
              assetFileNames: 'wapp.[ext]',
              format: 'iife',
              name: 'wapp',
            },
      },
    },
  server: {
    port: 3003,
    proxy: {
      // JSON files and /f/ paths
      '^(.*)\\.json$': { target: proxyTarget, changeOrigin: true },
      '/f': { target: proxyTarget, changeOrigin: true },
      
      // WebSockets
      '/ws': { 
        target: wsTarget, 
        ws: true,
        rewrite: (path) => path // ensures /ws stays /ws
      },
    },
  },
}});

