<script lang="ts">
  "use strict";
  import { _ } from "../services/i18n";
  import NavTabs from "../app/nav_tabs.svelte";

  import PaneMcuSettings from "../panes/mcu_settings.svelte";
  import PaneFirmwareEsp32 from "../panes/firmware_esp32.svelte";
  import DiagCharts from "../panes/diag_charts.svelte";
  import { TabIdx, stickChartControls } from "../store/app_state";

  let tabIdxMain = $derived($TabIdx["main"] ?? 0);
  let tabIdxCfg = $derived($TabIdx["cfg"] ?? 0);
  let tabIdxFw = $derived($TabIdx["fw"] ?? 0);
  let tabIdxSettings = $derived($TabIdx["settings"] ?? 0);
  let tabIdxCharts = $derived($TabIdx["charts"] ?? 0);

  if (import.meta.env.MODE === "github") {
    tabIdxMain = 1;
  }
</script>

<svelte:window
  onkeydown={(e) => {
    // Only trigger if not typing in an input
    if (e.target.tagName === "INPUT") return;

    if (e.key >= "1" && e.key <= "4") {
      tabIdxCharts = parseInt(e.key) - 1;
    }

    if (e.key === "Tab") {
      e.preventDefault(); // Stop focus jumping
      tabIdxCharts = (tabIdxCharts + 1) % 4;
    }
    if (e.key === "c") {
      e.preventDefault(); // Stop focus jumping
      $stickChartControls = !$stickChartControls;
    }
  }}
/>

<div class="text-center">
  <div class="mx-auto">
    <div id="navTabs" class="flex flex-col items-center px-1 border-none">
      <div class="navtab-main">
        <NavTabs
          nav_tabs={[...(import.meta.env.MODE === "mcu" ? [{ name: $_("MCU"), idx: 0 }] : []), { name: $_("Charts"), idx: 1 }, { name: $_("Help"), idx: 2 }]}
          name="main"
        />
      </div>
    </div>

    {#if !tabIdxMain && import.meta.env.MODE === "mcu"}
      <div id="navTabs" class="flex flex-col items-center px-1 border-none">
        <div class="navtab-sub">
          <NavTabs
            nav_tabs={[
              { name: $_("Settings"), idx: 0 },
              { name: $_("app.nav_main_firmware"), idx: 1 },
            ]}
            name="cfg"
          />
        </div>
      </div>
      <div class="w-fit mx-auto">
        {#if !tabIdxCfg}
          <PaneMcuSettings />
        {:else if tabIdxCfg === 1}
          <PaneFirmwareEsp32 />
        {/if}
      </div>
    {:else if tabIdxMain === 1 || import.meta.env.MODE === "github"}
      <div id="navTabs" class="flex flex-col items-center px-1 border-none">
        <div class="navtab-sub">
          <NavTabs
            nav_tabs={[
              { name: $_("Charts 1"), idx: 0 },
              { name: $_("Charts 2"), idx: 1 },
              { name: $_("Charts 3"), idx: 2 },
              { name: $_("Charts 4"), idx: 3 },
            ]}
            name="charts"
          />
        </div>
      </div>

      <div style="display: {!tabIdxCharts ? 'block' : 'none'}">
        <DiagCharts chart_index={0} chart_index_viewed={tabIdxCharts} />
      </div>
      <div style="display: {tabIdxCharts === 1 ? 'block' : 'none'}">
        <DiagCharts chart_index={1} chart_index_viewed={tabIdxCharts} />
      </div>
      <div style="display: {tabIdxCharts === 2 ? 'block' : 'none'}">
        <DiagCharts chart_index={2} chart_index_viewed={tabIdxCharts} />
      </div>
      <div style="display: {tabIdxCharts === 3 ? 'block' : 'none'}">
        <DiagCharts chart_index={3} chart_index_viewed={tabIdxCharts} />
      </div>
    {:else if tabIdxMain === 2 || import.meta.env.MODE === "github"}{/if}
  </div>
</div>
