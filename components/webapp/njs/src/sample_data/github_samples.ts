
  export async function getGithubSamples(): objects[] {
    const user = "zwiebert";
    const repo = "r19ana";
    const folder = "main/data"; // the folder where your .bin files sit

    const url = `https://api.github.com/repos/${user}/${repo}/contents/${folder}`;

    try {
      const response = await fetch(url);
      const files = await response.json();
      console.log("files: ", files);
      // This returns an array of objects with 'name' and 'download_url'
      return files
        .filter((file) => file.name.endsWith(".bin"))
        .map((file) => ({
          name: file.name,
          url: file.download_url, // This is the raw URL to the file
        }));
    } catch (e) {
      console.error("Could not scan GitHub samples", e);
      return [];
    }
  }

  export async function fetchBinaryData(url: string) {
    // 1. Declare variables at the top of the function scope
    let response;
    let buffer;

    try {
      // 2. Assign the result to your lowercase 'response' variable
      response = await fetch(url);

      if (!response.ok) throw new Error("Network response was not ok");

      // 3. Use the lowercase 'response' variable here
      buffer = await response.arrayBuffer();
      diag_data = new Uint8Array(buffer);

      error = false;
    } catch (e) {
      // Note: Do NOT try to access 'response' here if fetch failed
      error = e.message;
    }
  }
