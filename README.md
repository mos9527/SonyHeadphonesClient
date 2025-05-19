# Sony Headphones Client

<center><img src="https://github.com/user-attachments/assets/0c593a3f-148b-4a4d-8a60-e3483ca404bd"/></center>

A fork of [Plutoberth's original SonyHeadphonesClient](https://github.com/Plutoberth/SonyHeadphonesClient), now updated with support for Sony's newer Bluetooth/TWS devices (*specifically* for the earbunds (i.e. WF-1000XM5), see [feature compatibility matrix](#features) for details.) and additional functionalities.

[![Linux & Windows](https://github.com/mos9527/sonyheadphonesclient/actions/workflows/cmake.yml/badge.svg)](https://github.com/mos9527/SonyHeadphonesClient/actions/workflows/cmake.yml)

[![Github all releases](https://img.shields.io/github/downloads/mos9527/SonyHeadphonesClient/total.svg)](https://GitHub.com/mos9527/SonyHeadphonesClient/releases/)

## DISCLAIMER

**THIS PROGRAM IS NOT AFFILIATED WITH SONY. YOU ARE RESPONSIBLE FOR ANY DAMAGE THAT MAY OCCUR WHILE USING THIS PROGRAM.**

## Features

Only the device-specific functions are listed here. If unspecified, the functionalities would be generally available.

If the feature support status for your own device is missing/incorrect/untested here, feel free to submit an [Issue](https://github.com/mos9527/SonyHeadphonesClient/issues/new) or a Pull Request so this table can be kept up-to-date!

**ATTENTION:** Older devices (e.g. XM3s) **will not work** with this fork due to updated BT protocols and are therefore discarded. Use [Plutoberth's original SonyHeadphonesClient](https://github.com/Plutoberth/SonyHeadphonesClient) instead.

Check the [Device Support](docs/device-support/) folder for more details on confirmed per-device function availability.

## Downloads

Windows, macOS and most flavors of Linux are supported OOTB. You can find the latest binaries in the [Releases](https://github.com/mos9527/SonyHeadphonesClient/releases) page.


## For Developers
### Cloning the repo
```git clone --recurse-submodules https://github.com/mos9527/SonyHeadphonesClient.git```

### Building
`cmake` is used for Windows, macOS and Linux builds. You can build this project just like any other `cmake` ones. 

#### Windows (DirectX 11/Win32 APIs)
No extra dependency is required besides the Windows SDK.

Install [cmake](https://cmake.org/download/) (3.29.3 works) and install [Visual Studio Community 2022](https://visualstudio.microsoft.com/vs/) the C++ components:
* MSVC - VS 2022 C++ x64/x86 build tools
* Windows SDK

From within Visual Studio Run the `Tools --> Command Line --> Developer Command Prompt` ,and then:
```
cd Client
mkdir build
cd build
cmake ..
cmake --build .
```

#### Linux (OpenGL/GLFW)

You'll need `DBus` and `libbluetooth`/`bluez` support for BT.

`glfw` will be built alongside the application. Please follow the guide in https://www.glfw.org/docs/3.3/compile.html#compile_deps to ensure its dependencies.

- Debian / Ubuntu:

```bash
sudo apt install libbluetooth-dev libdbus-1-dev
```

- Fedora:

```bash
sudo dnf install bluez-libs-devel dbus-devel
```

- Arch Linux:

```bash
sudo pacman -S bluez dbus
```
The build commands are the same as Windows.

#### macOS (Metal/GLFW)

No extra dependency is required. The resultant binary should be universal (x86_64 and arm64).

`glfw` will be built alongside the application.

```bash
# Install cmake
brew install cmake
```
To build a universal binary, you can use the following commands:
```bash
cd Client
mkdir build
cd build
cmake -DCMAKE_OSX_ARCHITECTURES="x86_64;arm64" ..
cmake --build .
```
An app bundle will be created in the `build` directory named `SonyHeadphonesClient.app`.

## Contributing
### Capturing Bluetooth Packets
See [Packet Capture](docs/packet-capture.md) doc for more info.

## Contributors

* [Plutoberth](https://github.com/Plutoberth) - Initial Work and Windows Version
* [Mr-M33533K5](https://github.com/Mr-M33533K5) - BT Interface and Other Help
* [semvis123](https://github.com/semvis123) - macOS Version
* [jimzrt](https://github.com/jimzrt) - Linux Version
* [guilhermealbm](https://github.com/guilhermealbm) - Noise Cancelling Switch

## License

Distributed under the [MIT License](https://github.com/Plutoberth/SonyHeadphonesClient/blob/master/LICENSE). See LICENSE for more information.
