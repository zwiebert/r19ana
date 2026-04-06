<script>
  import { _ } from "../services/i18n";
  import { Z } from "../store/curr_zone";
  import { ZoneTimers } from "../store/zones";
  import { ZoneActiveTimerMask } from "../store/curr_zone";
  import * as httpFetch from "../app/fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_TIMERS);
  });

  const args_same_types = ["r", "di", "ir", "ip"];
  const args_time_types = ["d1", "d0", "per", "sb", "se"];

  const default_args = {
    d1: "00:00:00",
    d0: "00:00:00",
    r: 0,
    per: "00:00",
    di: 0,
    sb: "00:00",
    se: "00:00",
    ir: false,
    ip: false,
  };

  $: args = Object.assign({}, default_args);
  $: tn = 0;

  function update_args(timer) {
    args_same_types.forEach((key) => {
      args[key] = timer[key] ? timer[key] : default_args[key];
    });
    args_time_types.forEach((key) => {
      args[key] = timer[key] ? seconds_to_timeString(timer[key]) : default_args[key];
    });
  }

  $: {
    let timer = $ZoneTimers["timer" + $Z + "." + tn] || {};
    console.log("timer updated: ", timer);
    update_args(timer);
  }

  function post_timer() {
    console.log(args);
    let cmd = { timer: {} };
    cmd.timer.vn = $Z;
    cmd.timer.tn = tn;
    args_same_types.forEach((key) => {
      if (args[key]) cmd.timer[key] = args[key];
    });

    args_time_types.forEach((key) => {
      let secs = timeString_to_seconds(args[key]);
      if (secs) cmd.timer[key] = secs;
    });

    console.log(cmd.timer);
    httpFetch.sendRv(cmd);
  }

  function timeString_to_seconds(ts) {
    let h = parseInt(ts.substr(0, 2));
    let m = parseInt(ts.substr(3, 2));
    let s = ts.length > 5 ? parseInt(ts.substr(6, 2)) : 0;
    return (h * 60 + m) * 60 + s;
  }

  function seconds_to_timeString(s) {
    let hp = ((s / 3600) % 24).toFixed(0);
    let mp = ((s / 60) % 60).toFixed(0);
    let sp = (s % 60).toFixed(0);

    let ts = ("00" + hp).slice(-2);
    ts += ":" + ("00" + mp).slice(-2);
    if (sp) ts += ":" + ("00" + sp).slice(-2);

    return ts;
  }

</script>

<input type="number" min="0" max="10" step="1" bind:value={tn} />
<div class="flex">
  {#each { length: 10 } as _, i}
    <div class="{$ZoneActiveTimerMask & (1<<i) ? 'bg-selected' : ''}">
      <input type="radio" bind:group={tn} value={i} />
    </div>
  {/each}
</div>

<table>
  <tr>
    <th>On-Duration</th>
    <td>
      <input type="time" step="15" bind:value={args.d1} />
    </td>
  </tr>
  <tr>
    <th>Off-Duration</th>
    <td>
      <input type="time" step="15" bind:value={args.d0} />
    </td>
  </tr>
  <tr>
    <th>Repeats</th>
    <td>
      <input type="number" bind:value={args.r} />
    </td>
  </tr>
  <tr>
    <th>Period</th>
    <td>
      <input type="time" step="15" bind:value={args.per} />
    </td>
  </tr>
  <tr>
    <th>Day-Interval</th>
    <td>
      <input type="number" bind:value={args.di} />
    </td>
  </tr>
  <tr>
    <th>Daily Begin</th>
    <td>
      <input type="time" bind:value={args.sb} />
    </td>
  </tr>
  <tr>
    <th>Daily End</th>
    <td>
      <input type="time" bind:value={args.se} />
    </td>
  </tr>
  <tr>
    <th>Ignore RainSensor</th>
    <td>
      <input type="checkbox" bind:checked={args.ir} />
    </td>
  </tr>
  <tr>
    <th>Ignore Pause</th>
    <td>
      <input type="checkbox" bind:checked={args.ip} />
    </td>
  </tr>
</table>

<button type="button" on:click={post_timer}>Save</button>
