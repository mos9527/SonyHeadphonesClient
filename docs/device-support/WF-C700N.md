WF-C700N
---

Tested on firmware 2.0.5 and 2.0.7, MDR V2 (protocol `0x03000015`), with both command
tables enabled. The device reports its model name as `WF-C700N` and its series as
`ModelSeries::NO_SERIES` (`0x00`). No packet capture is committed for this device.

**NOTE:** **✅**: Supported, ❌: Unsupported, **?**: Untested, **~**: Supported officially, pending implementation.

In this table **✅** means the device advertises the function, libmdr requests it, and the
device answered during initialization. Ambient sound control, volume and playback were
additionally exercised in the write direction against the device.

| Feature                                  | Status |
|------------------------------------------|--------|
| Current Playing (req. host support)      | ✅      |
| Battery Life (L/R + case)                | ✅      |
| Volume                                   | ✅      |
| Track Controls (play/pause, prev/next)   | ✅      |
| NC/AMB Settings                          | ✅      |
| Voice Guidance                           | ✅      |
| Multipoint Control (device change)       | ✅      |
| Fix Playback Device (Source Switch)      | ✅      |
| Equalizer                                | ✅      |
| Touch Sensor Gesture                     | ✅      |
| Power Off                                | ✅      |
| DSEE (Upscaling)                         | ✅      |
| Bluetooth Connection Quality             | ✅      |
| Automatic Power Off                      | ✅      |
| Voice Guidance Volume                    | ❌      |
| Sound Pressure                           | ❌      |
| Listening Modes                          | ❌      |
| Speak to Chat                            | ❌      |
| Head Gesture                             | ❌      |
| Pause When Headphones Are Removed        | ❌      |
| Adaptive Volume Control                  | ❌      |
| Quick Access                             | ❌      |
| Auto Play                                | ❌      |
| Link Auto Switch                         | ❌      |
| Adaptive Sound Control (`AUTO_NCASM`)    | **~**  |
| Wide Area Tap (repeat-tap training)      | **~**  |

Notes:

- **NC/AMB Settings** use `MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT`,
  which maps to inquired type `MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS` (`0x17`).
  Confirmed against the device rather than inferred: it answers `NCASM_GET_PARAM 0x17` during
  initialization and notifies with `0x17` unprompted when the button is used.
- **Firmware 2.0.5 accepts NC/AMB writes and does not act on them.** The device acknowledges
  each `NCASM_SET_PARAM`, answers it with an `NCASM_NTFY_PARAM` reporting exactly the state
  that was asked for, and stays in the mode it was already in. Only its own button changes
  anything, and only the button produces a `LOG_NTFY_PARAM` operation record; a write from the
  client produces none. **Firmware 2.0.7 fixes this** - noise cancelling, ambient sound and off
  all switch from the client. There is nothing to work around in libmdr; the frames it sends
  are identical on both firmwares and match Sony's own payload definition field for field.
  Changes made from the client play no confirmation sound on either firmware. Only the button
  does, so silence is not a symptom here.
- **Adaptive Sound Control**: the device advertises `AUTO_NCASM`, which libmdr neither reads
  nor writes. It is not the same thing as `MDR_FEATURE_ADAPTIVE_AMBIENT_SOUND`, which follows
  `MODE_NC_ASM_..._NOISE_ADAPTATION` and is absent here.
- **Voice Guidance Volume**: the device advertises
  `VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH`, the
  variant without volume adjustment, so the guidance can be switched but not made quieter.
- **Sound Pressure**: no `SAFE_LISTENING_*` function is advertised.
- **Listening Modes, Speak to Chat, Head Gesture, Pause When Headphones Are Removed**: the
  device advertises no corresponding function, so nothing is requested for them.
- **Multipoint** is exposed through `GENERAL_SETTING_1`, whose capability names it
  `MULTIPOINT_SETTING`, alongside `PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT` in table 2.

### Advertised functions

Table 1 (23):

```
CONCIERGE_DATA                                   CONNECTION_STATUS
CODEC_INDICATOR                                  UPSCALING_INDICATOR
BLE_SETUP                                        UNNECESSARY_AUTO_RECONNECTION
LEFT_RIGHT_BATTERY_LEVEL_INDICATOR               POWER_OFF
AUTO_POWER_OFF                                   TANDEM_KEEP_ALIVE
CRADLE_BATTERY_LEVEL_WITH_THRESHOLD              FW_UPDATE_MTK_TRANSFER_WITHOUT_DISCONNECTION
PRESET_EQ
MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT
AUTO_NCASM                                       FIXED_MESSAGE
PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT  ACTION_LOG_NOTIFIER
TIME_SERIES_OPERATIONLOG_NOTIFIER
CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY
UPSCALING_AUTO_OFF                               GENERAL_SETTING_1
ASSIGNABLE_SETTING
```

Table 2 (4):

```
VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH
PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT
SOURCE_SWITCH_CONTROL
REPEAT_TAP_TRAINING_MODE
```
