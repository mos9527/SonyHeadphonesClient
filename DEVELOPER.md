# SHC Developer's Guide

This is the best place to start for anyone (humans, coding agents, etc.) who wishes to contribute to **SonyHeadphonesClient** (hereafter abbreviated as "SHC"), or modify it for their own use.

It's recommended that you read [CONTRIBUTING.md](./.github/CONTRIBUTING.md) first before moving on.

## Building

A C++20 compliant compiler and CMake 3.31+ are required. GCC 14, Clang 21 and MSVC 19 have been used for development and are guaranteed to be supported.

Third-party dependencies (see [`contrib`](./contrib/)) are managed by CMake's `FetchContent` and are always statically linked.

**Codegen:** `MDR_ENABLE_CODEGEN` is `ON` by default and requires LLVM/libclang - configuration fails if it cannot be found. See [tooling](./tooling/README.md#setting-up-llvm) for setting up LLVM, or pass `-DMDR_ENABLE_CODEGEN=OFF` to build with the checked-in generated sources.

### Desktop (Windows, macOS, Linux)

#### Linux Dependencies

You need DBus, BlueZ and Fontconfig development packages installed.
- Debian (Ubuntu): `sudo apt install libbluetooth-dev libdbus-1-dev libfontconfig1-dev`
- Fedora: `sudo dnf install bluez-libs-devel dbus-devel fontconfig-devel`
- Arch Linux: `sudo pacman -S bluez dbus fontconfig`

You may also want `bluez-tools`/`bluez-utils` installed for testing.

#### Example

```bash
mkdir build
cd build
cmake ..
cmake --build . --target SonyHeadphonesClient
```

### Web (Emscripten)

- Install the SDK with `emsdk` and verify your installation - https://emscripten.org/docs/getting_started/downloads.html
- Run `emcmake cmake ...` in place of configuration.
  - Or, set up CMake Toolchain variables manually, e.g.
    ```
    -DCMAKE_TOOLCHAIN_FILE=/usr/lib/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_CROSSCOMPILING_EMULATOR=/usr/lib/emsdk/node/20.18.0_64bit/bin/node
    ```
- No extra dependencies are required.
- Shared libraries are unavailable on Emscripten, so `MDR_BUILD_SHARED` defaults to `OFF` and the tests must be disabled with `-DBUILD_TESTING=OFF`.

#### Example

```bash
mkdir build
cd build
emcmake cmake .. -DBUILD_TESTING=OFF
cmake --build . --target SonyHeadphonesClient
```

Run the generated page with `emrun client/index.html` - or host it with any static HTTP server.

#### IntelliJ CLion/Rider

Set up the CMake Toolchain variables above in a new CMake Profile (in CMake options). You can then build from there, and serve the static content within `<build directory>/client/` with any static HTTP server.

### CMake Options

| Option | Default | Description |
| --- | --- | --- |
| `MDR_BUILD_CLIENT` | `ON` | Build the Client app and its platform backends. |
| `MDR_CLIENT_DEBUGGER` | `OFF` | Build the [Protocol Debugger](#the-protocol-debugger) in all configurations. Always built for `Debug` builds and multi-config generators (requires `MDR_BUILD_CLIENT`). |
| `MDR_DEBUG` | `OFF` | Define `MDR_DEBUG` (debug features) in all configurations. Forced `ON` for `Debug` builds; also implies `MDR_ENABLE_LOG`. |
| `MDR_DEBUG_TRAPS` | `OFF` | Break into the debugger (`MDR_TRAP`) when MDR operations / `MDR_CHECK`s fail. `Debug` builds only. |
| `MDR_ENABLE_LOG` | `OFF` | Enable `MDR_LOG` output and source locations in non-`Debug` builds. Always on for `Debug` builds. |
| `MDR_NO_EXCEPTIONS` | `ON` | Compile without C++ exceptions. See [`noexcept` Container Usage](#noexcept-container-usage-libmdr-libmdr-bt-client). |
| `MDR_BUILD_WITH_ASAN` | `OFF` | Build with AddressSanitizer. |
| `MDR_BLE` | `ON` | Build with BLE backend support (defines `MDR_BLE`). |
| `MDR_BUILD_SHARED` | Platform-dependent | Also build `mdr` and `mdr-bt` as shared libraries. Defaults to `ON` where shared libraries are supported (i.e. not Emscripten). Required by the tests. |
| `MDR_ENABLE_CODEGEN` | `ON` | Enable [codegen](./tooling/) targets. Requires LLVM/libclang; configuration fails if LLVM is not found. Set to `OFF` to use the checked-in generated sources. |
| `MDR_STATIC_CRT` | `ON` | **MSVC only.** Statically link the MSVC runtime (`/MT`). Ignored if `CMAKE_MSVC_RUNTIME_LIBRARY` is set. |
| `MDR_DYNAMIC_UCRT` | `ON` | **MSVC only.** With `MDR_STATIC_CRT`, keep the UCRT dynamically linked in non-`Debug` builds. |
| `BUILD_TESTING` | `ON` | Standard CTest option. Builds the C ABI and [replay tests](./tests/) (plus the debugger tests when the Protocol Debugger is built); requires `MDR_BUILD_SHARED=ON`. |

## Debugging (libmdr, libmdr-bt, client)

### The Protocol Debugger

With the CMake option `MDR_CLIENT_DEBUGGER`, the **Protocol Debugger** option becomes available in the Client app's Device Discovery screen, and in the top-left drop-down menu of the main app screen.

The Protocol Debugger allows you to *view, replay, modify and send* manipulated packets to a real device, and is enabled by default in nightly CI builds.

See the [Packet Capture and Test Contribution Guide](./tests/) for usage, bug reporting, and what to do when the device disconnects unexpectedly.

## `noexcept` Container Usage (libmdr, libmdr-bt, client)

SHC's user-facing components, `libmdr`, `libmdr-bt` and `client`, may be compiled w/o exception handling (on supported platforms), with respect to the `MDR_NO_EXCEPTIONS` CMake option, which is `ON` by default.

STL containers and memory allocators throw by default. The C++ `mdr/Protocol.hpp` header thus provides typedefs for STL containers that use a non-throwing allocator (`mdr::MDRAllocator`):

- **ALWAYS** use `mdr::...` containers (`Vector`, `String`, `Deque`, etc.). Avoid using `std::...` containers.
- **ALWAYS** use `mdr::Format` in place of `fmt::format`, which returns a non-throwing `mdr::String`.
- **ALWAYS** use `mdr::MDRAllocator`, `mdr::Construct`, `mdr::Destruct` for manual memory management in place of `std::allocator`, `new`, `delete`.

This in turn means that:
- OOMs and OOB accesses (that are checked, e.g. `operator[]`) **ALWAYS** trigger the [doom function](https://github.com/microsoft/STL/blob/da52dc0fcadbd83690cfd1ed32408d2847439493/stl/inc/__msvc_doom_core.hpp#L24) (`std::abort`, called by `MDR_CHECK`).
- `MDR_CHECK` failures are irrecoverable, and should be caught during development. With `MDR_DEBUG_TRAPS` enabled in `Debug` builds, they also break into your debugger before aborting.

For `tooling` and non-project specific code, there is no restriction on memory management nor exception handling.

## Client Localization (client)

See [Contributing to Client Localization](./client/I18N/README.md) for more info.

## Payload Struct Implementation Details (libmdr)

**NOTE:** This section is for informative purposes only, as `libmdr` now generates the struct details via [codegen](./tooling/) deterministically, and the structs themselves are generated from IR via [tooling/ida](./tooling/ida/).

To regenerate after modifying a source header, run `cmake --build <build-dir> --target codegen`. See [Running codegen](./tooling/README.md#running-codegen).

---

An MDR Payload struct:
- ALWAYS has a `Command` enum field named `command` as the first field, with a default value set to the command it represents.
- ALWAYS implements the `MDRIsSerializable` concept, which means it provides static `Serialize`, `Deserialize` and `Validate` functions to serialize, deserialize and validate itself to/from a byte buffer,
  either through the trivial serialization macro (`MDR_DEFINE_TRIVIAL_SERIALIZATION`) or through external serialization functions (see below).
- MAY contain dynamic array types (vectors or strings), otherwise it should use the trivial serialization macro (see the next section).
- ALWAYS trivially copyable (no user-defined constructors, destructors, or copy/move operators) UNLESS it contains dynamic array types (vectors or strings).
- ALWAYS in standard layout (no virtual functions, no multiple inheritance, all non-static data members have the same access control).

### Trivially Serializable Payloads (PODs)

These are payload structs that can be trivially serialized and deserialized in memory. This implies:
- No dynamic array types (vectors or strings)
- In-memory representation is the same as packet representation (no padding, no endianness issues)
- Enums can be cast in well-defined ways into integers

#### Implementation Details

```c++
#pragma pack(push, 1)
... <extra code>
struct ConnectRetProtocolInfo
{
    // CODEGEN EnumRange Command::CONNECT_RET_PROTOCOL_INFO
    Command command{Command::CONNECT_RET_PROTOCOL_INFO}; // 0x0
    // CODEGEN Ignore OUT_OF_RANGE is expected
    ConnectInquiredType type{ConnectInquiredType::FIXED_VALUE}; // 0x1
    Int32BE protocolVersion{}; // 0x2
    // CODEGEN Ignore OUT_OF_RANGE is expected
    EnableDisable supportTable1Value{EnableDisable::ENABLE}; // 0x6
    // CODEGEN Ignore OUT_OF_RANGE is expected
    EnableDisable supportTable2Value{EnableDisable::ENABLE}; // 0x7

    MDR_DEFINE_TRIVIAL_SERIALIZATION(ConnectRetProtocolInfo);
};
... <extra code>
#pragma pack(pop)
```
- The struct is tightly packed (1-byte alignment, enforced by a `static_assert`).
- The fields are defined in the order they appear in the packet, with appropriate types.
- The `MDR_DEFINE_TRIVIAL_SERIALIZATION` macro defines `memcpy`-based `Serialize` and `Deserialize` functions, and declares `Validate`, whose body is generated by codegen (see [Validation](#validation)).

### Non-Trivially Serializable Payloads

These are payload structs that cannot be trivially serialized and deserialized, usually because they contain dynamic array types (vectors or strings).

```c++
struct ConnectRetSupportFunction
{
    // CODEGEN EnumRange Command::CONNECT_RET_SUPPORT_FUNCTION
    Command command{Command::CONNECT_RET_SUPPORT_FUNCTION}; // 0x0
    // CODEGEN Ignore OUT_OF_RANGE is expected
    ConnectInquiredType type{ConnectInquiredType::FIXED_VALUE}; // 0x1
    MDRPodArray<SupportFunction> supportFunctions; // 0x2

    MDR_DEFINE_EXTERN_SERIALIZATION(ConnectRetSupportFunction);
};
... <extra code>
// NOTE: Auto-generated serialization implementation
// libmdr/src/Generated/ProtocolV2T1Serialization.cpp
MDRResult<size_t> ConnectRetSupportFunction::Serialize(const ConnectRetSupportFunction& data, UInt8* out, size_t maxSize)
{
    UInt8* ptr = out;
    MDR_TRY(size_t, Validate(data));
    MDR_TRY_SIZE(size_t, MDRPod::Write(data.command, &ptr, maxSize));
    MDR_TRY_SIZE(size_t, MDRPod::Write(data.type, &ptr, maxSize));
    MDR_TRY_SIZE(size_t, (MDRPodArray<SupportFunction>::Write)(data.supportFunctions, &ptr, maxSize));
    return MDRResult<size_t>::Success(ptr - out);
}
MDRResult<ConnectRetSupportFunction> ConnectRetSupportFunction::Deserialize(const UInt8* data, size_t maxSize)
{
    ConnectRetSupportFunction out{};
    MDR_TRY_SIZE(ConnectRetSupportFunction, MDRPod::Read(&data, out.command, maxSize));
    MDR_TRY_SIZE(ConnectRetSupportFunction, MDRPod::Read(&data, out.type, maxSize));
    MDR_TRY_SIZE(ConnectRetSupportFunction, (MDRPodArray<SupportFunction>::Read)(&data, out.supportFunctions, maxSize));
    MDR_TRY(ConnectRetSupportFunction, Validate(out));
    return MDRResult<ConnectRetSupportFunction>::Success(std::move(out));
}
```
- The struct has NO alignment requirements (any packing).
- Field declarations have NO ordering requirements, though following the packet order is recommended and has been followed in practice.
- The struct ALWAYS implements external static `Serialize`, `Deserialize` and `Validate` functions to handle the serialization, deserialization and validation logic.
- Helper functions are provided to perform reads/writes on subtypes for fields:
    - ALL `Read`s return `MDRResult<size_t>` containing the number of bytes consumed or an error code, and advance the `*ppSrcBuffer` pointer by the number of bytes read.
    - ALL `Write`s return `MDRResult<size_t>` containing the number of bytes written or an error code, and advance the `*ppDstBuffer` pointer by the number of bytes written.
    - The `maxSize` parameter is the number of bytes remaining in the buffer. `MDR_TRY_SIZE` decrements it by the result of each successful call.
    - The fields may have `Read`/`Write` functions generated by the codegen tool, or manually implemented (see below).
    - Use `MDRPod::Read` and `MDRPod::Write` for basic types and PODs.
    - Use `MDRPodArray<T>` for length-byte-prefixed dynamic arrays of PODs.
    - Use `MDRArray<T>` for length-byte-prefixed dynamic arrays of non-PODs, and `MDRFixedArray<T, Size>` for fixed-size arrays of non-PODs.
    - Use `MDRPrefixedString` for length-byte-prefixed strings, and `MDRPrefixedString16BE` for big-endian `UInt16`-prefixed strings.

### Snippets

#### Read/Write signature

- This is meant for non-trivial Read/Write functions, for **fields** that require special handling.
- Failures are returned through `MDRResult<T>` and never throw.
- Declaration in headers ALWAYS uses the `MDR_DEFINE_EXTERN_READ_WRITE(SubType)` macro.
- Implementation in translation units ALWAYS uses the following signatures:
```c++
    static MDRResult<size_t> Read(const UInt8** ppSrcBuffer, SubType& out, size_t maxSize);
    static MDRResult<size_t> Write(const SubType& data, UInt8** ppDstBuffer, size_t maxSize);
```

#### Non-trivial Serialization/Deserialization signature

- This is meant for non-trivial Serialize/Deserialize functions, for **structs** that require special handling.
- Failures are returned through `MDRResult<T>` and never throw.
- Declaration in headers ALWAYS uses the `MDR_DEFINE_EXTERN_SERIALIZATION(Type)` macro.
- Implementation in translation units ALWAYS uses the following signatures:
```c++
    static MDRResult<size_t> Serialize(const Type& data, UInt8* out, size_t maxSize);
    static MDRResult<Type> Deserialize(const UInt8* data, size_t maxSize);
```
- To have the codegen *exclude* a struct, include `MDR_CODEGEN_IGNORE_SERIALIZATION` within the context
  of the said struct.

### Validation

- ALL structs MUST implement data validation.
  - Codegen ALWAYS generates the `Validate` body for every struct that declares it (i.e. through either serialization macro).
  - Enum fields are automatically checked with `is_valid(...)`; nested structs are inlined and checked recursively.
  - `CODEGEN` comments hint the codegen tool to generate additional validation code for the fields.
  - For details, see the next section.
- Failed checks (`MDR_VALIDATE`) return `MDR_RESULT_ERROR_MALFORMED_PAYLOAD`, and trap the debugger the same way `MDR_TRY` does. Define `MDR_VALIDATION_NO_DEBUG_TRAPS` to disable traps for validation failures only.

For example, the `ConnectRetProtocolInfo` struct above generates:
```c++
MDRResult<void> ConnectRetProtocolInfo::Validate(const ConnectRetProtocolInfo& data) {
    MDR_VALIDATE(is_valid(data.command));
    MDR_VALIDATE(data.command == Command::CONNECT_RET_PROTOCOL_INFO);
    // data.type ignored: OUT_OF_RANGE is expected
    // data.supportTable1Value ignored: OUT_OF_RANGE is expected
    // data.supportTable2Value ignored: OUT_OF_RANGE is expected
    return MDRResult<void>::Success();
}
```

#### `CODEGEN` validation comments

`CODEGEN` comments are one-line markups that provide hints to the codegen tool to generate validation code for the fields.
- Placed directly above the field they apply to.
- Multiple `CODEGEN` comments can be placed above a field, each on its own line.
- General syntax: `CODEGEN [Verb] [Arguments]`

##### Verbs

- `CODEGEN EnumRange [Values...]`

  Declare that the field is an enum, and its value must be one of the specified values.
  Multiple values are separated by spaces. Values are emitted verbatim as `field == value`, so they must be fully qualified, e.g. `CODEGEN EnumRange Command::CONNECT_RET_PROTOCOL_INFO`.

  Do not use field-level `EnumRange` when the enum type declares `OUT_OF_RANGE`.
  Those are open enums: Sound Connect maps unknown wire bytes to the sentinel,
  so a pin rejects values the official client accepts. Use `Ignore` instead.
  Nested `CODEGEN Field … EnumRange` on parent-linked variants is a variant tag,
  not a wire-closed set, and is the remaining exception.
- `CODEGEN Range [Min] [Max]`

  Declare that the field is a numeric type, and its value must be within the specified range (inclusive).
  `Min` and `Max` are parsed as **decimal integers** only; hex literals and named constants are not supported.
- `CODEGEN Field [Field Name] [Verb] [Arguments]`

  Declare that the field is a struct, and the specified verb and arguments apply to the specified field within the struct.
  Nested validation can be achieved by declaring `Field field1.field2 ...` on, e.g. `base` to reach `base.field1.field2`, or
  with `Field field1 Field field2 ...` on the same line.
- `CODEGEN Ignore [reason]`

  Skip automatic and explicit validation for the field. The reason is required and is echoed into
  generated validation sources as `// <field> ignored: [reason]`.
  Use this for every field whose enum type declares `OUT_OF_RANGE`, including
  discriminators (`inquiredType`, `dataType`, `type`). Do not keep a per-payload
  ignore-list; the sentinel on the type is the rule.

##### Array (Iterable) types

Arrays of objects of any type can have their validation code emitted through the codegen as well. This applies to:
- `MDRArray<T>`
- `MDRPodArray<T>`
- `MDRFixedArray<T, Size>`

All prior verbs, if specified, will be applied to _each_ element in the array. Nested structs will be automatically inlined in-place and
emit corresponding code with proper nesting and indentation.

### Summary

#### Macros to use in Headers

- `MDR_DEFINE_TRIVIAL_SERIALIZATION(Type)` for trivially serializable structs.
- `MDR_DEFINE_EXTERN_SERIALIZATION(Type)` for non-trivially serializable structs, where codegen will generate implementations.
  - `MDR_CODEGEN_IGNORE_SERIALIZATION` to exclude a struct from codegen serialization impl generation, and implement manually.
- `MDR_DEFINE_EXTERN_READ_WRITE(SubType)` for non-trivially serializable field struct declarations, where codegen will generate implementations.
  - `MDR_CODEGEN_IGNORE_SERIALIZATION` to exclude a field struct from codegen serialization impl generation, and implement manually.
- `// CODEGEN Ignore [reason]` on a field to exclude it from codegen validation.
