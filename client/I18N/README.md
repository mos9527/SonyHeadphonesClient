# Client Localization Guide

The reference Client App uses IDs to localize UI text, which are defined in `Strings.hpp`.

See also `tooling/i18n-check` for validation guide.

## Contribution Guide

- **Adding Text**: Add the corresponding ID to `Strings.hpp` and define the coorresponding **English** version as baseline in `Default.cpp`
  - It's **not necessary** for you to localize the text immediately for other locales.

- **Use the `_Def` Macro**: For example:
  - Where `Disconnected` is the ID and `"Device Disconnected"` is the English (Default) version:
```cpp
    _Def(Disconnected, "Device Disconnected")
```

- **Localizing Text**: You may modify the respective `.cpp` files to localize them. 
  - As a general rule, you should keep the strings in the same *relative* order as the IDs in `Strings.hpp`.

- **Adding New Locales (for maintainers)**: Changes to `Strings.hpp` is needed for all platforms.
  - `enum class AppLocale`, `constexpr const char* format_as(AppLocale locale)`, `ParseLocale(std::string_view code)` and `kLocaleOptionString` must be updated accordingly.
  - For the **Web Client**, `getPreferredAppLocale` in `Platform/Emscripten/loader.js` should also be updated.
  - For the **Desktop Client**, `GetPreferredAppLocale` in `SDLMain.cpp` should also be updated.
  - Furthermore, `clientPlatformLocateFontBinary` implementations for **ALL** platforms should be updated as well.

  Due to the scope of the changes, it's not recommended for one to introduce new locales. Instead, please consider submitting an Issue instead for project maintainers to add them for you.
