<script>
  import { _ } from "../services/i18n";
  import {
    PressControlStatus,
    WaterPumpStatus,
    RainSensorStatus,
  } from "../store/zones";
  import * as httpFetch from "../app/fetch.js";
  import { onMount } from "svelte";

  import WiRain from 'svelte-icons/wi/WiRain.svelte';
  import WiDaySunny from 'svelte-icons/wi/WiDaySunny.svelte';

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_RV_STATUS);
  });
</script>

<span class="{$PressControlStatus ? 'bg-selected fg-selected' : 'line-through'}">
  {$_('app.pressControl')}
</span>
<span class="{$WaterPumpStatus ? 'bg-selected fg-selected' : 'line-through'}">
  {$_('app.pump')}
</span>
<span class="{$RainSensorStatus ? 'bg-selected fg-selected' : 'line-through'}">
  {$_('app.rainSensor')}
</span>

<!--
<div class="h-8 w-8 text-gray-800">
{#if $RainSensorStatus}
<WiRain />
{:else}
<WiDaySunny />
{/if}
</div>
-->