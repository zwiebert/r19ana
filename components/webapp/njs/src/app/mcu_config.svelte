<script>
  "use strict";
  import { _ } from "../services/i18n";
  import tippy from "sveltejs-tippy";
  import { McuConfig, McuConfigKeys } from "../store/mcu_config.js";
  import { McuDocs_cliHelpConfig } from "../store/mcu_docs.js";
  import * as appDebug from "../app/app_debug.js";
  import * as httpFetch from "../app/fetch.js";
  import { req_mcuRestart } from "../app/misc.js";
  import { onMount, onDestroy } from "svelte";
  import { ReloadProgress, TabIdx, GuiAcc } from "../store/app_state.js";
  import NavTabs from "../app/nav_tabs.svelte";

  import McuConfigGpio from "../components/mcu_config/gpio.svelte";
  import McuConfigGpioSelect from "../components/mcu_config/gpio_select.svelte";
  import McuConfigNetwork from "../components/mcu_config/network.svelte";
  import McuConfigLanPhy from "../components/mcu_config/lan_phy.svelte";
  import McuConfigNumber from "../components/mcu_config/number.svelte";
  import McuConfigEnable from "../components/mcu_config/enable.svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG);
  });

  $: tabIdxMcc = $TabIdx["mcc"] || 0;

  $: mcuConfigNames = {}; // localized GUI labels
  $: {
    for (let key of $McuConfigKeys) {
      mcuConfigNames[key] = $_({ id: "mcuConfigNames." + key, default: key });
    }
  }

  let mcuConfig = {};
  function updateMcuConfig(obj) {
    mcuConfig = { ...obj };
  }

  $: {
    updateMcuConfig($McuConfig);
  }

  $: mcuConfigKeysNetwork = $McuConfigKeys.filter((val) => val === "network");
  $: mcuConfigKeysMQTT = $McuConfigKeys.filter((val) => val.startsWith("mqtt-"));
  $: mcuConfigKeysHTTP = $McuConfigKeys.filter((val) => val.startsWith("http-"));
  $: mcuConfigKeysWLAN = $McuConfigKeys.filter((val) => val.startsWith("wlan-"));
  $: mcuConfigKeysLAN = $McuConfigKeys.filter((val) => val.startsWith("lan-"));
  $: mcuConfigKeysNTP = $McuConfigKeys.filter((val) => val.startsWith("ntp-"));
  $: mcuConfigKeysTime = $McuConfigKeys.filter((val) => val === "rtc" || val.startsWith("rtc-") || val === "tz");
  $: mcuConfigKeysIgnore = ["rf-repeater"];

  $: mcuConfigKeysPin = $McuConfigKeys.filter((val) => val.startsWith("gpio") || val === "set-button-pin");

  $: mcuConfigKeysMisc = $McuConfigKeys.filter(
    (val) =>
      !(
        val === "gm-used" ||
        mcuConfigKeysNetwork.includes(val) ||
        mcuConfigKeysMQTT.includes(val) ||
        mcuConfigKeysHTTP.includes(val) ||
        mcuConfigKeysWLAN.includes(val) ||
        mcuConfigKeysLAN.includes(val) ||
        mcuConfigKeysNTP.includes(val) ||
        mcuConfigKeysTime.includes(val) ||
        mcuConfigKeysIgnore.includes(val) ||
        val.endsWith("-pin") ||
        mcuConfigKeysPin.includes(val)
      )
  );

  $: {
    console.log("mcuConfigKeysMisc: ", mcuConfigKeysMisc);
    console.log("$McuConfigKeys: ", $McuConfigKeys);
    console.log("mcuConfig: ", mcuConfig);
  }

  $: gpios = $McuConfig["gpio"] || "..........................................";

  export function reload_config() {
    updateMcuConfig($McuConfig);
  }

  function hClick_Reload() {
    reload_config();
    httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG);
  }

  function hClick_SaveOld() {
    mcuConfig_fromHtml_toMcu();
  }

  function mcuConfig_fromHtml_toMcu() {
    const cfg = $McuConfig;

    let new_cfg = {};
    let has_changed = false;
    // eslint-disable-next-line no-unused-vars
    Object.keys(cfg).forEach(function (key, idx) {
      let new_val = 0;
      let el = document.getElementById("cfg_" + key);
      appDebug.dbLog("key: " + key);

      switch (el.type) {
        case "checkbox":
          new_val = el.checked ? 1 : 0;
          break;
        case "number":
          new_val = el.valueAsNumber;
          break;
        default:
          new_val = el.value;
      }
      let old_val = cfg[key];
      if (new_val !== old_val) {
        new_cfg[key] = new_val;
        has_changed = true;
        appDebug.dbLog(key);
      }
    });

    if (has_changed) {
      new_cfg.all = "?";
      let url = "/cmd.json";
      httpFetch.http_postRequest(url, { config: new_cfg });
    }
  }
  function hClick_Save() {
    mcuConfig_saveModifiedOptions();
  }

  function hClick_RestartMcu() {
    misc.req_mcuRestart();
  }

  function mcuConfig_getCfgMod() {
    const cfg_mcu = $McuConfig;
    const cfg_ui = mcuConfig;
    let cfg_mod = {};
    let hasModified = false;

    Object.keys(cfg_ui).forEach(function (key, idx) {
      const val_ui = cfg_ui[key];
      if (cfg_mcu[key] !== val_ui) {
        hasModified = true;
        if (typeof val_ui === "boolean") {
          cfg_mod[key] = val_ui ? 1 : 0;
        } else {
          cfg_mod[key] = cfg_ui[key];
        }
      }
    });
    return hasModified ? cfg_mod : null;
  }

  function mcuConfig_saveModifiedOptions() {
    let cfg_mod = mcuConfig_getCfgMod();
    if (cfg_mod === null) return;
    McuConfig.update(mcuConfig);
    Object.keys(cfg_mod).forEach(function (key, idx) {
      if (key.startsWith("gpio") && key !== "gpio" && cfg_mod[key] === "d") {
        McuConfig.remove(key);
      }
    });

    httpFetch.http_postCommand({ config: cfg_mod });

    setTimeout(() => {
      httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG);
    }, 500);

    setTimeout(() => {
      httpFetch.http_fetchByMask(httpFetch.FETCH_CONFIG);
    }, 2500);
  }

  function cfg_by_name(name) {
    let res = {};
    res.name = name;
    return res;
  }
  function hasKeyEdited(key, val1, val2) {
    if (val1 === val2) return false;
    if (key.endsWith("-enable") && !val1 === !val2) return false;
    return true;
  }
  ///////////////// wizard ////////////////////
  $: wiz_gpio = -1;
  $: wiz_gpio_status = "";

  function wiz_addGpio() {
    if (wiz_gpio === -1) return;
    let cmd = { to: "tfmcu", config: {} };
    const gpioKey = "gpio" + wiz_gpio;
    cmd.config[gpioKey] = "i";
    wiz_gpio = -1;

    httpFetch.http_postCommand(cmd);
    wiz_gpio_status = "";
    setTimeout(() => {
      httpFetch.http_postCommand({ config: { gpio: "?" } });
      httpFetch.http_postCommand({ config: { gpio: "$" } });
      setTimeout(() => {
        wiz_gpio_status = $McuConfig[gpioKey] ? '<span class="bg-green-500">ok</span>' : '<span class="bg-red-500">error</span>';
      }, 200);
    }, 500);
  }

  /////////////////////////////////////////////
</script>
<h3>Configure Net-MCU</h3>
<div class="navtab-sub2 flex flex-col items-center px-1 border-none">
  <NavTabs
    nav_tabs={[
      { name: $_("navTab.cfg.mcu.network.tab"), idx: 0 },
      { name: $_("navTab.cfg.mcu.misc.tab"), idx: 1 },
    ]}
    name="mcc"
    vertical={false}
  />
</div>
{#if tabIdxMcc === 0}
  {#if mcuConfigKeysNetwork.length > 0}
    <h4 class="text-center" use:tippy={{ content: $_("panes.network.tt.header") }}>{$_("panes.network.header")}</h4>
    <table class="conf-table top_table rounded-xl overflow-hidden">
      {#each mcuConfigKeysNetwork as key, i}
        <tr>
          <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
            ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label
            ></td
          >
          {#if key.endsWith("-enable")}
            <td>
              <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
            </td>
          {:else if key === "network"}
            <td>
              <McuConfigNetwork name={key} bind:value={mcuConfig[key]} has_lan={mcuConfigKeysLAN.length > 0 || mcuConfig.network === "lan"} />
            </td>
          {:else}
            <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
          {/if}
        </tr>
      {/each}
    </table>

    {#if mcuConfigKeysLAN.length > 0 && (mcuConfig.network === "lan" || mcuConfig.network === "lan-wlan")}
      <div class="area">
        <table class="conf-table top_table rounded-xl overflow-hidden">
          <caption>{$_("mcuConfig.ethernet")}</caption>
          {#each mcuConfigKeysLAN as key, i}
            <tr>
              <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                  >{mcuConfigNames[key]}</label
                ></td
              >
              {#if key.endsWith("-enable")}
                <td>
                  <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                </td>
              {:else if key === "lan-phy"}
                <td>
                  <McuConfigLanPhy name={key} bind:value={mcuConfig[key]} />
                </td>
              {:else if key === "lan-pwr-gpio"}
                <td>
                  <McuConfigGpioSelect name={key} bind:value={mcuConfig[key]} max="36" />
                </td>
              {:else}
                <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
              {/if}
            </tr>
          {/each}
        </table>
      </div>
    {/if}

    {#if mcuConfig.network !== "none"}
      {#if mcuConfigKeysWLAN.length > 0 && (mcuConfig.network === "wlan" || mcuConfig.network === "lan-wlan")}
        <div class="area">
          <table class="conf-table top_table rounded-xl overflow-hidden">
            <caption>{$_("mcuConfig.wlan_station")}</caption>
            {#each mcuConfigKeysWLAN as key, i}
              <tr>
                <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                  ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                    >{mcuConfigNames[key]}</label
                  ></td
                >
                {#if key.endsWith("-enable")}
                  <td>
                    <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                  </td>
                {:else}
                  <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                {/if}
              </tr>
            {/each}
          </table>
        </div>
      {/if}

      {#if mcuConfigKeysNTP.length > 0}
        <div class="area">
          <table class="conf-table top_table rounded-xl overflow-hidden">
            <caption>{$_("mcuConfig.ntp_client")}</caption>
            {#each mcuConfigKeysNTP as key, i}
              <tr>
                <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                  ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                    >{mcuConfigNames[key]}</label
                  ></td
                >
                {#if key.endsWith("-enable")}
                  <td>
                    <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                  </td>
                {:else}
                  <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                {/if}
              </tr>
            {/each}
          </table>
        </div>
      {/if}

      {#if mcuConfigKeysMQTT.length > 0}
        <div class="area">
          <table class="conf-table top_table rounded-xl overflow-hidden">
            <caption>{$_("mcuConfig.mqtt_client")}</caption>
            {#each mcuConfigKeysMQTT as key, i}
              <tr>
                <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                  ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                    >{mcuConfigNames[key]}</label
                  ></td
                >

                {#if key.endsWith("-enable")}
                  <td>
                    <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                  </td>
                {:else}
                  <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                {/if}
              </tr>
            {/each}
          </table>
        </div>
      {/if}

      {#if mcuConfigKeysHTTP.length > 0}
        <div class="area">
          <table class="conf-table top_table rounded-xl overflow-hidden">
            <caption>{$_("mcuConfig.http_server")}</caption>
            {#each mcuConfigKeysHTTP as key, i}
              <tr>
                <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
                  ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}"
                    >{mcuConfigNames[key]}</label
                  ></td
                >

                {#if key.endsWith("-enable")}
                  <td>
                    <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
                  </td>
                {:else}
                  <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
                {/if}
              </tr>
            {/each}
          </table>
        </div>
      {/if}
    {/if}
  {/if}
{:else if tabIdxMcc === 1}
  <h4 class="text-center" use:tippy={{ content: $_("panes.misc.tt.header") }}>{$_("panes.misc.header")}</h4>

  {#if mcuConfigKeysTime.length > 0}
    <div class="area">
      <table class="conf-table top_table rounded-xl overflow-hidden">
        {#each mcuConfigKeysTime as key, i}
          <tr>
            <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
              ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label
              ></td
            >
            {#if key.endsWith("-enable")}
              <td>
                <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
              </td>
            {:else}
              <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
            {/if}
          </tr>
        {/each}
      </table>
    </div>
  {/if}

  <div class="area" id="config-div">
    <table id="cfg_table_id" class="conf-table top_table rounded-xl overflow-hidden">
      {#each mcuConfigKeysMisc as key, i}
        <tr>
          <td use:tippy={{ content: $McuDocs_cliHelpConfig[key] }}
            ><label class="config-label {hasKeyEdited(key, mcuConfig[key], $McuConfig[key]) ? 'font-bold' : ''}" for="cfg_{key}">{mcuConfigNames[key]}</label
            ></td
          >
          {#if key.endsWith("-enable") || key === "stm32-bootgpio-inv"}
            <td>
              <McuConfigEnable name={key} bind:value={mcuConfig[key]} />
            </td>
          {:else if key === "verbose"}
            <td>
              <McuConfigNumber name={key} bind:value={mcuConfig[key]} min="0" max="5" />
            </td>
          {:else}
            <td><input class="config-input text" type="text" id="cfg_{key}" name={key} bind:value={mcuConfig[key]} /></td>
          {/if}
        </tr>
      {/each}
    </table>
  </div>
{/if}

<br />
<button id="crlb" type="button" on:click={hClick_Reload}>
  {$_("app.reload")}
</button>
<button id="csvb" type="button" on:click={hClick_Save}>
  {$_("app.save")}
</button>
<br />
<br />
<button id="mrtb" type="button" on:click={() => req_mcuRestart()}>
  {$_("app.restartMcu")}
</button>
<br />
<div id="config_restart_div" />
{#if $ReloadProgress > 0}
  <strong>{$_("app.msg_waitForMcuRestart")}</strong>
  <br />
  <progress id="reload_progress_bar" value={$ReloadProgress} max="100" />
{/if}

<style lang="scss">
</style>
