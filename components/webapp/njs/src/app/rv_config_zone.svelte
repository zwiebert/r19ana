<script>
  "use strict";

  import { ZoneDurations, ZoneRemainingSeconds, ZoneDisabled } from "../store/zones";
  import * as httpFetch from "../app/fetch.js";

  export let zoneIdx;
  export let zoneNames;

  let checked = getBit($ZoneDisabled, zoneIdx) !== 0;

  $: {
    ZoneDisabled.update((old) => {
      return putBit(old, zoneIdx, checked);
    });
  }

  function getBit(number, bitPosition) {
    return (number & (1 << bitPosition)) === 0 ? 0 : 1;
  }

  function setBit(number, bitPosition) {
    return number | (1 << bitPosition);
  }

  function clearBit(number, bitPosition) {
    const mask = ~(1 << bitPosition);
    return number & mask;
  }

  function putBit(number, bitPosition, bitValue) {
    return bitValue ? setBit(number, bitPosition) : clearBit(number, bitPosition);
  }
</script>

<td>
  <input class="w-40" type="text" bind:value={zoneNames[zoneIdx]} disabled={checked} />
</td>
<td>
  <input type="checkbox" bind:checked />
</td>
