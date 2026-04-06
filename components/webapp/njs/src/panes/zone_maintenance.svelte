<script>
  import { _ } from "../services/i18n";
  import { Z, ZoneDurationMmss, ZoneRemainingMmss } from "../store/curr_zone";

  import * as httpFetch from "../app/fetch.js";
  import { onMount } from "svelte";
  import SelectZone from "../components/select_zone.svelte";
  import RvStatus from "../components/rv_status.svelte";
  import ZoneData from "../components/zone_data.svelte";


  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_NAMES | httpFetch.FETCH_ZONE_DURATIONS | httpFetch.FETCH_ZONE_REMAINING_DURATIONS);
  });

  function cmdDuration(args) {
    let cmd = {};
    cmd.timer = args;
    httpFetch.sendRv(cmd);
  }

  function onClickRun() {
    cmdDuration({ vn: $Z, d1: 60, ir: true, ip: true });
  }
  function onClickStop() {
    cmdDuration({ vn: $Z });
  }
</script>


<div class="main-area">
<h3>Test Zones</h3>
  <div class="text-center">
    <SelectZone />
  </div>

  <div class="area text-center">
  <h4>One Minute Test</h4>
    <button class="text-xl p-8 m-2" on:click={onClickRun}>Start</button>
    <button class="text-xl p-8 m-2" on:click={onClickStop}>Stop</button>
    <hr />
    <p>{$_('app.duration')}: {$ZoneDurationMmss} {$_('app.remaining')}: {$ZoneRemainingMmss}</p>
    <hr />
    <RvStatus />
  </div>
  
  <div class="area">
    <ZoneData />
  </div>

</div>
