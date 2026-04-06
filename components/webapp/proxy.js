#!/usr/bin/env node

'use strict';

const path = require('path');
const express = require('express');
const httpProxy = require('http-proxy');
const expressWs = require('express-ws')
const livereload = require("livereload");

const args = process.argv.slice(2);
const ipaddr = args[1] || '192.168.1.71';
const port = args[2] || 3002;
const port_lr = 35730;

const proj_dir=path.dirname(path.dirname(__dirname));
const build_dir=proj_dir+"/build/esp32dbg";
const njs_build_dir=args[0] || __dirname + '/njs/build_dev';
const cont_dir=__dirname

const mcu = 'http://' + ipaddr + ':80';
const mcu_ws = 'ws://' + ipaddr + ':80';

console.log("njs_build_dir: ", njs_build_dir);

// open livereload high port and start to watch public directory for changes
const liveReloadServer = livereload.createServer({"port":port_lr});
liveReloadServer.watch(njs_build_dir);


let app = express();
 expressWs(app);
let server = require('http').createServer(app);



// forward any requests to MCU
let proxy = httpProxy.createProxyServer({ ws: true });

app.all("/*.json", (req, res) => {
    proxy.web(req, res, { target: mcu });
});
app.all("/f/cli/*", (req, res) => {
    proxy.web(req, res, { target: mcu });
});

app.ws("/ws", (req, res) => {
    proxy.web(req, res, { target: mcu_ws });
});
server.on('upgrade', function (req, socket, head) { 
    console.log('head', JSON.stringify(head));
     proxy.ws(req, socket, head, { target: mcu_ws });
     });

// serve some files

// static files of MCU HTTP server
app.get("/", (req, res) => {
    res.sendFile(cont_dir + '/wapp_dev.html');
});
app.get("/wapp_dev.html", (req, res) => {
    res.sendFile(cont_dir + '/wapp_dev.html');
});
app.get("/f/js/wapp.js", (req, res) => {
    res.sendFile(njs_build_dir+'/wapp.js');
});
app.get("/f/js/wapp.js.map", (req, res) => {
    res.sendFile(njs_build_dir+'/wapp.js.map');
});
app.get("/f/css/global.css", (req, res) => {
    res.sendFile(cont_dir + '/njs/src/app.css');
});
app.get("/f/css/wapp.css", (req, res) => {
    res.sendFile(njs_build_dir+'/wapp.css');
});
app.get("/f/css/wapp.css.map", (req, res) => {
    res.sendFile(njs_build_dir+'/wapp.css.map');
});
app.get("/src/", (req, res) => {
    res.sendFile(cont_dir + '/njs/src/');
});




server.listen(port);


/*
Local Variables:
compile-command: "jshint proxy.js"
End:
*/
