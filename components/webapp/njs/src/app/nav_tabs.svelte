<script lang="ts">
  "use strict";
  import { TabIdx } from "../store/app_state";
  import { onMount } from "svelte";

  interface Inav_tabs {
    name: string;
    idx: number;
  }
  let { name = "nav_tab", nav_tabs = [], vertical = false }: { name: string; nav_tabs: Inav_tabs[]; vertical: boolean } = $props();

  function setTabIdx(idx) {
    let obj = {};
    obj[name] = idx;
    TabIdx.update(obj);
  }

  function isIdxInTabs(idx) {
    for (const t of nav_tabs) if (t.idx === idx) return true;
    return false;
  }

  function fixMissingIdx() {
    const idx = $TabIdx[name] as number;
    if (isIdxInTabs(idx)) return;
    for (const t of nav_tabs) {
      setTabIdx(t.idx);
      return;
    }
  }

  onMount(fixMissingIdx);
</script>

<div id="tabBar" class="overflow-hidden">
  {#each nav_tabs as tab}
    <button
      class="p-0 m-1 {($TabIdx[name] || 0) === tab.idx ? 'is-selected' : 'bg-transparent'} text-lg font-medium border-none outline-none tabbt"
      id="tabbt{tab.idx}"
      onclick={() => setTabIdx(tab.idx)}
    >
      {tab.name}
    </button>
    {#if vertical}<br />{/if}
  {/each}
</div>

<style lang="scss">
  /* Style the buttons that are used to open the tab content */
</style>
