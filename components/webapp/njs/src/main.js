"use strict";

import App from "./app/main.svelte";
import * as connWs from "./net/conn_ws.js";
import { mount } from "svelte";

export default function init() {
  generate_html();
  if (import.meta.env.MODE === "mcu")
    setTimeout(() => {
      connWs.websocket();
    }, 1000);
}

function generate_html() {
  // eslint-disable-next-line no-unused-vars
  const app = mount(App, { target: document.getElementById("app"), props: {} });
}

init();
