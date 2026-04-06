import { derived } from "svelte/store";
import { PersistentIntStore } from "./custom_stores.js";
import { ZoneNames, ZoneDurations, ZoneRemainingSeconds, ZoneLPHs, ZoneTimers } from "./zones.js";
import { formatSecondsToMMSS } from "../app/misc.js";

export const Z = PersistentIntStore("group");
export const ZoneName = derived([Z, ZoneNames], ([z, names]) => names[z] || "");

export const ZoneDuration = derived([Z, ZoneDurations], ([z, durations]) => durations[z] || 0);
export const ZoneLPH = derived([Z, ZoneLPHs], ([z, lphs]) => lphs[z] || 0);
export const ZoneDurationMmss = derived(ZoneDuration, (duration) => formatSecondsToMMSS(duration));
export const ZoneRemaining = derived([Z, ZoneRemainingSeconds], ([z, remaining]) => remaining[z] || 0);
export const ZoneRemainingMmss = derived(ZoneRemaining, (remaining) => formatSecondsToMMSS(remaining));

const update_zat = function ([z, timers]) {
  let msk = 0;
  for (let i = 0; i < 10; ++i) {
    const key = "timer" + z + "." + i;
    if (timers[key] && timers[key].d1) {
        msk |= 1 << i;
    } 
  }
  return msk;
};

export const ZoneActiveTimerMask = derived([Z, ZoneTimers], update_zat);
