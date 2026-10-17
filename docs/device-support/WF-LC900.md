WF-LC900 (LinkBuds Clip)
---

Tested on firmware 2.0.3, MDR V2 (protocol `0x03003015`), with both command tables
enabled. The device reports its model name as `LinkBuds Clip` and its series as
`ModelSeries::LINK_BUDS` (`0x60`). Packet captures:
[`tests/WF-LC900-2.0.3/`](../../tests/WF-LC900-2.0.3/) (initialization and sync),
[`tests/WF-LC900-2.0.3-listening/`](../../tests/WF-LC900-2.0.3-listening/) (every listening
mode and every background-music distance) and
[`tests/WF-LC900-2.0.3-equalizer/`](../../tests/WF-LC900-2.0.3-equalizer/) (the preset
capability, and Heavy, Clear, Hard and Soft applied in turn). Paired device names, their
addresses and track metadata are replaced with placeholders in all three.

**NOTE:** **✅**: Supported, ❌: Unsupported, **?**: Untested, **~**: Supported officially, pending implementation.

In this table **✅** means the device advertises the function, libmdr requests it, and the
device answered during initialization. The write/toggle path was not exercised feature by
feature.

| Feature                                  | Status |
|------------------------------------------|--------|
| Current Playing (req. host support)      | ✅      |
| Sound Pressure                           | ✅      |
| Battery Life (L/R + case)                | ✅      |
| Volume                                   | ✅      |
| Track Controls (play/pause, prev/next)   | ✅      |
| NC/AMB Settings                          | ❌      |
| Voice Guidance                           | ✅      |
| Voice Guidance Volume                    | ✅      |
| Multipoint Control (device change)       | ✅      |
| Fix Playback Device (Source Switch)      | ✅      |
| Equalizer                                | ✅      |
| Touch Sensor Gesture                     | ✅      |
| Power Off                                | ✅      |
| DSEE (Upscaling)                         | ✅      |
| Listening Mode: Ambient Background Music | ✅      |
| Listening Mode: Voice Boost              | ✅      |
| Listening Mode: Sound Leakage Reduction  | ✅      |
| Bluetooth Connection Quality             | ✅      |
| Cinema Upmix                             | ❌      |
| Pause When Headphones Are Removed        | ❌      |
| Speak to Chat                            | ❌      |
| Head Gesture                             | ❌      |
| Automatic Power Off                      | ❌      |
| Adaptive Volume Control                  | **~**  |
| Quick Access                             | **~**  |
| Auto Play                                | **~**  |
| Link Auto Switch                         | **~**  |
| Wide Area Tap (repeat-tap training)      | **~**  |

Notes:

- **NC/AMB, Speak to Chat, Head Gesture, Automatic Power Off**: the device advertises no
  corresponding function at all — it is an open-ear design with no noise cancelling.
  libmdr correctly sends no `NCASM` command during the whole session.
- **Listening modes** are one exclusive setting - at most one is active, and Standard means
  none is. This device offers three of them (`BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE`,
  `VOICE_CONTENTS`, `SOUND_LEAKAGE_REDUCTION`), matching the four options Sound Connect
  shows. All three, and all three background-music distances, are confirmed against the
  device in the listening capture. The device confirms each change by notifying the state of
  every mode, not just the one that changed.
- **The equalizer and DSEE are switched off while any listening mode is active.** The device
  reports this with `EQEBB_NTFY_STATUS PRESET_EQ DISABLE` and
  `AUDIO_NTFY_STATUS UPSCALING DISABLE`, and reports them available again on Standard. Both
  reach the client through `MDREqualizer.available` and `MDREqualizer.dsee_available`, which
  are about what the device will act on right now - `MDR_FEATURE_EQUALIZER` and
  `MDR_FEATURE_DSEE` stay set throughout, because the hardware still has both. libmdr no
  longer relies on those notifications alone: applying a listening mode asks for both
  statuses, since nothing else ever re-reads them. Confirmed on 2.0.3 - a client that only
  waited for the notification kept the equalizer on screen as usable through every mode
  switch, and one that asks greys it out with the upscaling, every time.
- **The equalizer preset list** comes from `EQEBB_GET_CAPABILITY PRESET_EQ`, which the first
  two captures predate. The device answers with `band` 10, `step` 13 and eight presets -
  `OFF`, `HEAVY`, `CLEAR`, `HARD`, `SOFT`, `CUSTOM`, `USER_SETTING1`, `USER_SETTING2` - and
  **an empty name for every one of them**, so `MDR_TEXT_EQUALIZER_PRESET_NAME` is empty here
  and a UI needs its own labels. Ten bands of thirteen steps is the ±6 range libmdr assumes
  for a ten-band device, confirmed rather than inferred.
- **Applying a preset must not carry band steps.** The device recomputes the curve for the
  preset it was given and notifies it; band steps sent alongside are what defines a *custom*
  EQ, so it answers by switching to `CUSTOM` and dropping the preset. The equalizer capture
  above was taken before the commit paths learned to tell the caller's intent from the
  device's own report, so the device's switch to `CUSTOM` after each of Heavy, Clear, Hard
  and Soft is in it.
- **Cinema Upmix**: the device advertises `LISTENING_OPTION` but not `UPMIX_CINEMA`, so the
  Cinema mode does not exist here and is no longer offered. In the capture above, taken
  before the per-mode gating existed, the request is acknowledged and then never answered.
- **Pause When Headphones Are Removed**: the device does not advertise
  `PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF`. libmdr no longer requests it; the
  capture above still shows the unconditional request, acknowledged and unanswered.
- The device pushes an unsolicited `PLAY_NTFY_PARAM` immediately after connecting, at a
  point that varies between runs. Landing before `CONNECT_RET_PROTOCOL_INFO` used to abort
  the session, and its effect on the sequence counter used to desynchronize the exchange;
  both are handled as of the capture above.

### Advertised functions

Table 1 (30):

```
CONCIERGE_DATA                                   CONNECTION_STATUS
CODEC_INDICATOR                                  UPSCALING_INDICATOR
BLE_SETUP                                        TUTORIAL_CONTENTS_SELECT_ON_CONCIERGE
UNNECESSARY_AUTO_RECONNECTION                    PHONE_AND_CONNECTED_DEVICE_INFOMATION_FOR_CLASSIC
POWER_OFF                                        TANDEM_KEEP_ALIVE
LR_BATTERY_LEVEL_WITH_THRESHOLD                  CRADLE_BATTERY_LEVEL_WITH_THRESHOLD
FW_UPDATE_MTK_TRANSFER_WITHOUT_DISCONNECTION     PRESET_EQ
FIXED_MESSAGE                                    PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT
GATT_CONNECTABLE                                 INTEGRATED_AUTO_PLAY
ACTION_LOG_NOTIFIER                              GENERAL_SETTING_2
GENERAL_SETTING_3                                CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY
UPSCALING_AUTO_OFF                               BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE
LISTENING_OPTION                                 VOICE_CONTENTS
SOUND_LEAKAGE_REDUCTION                          ASSIGNABLE_SETTING
AUTO_VOLUME                                      QUICK_ACCESS
```

Table 2 (7):

```
SOURCE_SWITCH_CONTROL
PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT
VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT
SAFE_LISTENING_TWS_1
REPEAT_TAP_TRAINING_MODE
QUICK_ACCESS_EASY_SETTING
LINK_AUTO_SWITCH_FOR_HEADSETS
```
