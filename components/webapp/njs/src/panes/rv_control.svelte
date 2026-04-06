<script>
  import * as httpFetch from "../app/fetch.js";
  import { _ } from "../services/i18n";
  import Zones from "../app/zones.svelte";
  import RvStatus from "../components/rv_status.svelte";
  import { Stm32Time } from "../store/zones.js";
</script>

<div class="main-area">
  <h3 class="text-center">RV Status</h3>

  <div class="area">
  <h4 class="text-center">Zones</h4>
    <Zones />
  </div>

  <div class="area">
  <h4 class="text-center">Misc</h4>
    <RvStatus />
    <hr />
    <label>UTC<input type="text" value={$Stm32Time} /></label>
  </div>

  <div class="text-right">
    <button
      on:click={() =>
        httpFetch.http_fetchByMask(
          httpFetch.FETCH_ZONE_DURATIONS | httpFetch.FETCH_ZONE_REMAINING_DURATIONS | httpFetch.FETCH_ZONE_NAMES | httpFetch.FETCH_RV_STATUS,
        )}
    >
      {$_("app.reload")}
    </button>
  </div>
</div>
