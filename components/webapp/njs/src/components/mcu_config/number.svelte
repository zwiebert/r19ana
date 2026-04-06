<script>
  export let name;
  export let value;
  export let min;
  export let max;
  export let step = "1";
</script>

<input class="config-input number" type="number" id="cfg_{name}" {name} bind:value {min} {max} {step} />
