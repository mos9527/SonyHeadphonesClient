Tooling
===
AST walker and codegen for `libmdr` - with extra helper utilities.

## Running codegen

Codegen is a proper CMake dependency. Modify a source header and re-run:

```sh
cmake --build <build-dir> --target codegen
```

This rebuilds only the stale tools and regenerates only the affected files.
A full `cmake --build` will also regenerate before compiling `mdr`.

## Setting up LLVM
The `[...]Codegen` executables require LLVM.

### Windows
Get the latest release from https://github.com/llvm/llvm-project/releases.

Set `CMAKE_PREFIX_PATH` to include `C:\Program Files\LLVM` (or wherever installed).

### Linux
Your `clang` package likely includes the requisite libs already — if not, try `llvm-17-dev libclang-17-dev`.

### macOS
Install `llvm` from Homebrew — CMake will locate it automatically.

## Credits

`BinaryEmbed.cpp` is adapted from

- https://github.com/ocornut/imgui/blob/master/misc/fonts/binary_to_compressed_c.cpp