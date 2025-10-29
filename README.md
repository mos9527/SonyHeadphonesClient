SonyHeadphonesClient
===

Building (CMake)
---
[README TODO]

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
- Run `emcmake cmake ...` in place of all `cmake` commands
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