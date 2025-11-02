SonyHeadphonesClient
===

A spiritual successor to  [Plutoberth's original SonyHeadphonesClient](https://github.com/Plutoberth/SonyHeadphonesClient) - now with standardized support for newer devices and more platforms.

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

### Building (Regular CMake)
This is no different from your regular CMake projects.
Dependencies are managed by CMake's `FetchContent` and are always statically linked, so no worries - Expect things to *just work*.

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
