<script lang="ts">
  let {onDataLoaded = (data_array) => {}, mode = "drop", proccessing = false} = $props();
  let dragOver = $state(false); // Svelte 5 state for styling
  let fileName = $state("");

  async function handleDrop(e) {
    e.preventDefault();
    dragOver = false;

    // Get the first file from the drag event
    const file = e.dataTransfer.files[0];
    if (!file) return;

    fileName = file.name; // You can get the name, but NOT the path
    
    // Read the file as binary data
    const buffer = await file.arrayBuffer();
    diag_data(new Uint8Array(buffer));
  }

</script>

<div 
  role="region"
  aria-label="File drop zone"
  ondragover={(e) => { e.preventDefault(); dragOver = true; }}
  ondragleave={() => dragOver = false}
  ondrop={handleDrop}
  class="drop-zone {dragOver ? 'active' : ''}"
>
  {#if fileName}
    <p>Loaded: {fileName}</p>
  {:else}
    <p>Drop your telemetry log here</p>
  {/if}
</div>

<style>
  .drop-zone {
    border: 2px dashed #ccc;
    padding: 40px;
    text-align: center;
    transition: background 0.2s;
  }
  .drop-zone.active {
    background: #e1f5fe;
    border-color: #03a9f4;
  }
</style>
