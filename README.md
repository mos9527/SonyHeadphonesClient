SonyHeadphonesClient
===

Building (Regular CMake)
---
This is no different from your regular CMake projects.

```bash
mkdir build
cd build
cmake ..
cmake --build . --target SonyHeadphonesClient
```

Building (emscripten)
---
- Install the SDK with `emsdk` and verify your installation - https://emscripten.org/docs/getting_started/downloads.html
- Run `emcmake cmake ...` in place of configuration.
  - Or, setup CMake toolchain variables manually with e.g.
  
      ```-DCMAKE_TOOLCHAIN_FILE=/usr/lib/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_CROSSCOMPILING_EMULATOR=/usr/lib/emsdk/node/20.18.0_64bit/bin/node```
- Example usage
```bash
mkdir build
cd build
emcmake cmake ..
cmake --build . --target SonyHeadphonesClient
 ```
- Run the generated page with `emrun client/index.html` - or host it with
```bash
python -m http.server -d client
```