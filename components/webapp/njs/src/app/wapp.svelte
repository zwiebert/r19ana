<script>
  "use strict";
  import { _ } from "../services/i18n";
  import NavTabs from "../app/nav_tabs.svelte";

  import PaneRvControl from "../panes/rv_control.svelte";
  import PaneRvSettings from "../panes/rv_settings.svelte";
  import PaneMcuSettings from "../panes/mcu_settings.svelte";
  import PaneFirmwareEsp32 from "../panes/firmware_esp32.svelte";
  import PaneFirmwareStm32 from "../panes/firmware_stm32.svelte";
  import PaneZoneMaintenance from "../panes/zone_maintenance.svelte";
  import PaneZoneTimers from "../panes/zone_timers.svelte";
  import PaneFullAuto from "../panes/full_auto.svelte";
  import { TabIdx } from "../store/app_state.js";

  $: tabIdxMain = $TabIdx["main"] || 0;
  $: tabIdxCfg = $TabIdx["cfg"] || 0;
  $: tabIdxFw = $TabIdx["fw"] || 0;
  $: tabIdxSettings = $TabIdx["settings"] || 0;
</script>

<style lang="scss">
  @import "../styles/app.scss";
</style>

<div id="navTabs" class="flex flex-col items-center px-1 border-none">
  <div class="navtab-main">
    <NavTabs nav_tabs={[
      { name:$_('app.nav_main_rv'), idx:0},
      { name:$_('app.nav_main_config'), idx:1},
      { name: 'Maintenance', idx:2},
      {name: 'Timers', idx:3 },
      {name:'Auto', idx:4}]} name="main" />
  </div>
  {#if !tabIdxMain}
    <PaneRvControl />
  {:else if tabIdxMain === 1}
    <div class="navtab-sub">
      <NavTabs nav_tabs={
        [{name:$_('app.nav_cfg_netMcu'),idx:0},
        {name: $_('app.nav_cfg_rv'),idx:1},
        {name: $_('app.nav_main_firmware'), idx:2}]} name="cfg" />
    </div>
    {#if !tabIdxCfg}
      <PaneMcuSettings />
    {:else if tabIdxCfg === 1}
      <PaneRvSettings />
    {:else if tabIdxCfg === 2}
      <div class="navtab-sub2">
        <NavTabs nav_tabs={[{name:$_('app.nav_fw_netMcu'),idx:0}, {name:$_('app.nav_fw_rv'), idx:1}]} name="fw" />
      </div>
      {#if !tabIdxFw}
        <PaneFirmwareEsp32 />
      {:else if tabIdxFw === 1}
        <PaneFirmwareStm32 />
      {/if}
    {/if}
  {:else if tabIdxMain === 2}
    <PaneZoneMaintenance />
  {:else if tabIdxMain === 3}
     <PaneZoneTimers />
  {:else if tabIdxMain === 4}
     <PaneFullAuto />
  {/if}
</div>
