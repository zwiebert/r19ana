<script lang="ts">
  interface Iprops {
    name: string;
    value: number;
    min: number;
    max: number;
    step: number;
  }
  let { name, value, min, max, step = 1 }: Iprops = $props();
</script>

<input class="config-input number" type="number" id="cfg_{name}" {name} bind:value {min} {max} {step} />
