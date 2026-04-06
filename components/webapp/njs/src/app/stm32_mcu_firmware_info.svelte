<script>
  "use strict";
  import { Stm32McuFirmwareVersion } from "../store/mcu_firmware.js";
  import * as httpFetch from "../app/fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    setTimeout(httpFetch.http_fetchByMask, 1000, httpFetch.FETCH_RV_VERSION);
  });
</script>

<ul>
  <li>Version: {$Stm32McuFirmwareVersion}</li>
</ul>
