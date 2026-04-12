<style global lang="scss">
  @use "../styles/app.scss";
  @tailwind base;
  @tailwind components;
  @tailwind utilities;
  @reference "tailwindcss";


  button,
  input,
  select {
    @apply rounded;
  }
</style>