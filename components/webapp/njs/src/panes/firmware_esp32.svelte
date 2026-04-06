<script>
  "use strict";
  import { _ } from "../services/i18n";
  import { req_mcuRestart, DISTRO } from "../app/misc.js";
  import McuFirmwareUpd from "../app/mcu_firmware_upd.svelte";
  import McuFirmwareInfo from "../app/mcu_firmware_info.svelte";

  let fwbtns = [
    { name: "latest master", ota_name: "github-master" },
    { name: "latest beta", ota_name: "github-beta" },
  ];

  if (!DISTRO) {
    fwbtns.push({
      name: "from URL",
      ota_name: "netotaFromURL",
      input: "input",
      input_value: "http://192.168.1.82:8000/netmcu.bin",
    });
  }
</script>

<div class="area">
  <McuFirmwareUpd {fwbtns} chip="" updSecs="14" />
  <McuFirmwareInfo />
  <button type="button" on:click={() => req_mcuRestart()}>{$_('app.restartMcu')}</button>
</div>
