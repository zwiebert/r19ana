
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

