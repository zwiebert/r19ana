<script>
  import { _ } from "../services/i18n";
  import {
    Z,
    ZoneName,
  } from "../store/curr_zone";
  import * as httpFetch from "../app/fetch.js";
  import { onMount } from "svelte";

  onMount(() => {
    httpFetch.http_fetchByMask(
      httpFetch.FETCH_ZONE_NAMES);
  });


$: name = $ZoneName || "";


function shn_fromHtml_toMcu(val) {
    let rd = { to: "netmcu", kvs: { zn: "?" } };
    rd.kvs["zn" + $Z.toString()] = val;

    let url = "/cmd.json";
    httpFetch.http_postRequest(url, rd);
  }

  function hChange_Name() {
    shn_fromHtml_toMcu(name);
  }

</script>


<div class="text-center">
    <label>{$_('app.msg_enterNewName')}</label>
    <br />
    <input type="text" name="name" bind:value={name} on:change={hChange_Name} />
  </div>