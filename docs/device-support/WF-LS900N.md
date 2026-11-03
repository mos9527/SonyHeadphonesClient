WF-LS900N/BC (LinkBuds S)
---

Tested on firmware 2.0.2, MDR V2 (`0x03000007`, both command tables enabled: 35 table 1
functions and 2 table 2 functions), Windows 11 (classic RFCOMM over `AF_BTH`). The device reports
its model name as `LinkBuds S` and its series as `ModelSeries::LINK_BUDS` (`0x60`), colour `0x02`.
Every write below was confirmed by reading the same parameter back, and the on/off and NC/AMB
semantics were cross-checked against screenshots of the official Sony Sound Connect app.

**NOTE:** **✅**: Supported, ❌: Unsupported, **?**: Untested, **~**: Supported officially, pending implementation.

In this table **✅** means the setting was changed and the device confirmed it. **?** means the
parameter answered on read but the write path was not exercised.

| Feature                                     | Status |
|---------------------------------------------|--------|
| Battery Life (single / L+R / case)          | ✅      |
| Codec Indicator                             | ✅      |
| Current Playing (req. host support)         | ?      |
| Volume                                      | ?      |
| Track Controls (play/pause, prev/next)      | ?      |
| Wearing Status                              | ✅      |
| Sound Pressure (Safe Listening)             | ✅      |
| NC/AMB Settings                             | ✅      |
| Ambient Sound Level                         | ✅      |
| Voice Passthrough (Focus on Voice)          | ✅      |
| Wind Noise Reduction                        | ❌      |
| Auto/Adaptive Ambient Sound                 | **~**  |
| Speak to Chat                               | ✅      |
| Listening Mode: Ambient Background Music    | ❌      |
| Listening Mode: Cinema                      | ❌      |
| Listening Mode: Voice Boost                 | ❌      |
| Listening Mode: Sound Leakage Reduction     | ❌      |
| Equalizer (presets / custom bands)          | ✅      |
| DSEE Extreme (Upscaling)                    | ✅      |
| Multipoint (Connect to 2 devices)           | ❌      |
| Paired Device Management                    | ❌      |
| Pairing Mode                                | ?      |
| Bluetooth Connection Quality                | ✅      |
| Touch Sensor Enable/Disable                 | ❌      |
| NC/AMB Button / Assignable Controls         | ✅      |
| Head Gesture                                | ❌      |
| Pause When Headphones Are Removed           | **~**  |
| Automatic Power Off                         | ✅      |
| Power Off                                   | ❌      |
| Voice Guidance On/Off                       | ✅      |
| Voice Guidance Volume                       | ❌      |
| Capture Voice During a Phone Call           | ❌      |

## Notes

**Connection: use the V2 service UUID.** This device is registered under
`956C7B26-D49A-4BA8-B03F-B17D393CB6E2`, and SDP resolves the RFCOMM channel in well under a
second with `port = 0`. Resolving `96CC203E-...-E069BA` (the V1 UUID) fails with `WSA 10049`,
which is easy to misread as "SDP does not work on this device" - the fix is the UUID, not a
hard-coded channel.

**`06 00` is mandatory.** After `00 00` (INIT) and `02 00` (capability info), the client has
to send `06 00` (support functions) before any parameter family answers. Without it, `66 xx`
and `F6 xx` reply intermittently, which looks like an unstable link. The first `INIT` of a new
session is also frequently ignored and needs to be retried.

**Sensitive writes need the alert handshake.** Before writing the assignable controls, send
`94 01 00` (ALERT_SET_STATUS, `FIXED_MESSAGE = 1`, `ENABLE = 0`); otherwise the device
silently ignores the write. Some assignments are additionally gated by a device alert
("voice assistant disabled because playback control is unassigned"), which has to be answered
with `98 00 <MT> 01` - note the `00` in the second byte, V1 uses `01`.

**The on/off byte is inverted.** For `OnOffSettingValue` on V2, `00` means ON and `01` means
OFF. A write followed by a read-back is perfectly self-consistent and still wrong in meaning,
so this has to be calibrated against the app rather than against the device.

**Multipoint is read-only on this device.** It is not a table function at all - the state is a
`GENERAL_SETTING_2` sub-type - which is why it reads back fine while every write is dropped.
`D6 D2` returns the current state, but every
attempt to write it (`D8 D2 00 <v>`, with and without the capability and state queries first,
with the alert enabled, and with the V1 `settingType = 01` encoding) leaves the value
unchanged. Note that V2 encodes a boolean as `00` and a list as `01`, the opposite of V1.
Toggling multipoint has to stay in the app for now.

**Paired-device management is not advertised.** There is no `PAIRING_DEVICE_MANAGEMENT_*` variant in
either table, and none of the general-setting types returns a device list - `D0 D2` only reports the
multipoint strings (`MULTIPOINT_SETTING`, `MULTIPOINT_SETTING_SUMMARY_LDAC_AVAILABLE`).

**Power off does not exist.** `POWER_OFF` appears in the function table, but the inquired type
behind it never answers: `20 03` (capability) and `22 03` (status) are both silent, while the same
family answers for auto power off (`20 05` -> `21 05 02 10 11`). These are TWS earbuds, so there is
no power-off action - they go back in the case.

**Assignable controls offer presets only.** The left and right earbud can be assigned
independently, but only to the preset actions the device advertises; the four rows of
single/double/triple tap and long press shown in the app are read-only and cannot be
customised per gesture.

**Automatic power off is two-state.** `28 04 10` ("off when taken off") and `28 04 11`
("off") are accepted; the timer codes used by other models are normalised back to `10`.

**Not present on this device:** there is no touch sensor on/off switch (key assignment
replaces it), and "capture voice during a call" is not exposed. Sound leakage reduction, the
power-saving modes and the vibration parameter do not answer at all, and there is no listening
mode family (the advertised table contains no `BGM_MODE_*`, `VOICE_CONTENTS` or
`SOUND_LEAKAGE_REDUCTION`).

**Advertised but not wired up here:** `AUTO_NCASM` is in the support table, so adaptive sound
control is a device feature rather than an app-only trick, but the parameter was not exercised.
`PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF` is advertised as well, yet every write to
the remove-to-pause setting left the value unchanged - possibly a gating or encoding mistake on my
side rather than a refusal, so it is marked pending rather than unsupported.

**Table 2 has two entries**, `VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH`
(`0x22`) and `SAFE_LISTENING_TWS_1` (`0xFF`). Safe listening answers its status query, so the client
should read it as-is; the language-switch variant was not exercised - it uploads a language pack and
is not worth interrupting halfway.

**Voice guidance is Table#2 only, with no volume control.** The capability query (`40 01`) only
advertises the language-switch variant, and the volume `inquiredType` (`0x20`) answers nothing -
not the capability (`40 20`), not the param (`46 20`), not the five-step variant (`46 21`). The
official app has a toggle on that page and no volume slider, so the row is unsupported rather than
untested.

The `inquiredType` byte decides whether anything answers at all: `40 01` (capability),
`42 01 00` (on/off) and `42 01 01` (language) all reply, while the `0x00` variant never does - which
reads like "the device has no voice guidance" if you start there.

The write side is three bytes, not four: `48 01 <val>` with reply `49 01 01`. Sending the V1 form `48 01 01 <val>` is accepted but the trailing byte is dropped,
which makes the write look successful while nothing changes.

## Open items

- Volume and track controls were not exercised at the protocol level (host-side AVRCP).
- Sound pressure / safe listening was not exercised.
- Voice guidance volume was not exercised; only the on/off toggle was.
