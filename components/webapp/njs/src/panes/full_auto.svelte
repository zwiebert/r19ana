<script>
  import { _ } from "../services/i18n";
  import { Z, ZoneDurationMmss, ZoneRemainingMmss } from "../store/curr_zone";
  import { McuTime  } from "../store/mcu_firmware";

  import * as httpFetch from "../app/fetch.js";
  import * as httpResp from "../app/http_resp.js";
  import { onMount } from "svelte";
  import SelectZone from "../components/select_zone.svelte";
  import RvStatus from "../components/rv_status.svelte";
  import ZoneData from "../components/zone_data.svelte";
  import RvTimer from "../components/rv_timer.svelte";
  import { ZoneCount, ZoneNames, ZonesAuto, PastWeatherData, WeatherAdapters } from "../store/zones";
  import Zone from "../app/rv_config_zone.svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_NAMES);
    //get_data();
    get_zones();
    get_adapters();
  });
  $: zones = [...$ZonesAuto];
  $: adapters = [...$WeatherAdapters];
  $: sel_adapter_idx = 0;
  $: sel_zone_idx = $Z;
  let zones_exists = [];
  function set_zone_exists(idx, val) {
    zones_exists[idx] = val;
  }
  $: {
    sel_zone_idx;
      get_zone(sel_zone_idx);
  }

  $: {
    sel_adapter_idx;
      get_adapter(sel_adapter_idx);
    
  }
  function new_zones() {
    for (let i = 0; sel_zone_idx < 0 && i < zones.length; ++i) {
      if (zones[i] !== null) {
        sel_zone_idx = i;
      }
    }
    for (let i = 0; i < zones.length; ++i) {
      set_zone_exists(i, zones[i] !== null && zones[i].flags.exists);
    }

  }
  $: {
    zones;
    console.log("new zones");
    new_zones();
  }
  $: {
    if (zones[sel_zone_idx] && "attr" in zones[sel_zone_idx]) sel_adapter_idx = zones[sel_zone_idx].attr.adapter;
  }

  function get_modified_existences() {
    let result = [];
    for (let i = 0; i < zones.length; ++i) {
      if ((zones_exists[i] && (zones[i] === null || !zones[i].flags.exists)) || (!zones_exists[i] && zones[i] !== null && zones[i].flags.exists)) {
        result.push(i);
      }
    }
    console.log("modified flags.exists", result);
    return result;
  }

  function resp_data(obj) {
    console.log("ZonesAuto:", $ZonesAuto);
    console.log("PastWeatherData", $PastWeatherData);
    console.log("WeatherAdapters:", $WeatherAdapters);
  }
  function get_zone(idx) {
    if (sel_zone_idx < zones.length && zones[sel_zone_idx]) {
    const key = "zone." + idx;
    let obj = { json: { auto: { get: {} } } };
    obj.json.auto.get[key] = {};
    httpFetch.http_postRequest("/cmd.json", obj);
    }
  }
  function get_adapter(idx) {
    if (sel_adapter_idx < adapters.length && adapters[sel_adapter_idx]) {
    const key = "adapter." + idx;
    let obj = { json: { auto: { get: {} } } };
    obj.json.auto.get[key] = {};
    httpFetch.http_postRequest("/cmd.json", obj);
    }
  }
  function get_zones() {
    let obj = { json: { auto: { get: { zones: [] } } } };
    httpFetch.http_postRequest("/cmd.json", obj);
  }
  function get_adapters() {
    let obj = { json: { auto: { get: { adapters: [] } } } };
    httpFetch.http_postRequest("/cmd.json", obj);
  }
  function get_data() {
    let obj = { json: { auto: { get: { zones: [], adapters: [], past_wd: [] } } } };
    httpFetch.http_postRequest("/cmd.json", obj);
  }

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

  function f(v) {
    if (v == null) return "";
    if ("flags" in v) {
      return JSON.stringify(v.flags);
    }
    return "--";
  }

  function secs_to_dh(secs) {
    const hours = secs / 60 / 60;

    const d = Math.floor(hours / 24).toFixed(0);
    const hh = (hours % 24).toFixed(0);
    const mm = ((secs / 60) % 60).toFixed(0);
    return `${d}d-${hh}h:${mm}m`;
  }

  function epoch_to_dh(secs) {
    const now_s = $McuTime;
    console.log("now_s, secs", now_s, secs);
    if (secs === 0 || now_s < secs || now_s > secs + 60 * 60 * 24 * 365) return "long ago";

    return secs_to_dh(now_s - secs);
  }

  function save_zone() {
    const key = "zone." + sel_zone_idx;
    let obj = { json: { auto: { update: {} } } };
    obj.json.auto.update[key] = zones[sel_zone_idx];
    httpFetch.http_postRequest("/cmd.json", obj);
  }

  function enable_zone(idxs) {
    let obj = { json: { auto: { update: {} } } };
    for (let idx of idxs) {
      let zobj = { flags: { exists: 1 } };
      zobj.name = "New zone " + idx;
      const key = "zone." + idx;
      obj.json.auto.update[key] = zobj;
    }
    httpFetch.http_postRequest("/cmd.json", obj);
  }

  function adapter_add_rm(idx, add) {
    let obj = { json: { auto: { update: {} } } };
    let aobj = { flags: {} };
    aobj.flags.exists = add;
    if (add) {
      aobj.name = "New adapter " + idx;
    }
    const key = "adapter." + idx;
    obj.json.auto.update[key] = aobj;
    httpFetch.http_postRequest("/cmd.json", obj);
    get_adapter(idx); // fetch updated data
  }

  function adapter_add(idx) {
    adapter_add_rm(idx, true);
  }
  function adapter_rm(idx) {
    adapter_add_rm(idx, false);
  }
</script>

<div class="main-area">
<h3>Weather Controlled Timers</h3>
  <div class="text-center">
    <SelectZone />
  </div>
  <div class="area">
    <label
      >Enable Timer for Zone {sel_zone_idx}:
      <input
        type="checkbox"
        bind:checked={zones_exists[sel_zone_idx]}
        on:change={() => {
          const mel = get_modified_existences();
          let obj = { json: { auto: { update: {} } } };
          for (let i of mel) {
            const key = "zone." + sel_zone_idx;
            obj.json.auto.update[key] = { flags: { exists: zones_exists[sel_zone_idx] } };
          }
          httpFetch.http_postRequest("/cmd.json", obj);
        }}
      />
    </label>

    {#if sel_zone_idx < zones.length && zones[sel_zone_idx] && zones_exists[sel_zone_idx]}
      Info: Last Time Wet: {epoch_to_dh(zones[sel_zone_idx].state.last_time_wet)}

      <hr />

      <table class="border-none">
        <tr>
          <th>BeforeSunrise</th><td>
            <input type="number" min="0" step="30" bind:value={zones[sel_zone_idx].attr.before_sunrise_s} style="width:8ch;" />
            {zones[sel_zone_idx].attr.before_sunrise_s / 60} min
          </td>
        </tr>
        <tr>
          <th>Duration</th><td>
            <input type="number" min="0" step="30" bind:value={zones[sel_zone_idx].attr.duration_s} style="width:8ch;" />
            {zones[sel_zone_idx].attr.duration_s / 60} min
          </td>
        </tr>
        <tr>
          <th>Interval</th><td>
            <input type="number" min="0" step="3600" bind:value={zones[sel_zone_idx].attr.interval_s} style="width:8ch;" />
            {secs_to_dh(zones[sel_zone_idx].attr.interval_s)}
          </td>
        </tr>
        <tr>
          <th>Ignore Rain Sensor</th><td>
            <input type="checkbox" bind:checked={zones[sel_zone_idx].flags.ignore_rain} style="width:8ch;" />
          </td>
        </tr>
        <tr>
          <th>Adapter</th>
          <td>
            <select bind:value={zones[sel_zone_idx].attr.adapter}>
              {#each adapters as v, i}
                {#if v !== null && v.flags.exists}
                  <option value={i}>{v.name} </option>
                {/if}
              {/each}
            </select>
            <button
              type="button"
              on:click={() => {
                for (let i = 0; i < adapters.length; ++i) {
                  if (!adapters[i].flags.exists) {
                    adapter_add([i]);
                    break;
                  }
                }
              }}>+</button
            >
            <button
              type="button"
              on:click={() => {
                adapter_rm([sel_adapter_idx]);
              }}>-</button
            >
          </td>
        </tr>
      </table>

      {#if sel_adapter_idx > 0 && sel_adapter_idx < adapters.length && adapters[sel_adapter_idx]}
        <div class="area">
          <h4 class="text-center">Adapter Settings</h4>
          <table class="border-none w-full">
            <tr>
              <th>Name</th><td>
                <input type="text" disabled={adapters[sel_adapter_idx].flags.read_only} bind:value={adapters[sel_adapter_idx].name} style="width:20ch;" />
              </td>
            </tr>
            <tr>
              <th>Temp</th><td>{adapters[sel_adapter_idx].temp}</td>
              <td
                ><input
                  type="range"
                  disabled={adapters[sel_adapter_idx].flags.read_only}
                  bind:value={adapters[sel_adapter_idx].temp}
                  min="0.006"
                  max="0.6"
                  step="0.01"
                /></td
              >
            </tr>
            <tr>
              <th>Humi</th><td>{adapters[sel_adapter_idx].humi}</td>
              <td
                ><input
                  type="range"
                  disabled={adapters[sel_adapter_idx].flags.read_only}
                  bind:value={adapters[sel_adapter_idx].humi}
                  min="0.001"
                  max="0.1"
                  step="0.001"
                  a
                /></td
              >
            </tr>
            <tr>
              <th>Wind</th><td>{adapters[sel_adapter_idx].wind}</td>
              <td
                ><input
                  type="range"
                  disabled={adapters[sel_adapter_idx].flags.read_only}
                  bind:value={adapters[sel_adapter_idx].wind}
                  min="0.0001"
                  max="0.01"
                  step="0.001"
                /></td
              >
            </tr>
            <tr> </tr><tr>
              <th>Clouds</th><td>{adapters[sel_adapter_idx].clouds}</td>
              <td
                ><input
                  type="range"
                  disabled={adapters[sel_adapter_idx].flags.read_only}
                  bind:value={adapters[sel_adapter_idx].clouds}
                  min="0.001"
                  max="0.1"
                  step="0.001"
                /></td
              >
            </tr>
          </table>
          <button type="button" on:click={get_data}>Reload Adapter</button>
          <button
            type="button"
            on:click={() => {
              get_modified_existences();
              const key = "adapter." + sel_adapter_idx;
              let obj = { json: { auto: { update: {} } } };
              obj.json.auto.update[key] = adapters[sel_adapter_idx];
              httpFetch.http_postRequest("/cmd.json", obj);
            }}>Apply Adapter</button
          >
        </div>
      {/if}
      <button type="button" on:click={get_data}>Reload</button>
      <button type="button" on:click={get_zones}>Reload Zones</button>
      <button
        type="button"
        on:click={() => {
          get_zone(sel_zone_idx);
        }}>Reload Zone</button
      >
      <button type="button" on:click={save_zone}>Apply Changes</button>
    {/if}
  </div>

  <hr />
  <button
    type="button"
    on:click={() => {
      let obj = { json: { auto: { command: { save: "" } } } };
      httpFetch.http_postRequest("/cmd.json", obj);
    }}>Save</button
  >

  <button
    type="button"
    on:click={() => {
      let obj = { json: { auto: { command: { restore: "" } } } };
      httpFetch.http_postRequest("/cmd.json", obj);
      get_zones();
      get_adapters();
    }}>Restore</button
  >
</div>
