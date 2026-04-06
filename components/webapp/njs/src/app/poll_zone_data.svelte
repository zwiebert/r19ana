<script>
  import {
    PressControlStatus,
  } from "../store/zones";
  import * as httpFetch from "../app/fetch.js";
  import { onMount, onDestroy } from "svelte";

  export let pollInterval = 1000;


  let on_destroy = [];
  onMount(() => {
    poll_data();
  });
  onDestroy(() => {
    stop_polling();
    for (const fn of on_destroy) {
      fn();
    }
  });

  $: {
    if ($PressControlStatus) start_polling();
    else stop_polling();
  }

  let interval = 0;

  function start_polling() {
    interval = setInterval(poll_data, pollInterval);
  }

  function stop_polling() {
    clearInterval(interval);
    interval = 0;
  }

  function poll_data() {
    httpFetch.http_fetchByMask(httpFetch.FETCH_ZONE_DURATIONS | httpFetch.FETCH_ZONE_REMAINING_DURATIONS);
  }
</script>
