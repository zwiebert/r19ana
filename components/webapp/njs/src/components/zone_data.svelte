<script>
  import { _ } from "../services/i18n";
  import {
    ZoneLPHs, ZoneDataMsg
  } from "../store/zones";
  import {
    Z, ZoneLPH,
  } from "../store/curr_zone";
  import * as httpFetch from "../app/fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_LPHS);
  });

  function postZoneData2() {
    let msg = {config:{}};
    msg.config["lph"+$Z] = lph;
    console.log(msg);
    httpFetch.sendRv(msg);
    
    msg = {};
    msg["lph"+$Z] = "?";
    setTimeout(() => httpFetch.sendKvs(msg), 1000);
  }

  $: lphx = $ZoneLPHs[$Z];
  $: lph = lphx;

</script>


<table>
  <tr><th>LPH</th><td><input type="number" step="1" bind:value={lph}></td></tr>
</table>

<button on:click={postZoneData2}>{$_('app.save')}</button>



