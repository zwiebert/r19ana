export async function fetchBinaryData(url: string, onDataLoaded: (data: Uint8Array, name: string) => void, onError: (error) => void) {
  if (!url) {
    if (onError) onError("url argument missing");
    return;
  }
  // 1. Declare variables at the top of the function scope
  let response;
  let buffer;

  try {
    // 2. Assign the result to your lowercase 'response' variable
    response = await fetch(url);

    if (!response.ok) throw new Error("Network response was not ok");

    // 3. Use the lowercase 'response' variable here
    buffer = await response.arrayBuffer();
    if (onDataLoaded) onDataLoaded(new Uint8Array(buffer), url.split("\\").pop().split("/").pop());
  } catch (e) {
    // Note: Do NOT try to access 'response' here if fetch failed
    if (onError) onError(e.message);
  }
}
