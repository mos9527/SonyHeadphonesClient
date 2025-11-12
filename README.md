SonyHeadphonesClient
===

A spiritual successor to  [Plutoberth's original SonyHeadphonesClient](https://github.com/Plutoberth/SonyHeadphonesClient) - now with standardized support for newer devices and more platforms.

There's no release build yet - but you can always grab the latest [nightly builds](https://nightly.link/mos9527/SonyHeadphonesClient/workflows/cmake/rewrite?preview), or use the [Web Version](#notes-on-web-platform).

[![Build](https://github.com/mos9527/sonyheadphonesclient/actions/workflows/cmake.yml/badge.svg)](https://github.com/mos9527/SonyHeadphonesClient/actions/workflows/cmake.yml) 
[![Nightly Builds](https://img.shields.io/badge/rewrite-builds-cyan)](https://nightly.link/mos9527/SonyHeadphonesClient/workflows/cmake/rewrite?preview)

## Roadmap
This brach is expected to be merged once the following features have been implemented.
- [ ] Support for legacy (`v1` protocol) devices, e.g. WH-1000XM4, WH-1000XM3
- [ ] Native macOS platform support

## Compatiblity

The following platforms (applies to `libmdr`, `client`) are *natively* supported with first-party effort.

| Platform         | Support Status | Maintainers          |
| ---------------- | -------------- | -------------------- |
| Windows          | Full Support   | @mos9527, @Amrsatrio |
| Linux            | Full Support   | @mos9527             |
| Web (Emscripten) | Full Support   | @mos9527             |

For device support, refer to `docs/device-support` to check. If the feature support status for your own device is missing/incorrect/untested here, feel free to submit an [Issue](https://github.com/mos9527/SonyHeadphonesClient/issues/new) so we can work on it!

## Notes on Web Platform

The client app is available as a Progressive Web App with exact UI/Feature parity seen in other platforms.

**Live version is available** at: https://mos9527.com/SonyHeadphonesClient/

A [Web Serial](https://caniuse.com/wf-serial) supporting browser is required - with a minor exception of Chrome on Android where [Web serial over Bluetooth on Android](https://cr-status.appspot.com/feature/5139978918821888) is supported as of Android build 138. You can expect the app to work on all reasonably new Desktop Chrome browsers, and the latest Android Chrome builds.

As always, status reports are welcome - please do submit an Issue if your browser supports the Web version of the client app.

## For Developers

We have extensive documentations available in the source files. Moreover, refer to the respective README files in each source folder to understand what they do!

A C++20 compliant compiler is required. GCC 14, Clang 21 and MSVC 19 has been used for development and are guanrateed to be supported.

### Building (Regular CMake)
This is no different from your regular CMake projects.
Third-party dependencies (see `contrib`) are managed by CMake's `FetchContent` and are always statically linked, so no worries - Expect things to *just work*.

**For Developers:** See also `tooling` for codegen dependencies.
#### Building on Linux
You need DBus and BlueZ development packages installed.
- Debian (Ubuntu): `sudo apt install libbluetooth-dev libdbus-1-dev`
- Fedora: `sudo dnf install bluez-libs-devel dbus-devel`
- Arch Linux: `sudo pacman -S bluez dbus`

**For Developers:** You may want to have `bluez-tools`/`bluez-utils` installed for testing.
#### Example
```bash
mkdir build
cd build
cmake ..
cmake --build . --target SonyHeadphonesClient
```

### Building (emscripten)
- Install the SDK with `emsdk` and verify your installation - https://emscripten.org/docs/getting_started/downloads.html
- Run `emcmake cmake ...` in place of configuration.
  - Or, set up CMake Toolchain variables manually with e.g. 
  ```-DCMAKE_TOOLCHAIN_FILE=/usr/lib/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_CROSSCOMPILING_EMULATOR=/usr/lib/emsdk/node/20.18.0_64bit/bin/node```
- No extra dependencies are required.
- Example usage
```bash
mkdir build
cd build
emcmake cmake ..
cmake --build . --target SonyHeadphonesClient
```
- Run the generated page with `emrun client/index.html` - or host it with any static HTTP server.

### IntelliJ CLion/Rider (emscripten)
Set up CMake Toolchain variables with a new CMake Profile (in CMake options) - you can 
then build from there and serve the static content within `<build directory>/client/` with any static HTTP server.

## Platform Quirks
### Linux
On Linux, you may not see player metadata (track title, artist, etc.) despite correct output from `playerctl metadata` command
while your device having proper AVRCP support (e.g. works on other platforms).
- You (or your distro) may have misconfigured [MPRIS](https://wiki.archlinux.org/title/MPRIS) service. One way to remedy
  is to manually run `mpris-proxy` (available in `bluez-tools`/`bluez-utils` package) in the background, or as a systemd service.
- See also
  - https://wiki.archlinux.org/title/MPRIS
  - https://github.com/bluez/bluez/issues/868 
- Example [systemd file](https://github.com/bluez/bluez/commit/daa86e06c376d6e92bb0d1e2f1edb649974bfcbd) for `mpris-proxy`
```ini
[Unit]
Description=Bluetooth mpris proxy
Documentation=man:mpris-proxy(1)

Wants=dbus.socket
After=dbus.socket

[Service]
Type=simple
ExecStart=/usr/bin/mpris-proxy

[Install]
WantedBy=default.target
```
You can then enable it as a systemd user service (change username as necessary).
```bash
sudo systemctl --machine mos9527@ --user enable mpris-proxy.service
sudo systemctl --machine mos9527@ --user start mpris-proxy.service
sudo systemctl --machine mos9527@ --user status mpris-proxy.service
● mpris-proxy.service - Bluetooth mpris proxy
     Loaded: loaded (/etc/systemd/user/mpris-proxy.service; enabled; preset: enabled)
     Active: active (running) since Wed 2025-11-12 22:49:07 CST; 3s ago
 Invocation: 2cca0507c0ca4a20ac650fc4b2c22a25
       Docs: man:mpris-proxy(1)
   Main PID: 123829
      Tasks: 1 (limit: 33257)
     Memory: 432K (peak: 1.7M)
        CPU: 5ms
     CGroup: /user.slice/user-1000.slice/user@1000.service/app.slice/mpris-proxy.service
             └─123829 /usr/bin/mpris-proxy
```