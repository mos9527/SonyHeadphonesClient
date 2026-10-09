Client
===
Reference client interface implementation for every platform `libmdr-bt` supports.

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

CJK fonts are covered by `Noto Sans CJK Regular` (Sans 2.004, SIL Open Font License) per locale (jp/sc/tc/lr)

- https://github.com/notofonts/noto-cjk

## Material You Theme

Contribution by [@salmon-21](https://github.com/salmon-21) in https://github.com/mos9527/SonyHeadphonesClient/pull/41

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