<script lang="ts">
  let {
    onDataLoaded,
    mode = "both", // 'drop', 'button', or 'both'
    processing = false,
  }: { onDataLoaded: (data: Uint8Array, name: string) => void; mode: string; processing: boolean } = $props();

  let isDragging = $state(false);
  let fileInput;

  async function handleFiles(files) {
    if (!files || files.length === 0) return;
    isDragging = false;

    // 1. Read file to ArrayBuffer
    const buffer = await files[0].arrayBuffer() as Uint8Array;

    // 2. Pass Uint8Array to your parent callback
    console.log("files", files);
    onDataLoaded(new Uint8Array(buffer), files[0].name as string);
  }

  const onDrop = (e) => {
    e.preventDefault();
    handleFiles(e.dataTransfer.files);
  };
</script>

<svelte:window ondragenter={() => (isDragging = true)} ondragover={(e) => e.preventDefault()} />

<!-- Hidden Input for the Button -->
<input type="file" bind:this={fileInput} class="hidden" onchange={(e) => handleFiles(e.target.files)} />

<div class="flex flex-col items-center gap-4">
  <!-- Mode: Button (Mobile Friendly) -->
  {#if mode === "both" || mode === "button"}
    <button
      onclick={() => fileInput.click()}
      disabled={processing}
      class="bg-blue-600 text-white px-6 py-2 rounded shadow hover:bg-blue-700 disabled:bg-gray-400"
    >
      {processing ? "Processing..." : "Local Data Files"}
    </button>
  {/if}

  <!-- Mode: Inline Drop Zone (Desktop Visual) -->
  {#if mode === "both" || mode === "drop"}
    <div class="w-64 h-32 border-2 border-dashed border-gray-300 rounded flex items-center justify-center bg-gray-50 text-gray-500 text-sm">
      Drag & Drop Here
    </div>
  {/if}
</div>

<!-- Full Screen Overlay + Spinner -->
{#if isDragging || processing}
  <div
    role="button"
    tabindex="0"
    class="fixed inset-0 z-50 flex flex-col items-center justify-center transition-all
           {isDragging ? 'bg-blue-900/40 backdrop-blur-sm border-[12px] border-dashed border-white/50' : 'bg-white/80'}"
    ondragleave={(e) => {
      if (!e.relatedTarget) isDragging = false;
    }}
    ondrop={onDrop}
  >
    {#if processing}
      <!-- Simple CSS Spinner -->
      <div class="w-12 h-12 border-4 border-blue-600 border-t-transparent rounded-full animate-spin"></div>
      <p class="mt-4 font-bold text-blue-800">Compiling Data...</p>
    {:else}
      <h2 class="text-white text-3xl font-bold pointer-events-none">Drop to Load Binary Data</h2>
    {/if}
  </div>
{/if}

<style>
  .animate-spin {
    animation: spin 1s linear infinite;
  }
  @keyframes spin {
    from {
      transform: rotate(0deg);
    }
    to {
      transform: rotate(360deg);
    }
  }
</style>
