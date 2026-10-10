SonyHeadphonesClient
===

A spiritual successor to [Plutoberth's original SonyHeadphonesClient](https://github.com/Plutoberth/SonyHeadphonesClient) - now with standardized support for newer devices and more platforms.

There's no release build yet - but you can always grab the latest [nightly builds](https://nightly.link/mos9527/SonyHeadphonesClient/workflows/cmake/v1-compat?preview), or use the [Web Version](#running-on-the-web).

[![Build](https://github.com/mos9527/sonyheadphonesclient/actions/workflows/cmake.yml/badge.svg)](https://github.com/mos9527/SonyHeadphonesClient/actions/workflows/cmake.yml)
[![Nightly Builds](https://img.shields.io/badge/v1compat-builds-cyan)](https://nightly.link/mos9527/SonyHeadphonesClient/workflows/cmake/v1-compat?preview)

## Roadmap
This branch is expected to be merged/released once the following features have been implemented.
- [ ] Support for legacy (`v1` protocol) devices, e.g. WH-1000XM4, WH-1000XM3
  - Already WIP. You're looking at it *right now*
  - Track the progress here: https://github.com/mos9527/SonyHeadphonesClient/pull/56
  - Feel free to submit support status in new Issues regarding v1 (XM4 and older) devices. We *really* need more volunteers :(
- [x] Native macOS platform support

## Compatibility

The following platforms (applies to `libmdr`, `client`) are *natively* supported with first-party effort.

| Platform         | Support Status | Maintainers          |
|------------------|----------------|----------------------|
| Windows          | Full Support   | [@mos9527](https://github.com/mos9527), [@Amrsatrio](https://github.com/Amrsatrio) |
| Linux            | Full Support   | [@mos9527](https://github.com/mos9527)             |
| macOS            | Full Support   | [@mos9527](https://github.com/mos9527)             |
| Web (Emscripten) | Full Support   | [@mos9527](https://github.com/mos9527)             |

For device support, refer to [`docs/device-support`](./docs/device-support/). If the feature support status for your own device is missing/incorrect/untested there, feel free to submit an [Issue](https://github.com/mos9527/SonyHeadphonesClient/issues/new) so we can work on it!

## Running on the Web

The client app is available as a Progressive Web App (PWA) with full UI/feature parity with other platforms.

**Live version is available** at: https://mos9527.com/SonyHeadphonesClient/

A [Web Serial](https://caniuse.com/wf-serial)-supporting browser is required. You can expect the app to work on:
- Desktop Chrome (89+), and Chromium derivatives (also 89+, e.g. Edge, Opera)
- [Android Chrome (148+)](https://chromestatus.com/feature/6043992171085824)
- [Desktop Firefox (151+)](https://hacks.mozilla.org/2026/05/web-serial-support-in-firefox/)

## Fonts & Localization

The Client app uses your system's locale settings to select its application locale (`AppLocale`) and the default font your OS provides.

Locale can overridden by using `--locale <locale>`.
- For the Web version, this can be manually specified in the URL via `...?locale=jp`, e.g. `https://mos9527.com/SonyHeadphonesClient/?locale=jp`.

The following locales are supported:

| Locale | Language | Localizer |
| ------ | -------- | --------- |
| default | Default (English) | - |
| en | Default (English) | - |
| sc | 简体中文/Simplified Chinese | [@mos9527](https://github.com/mos9527) |
| tc | 繁体中文/Traditional Chinese | [@mos9527](https://github.com/mos9527) |
| jp | 日本語/Japanese | TBD |
| kr | 한국어/Korean | TBD |

Additionally, for the Desktop (non-Web) client, `--font <file-path>` optionally loads an external font instead of the default font selected for the locale.

- Running `./SonyHeadphonesClient.exe --locale jp --font NotoSansCJKjp-Regular.otf` selects the Japanese application locale and the specified Noto Sans font.

## For Developers

See the [SHC Developer's Guide](./DEVELOPER.md) for [building instructions](./DEVELOPER.md#building) and development details.

See also [Contributing](./.github/CONTRIBUTING.md) for basic guidelines.

Extensive documentation is also available in the source files. Refer to the respective README files in each source folder to understand what they do!

## Platform Quirks
### Linux
On Linux, you may not see player metadata (track title, artist, etc.) despite correct output from the `playerctl metadata` command,
while your device has proper AVRCP support (e.g. it works on other platforms).

This mostly occurs with multipoint setups, and is mostly an implementation problem.
- You (or your distro) may have a misconfigured [MPRIS](https://wiki.archlinux.org/title/MPRIS) service. One way to remedy this
  is to manually run `mpris-proxy` (available in the `bluez-tools`/`bluez-utils` package) in the background, or as a systemd service.
- See also
  - https://wiki.archlinux.org/title/MPRIS
  - https://github.com/bluez/bluez/issues/868
  - [#65](https://github.com/mos9527/SonyHeadphonesClient/pull/65)
- Alternatively, the Linux Client App can be used as a fallback. This was introduced in [#63](https://github.com/mos9527/SonyHeadphonesClient/pull/63), where the wearing status of the device is used to control the host player status, thus bypassing AVRCP controls.
- This is disabled by default. You can enable it via the `--pause-media-on-remove` flag when launching the Client App. Thanks @phedoreanu for the implementation!

---

*This is a personal project, and is not officially endorsed by Sony Corporation in any capacity.*
