Tooling
===
AST walker and codegen for `libmdr` - with extra helper utilities.

| Directory | Contents |
| --- | --- |
| [`codegen/`](codegen/) | libclang-based codegen tools and their CMake glue, plus `BinaryEmbed` |
| [`scripts/`](scripts/) | Standalone Python helper scripts |
| [`i18n-check/`](i18n-check/) | Translation validation |
| [`ida/`](ida/) | Protocol IR extraction from the official app |
| [`theme-generator/`](theme-generator/) | Material You theme table generator |

## Running codegen

Codegen is disabled by default. Configure with `-DMDR_BUILD_CODEGEN=ON` after [setting up LLVM](#setting-up-llvm).

Codegen is a proper CMake dependency. Modify a source header and re-run:

```sh
cmake --build <build-dir> --target mdr_codegen
```

This rebuilds only the stale tools and regenerates only the affected files.
A full `cmake --build` will also regenerate before compiling `mdr`.

## Setting up LLVM
The `[...]Codegen` executables require LLVM.

### Windows
Get the latest release (e.g. LLVM-22.1.8-win64.exe) and install it, from https://github.com/llvm/llvm-project/releases.

`LLVM/bin` needs to be added to your `PATH` environment variable.

Hopefully CMake can find it in your Program Files. Otherwise set `CMAKE_PREFIX_PATH` to include `C:\Program Files\LLVM` (or wherever installed)

### Linux
Your `clang` package likely includes the requisite libs already — if not, try `llvm-17-dev libclang-17-dev`.

### macOS
Install `llvm` from Homebrew — CMake will locate it automatically.

## Scrubbing captures

[`scripts/scrub-capture.py`](scripts/scrub-capture.py) replaces the personal data in a packet capture - paired
device names and addresses, track metadata - with same-length placeholders, so
a capture can be contributed as a test fixture. See
[Contributing to Tests](../tests/README.md#scrub-personal-data).

## Client icon

[`scripts/make-icon.py`](scripts/make-icon.py) regenerates every platform's icon from
[`client/Platform/AppIcon.png`](../client/Platform/AppIcon.png). Replace that
file, then run `python tooling/scripts/make-icon.py` (requires `pip install pillow`)
and commit the outputs.

## README title

[`scripts/make-title-svg.py`](scripts/make-title-svg.py) regenerates [`docs/title.svg`](../docs/title.svg)
with the client icon embedded as a bitmap and the title converted to outlines.
Run `python tooling/scripts/make-title-svg.py` (requires `pip install fonttools`) and
commit the output.

## Bluetooth COM ports on Windows

[`scripts/bluetoothcom-win32.py`](scripts/bluetoothcom-win32.py) binds Windows'
built-in "Standard Serial over Bluetooth link" driver to a paired headset's MDR
RFCOMM service and thus binding it to an actual COM port!

See [Running on the Web](../README.md#running-on-the-web) for usage.

## Credits

[`codegen/BinaryEmbed.cpp`](codegen/BinaryEmbed.cpp) is adapted from

- https://github.com/ocornut/imgui/blob/master/misc/fonts/binary_to_compressed_c.cpp