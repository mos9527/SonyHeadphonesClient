# Sony Headphones Client

A fork of [Plutoberth's original SonyHeadphonesClient](https://github.com/Plutoberth/SonyHeadphonesClient), now updated with support for Sony's newer Bluetooth/TWS devices (*specifically* for the earbunds (i.e. WF-1000XM5), see [feature compatibility matrix](#features) for details.) and additional functionalities.

[![Build](https://github.com/mos9527/sonyheadphonesclient/actions/workflows/cmake.yml/badge.svg)](https://github.com/mos9527/SonyHeadphonesClient/actions/workflows/cmake.yml) 
[![Nightly Builds](https://img.shields.io/badge/nightly-builds-purple)](https://nightly.link/mos9527/SonyHeadphonesClient/workflows/cmake/master?preview)
[![Github all releases](https://img.shields.io/github/downloads/mos9527/SonyHeadphonesClient/total.svg)](https://GitHub.com/mos9527/SonyHeadphonesClient/releases/) 

<center><img width="1154" height="1320" alt="image" src="https://github.com/user-attachments/assets/f6635fe4-aa5f-4292-82a6-a64acd39282f"/></center>

## DISCLAIMER

**THIS PROGRAM IS NOT AFFILIATED WITH SONY. YOU ARE RESPONSIBLE FOR ANY DAMAGE THAT MAY OCCUR WHILE USING THIS PROGRAM.**

## Features

Only the device-specific functions are listed here. If unspecified, the functionalities would be generally available.

If the feature support status for your own device is missing/incorrect/untested here, feel free to submit an [Issue](https://github.com/mos9527/SonyHeadphonesClient/issues/new) or a Pull Request so this table can be kept up-to-date!

**ATTENTION:** Older devices (e.g. XM3s) **will not work** with this fork due to updated BT protocols and are therefore discarded. Use [Plutoberth's original SonyHeadphonesClient](https://github.com/Plutoberth/SonyHeadphonesClient) instead.

Check the [Device Support](docs/device-support/) folder for more details on confirmed per-device function availability.

## Downloads

Windows, macOS and most flavors of Linux are supported OOTB. You can find the latest binaries in the [Releases](https://github.com/mos9527/SonyHeadphonesClient/releases) page.

Nightly builds (builds up-to-date as of the latest commit in `master` branch) for all platforms are also available. You can [download them here](https://nightly.link/mos9527/SonyHeadphonesClient/workflows/cmake/master?preview).

## For Developers
### Cloning the repo
```git clone --recurse-submodules https://github.com/mos9527/SonyHeadphonesClient.git```

### Building
- `cmake` is required for Windows, macOS and Linux builds. A C++23 compliant compiler is also required.
- `glfw` will always be built and statically linked with the application.
Please follow the guide in https://www.glfw.org/docs/3.3/compile.html#compile_deps to ensure its dependencies.
- On non-macOS platforms, OpenGL is used for GUI rendering. Make sure your system has the requisite GL libs intalled. 

#### Windows
Install [cmake](https://cmake.org/download/) (3.29.3 works) and install [Visual Studio Community 2022](https://visualstudio.microsoft.com/vs/) the C++ components:
* MSVC - VS 2022 C++ x64/x86 build tools
* Windows SDK

You can build, and debug the app with [Visual Studio's CMake intergration](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170#ide-integration).

You can also do this from the command line. In Visual Studio Run `Tools --> Command Line --> Developer Command Prompt` ,and then:
```
cd Client
mkdir build
cd build
cmake ..
cmake --build .
```

#### Linux
`DBus` and `libbluetooth`(`bluez`) are required for Bluetooth support.

- Debian / Ubuntu:

```bash
sudo apt install libbluetooth-dev libdbus-1-dev
# For GLFW
sudo apt-get install xorg-dev
```

- Fedora:

```bash
sudo dnf install bluez-libs-devel dbus-devel
```

- Arch Linux:

```bash
sudo pacman -S bluez dbus
```

You can then build the app with CMake, with a C++23 compliant compiler installed.
```
cd Client
mkdir build
cd build
cmake ..
cmake --build .
```


#### macOS
You need to have XCode and CMake installed to build the application.

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
* [Amrsatrio](https://github.com/Amrsatrio) - Work on XM6 and co

## Third-party
* [GLFW](https://www.glfw.org/) - Window and Input
* [Dear ImGui](https://github.com/ocornut/imgui/) - GUI
* [tomlplusplus](https://github.com/marzer/tomlplusplus) - Config file parsing
* [cascadia-code](https://github.com/microsoft/cascadia-code) - Default font supplied with the app

## License

Distributed under the [MIT License](https://github.com/Plutoberth/SonyHeadphonesClient/blob/master/LICENSE). See LICENSE for more information.
