Tooling
===
AST walker and codegen for `libmdr` - with extra helper utilities.

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

`scrub-capture.py` replaces the personal data in a packet capture - paired
device names and addresses, track metadata - with same-length placeholders, so
a capture can be contributed as a test fixture. See
[Contributing to Tests](../tests/README.md#scrub-personal-data).

## Client icon

`make-icon.py` regenerates every platform's icon from
[`client/Platform/AppIcon.png`](../client/Platform/AppIcon.png). Replace that
file, then run `python tooling/make-icon.py` (requires `pip install pillow`)
and commit the outputs.

## README title

`make-title-svg.py` regenerates [`docs/title.svg`](../docs/title.svg)
with the client icon embedded as a bitmap and the title converted to outlines.
Run `python tooling/make-title-svg.py` (requires `pip install fonttools`) and
commit the output.

## Credits

`BinaryEmbed.cpp` is adapted from

- https://github.com/ocornut/imgui/blob/master/misc/fonts/binary_to_compressed_c.cpp