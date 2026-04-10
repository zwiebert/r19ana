#!/usr/bin/env node

'use strict';

const path = require('path');
const express = require('express');
const httpProxy = require('http-proxy');
const expressWs = require('express-ws')
const { createProxyMiddleware, fixRequestBody } = require('http-proxy-middleware');


const args = process.argv.slice(2);
const ipaddr = args[1] || '192.168.1.96';
const port = args[2] || 3004;
const ipaddr_vite = args[3] || 'localhost';

const proj_dir=path.dirname(path.dirname(__dirname));
const build_dir=proj_dir+"/build/esp32dbg";
const njs_build_dir=args[0] || __dirname + '/njs/build_dev';
const cont_dir=__dirname

const mcu = 'http://' + ipaddr + ':80';
const mcu_ws = 'ws://' + ipaddr + ':80';
const vite = 'http://' + ipaddr_vite + ':3003';


let app = express();
console.log("test is console.log works");

// 1. ESP32 Proxy - Only for specific patterns (e.g., .json or specific names)
app.use(createProxyMiddleware({

    XXpathFilter: (path, req) => {
    console.log("path:", path);
    if (path == "/cmd.json") {
     console.log("Match /cmd.json for esp32");
     return true;
   }

 return false;
 }, pathFilter:['/*.json', '/f/**'], // Add your ESP32 paths here. Don't mix strings with and without wildcards
    target: mcu,
    changeOrigin: true,
    on: {
        XproxyReq: (proxyReq, req, res) => {
            console.log(`[ESP32] Forwarding ${req.method} to: ${proxyReq.path}`);
            
            // Clean headers for ESP32
            proxyReq.removeHeader('origin');
            proxyReq.removeHeader('referer');
// 2. STRIP EVERYTHING EXCEPT THE ESSENTIALS
      // ESP32 servers often 405 if they see 'Sec-' or 'User-Agent' headers they don't like
      const headersToKeep = ['content-type', 'content-length', 'host', 'connection'];
      
      Object.keys(req.headers).forEach(header => {
        if (!headersToKeep.includes(header.toLowerCase())) {
          proxyReq.removeHeader(header);
        }
      });

         proxyReq.setHeader('Connection', 'close');

        if(req.method == 'POST')
          fixRequestBody(proxyReq, req);
      }
    }
}));

// 2. Vite Proxy - Catch-all for everything else (UI, assets, etc.)
app.use(createProxyMiddleware({
    target: vite, // Standard Vite port
    changeOrigin: true,
    on: {
        XXXproxyReq: (proxyReq, req, res) => {
            console.log(`[Vite] Forwarding ${req.method} to: ${proxyReq.path}`);
        }
    }
    // No pathFilter means it catches whatever the ESP32 proxy ignored
}));


app.listen(port);


/*
Local Variables:
compile-command: "jshint proxy.js"
End:
*/
