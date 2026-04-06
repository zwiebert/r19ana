<script>
  "use strict";

  import { ZoneDurations, ZoneRemainingSeconds } from "../store/zones";
  import * as httpFetch from "../app/fetch.js";
  import { formatSecondsToMMSS } from "../app/misc.js";

  export let zoneIdx;
  export let zoneNames;

  $: zoneName = zoneNames[zoneIdx] || "";

  $: duration = ($ZoneDurations[zoneIdx] / 60.0).toFixed(2) || 0;
  $: remaining_mmss = formatSecondsToMMSS($ZoneRemainingSeconds[zoneIdx]);
  $: checked = duration > 0.01;

  function onClickRun() {
    let cmd = { timer: {} };
    cmd.timer.vn = zoneIdx;
    if (document.getElementById("zrun" + zoneIdx).checked) cmd.timer.d1 = (document.getElementById("dur" + zoneIdx).valueAsNumber * 60).toFixed(0);
    httpFetch.sendRv(cmd);
  }
</script>

<th class="text-left">{zoneIdx} {zoneName}</th>
<td>
  <input class="w-16 text-right" type="number" value={duration} id="dur{zoneIdx}" />
</td>
<td>
  <input type="checkbox" {checked} on:click={onClickRun} id="zrun{zoneIdx}" />
</td>
<td>{remaining_mmss}</td>
