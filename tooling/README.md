Tooling
===
AST walker and codegen for `libmdr` - with extra helper utilities.

## Setting up LLVM
The `[...]Codegen` executables requires LLVM to be built.

### Windows
Get the latest release from https://github.com/llvm/llvm-project/releases.

Setup CMAKE_PREFIX_PATH, and include `C:\Program Files\LLVM` or wherever you'd installed it.

### Linux
You are on your own. Your `clang` package probably includes the requisite libs already - if not, try `llvm-dev`

### macOS
Get `llvm` from Homebrew. That should be enough.

## Credits

`BinaryEmbed.cpp` is adapted from

- https://github.com/ocornut/imgui/blob/master/misc/fonts/binary_to_compressed_c.cpp