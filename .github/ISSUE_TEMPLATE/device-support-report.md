---
name: Device Support Report
about: Feature support matrix for your own device.
title: "[Model Name Here]/[Alternative Model Name Here]"
labels: ''
assignees: Amrsatrio, mos9527

---

<!--
Thank you for submitting a feature report for your own device. Before you begin, make sure that:
- Your device is not already listed in docs/device-support
  (https://github.com/mos9527/SonyHeadphonesClient/tree/v1-compat/docs/device-support)
  or in the Issue Tracker (https://github.com/mos9527/SonyHeadphonesClient/issues).
  If it is, but the information there is missing/incorrect, comment on the existing issue or reference that page.
- You're running the latest nightly build (https://nightly.link/mos9527/SonyHeadphonesClient/workflows/cmake/v1-compat?preview),
  or the Web version (https://mos9527.com/SonyHeadphonesClient/).

If so, fill the table below according to your own experience. Any extra information is extremely welcome here!
Remove the rows that your device does not have at all (e.g. NC on open-ear devices), or mark them ❌.

ATTENTION: To keep other users up-to-date, DO NOT close your issue until a maintainer has added your
report to docs/device-support.

Text inside these comment markers is not shown in the issue - you don't need to remove it.
-->

**Device**
<!-- Model, Firmware Version and Series are shown in the "About" tab once connected. -->
 - Model: [e.g. WH-1000XM6]
 - Firmware Version: [e.g. 3.0.0]
 - Series: [e.g. LINK_BUDS]
 - Connection: [Classic / BLE (GATT)]
 - Protocol: [V1 / V2] (the one that connected with **Auto**, or the one you selected)

**Client**
<!-- Copy the version line shown under the app title on the device selection screen, e.g.
"Version: 2.0.0, Branch: v1-compat, Commit: abcdef0, On Windows (x64), en" -->
 - Version line: 
 - OS & version: [e.g. Windows 11 24H2, Ubuntu 24.04, Android 16; plus browser & version for the Web version]

---

**NOTE:** **✅**: Supported, ❌: Unsupported, **?**: Untested, **~**: Supported officially, pending implementation.

| Feature                                          | Status |
|--------------------------------------------------|--------|
| Battery Life (single / L+R / case)               |        |
| Codec Indicator                                  |        |
| Current Playing                                  |        |
| Volume                                           |        |
| Track Controls (play/pause, prev/next)           |        |
| Wearing Status                                   |        |
| Sound Pressure (Safe Listening)                  |        |
| NC/AMB Settings                                  |        |
| Ambient Sound Level / Voice Passthrough          |        |
| Wind Noise Reduction                             |        |
| Auto/Adaptive Ambient Sound                      |        |
| Speak to Chat                                    |        |
| Listening Mode: Ambient Background Music         |        |
| Listening Mode: Cinema                           |        |
| Listening Mode: Voice Boost                      |        |
| Listening Mode: Sound Leakage Reduction          |        |
| Equalizer (presets / custom bands)               |        |
| DSEE (Upscaling)                                 |        |
| Multipoint (Connect to 2 devices simultaneously) |        |
| Paired Device Management (switch / fix / unpair) |        |
| Pairing Mode                                     |        |
| Bluetooth Connection Quality                     |        |
| Touch Sensor Enable/Disable                      |        |
| NC/AMB Button / Assignable Controls              |        |
| Head Gesture                                     |        |
| Pause When Headphones Are Removed                |        |
| Automatic Power Off                              |        |
| Power Off                                        |        |
| Voice Guidance On/Off                            |        |
| Voice Guidance Volume                            |        |
| Capture Voice During a Phone Call                |        |

**Notes**
Anything that behaves differently from the official Sony app, features the client is missing,
error messages shown in the **Device Disconnected** dialog, etc.

**About tab - Features (optional)**
Paste or screenshot the **Features** list from the **About** tab.

**Packet capture (optional, but very welcome)**
Captures let us add your device to the replay tests (`tests/<model>-<firmware version>/`).
Launch the Client with `--record <capture-folder>`, connect, and exercise the features above - or use
**Export ZIP** in the **Device Disconnected** dialog. See the
[packet capture guide](https://github.com/mos9527/SonyHeadphonesClient/blob/v1-compat/tests/README.md).

Captures may contain paired device names, Bluetooth addresses and media metadata. Run
[`tooling/scrub-capture.py`](https://github.com/mos9527/SonyHeadphonesClient/blob/v1-compat/tooling/scrub-capture.py)
over the capture and review what is left before attaching it as a `.zip` file.
