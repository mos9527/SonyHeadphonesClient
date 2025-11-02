# Sony Headphones Client

**IMPORTANT:** All current maintenance effort happens in the [`rewrite` Branch](https://github.com/mos9527/SonyHeadphonesClient/tree/rewrite) - Issues/Pull Requests on the `master` branch will **NOT** be processed until the `rewrite` branch is merged.

You may enjoy the latest additions and conformance update in the `rewrite` branch right away **[In Your Browser](https://mos9527.com/SonyHeadphonesClient/)** - support status and more information are available [here](https://github.com/mos9527/SonyHeadphonesClient/tree/rewrite?tab=readme-ov-file#notes-on-web-platform).

---


A fork of [Plutoberth's original SonyHeadphonesClient](https://github.com/Plutoberth/SonyHeadphonesClient), now updated with support for Sony's newer Bluetooth/TWS devices (*specifically* for the earbunds (i.e. WF-1000XM5), see [feature compatibility matrix](#features) for details.) and additional functionalities.

[![Build](https://github.com/mos9527/sonyheadphonesclient/actions/workflows/cmake.yml/badge.svg)](https://github.com/mos9527/SonyHeadphonesClient/actions/workflows/cmake.yml) 
[![Nightly Builds](https://img.shields.io/badge/nightly-builds-purple)](https://nightly.link/mos9527/SonyHeadphonesClient/workflows/cmake/master?preview)
[![Github all releases](https://img.shields.io/github/downloads/mos9527/SonyHeadphonesClient/total.svg)](https://GitHub.com/mos9527/SonyHeadphonesClient/releases/) 

<center><img width="1332" height="1320" alt="image" src="https://github.com/user-attachments/assets/9a1983db-0a88-4315-8725-94e35ce59c83"/></center>

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
You can build, and debug the app with [Visual Studio's CMake intergration](https://learn.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-170#ide-integration), or any
C++ IDE with CMake support of your choosing.

No extra dependency other than the Windows SDK is required. MSVC (Visual Studio) is the recommended compiler for Windows platform.

You can use the following commands to build the binary:
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

You can use the following commands to build the binary:
```
cd Client
mkdir build
cd build
cmake ..
cmake --build .
```


#### macOS
You need to have XCode and CMake installed to build the application.

To build a universal binary (by specifying `-DCMAKE_OSX_ARCHITECTURES="x86_64;arm64"`), you can use the following commands:
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
