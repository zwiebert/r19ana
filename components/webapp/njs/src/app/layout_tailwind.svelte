<style global lang="scss">
  @tailwind base;
  @tailwind components;
  @tailwind utilities;
  @reference "tailwindcss";

  @import "../styles/app.scss";

  button,
  input,
  select {
    @apply rounded;
  }
</style>