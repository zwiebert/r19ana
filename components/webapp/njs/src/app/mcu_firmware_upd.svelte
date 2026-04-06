<script>
  import { _ } from "../services/i18n";
  import * as appDebug from "../app/app_debug.js";
  import * as httpFetch from "../app/fetch.js";
  import { req_mcuRestart } from "../app/misc.js";
  import {
    McuFirmwareUpdProgress,
    McuFirmwareUpdState,
    McuFirmwareUpdChip,
  } from "../store/mcu_firmware";

  "use strict";
  import { ReloadProgress } from "../store/app_state.js";

  export let fwbtns = [];
  export let chip = "";
  export let updSecs = 30;

  import { onMount, onDestroy } from "svelte";

  let on_destroy = [];
  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_GIT_TAGS);
  });
  onDestroy(() => {
    for (const fn of on_destroy) {
      fn();
    }
  });

  let netota_intervalID = 0;
  let netota_isInProgress = false;

  function netota_FetchFeedback() {
    let netmcu = { from:"wapp",to:"netmcu" };
    netmcu.mcu = {};
    netmcu.mcu[$McuFirmwareUpdChip + "ota"] = "?";
    let url = "/cmd.json";
    appDebug.dbLog("url: " + url);
    httpFetch.http_postRequest(url, netmcu);
  }

  function netFirmwareOTA(ota_name) {
    if (netota_isInProgress) return;
    McuFirmwareUpdState.set(0);
    McuFirmwareUpdChip.set(chip);
    let netmcu = { from:"wapp",to:"netmcu" };
    netmcu.mcu = {};
    netmcu.mcu[$McuFirmwareUpdChip + "ota"] = ota_name;
    let url = "/cmd.json";
    appDebug.dbLog("url: " + url);
    httpFetch.http_postRequest(url, netmcu);
    McuFirmwareUpdProgress.set(1);
    netota_intervalID = setInterval(netota_FetchFeedback, 1000);
    netota_isInProgress = true;
  }

$: {
    if (netota_isInProgress) {
      console.log('progress trace', $McuFirmwareUpdState);
      if ($McuFirmwareUpdState === 0 ) {
        //
      } else if ($McuFirmwareUpdState === 1) {
        //
      } else {
        clearInterval(netota_intervalID);
        netota_isInProgress = false;
      }
    }
  }
</script>

<div id="id-fwDiv">

  {#if $McuFirmwareUpdState !== 1}
    {#each fwbtns as bt}
      {#if !bt.input}
        <label>
          <button
            type="button"
            on:click={() => netFirmwareOTA(bt.ota_name)}>
            Update to {bt.name}
          </button>
        </label>
        <br />
        <br />
      {:else if bt.input === 'input'}
        <label>
          <button
            type="button"
            on:click={() => netFirmwareOTA(document.getElementById(bt.ota_name).value)}>
            Update {bt.name}
          </button>
          <input type="text" id={bt.ota_name} bind:value={bt.input_value} />
        </label>
        <br />
        <br />
      {/if}
    {/each}

    <slot></slot>
  {/if}

  {#if $McuFirmwareUpdChip === chip}
    {#if $McuFirmwareUpdState === 2}
      <br />
      <strong>
        Update failed
        <br />
        <br />
      </strong>
    {:else if $McuFirmwareUpdState === 3}
      <br />
      <strong>
        Update succeeded
        <button id="mrtb" type="button" on:click={() => req_mcuRestart()}>
          {$_('app.restartMcu')}
        </button>
        <br />
        <br />
      </strong>
    {:else if $McuFirmwareUpdState === 1}
      <strong>
        {$_('app.msg_firmwareIsUpdating')}
        <br />
      </strong>
      <br />
      <progress value={$McuFirmwareUpdProgress} max="{updSecs}" />
    {/if}
    {#if $ReloadProgress > 0}
      <strong>{$_('app.msg_waitForMcuRestart')}</strong>
      <br />
      <progress value={$ReloadProgress} max="100" />
    {/if}
  {/if}

</div>
