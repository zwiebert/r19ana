<script>
  "use strict";
  import { _ } from "../services/i18n";
  import {
    ZoneCount,
    ZoneNames,
    ZoneDisabled,
  } from "../store/zones";
  import Zone from "../app/zone.svelte";
  import * as httpFetch from "../app/fetch.js";
  import { onMount} from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_NAMES | httpFetch.FETCH_ZONE_DURATIONS | httpFetch.FETCH_ZONE_REMAINING_DURATIONS );
  });



function postZoneNames() {
  let netmcu = {to:"netmcu"};
  let kvs = {zn:'?'};
  netmcu.kvs = kvs;

  for (let i=0; i < $ZoneCount; ++i) {
    let sfx = i.toString();
    let key = 'zn'+sfx;
    let new_name =  zoneNames[i];
    let old_name = $ZoneNames[i];
    if (old_name !== new_name) {
      kvs[key] = new_name;
    }
  }

  console.log(JSON.stringify(netmcu));
  let url = '/cmd.json';
  console.log("url: "+url);
  httpFetch.http_postRequest(url, netmcu);
}


  $: zoneNames = [...$ZoneNames];

</script>

<table class="bg-transparent">
  <tr>
    <th>{$_('app.zone')}</th>
    <th>{$_('app.duration')}</th>
    <th>{$_('app.run')}</th>
    <th>{$_('app.remaining')}</th>
  </tr>
  {#each { length: $ZoneCount } as _, i}
  {#if ($ZoneDisabled & (1<<i)) === 0}
    <tr>
      <Zone zoneIdx={i} {zoneNames} />
    </tr>
  {/if}
    {/each}

</table>

<br>