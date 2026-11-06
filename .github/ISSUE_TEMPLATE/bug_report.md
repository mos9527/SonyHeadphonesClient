---
name: Bug report
about: Report a problem with the Client, libmdr or libmdr-bt
title: '[Model Name Here] Short description of the problem'
labels: 'bug'
assignees: ''

---

<!--
Before submitting, please make sure that:
- The issue still happens on the latest nightly build (https://nightly.link/mos9527/SonyHeadphonesClient/workflows/cmake/v1-compat?preview)
  or on the Web version (https://mos9527.com/SonyHeadphonesClient/).
- It has not already been reported in the Issue Tracker (https://github.com/mos9527/SonyHeadphonesClient/issues).
- If it concerns a feature of your device, check its page in docs/device-support
  (https://github.com/mos9527/SonyHeadphonesClient/tree/v1-compat/docs/device-support) for known limitations.
-->

**Describe the bug**
A clear and concise description of what the bug is.

**To Reproduce**
Steps to reproduce the behavior:
1. Connect to the device via '...' (Classic/BLE, Auto/V1/V2)
2. Open the '...' tab
3. Change '...'
4. See error

**Expected behavior**
A clear and concise description of what you expected to happen.

**Error messages / Screenshots**
If the device disconnected, copy the `Connection: ...` and `Headphones: ...` lines shown in the **Device Disconnected** dialog.
If applicable, add screenshots to help explain your problem.

**Client**
<!-- Copy the version line shown under the app title on the device selection screen, e.g.
"Version: 2.0.0, Branch: v1-compat, Commit: abcdef0, On Windows (x64), en" -->
 - Version line: 
 - Build: [e.g. nightly / Web / compatibility build / built from source]
 - OS & version: [e.g. Windows 11 24H2, Ubuntu 24.04 (BlueZ 5.72), macOS 15, Android 16, iOS 18]
 - Browser & version (Web version only): [e.g. Chrome 150]

**Device**
<!-- Model, Firmware Version and Series are shown in the "About" tab once connected. -->
 - Model: [e.g. WH-1000XM6]
 - Firmware Version: [e.g. 3.0.0]
 - Connection: [Classic / BLE (GATT)]
 - Protocol: [Auto / V1 / V2]

**Logs (optional)**
Logs are written to `stderr`. On Windows, launch with `-con` to open a log console; on Android use `logcat` (tag `MDR`);
on the Web version, open the browser's developer console. See [Logs](https://github.com/mos9527/SonyHeadphonesClient/blob/v1-compat/DEVELOPER.md#logs).

```
Paste relevant log output here
```

**Packet capture (optional)**
If the bug involves communication with your headphones, a packet capture helps a lot:
- After a disconnect error, use **Export ZIP** in the **Device Disconnected** dialog (also reachable via **Trigger disconnect error** in the top-left drop-down menu), or
- Launch the Client with `--record <capture-folder>` and reproduce the problem.

See the [packet capture guide](https://github.com/mos9527/SonyHeadphonesClient/blob/v1-compat/tests/README.md) for details,
then attach the capture as a `.zip` file. Include the headphone model and firmware version in the archive name.

Captures may contain paired device names, Bluetooth addresses and media metadata. Run
[`tooling/scrub-capture.py`](https://github.com/mos9527/SonyHeadphonesClient/blob/v1-compat/tooling/scrub-capture.py)
over the capture and review what is left before uploading; do not attach a capture if you are unsure what it contains.

**Additional context**
Add any other context about the problem here.
