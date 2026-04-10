Client
===
Reference client interface implementation for every platform `libmdr` supports.

## Credits
The following third-party libraries are used in the implementation.

- https://github.com/fmtlib/fmt

- https://github.com/ocornut/imgui
- https://github.com/libsdl-org/SDL

The custom font `PlexSansIcon` is created with the following source fonts.

- https://github.com/IBM/plex

- https://github.com/FortAwesome/Font-Awesome

...with the help of FontForge.

- https://fontforge.org/

The font `NeoXiHei-Code` is graciously provided by @lxgw, and is the default font for non Latin-1 or icon characters in the Web client.

- https://github.com/lxgw/NeoXiHei-Code

## Material You Theme

The client uses a Material You dark theme inspired by Sony Sound Connect. Surface/outline/error colors are fixed values extracted from the Sound Connect APK, while primary accent colors are dynamically selected based on the connected device's `ModelColor`.

Color palettes are precomputed using Google's [material-color-utilities](https://github.com/material-foundation/material-color-utilities) (SchemeTonalSpot) and stored as a constexpr table in `MaterialYouThemeTable.inc`.

### Regenerating the theme table

When adding support for new product colors:

1. Edit `tooling/theme-generator/generate.mjs` — add the new `ModelColor` entry to `modelColorToSource`
2. Run the generator:
   ```
   cd tooling/theme-generator
   npm install
   npm run generate
   ```
3. The script writes directly to `client/MaterialYouThemeTable.inc`
4. Update the `ModelColor` enum in `libmdr/include/mdr/ProtocolV2T1.hpp` if needed