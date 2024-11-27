# Sony Headphones Client

<center><img src="https://github.com/user-attachments/assets/0c593a3f-148b-4a4d-8a60-e3483ca404bd"/></center>

A fork of [Plutoberth's original SonyHeadphonesClient](https://github.com/Plutoberth/SonyHeadphonesClient), now updated with support for Sony's newer Bluetooth/TWS devices (*specifically* for the earbunds (i.e. WF-1000XM5), see [feature compatibility matrix](#features) for details.) and additional functionalities.

[![Linux & Windows](https://github.com/mos9527/sonyheadphonesclient/actions/workflows/cmake.yml/badge.svg)](https://github.com/mos9527/SonyHeadphonesClient/actions/workflows/cmake.yml)

[![Github all releases](https://img.shields.io/github/downloads/mos9527/SonyHeadphonesClient/total.svg)](https://GitHub.com/mos9527/SonyHeadphonesClient/releases/)

## DISCLAIMER

**THIS PROGRAM IS NOT AFFILIATED WITH SONY. YOU ARE RESPONSIBLE FOR ANY DAMAGE THAT MAY OCCUR WHILE USING THIS PROGRAM.**


## Download

Binaries for Windows and Linux are available here: [releases page](https://github.com/mos9527/SonyHeadphonesClient/releases).

## Features

Only the device-specific functions are listed here. If unspecified, the functionalities would be generally available.

If the feature support status for your own device is missing/incorrect/untested here, feel free to submit an [Issue](https://github.com/mos9527/SonyHeadphonesClient/issues/new) or a Pull Request so this table can be kept up-to-date!

**NOTE:** **✅**: Supported, ❌: Unsupported, **?**: Untested, **~**: Supported offically, pending implementation.

| Feature                                      | WF-1000XM5 (Earbuds) | WH-1000XM5 (Over-ear) | WF-LS900N/BC (LinkBuds S)                                    |
| -------------------------------------------- | -------------------- | --------------------- | ------------------------------------------------------------ |
| Current Playing (req. host support)          | ✅                    | ?                     | ?                                                            |
| Sound Pressure                               | ✅                    | ?                     | ?                                                            |
| Battery Life                                 | ✅                    | ?                     | ?                                                            |
| Volume                                       | ✅                    | ?                     | ?                                                            |
| NC/AMB Settings                              | ✅                    | ?                     | ?                                                            |
| Voice Guidance Volume                        | ✅                    | ?                     | ?                                                            |
| Track controls (play/pause, prev/next)       | ✅                    | ?                     | ?                                                            |
| Multipoint Control (device change)           | ✅                    | ?                     | ?                                                            |
| Speak to Chat                                | ✅                    | ?                     | ?                                                            |
| Equalizer                                    | ✅                    | ?                     | ?                                                            |
| Touch Sensor Gesture                         | ✅                    | ?                     | ?                                                            |
| Power Off                                    | ✅                    | ?                     | ?                                                            |
| Capture Voice During a Phone Call            | ✅                    | ?                     | ❌ ([#1](https://github.com/mos9527/SonyHeadphonesClient/pull/1)) |
| Mapping headphone gestures to Shell Commands | ✅                    | ?                     | ?                                                            |

## Platforms

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

#### Linux (GLEW(OpenGL)/GLFW)

You'll need `DBus` and `libbluetooth`/`bluez` support for BT and `glfw`,`glew` for building the GUI on your distro.

- Debian / Ubuntu:

```bash
sudo apt install libbluetooth-dev libglew-dev libglfw3-dev libdbus-1-dev
```

- Fedora:

```bash
sudo dnf install bluez-libs-devel glew-devel glfw-devel dbus-devel
```

- Arch Linux:

```bash
sudo pacman -S bluez glew dbus glfw
```

#### macOS (Metal/GLFW)

`glfw` is required for GUI. This should build on most Macs, including Apple Silicon ones.

```bash
brew install glfw
```

## Contributors

* [Plutoberth](https://github.com/Plutoberth) - Initial Work and Windows Version
* [Mr-M33533K5](https://github.com/Mr-M33533K5) - BT Interface and Other Help
* [semvis123](https://github.com/semvis123) - macOS Version
* [jimzrt](https://github.com/jimzrt) - Linux Version
* [guilhermealbm](https://github.com/guilhermealbm) - Noise Cancelling Switch

## License

Distributed under the [MIT License](https://github.com/Plutoberth/SonyHeadphonesClient/blob/master/LICENSE). See LICENSE for more information.
