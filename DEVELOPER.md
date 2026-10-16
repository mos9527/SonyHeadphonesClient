# SHC Developer's Guide

This is the best place to start for anyone (humans, coding agents, etc.) who wishes to contribute to **SonyHeadphonesClient** (hereafter abbreviated as "SHC"), or modify it for their own use.

It's recommended that you read [CONTRIBUTING.md](./.github/CONTRIBUTING.md) first before moving on.

## Building

A C++20 compliant compiler and CMake 3.31+ are required. GCC 14, Clang 21 and MSVC 19 have been used for development and are guaranteed to be supported.

**Third-party dependencies** (see [`contrib`](./contrib/)) are managed by CMake's `FetchContent` and are always statically linked.

**OS-level dependencies** beyond C/C++ runtime are required **for and only for** `libmdr-bt` and `client` targets. Other build targets are always platform-agnostic (including `libmdr`).

### Desktop (Windows, macOS, Linux)

For these platforms, you can build SHC with relative ease using CMake. Check out the offical [Getting Started with CMake](https://cmake.org/cmake/help/latest/guide/tutorial/Getting%20Started%20with%20CMake.html#build-and-run) guide if you're unfamiliar with the process!

#### Windows

Windows SDK (8.1 and up required for BLE, 7 and up required when `MDR_BT_BLE_SUPPORT` is off) is required.

#### macOS

Xcode (14.0+) is required for C++20 support.

#### Linux

You need DBus, BlueZ and Fontconfig development packages installed.
- Debian (Ubuntu): `sudo apt install libbluetooth-dev libdbus-1-dev libfontconfig1-dev`
- Fedora: `sudo dnf install bluez-libs-devel dbus-devel fontconfig-devel`
- Arch Linux: `sudo pacman -S bluez dbus fontconfig`

#### Example

```bash
mkdir build
cd build
cmake ..
cmake --build . --target SonyHeadphonesClient
```

### Web (Emscripten)

Install the Emscripten SDK with `emsdk` or other means (e.g. `winget`, `brew`...) and verify your installation - https://emscripten.org/docs/getting_started/downloads.html

- Run `emcmake cmake ...` in place of configuration.
  - Or, set up CMake Toolchain variables manually, e.g.
    ```
    -DCMAKE_TOOLCHAIN_FILE=/usr/lib/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake -DCMAKE_CROSSCOMPILING_EMULATOR=/usr/lib/emsdk/node/20.18.0_64bit/bin/node
    ```
  - You can also do this in your IDEs to build Emscripten targets directly in situ.
- No extra dependencies are required.
- Shared libraries are unavailable on Emscripten, so `MDR_BUILD_SHARED` defaults to `OFF` and the tests **must** be disabled with `-DMDR_BUILD_TESTING=OFF`.

#### Example

```bash
mkdir build
cd build
emcmake cmake .. -DMDR_BUILD_CODEGEN=OFF -DMDR_BUILD_TESTING=OFF
cmake --build . --target SonyHeadphonesClient
```

Run the generated page with `emrun client/index.html` - or host it with any static HTTP server.


### Android (Android Studio)

Open `client/Platform/Android` in Android Studio. Relatively new Android SDK, NDK, and CMake should be installed through the Android Studio SDK Manager.

The app targets Android 5.0 (API 21) and later, and by default builds for ARMv7 and ARM64 architectures.
 
**NOTE:** As the Bluetooth functions are only provided via Java/JNI, there's no dedicated `libmdr-bt` backend support, and it's only implemented in the `client`.

**NOTE:** The [Protocol Debugger](#the-protocol-debugger) is available, and packet export is implemented via [Android Sharesheet](https://developer.android.com/develop/ui/compose/sharing/send#using-android-system-sharesheet)

### OpenHarmony (DevEco Studio)

Open `client/Platform/OpenHarmony` in [DevEco Studio](https://developer.huawei.com/consumer/cn/deveco-studio/). OpenHarmony SDK API Version 20 (6.0) or later is required.

[Signing](https://developer.huawei.com/consumer/cn/doc/doccenter-deveco-studio/ide-signing) is required for this to run on a actual device.

#### Documentation

Once you have DevEco installed, you can find all you need *locally* in (change your path accordingly):
- `C:\Program Files\Huawei\DevEco Studio\plugins\openharmony\ohos-info-center-view` 

For more information, these links will also help:
- [开发者文档中心 - 华为HarmonyOS开发者](https://developer.huawei.com/consumer/cn/doc/doccenter)
- [Document Center - HUAWEI Developers](https://developer.huawei.com/consumer/cn/doc/doccenter)
- [libsdl-org/SDL/docs/README-harmonyos.md](https://github.com/libsdl-org/SDL/blob/main/docs/README-harmonyos.md) by [@icculus](https://github.com/icculus)

**NOTE:** As the Bluetooth functions are only provided via `ArkTS` interop, there's no dedicated `libmdr-bt` backend support, and it's only implemented in the `client`.

**NOTE:** The [Protocol Debugger](#the-protocol-debugger) exports packets and ZIP archives through the HarmonyOS system share panel (`Share Kit`), including its **Save as file** action.

### iOS (Xcode)

Xcode (15.0+) and an Apple ID are required. You can generate a Xcode Project from CMake:

```bash
cmake -B build-ios -G Xcode -DCMAKE_SYSTEM_NAME=iOS \
      -DMDR_BUILD_CODEGEN=OFF -DMDR_BUILD_TESTING=OFF -DMDR_BUILD_SHARED=OFF \
      -DCMAKE_OSX_DEPLOYMENT_TARGET=15.0 \
      -DCMAKE_XCODE_ATTRIBUTE_CODE_SIGN_STYLE="Automatic" # If you have a team ID, use `-DCMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM=<TEAM_ID>` instead.
# Open the generated project 
open build-ios/SonyHeadphonesClient.xcodeproj
```

Signing in Xcode is required. You may want to configure **Signing & Capabilities** for the `SonyHeadphonesClient` target first to a valid one, or preconfigure it in the aforementioned CMake commandline.

#### Installing Unsigned IPAs

The CI builds are unsigned (see the [main README](./README.md) for download), and must be re-signed before they can be installed on a device. Any IPA sideloading tool that signs with your own Apple ID works, e.g. [Sideloadly](https://sideloadly.io/), [AltStore](https://altstore.io/) or [SideStore](https://sidestore.io/).

- With a free Apple ID, the signature expires after 7 days and the app has to be re-signed (AltStore/SideStore can refresh it automatically).
- On iOS 16 and later, *Developer Mode* must be enabled (*Settings > Privacy & Security > Developer Mode*) before a development-signed app will launch.
- If signing fails because the bundle identifier is already taken, change it in your sideloading tool.

**NOTE:** Only Bluetooth Classic devices are supported, through the External Accessory (iAP2) protocols `jp.co.sony.songpal.mdr.link` (V1) and `jp.co.sony.songpal.mdr.link2` (V2). Apps using `libmdr-bt` must declare both in `UISupportedExternalAccessoryProtocols`; see [`Info.plist`](./client/Platform/iOS/Info.plist).

**NOTE:** Only one app can hold an accessory session at a time. Quit Sound Connect before connecting.

**NOTE:** Device addresses are derived from the accessory's model and serial numbers, and are not Bluetooth MAC addresses.

**NOTE:** App Store (and possibly external TestFlight) distribution of External Accessory apps requires the accessory manufacturer to whitelist the app. Use development signing or TestFlight internal testing.

**NOTE:** The [Protocol Debugger](#the-protocol-debugger) exports packets and ZIP archives through [`UIActivityViewController`](https://developer.apple.com/documentation/uikit/uiactivityviewcontroller), including **Save to Files** when available.

### CMake Options

#### Common

These apply to `libmdr`, `libmdr-bt`, their shared libraries and the `client`.

| Option | Default | Description |
| --- | --- | --- |
| `MDR_NO_EXCEPTIONS` | `ON` | Compile without C++ exceptions. See also [`noexcept` Container Usage](#noexcept-container-usage-libmdr-libmdr-bt-client). |
| `MDR_DEBUG` | `OFF` | Define `MDR_DEBUG` (debug features) in all configurations. Forced `ON` for `Debug` builds; also implies `MDR_ENABLE_LOG`. |
| `MDR_ENABLE_LOG` | `OFF` | Enable `MDR_LOG` output and source locations in non-`Debug` builds. Always on for `Debug` builds. |
| `MDR_DEBUG_TRAPS` | `OFF` | Define `MDR_DEBUG_TRAPS`. Break into the debugger (`MDR_TRAP`) when MDR operations / `MDR_CHECK`s fail. `Debug` builds only. |

#### Building
| Option | Default | Description |
| --- | --- | --- |
| `MDR_BUILD_TESTING` | `ON` | Build and register the C ABI and [replay tests](./tests/) with CTest. Requires `MDR_BUILD_SHARED=ON`. |
| `MDR_BUILD_SHARED` | Platform-dependent | Also build the `mdr_shared` target and, with `MDR_BUILD_BT=ON`, the `mdr-bt_shared` target. Outputs to `mdr-shared` and `mdr-bt-shared`. Defaults to `ON` where shared libraries are supported (i.e. **not** Emscripten). Required by the tests. |
| `MDR_BUILD_CODEGEN` | `ON` | Build [codegen](./tooling/) tools and targets, including `mdr_codegen` for all generated protocol and debugger files. Requires a native toolchain and LLVM/libclang. Set to `OFF` to use the checked-in generated sources. |
| `MDR_BUILD_WITH_ASAN` | `OFF` | Build with AddressSanitizer enabled. |
| `MDR_MSVC_STATIC_CRT` | `ON` | **MSVC only.** Statically link the MSVC runtime (`/MT`). Ignored if `CMAKE_MSVC_RUNTIME_LIBRARY` is set. |
| `MDR_MSVC_DYNAMIC_UCRT` | `ON` | **MSVC only.** With `MDR_MSVC_STATIC_CRT`, keep the UCRT dynamically linked in non-`Debug` builds. |

#### `libmdr-bt`

| Option | Default | Description |
| --- | --- | --- |
| `MDR_BUILD_BT` | `ON` | Build `libmdr-bt` and its platform Bluetooth backends.  |
| `MDR_BT_BLE_SUPPORT` | `ON` | With `MDR_BUILD_BT=ON`, define `MDR_BT_BLE_SUPPORT` and build optional Bluetooth LE backends alongside Classic Bluetooth backends. |

#### `client`

| Option | Default | Description |
| --- | --- | --- |
| `MDR_BUILD_CLIENT` | `ON` | Build the Client app and its platform backends. Requires `MDR_BUILD_BT=ON`. |
| `MDR_CLIENT_DEBUGGER` | `OFF` | With `MDR_BUILD_CLIENT=ON`, build, link and enable the [Protocol Debugger](#the-protocol-debugger), defining `MDR_CLIENT_DEBUGGER`, in all configurations. Always enabled in `Debug` builds. |


#### Example Configurations

| Configuration | Options |
| --- | --- |
| Client with checked-in generated sources | `MDR_BUILD_CLIENT=ON`, `MDR_BUILD_BT=ON`, `MDR_BUILD_CODEGEN=OFF` |
| Protocol library without Bluetooth backends | `MDR_BUILD_CLIENT=OFF`, `MDR_BUILD_BT=OFF`, `MDR_BUILD_CODEGEN=OFF`, `MDR_BUILD_TESTING=OFF` |
| Codegen targets only | `MDR_BUILD_CLIENT=OFF`, `MDR_BUILD_BT=OFF`, `MDR_BUILD_CODEGEN=ON`, `MDR_BUILD_TESTING=OFF`; build target `mdr_codegen` |

## Debugging (client)

### Logs

See also [Logging - Coding Style](#logging-libmdr-libmdr-bt-client) (note this is by-default `OFF`!). Logs writes to `stderr`, and on some platforms, they are redirected to respective log systems.

Logs are written to `stderr`, and you should see them from the terminal you launched the app from from some platforms, with special cases of:

| Platform | Log System |
| --- | --- |
| Windows| You need `-con` to view the logs with a newly allocated console. |
| Android | Log output is redirected to logcat, with tag `MDR`. |
| iOS | Log output is visible in Xcode console. |
| Web | Logs are automatically redirected to `console.log` by Emscripten. |
| OpenHarmony | **TODO**. Logs are not currently supported! |

### The Protocol Debugger

With the CMake option `MDR_CLIENT_DEBUGGER`, the **Protocol Debugger** option becomes available in the Client app's Device Discovery screen, and in the top-left drop-down menu of the main app screen.

The Protocol Debugger allows you to *view, replay, modify and send* manipulated packets to a real device, and is enabled by default in nightly CI builds.

See the [Packet Capture and Test Contribution Guide](./tests/) for usage, bug reporting, and what to do when the device disconnects unexpectedly.

## Coding Style

Respect `.clang-format` and `.clang-tidy` for formatting and linting. Furthermore, the following rules always apply.

### Exception Handling

**C++ Exceptions are optional, and should be considered disabled by default.**

SHC's user-facing components, `libmdr`, `libmdr-bt` and `client`, may be compiled w/o exception handling (on supported platforms), with respect to the `MDR_NO_EXCEPTIONS` CMake option, which is `ON` by default.

STL containers and memory allocators throw by default. The C++ `mdr/Protocol.hpp` header thus provides typedefs for STL containers that use a non-throwing allocator (`mdr::MDRAllocator`):

- **ALWAYS** use `mdr::...` containers (`Vector`, `String`, `Deque`, etc.). Avoid using `std::...` containers.
- **ALWAYS** use `mdr::Format` in place of `fmt::format`, which returns a non-throwing `mdr::String`.
- **ALWAYS** use `mdr::MDRAllocator`, `mdr::Construct`, `mdr::Destruct` for manual memory management in place of `std::allocator`, `new`, `delete`.

This in turn means that:
- OOMs and OOB accesses (that are checked, e.g. `operator[]`) **ALWAYS** trigger the [doom function](https://github.com/microsoft/STL/blob/da52dc0fcadbd83690cfd1ed32408d2847439493/stl/inc/__msvc_doom_core.hpp#L24) (`std::abort`, called by `MDR_CHECK`).
- `MDR_CHECK` failures are irrecoverable, and should be caught during development. With `MDR_DEBUG_TRAPS` enabled in `Debug` builds, they also break into your debugger before aborting.

For `tooling` and non-project specific code, there is no restriction on memory management nor exception handling.

### `std::ranges`, `std::views` Libraries

**No usage whatsoever is allowed.** 

- Portability is a *requirement* for SHC. Some platforms we support (e.g. older Linuxes) are stuck with [older compilers](https://cppreference.com/cpp/compiler_support) that only offered partial coverage for these libraries.
- Furthermore, usage of which *inadvertently* lead to compile-time, codegen bloat and sometimes runtime performance regressions due to excessive template instantiation, heavy header parsing, and possible runtime cost (e.g. `std::views::filter_view`) that's otherwise avoidable.

Whenever you'd like to trade performance for syntactic sugar, **do not**. Stick with standard `<algorithm>` library and for-loops that's both easier to write/comprehend and faster to compile/run.

### C ABI (libmdr, libmdr-bt)

`libmdr` provides stable ABI **per ABI version**. This is defined by `MDR_ABI_VERSION` in `libmdr/include/mdr-c/Base.h`.

**Bump this version by `1` if you are breaking ABI**, for example:
- *Removing* functions (note: adding new functions while adhering to the rest of the rules is allowed) in C-headers.
- *Changing* function signatures (this includes changing the underlying structs) in C-headers.
  
`libmdr-bt` provides stable ABI, and is **guaranteed** to not change between `libmdr` ABI versions, with the sole exception for **adding** platform backends.

- **NOTE:** `git blame` the `#define MDR_ABI_VERSION` line to see all ABI-breaking changes introduced to `libmdr`.

### C ABI Design (libmdr)

Properties can be **read-only**, **read-write**, or **write-only**. 

**Read-only** properties should provide **only** a getter function.
- The function signature is as follows (`MDRHeadphones*, PropertyPointer(s)...`):
  ```c++
  mdrHeadphonesGetEqualizerPresets(MDRHeadphones* headphones, MDREqualizerPreset* presets, uint32_t* inout_count)
  ```

For **read-write** properties, the `MDRPropertyType type` parameter selects the getter's cached view:
- `MDR_PROPERTY_CURRENT` returns the latest device-reported value, or its default before any report. Getters do not request fresh data, and ACKs do not update ordinary property values.
- `MDR_PROPERTY_DESIRED` preserves local edits made through `mdrHeadphonesSet...` until completion. With no outstanding local write, subsequent device reports update this view too.
- Read-only fields in a mixed structure use the same value in both views. One-shot action fields may reset after completion.
- Acknowledgement clears the outstanding write, not the difference between the two views. `CLEAN` does not guarantee `mDesired == mCurrent` or that the device accepted the target value.
- The internal lifecycle is `CLEAN` -> `STAGED` -> `SUBMITTED` -> `CLEAN`. The submitted payload is frozen; state-changing setters are rejected while a request is in progress. Removing revision tracking depends on this restriction remaining in place.
- `mdrHeadphonesRequestSync` refreshes only the protocol family's implemented synchronization subset; it is not a readback of every writable property.

**Read-write** properties should provide **both** a getter and a setter function.
- `MDRPropertyType type` **MUST** be the getter's **FIRST** argument after `MDRHeadphones*`. Setters do not take a view selector; they stage the desired value.
- The getter signature is as follows (`MDRHeadphones*, MDRPropertyType, PropertyPointer(s)...):
```c++
mdrHeadphonesGetPower(MDRHeadphones* headphones, MDRPropertyType type, MDRPower* out_power)
```

**Write-only** property should provide **only** a setter function.
- For example, `mdrHeadphonesSetAlertResponse(MDRHeadphones* headphones, MDRAlertAction action)` stages an alert response without exposing a getter.
- Like read-write setters, write-only setters stage changes, make `mdrHeadphonesIsDirty` return true, and send them only through `mdrHeadphonesRequestCommit`.
- One-shot values such as alert responses are consumed after acknowledgement so the same action can be staged for a later request.

### Non-blocking IO (`MDRConnection`)

**None of the `MDRConnection` functions should block.** Bluetooth backends may be implemented in `libmdr-bt`, the `client`, or downstream consumers by providing their own `MDRConnection` VTable, which `libmdr` uses to communicate with the device.

`libmdr` is a by essence state machine that **only** advances when manually polled, through `mdrHeadphonesPoll`, which depends on the `MDRConnection`, therefore impling non-blocking (polling) IO.


### Logging (libmdr, libmdr-bt, client)

Logging can be enabled via the CMake option `MDR_ENABLE_LOG` and is by default `OFF` in non-Debug builds. 

`libmdr`, `libmdr-bt`, and `client` all use the `MDR_LOG` macro for logging, which is a wrapper around `mdr::Format` that prints to `stderr` with `fmt::format`. 

Example:

```cpp
MDR_LOG("Device disconnected")
MDR_LOG("BluetoothFindFirstDevice FAILED on radio #{} error={}", radioIndex, err)
```

For the log strings themselves:
- **DO NOT** include tags (e.g. `MDR_LOG("[BT] Device disconnected")`), or `MDR_SOURCE_LOCATION` in your log string. The strings themselves *should* be self-explanatory and trivally searchable to cross-reference with source.
- For strings that's only meant for a debug build, use `MDR_LOG_DEBUG`, which is automatically stripped in non-Debug builds even if `MDR_ENABLE_LOG` is `ON`.
- There's no log levels, and platform backends that outputs them with one should opt for `INFO` level (e.g. logcat).

### Payload Struct Implementation Details (libmdr)

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
