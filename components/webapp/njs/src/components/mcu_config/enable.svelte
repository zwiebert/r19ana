<script lang="ts">
  let { name, value }: { name: string; value: boolean } = $props();
</script>

<input class="config-input cb" type="checkbox" id="cfg_{name}" {name} bind:checked={value} />
