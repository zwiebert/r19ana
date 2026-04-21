// eslint.config.js
import svelte from 'eslint-plugin-svelte';
import tseslint from 'typescript-eslint';

export default tseslint.config(
  ...tseslint.configs.recommendedTypeChecked, // Enables type-aware rules
  ...svelte.configs['flat/recommended'],
  {
    languageOptions: {
      parserOptions: {
        // Option A: Modern v8+ recommendation (preferred)
        projectService: true,
        // Option B: Older/explicit way if A fails
        // project: './tsconfig.json', 
        
        extraFileExtensions: ['.svelte'], // Critical for Svelte files
        tsconfigRootDir: import.meta.dirname,
      },
    },
  },
  {
    files: ['**/*.svelte'],
    languageOptions: {
      parserOptions: {
        // Tells ESLint to use the TS parser for <script> blocks in Svelte
        parser: tseslint.parser,
      },
    },
  },
  {
    rules: {
      '@typescript-eslint/no-for-in-array': 'error',
    },
  }
);
