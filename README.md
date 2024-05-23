# Sony Headphones Client
A continuation of the work done by [Plutoberth/SonyHeadphonesClient](https://github.com/Plutoberth/SonyHeadphonesClient) ,supporting newer devices,like Sony's WF-1000XM5. (and hopefully others as well...)

[![Linux & Windows](https://github.com/mos9527/sonyheadphonesclient/actions/workflows/cmake.yml/badge.svg)](https://github.com/mos9527/SonyHeadphonesClient/actions/workflows/cmake.yml)

[![Github all releases](https://img.shields.io/github/downloads/mos9527/SonyHeadphonesClient/total.svg)](https://GitHub.com/mos9527/SonyHeadphonesClient/releases/)

## DISCLAIMER

- **THIS PROGRAM IS NOT AFFILIATED WITH SONY. YOU ARE RESPONSIBLE FOR ANY DAMAGE THAT MAY OCCUR WHILE USING THIS PROGRAM.**

- This fork has only been confirmed to work with **WF-1000XM5** on Windows. Newer / contemporary devices may work as well, but their support is not guaranteed. **Older devies (i.e. XM4,XM3) won't work due to protocol differences.**

- For older / incompatible devices, please refer to the original repo:[Plutoberth/SonyHeadphonesClient](https://github.com/Plutoberth/SonyHeadphonesClient)

## Screenshot

**NOTE:** May not be up-to-date. See [Features](#features) for all available functionalities!

![image](https://github.com/mos9527/SonyHeadphonesClient/assets/31397301/d564715f-f305-4f86-8303-cfebf67ec00b)

## Download

Binaries for Windows and Linux are available here: [releases page](https://github.com/mos9527/SonyHeadphonesClient/releases).

## Features
- Live update of the following device stats 
  - [x] Track information (i.e. title, album, artist)
  - [x] Sound Pressure
  - [x] Battery Life
- Direct control of the following parameters
  - [x] Volume
  - [x] Ambient Sound / Noise Cancelling toggle
  - [x] Voice Passthrough toggle
  - [x] Ambient Sound strength
  - [x] Voice Guidance Volume *(useful! for debugging...)*
  - [x] Track controls (i.e. play/pause, prev/next)
  - [x] Multipoint Device selection
  - [x] Speak-to-Chat
  - [x] Equalizer  
  - [x] Headphone touch sensor gesture funtion remapping
  - [x] Power Off
  - [ ] Adaptive Sound Control
- Utilities, such as
  - [x] Mapping headphone gestures to Shell Commands

## Supported Platforms And Headsets

### Headsets

* **WF-1000-XM5**: Fully tested with all implemented features
* ...hopefully some more devices can be listed here

### Platforms

- [x] Windows
- [x] Linux  
- [ ] macOS
- [ ] ~~TempleOS~~

---

*Most of the following is copy-pasted from the original repo...*

## For Developers

```git clone --recurse-submodules https://github.com/mos9527/SonyHeadphonesClient.git```

### Compiling
#### Windows
Install [cmake](https://cmake.org/download/) (3.29.3 works) and install [Visual Studio Community 2022](https://visualstudio.microsoft.com/vs/) the C++ components:
* MSVC - VS 2022 C++ x64/x86 build tools
* Windows SDK

From within Visual Studio Run the `Tools --> Command Line --> Developer Command Prompt` and then:
```
cd Client
mkdir build
cd build
cmake ..
cmake --build .
```

#### Linux

Debian / Ubuntu:
```bash
sudo apt install libbluetooth-dev libglew-dev libglfw3-dev libdbus-1-dev
```

Fedora:
```bash
sudo dnf install bluez-libs-devel glew-devel glfw-devel dbus-devel
```

Arch Linux:
```bash
sudo pacman -S bluez glew dbus glfw
```

#### macOS

~~Use the provided xcodeproj file.~~ 

## Contributors

* [Plutoberth](https://github.com/Plutoberth) - Initial Work and Windows Version
* [Mr-M33533K5](https://github.com/Mr-M33533K5) - BT Interface and Other Help
* [semvis123](https://github.com/semvis123) - macOS Version
* [jimzrt](https://github.com/jimzrt) - Linux Version
* [guilhermealbm](https://github.com/guilhermealbm) - Noise Cancelling Switch

## License

Distributed under the [MIT License](https://github.com/Plutoberth/SonyHeadphonesClient/blob/master/LICENSE). See LICENSE for more information.
