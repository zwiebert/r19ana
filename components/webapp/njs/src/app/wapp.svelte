<script>
  "use strict";
  import { _ } from "../services/i18n";
  import NavTabs from "../app/nav_tabs.svelte";

  import PaneMcuSettings from "../panes/mcu_settings.svelte";
  import PaneFirmwareEsp32 from "../panes/firmware_esp32.svelte";
  import PaneZoneMaintenance from "../panes/zone_maintenance.svelte";
  import { TabIdx } from "../store/app_state.js";

  $: tabIdxMain = $TabIdx["main"] || 0;
  $: tabIdxCfg = $TabIdx["cfg"] || 0;
  $: tabIdxFw = $TabIdx["fw"] || 0;
  $: tabIdxSettings = $TabIdx["settings"] || 0;
  $: tabIdxCharts = $TabIdx["charts"] || 0;
</script>

<style lang="scss">
  @use "../styles/app.scss";
</style>

<div id="navTabs" class="flex flex-col items-center px-1 border-none">
  <div class="navtab-main">
    <NavTabs nav_tabs={[
      { name:$_('MCU'), idx:0},
      { name: $_('Charts'), idx:1} ]} name="main" />
  </div>
  {#if !tabIdxMain}
    <div class="navtab-sub">
      <NavTabs nav_tabs={
        [{name:$_('Settings'),idx:0},
        {name: $_('app.nav_main_firmware'), idx:1}]} name="cfg" />
    </div>
    {#if !tabIdxCfg}
      <PaneMcuSettings />
    {:else if tabIdxCfg === 1}
        <PaneFirmwareEsp32 />
    {/if}
  {:else if tabIdxMain === 1}
    <div class="navtab-sub">
      <NavTabs nav_tabs={
        [{name:$_('Chart 1'),idx:0},
        {name:$_('Chart 2'),idx:1},
        {name:$_('Chart 3'),idx:2},
        {name: $_('Chart 4'), idx:3}]} name="charts" />
    </div>
    <div style="display: {!tabIdxCharts ?  'block' : 'none'}">
      <PaneZoneMaintenance chart_index={0} />
    </div>
    <div style="display: {tabIdxCharts === 1 ? 'block' : 'none'}">
      <PaneZoneMaintenance chart_index={1} />
    </div>
    <div style="display: {tabIdxCharts === 2 ? 'block' : 'none'}">
      <PaneZoneMaintenance chart_index={2} />
    </div>
    <div style="display: {tabIdxCharts === 3 ? 'block' : 'none'}">
      <PaneZoneMaintenance chart_index={3} />
    </div>
  {/if}
</div>
