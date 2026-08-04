#pragma once

#include "Base.h"
#include "Connection.h"
#include <stddef.h>
#include <stdint.h>

/*
 * Canonical, protocol-neutral C API for headphone state and control.
 *
 * All public structures are extensible: callers set struct_size to sizeof the
 * structure before passing it. Getters accept CURRENT (device-confirmed) or
 * EFFECTIVE (current state plus locally staged changes). Setters stage effective
 * state; MDR_OPERATION_APPLY sends all staged changes.
 *
 * Text and arrays always use caller-owned storage. A NULL buffer with an
 * in/out count of zero queries the required element count, including the NUL
 * terminator for text.
 */

#if !defined(MDR_DEPRECATED)
#if defined(_MSC_VER)
#define MDR_DEPRECATED(message) __declspec(deprecated(message))
#elif defined(__GNUC__) || defined(__clang__)
#define MDR_DEPRECATED(message) __attribute__((deprecated(message)))
#else
#define MDR_DEPRECATED(message)
#endif
#endif

/** Opaque headphone session. The associated connection remains caller-owned. */
typedef struct MDRHeadphones MDRHeadphones;

typedef uint8_t MDRBoolean;
#define MDR_FALSE ((MDRBoolean)0u)
#define MDR_TRUE ((MDRBoolean)1u)

typedef uint8_t MDRStateView;
#define MDR_STATE_CURRENT ((MDRStateView)0u)
#define MDR_STATE_EFFECTIVE ((MDRStateView)1u)

typedef uint8_t MDRFeatureAvailability;
#define MDR_FEATURE_UNKNOWN ((MDRFeatureAvailability)0u)
#define MDR_FEATURE_UNAVAILABLE ((MDRFeatureAvailability)1u)
#define MDR_FEATURE_AVAILABLE ((MDRFeatureAvailability)2u)

typedef uint16_t MDRDomain;
#define MDR_DOMAIN_NONE ((MDRDomain)0u)
#define MDR_DOMAIN_IDENTITY ((MDRDomain)1u)
#define MDR_DOMAIN_BATTERY ((MDRDomain)2u)
#define MDR_DOMAIN_PLAYBACK ((MDRDomain)3u)
#define MDR_DOMAIN_NOISE_CONTROL ((MDRDomain)4u)
#define MDR_DOMAIN_SPEAK_TO_CHAT ((MDRDomain)5u)
#define MDR_DOMAIN_LISTENING_MODE ((MDRDomain)6u)
#define MDR_DOMAIN_EQUALIZER ((MDRDomain)7u)
#define MDR_DOMAIN_PAIRED_DEVICES ((MDRDomain)8u)
#define MDR_DOMAIN_PAIRING ((MDRDomain)9u)
#define MDR_DOMAIN_GENERAL_SETTINGS ((MDRDomain)10u)
#define MDR_DOMAIN_ASSIGNABLE_CONTROLS ((MDRDomain)11u)
#define MDR_DOMAIN_POWER ((MDRDomain)12u)
#define MDR_DOMAIN_VOICE_GUIDANCE ((MDRDomain)13u)
#define MDR_DOMAIN_CONNECTION_MODE ((MDRDomain)14u)
#define MDR_DOMAIN_SAFE_LISTENING ((MDRDomain)15u)

typedef uint32_t MDRFeature;
#define MDR_FEATURE_IDENTITY ((MDRFeature)1u)
#define MDR_FEATURE_BATTERY_SINGLE ((MDRFeature)2u)
#define MDR_FEATURE_BATTERY_LEFT_RIGHT ((MDRFeature)3u)
#define MDR_FEATURE_BATTERY_CASE ((MDRFeature)4u)
#define MDR_FEATURE_PLAYBACK_METADATA ((MDRFeature)5u)
#define MDR_FEATURE_PLAYBACK_CONTROL ((MDRFeature)6u)
#define MDR_FEATURE_PLAYBACK_VOLUME ((MDRFeature)7u)
#define MDR_FEATURE_NOISE_CANCELLING ((MDRFeature)8u)
#define MDR_FEATURE_AMBIENT_SOUND ((MDRFeature)9u)
#define MDR_FEATURE_ADAPTIVE_AMBIENT_SOUND ((MDRFeature)10u)
#define MDR_FEATURE_SPEAK_TO_CHAT ((MDRFeature)11u)
#define MDR_FEATURE_LISTENING_MODE ((MDRFeature)12u)
#define MDR_FEATURE_EQUALIZER ((MDRFeature)13u)
#define MDR_FEATURE_DSEE ((MDRFeature)14u)
#define MDR_FEATURE_PAIRED_DEVICE_MANAGEMENT ((MDRFeature)15u)
#define MDR_FEATURE_PAIRING_MODE ((MDRFeature)16u)
#define MDR_FEATURE_GENERAL_SETTINGS ((MDRFeature)17u)
#define MDR_FEATURE_ASSIGNABLE_CONTROLS ((MDRFeature)18u)
#define MDR_FEATURE_NOISE_CONTROL_BUTTON ((MDRFeature)19u)
#define MDR_FEATURE_AUTO_POWER_OFF ((MDRFeature)20u)
#define MDR_FEATURE_WEARING_DETECTION ((MDRFeature)21u)
#define MDR_FEATURE_AUTO_PAUSE ((MDRFeature)22u)
#define MDR_FEATURE_HEAD_GESTURE ((MDRFeature)23u)
#define MDR_FEATURE_VOICE_GUIDANCE ((MDRFeature)24u)
#define MDR_FEATURE_VOICE_GUIDANCE_VOLUME ((MDRFeature)25u)
#define MDR_FEATURE_SHUTDOWN ((MDRFeature)26u)
#define MDR_FEATURE_CONNECTION_MODE ((MDRFeature)27u)
#define MDR_FEATURE_SAFE_LISTENING ((MDRFeature)28u)

typedef uint8_t MDROperation;
#define MDR_OPERATION_NONE ((MDROperation)0u)
#define MDR_OPERATION_INITIALIZE ((MDROperation)1u)
#define MDR_OPERATION_SYNC ((MDROperation)2u)
#define MDR_OPERATION_APPLY ((MDROperation)3u)

typedef uint8_t MDREventType;
#define MDR_EVENT_NONE ((MDREventType)0u)
#define MDR_EVENT_STATE_CHANGED ((MDREventType)1u)
#define MDR_EVENT_OPERATION_COMPLETE ((MDREventType)2u)
#define MDR_EVENT_ALERT ((MDREventType)3u)
#define MDR_EVENT_INTERACTION ((MDREventType)4u)
#define MDR_EVENT_DEVICE_MESSAGE ((MDREventType)5u)
#define MDR_EVENT_UNHANDLED ((MDREventType)6u)
#define MDR_EVENT_ERROR ((MDREventType)7u)

typedef int32_t MDRPacketDirection;
#define MDR_PACKET_DIRECTION_RX ((MDRPacketDirection)0u)
#define MDR_PACKET_DIRECTION_TX ((MDRPacketDirection)1u)

typedef uint16_t MDRText;
#define MDR_TEXT_MODEL_NAME ((MDRText)1u)
#define MDR_TEXT_UNIQUE_ID ((MDRText)2u)
#define MDR_TEXT_FIRMWARE_VERSION ((MDRText)3u)
#define MDR_TEXT_MODEL_SERIES ((MDRText)4u)
#define MDR_TEXT_MODEL_COLOR ((MDRText)5u)
#define MDR_TEXT_TRACK_TITLE ((MDRText)6u)
#define MDR_TEXT_TRACK_ALBUM ((MDRText)7u)
#define MDR_TEXT_TRACK_ARTIST ((MDRText)8u)
#define MDR_TEXT_PAIRED_DEVICE_ID ((MDRText)9u)
#define MDR_TEXT_PAIRED_DEVICE_NAME ((MDRText)10u)
#define MDR_TEXT_GENERAL_SETTING_SUBJECT ((MDRText)11u)
#define MDR_TEXT_GENERAL_SETTING_SUMMARY ((MDRText)12u)
#define MDR_TEXT_LAST_ERROR ((MDRText)13u)
#define MDR_TEXT_LAST_ALERT ((MDRText)14u)
#define MDR_TEXT_LAST_INTERACTION ((MDRText)15u)
#define MDR_TEXT_LAST_DEVICE_MESSAGE ((MDRText)16u)

typedef uint8_t MDRAudioCodec;
#define MDR_AUDIO_CODEC_UNKNOWN ((MDRAudioCodec)0u)
#define MDR_AUDIO_CODEC_SBC ((MDRAudioCodec)1u)
#define MDR_AUDIO_CODEC_AAC ((MDRAudioCodec)2u)
#define MDR_AUDIO_CODEC_LDAC ((MDRAudioCodec)3u)
#define MDR_AUDIO_CODEC_APTX ((MDRAudioCodec)4u)
#define MDR_AUDIO_CODEC_APTX_HD ((MDRAudioCodec)5u)
#define MDR_AUDIO_CODEC_LC3 ((MDRAudioCodec)6u)
#define MDR_AUDIO_CODEC_OTHER ((MDRAudioCodec)255u)

typedef uint8_t MDRBatteryPart;
#define MDR_BATTERY_MAIN ((MDRBatteryPart)0u)
#define MDR_BATTERY_LEFT ((MDRBatteryPart)1u)
#define MDR_BATTERY_RIGHT ((MDRBatteryPart)2u)
#define MDR_BATTERY_CASE ((MDRBatteryPart)3u)

typedef uint8_t MDRChargingState;
#define MDR_CHARGING_UNKNOWN ((MDRChargingState)0u)
#define MDR_CHARGING_NO ((MDRChargingState)1u)
#define MDR_CHARGING_YES ((MDRChargingState)2u)
#define MDR_CHARGING_COMPLETE ((MDRChargingState)3u)

typedef uint8_t MDRPlaybackStatus;
#define MDR_PLAYBACK_UNKNOWN ((MDRPlaybackStatus)0u)
#define MDR_PLAYBACK_STOPPED ((MDRPlaybackStatus)1u)
#define MDR_PLAYBACK_PLAYING ((MDRPlaybackStatus)2u)
#define MDR_PLAYBACK_PAUSED ((MDRPlaybackStatus)3u)

typedef uint8_t MDRPlaybackAction;
#define MDR_PLAYBACK_PLAY ((MDRPlaybackAction)1u)
#define MDR_PLAYBACK_PAUSE ((MDRPlaybackAction)2u)
#define MDR_PLAYBACK_NEXT ((MDRPlaybackAction)3u)
#define MDR_PLAYBACK_PREVIOUS ((MDRPlaybackAction)4u)

typedef uint8_t MDRNoiseMode;
#define MDR_NOISE_MODE_OFF ((MDRNoiseMode)0u)
#define MDR_NOISE_MODE_CANCELLING ((MDRNoiseMode)1u)
#define MDR_NOISE_MODE_AMBIENT ((MDRNoiseMode)2u)

typedef uint8_t MDRAdaptiveSensitivity;
#define MDR_ADAPTIVE_SENSITIVITY_UNKNOWN ((MDRAdaptiveSensitivity)0u)
#define MDR_ADAPTIVE_SENSITIVITY_LOW ((MDRAdaptiveSensitivity)1u)
#define MDR_ADAPTIVE_SENSITIVITY_STANDARD ((MDRAdaptiveSensitivity)2u)
#define MDR_ADAPTIVE_SENSITIVITY_HIGH ((MDRAdaptiveSensitivity)3u)

typedef uint8_t MDRNoiseButtonMode;
#define MDR_NOISE_BUTTON_NONE ((MDRNoiseButtonMode)0u)
#define MDR_NOISE_BUTTON_NOISE_AMBIENT_OFF ((MDRNoiseButtonMode)1u)
#define MDR_NOISE_BUTTON_NOISE_AMBIENT ((MDRNoiseButtonMode)2u)
#define MDR_NOISE_BUTTON_NOISE_OFF ((MDRNoiseButtonMode)3u)
#define MDR_NOISE_BUTTON_AMBIENT_OFF ((MDRNoiseButtonMode)4u)

typedef uint8_t MDRSpeechSensitivity;
#define MDR_SPEECH_SENSITIVITY_UNKNOWN ((MDRSpeechSensitivity)0u)
#define MDR_SPEECH_SENSITIVITY_AUTO ((MDRSpeechSensitivity)1u)
#define MDR_SPEECH_SENSITIVITY_LOW ((MDRSpeechSensitivity)2u)
#define MDR_SPEECH_SENSITIVITY_HIGH ((MDRSpeechSensitivity)3u)

typedef uint8_t MDRSpeakTimeout;
#define MDR_SPEAK_TIMEOUT_UNKNOWN ((MDRSpeakTimeout)0u)
#define MDR_SPEAK_TIMEOUT_SHORT ((MDRSpeakTimeout)1u)
#define MDR_SPEAK_TIMEOUT_MEDIUM ((MDRSpeakTimeout)2u)
#define MDR_SPEAK_TIMEOUT_LONG ((MDRSpeakTimeout)3u)
#define MDR_SPEAK_TIMEOUT_MANUAL ((MDRSpeakTimeout)4u)

typedef uint8_t MDRListeningMode;
#define MDR_LISTENING_STANDARD ((MDRListeningMode)0u)
#define MDR_LISTENING_BACKGROUND_MUSIC ((MDRListeningMode)1u)
#define MDR_LISTENING_CINEMA ((MDRListeningMode)2u)

typedef uint8_t MDRRoomSize;
#define MDR_ROOM_UNKNOWN ((MDRRoomSize)0u)
#define MDR_ROOM_SMALL ((MDRRoomSize)1u)
#define MDR_ROOM_MEDIUM ((MDRRoomSize)2u)
#define MDR_ROOM_LARGE ((MDRRoomSize)3u)

typedef uint8_t MDREqualizerPreset;
#define MDR_EQ_OFF ((MDREqualizerPreset)0u)
#define MDR_EQ_ROCK ((MDREqualizerPreset)1u)
#define MDR_EQ_POP ((MDREqualizerPreset)2u)
#define MDR_EQ_JAZZ ((MDREqualizerPreset)3u)
#define MDR_EQ_DANCE ((MDREqualizerPreset)4u)
#define MDR_EQ_EDM ((MDREqualizerPreset)5u)
#define MDR_EQ_R_AND_B_HIP_HOP ((MDREqualizerPreset)6u)
#define MDR_EQ_ACOUSTIC ((MDREqualizerPreset)7u)
#define MDR_EQ_BRIGHT ((MDREqualizerPreset)8u)
#define MDR_EQ_EXCITED ((MDREqualizerPreset)9u)
#define MDR_EQ_MELLOW ((MDREqualizerPreset)10u)
#define MDR_EQ_RELAXED ((MDREqualizerPreset)11u)
#define MDR_EQ_VOCAL ((MDREqualizerPreset)12u)
#define MDR_EQ_TREBLE ((MDREqualizerPreset)13u)
#define MDR_EQ_BASS ((MDREqualizerPreset)14u)
#define MDR_EQ_SPEECH ((MDREqualizerPreset)15u)
#define MDR_EQ_HEAVY ((MDREqualizerPreset)16u)
#define MDR_EQ_CLEAR ((MDREqualizerPreset)17u)
#define MDR_EQ_HARD ((MDREqualizerPreset)18u)
#define MDR_EQ_SOFT ((MDREqualizerPreset)19u)
#define MDR_EQ_GAMING ((MDREqualizerPreset)20u)
#define MDR_EQ_FPS_1 ((MDREqualizerPreset)21u)
#define MDR_EQ_FPS_2 ((MDREqualizerPreset)22u)
#define MDR_EQ_FPS_3 ((MDREqualizerPreset)23u)
#define MDR_EQ_CUSTOM ((MDREqualizerPreset)24u)
#define MDR_EQ_USER_1 ((MDREqualizerPreset)25u)
#define MDR_EQ_USER_2 ((MDREqualizerPreset)26u)
#define MDR_EQ_USER_3 ((MDREqualizerPreset)27u)
#define MDR_EQ_USER_4 ((MDREqualizerPreset)28u)
#define MDR_EQ_USER_5 ((MDREqualizerPreset)29u)
#define MDR_EQ_UNKNOWN ((MDREqualizerPreset)255u)

typedef uint8_t MDRDseeType;
#define MDR_DSEE_UNKNOWN ((MDRDseeType)0u)
#define MDR_DSEE_STANDARD ((MDRDseeType)1u)
#define MDR_DSEE_HX ((MDRDseeType)2u)
#define MDR_DSEE_HX_AI ((MDRDseeType)3u)
#define MDR_DSEE_ULTIMATE ((MDRDseeType)4u)

typedef uint8_t MDRPairedDeviceCommand;
#define MDR_PAIRED_DEVICE_CONNECT ((MDRPairedDeviceCommand)1u)
#define MDR_PAIRED_DEVICE_DISCONNECT ((MDRPairedDeviceCommand)2u)
#define MDR_PAIRED_DEVICE_SELECT_PLAYBACK ((MDRPairedDeviceCommand)3u)
#define MDR_PAIRED_DEVICE_UNPAIR ((MDRPairedDeviceCommand)4u)

typedef uint8_t MDRGeneralSettingType;
#define MDR_GENERAL_SETTING_UNKNOWN ((MDRGeneralSettingType)0u)
#define MDR_GENERAL_SETTING_BOOLEAN ((MDRGeneralSettingType)1u)

typedef uint8_t MDRAssignableAction;
#define MDR_ASSIGNABLE_NONE ((MDRAssignableAction)0u)
#define MDR_ASSIGNABLE_PLAYBACK ((MDRAssignableAction)1u)
#define MDR_ASSIGNABLE_NOISE_CONTROL ((MDRAssignableAction)2u)
#define MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS ((MDRAssignableAction)3u)
#define MDR_ASSIGNABLE_TRACK_CONTROL ((MDRAssignableAction)4u)
#define MDR_ASSIGNABLE_VOICE_ASSISTANT ((MDRAssignableAction)5u)
#define MDR_ASSIGNABLE_QUICK_ACCESS ((MDRAssignableAction)6u)

typedef uint8_t MDRWearingPowerMode;
#define MDR_WEARING_POWER_UNAVAILABLE ((MDRWearingPowerMode)0u)
#define MDR_WEARING_POWER_DISABLED ((MDRWearingPowerMode)1u)
#define MDR_WEARING_POWER_WHEN_REMOVED ((MDRWearingPowerMode)2u)

typedef uint8_t MDRAudioPriority;
#define MDR_AUDIO_PRIORITY_UNKNOWN ((MDRAudioPriority)0u)
#define MDR_AUDIO_PRIORITY_QUALITY ((MDRAudioPriority)1u)
#define MDR_AUDIO_PRIORITY_STABILITY ((MDRAudioPriority)2u)

typedef struct MDRHeadphonesStatus
{
    uint32_t struct_size;
    MDROperation active_operation;
    MDRBoolean ready;
    MDRBoolean dirty;
    MDRBoolean initialized;
} MDRHeadphonesStatus;

typedef struct MDREvent
{
    uint32_t struct_size;
    MDREventType type;
    MDRDomain domain;
    MDROperation operation;
    MDRResult result;
    uint32_t detail;
} MDREvent;

typedef struct MDRIdentity
{
    uint32_t struct_size;
    uint32_t protocol_version;
    MDRAudioCodec audio_codec;
    uint8_t model_color;
} MDRIdentity;

typedef struct MDRBattery
{
    uint32_t struct_size;
    MDRBatteryPart part;
    MDRBoolean present;
    uint8_t level_percent;
    uint8_t update_threshold_percent;
    MDRChargingState charging;
} MDRBattery;

typedef struct MDRPlayback
{
    uint32_t struct_size;
    MDRPlaybackStatus status;
    uint8_t volume;
} MDRPlayback;

typedef struct MDRPlaybackCommand
{
    uint32_t struct_size;
    MDRPlaybackAction action;
} MDRPlaybackCommand;

typedef struct MDRNoiseControl
{
    uint32_t struct_size;
    MDRNoiseMode mode;
    uint8_t ambient_level;
    MDRBoolean focus_on_voice;
    MDRNoiseButtonMode button_mode;
    MDRBoolean adaptive_ambient;
    MDRAdaptiveSensitivity adaptive_sensitivity;
} MDRNoiseControl;

typedef struct MDRSpeakToChat
{
    uint32_t struct_size;
    MDRBoolean enabled;
    MDRSpeechSensitivity sensitivity;
    MDRSpeakTimeout timeout;
} MDRSpeakToChat;

typedef struct MDRListening
{
    uint32_t struct_size;
    MDRListeningMode mode;
    MDRRoomSize background_room;
} MDRListening;

typedef struct MDREqualizer
{
    uint32_t struct_size;
    MDREqualizerPreset preset;
    int8_t clear_bass;
    uint32_t band_count;
    MDRBoolean dsee_enabled;
    MDRDseeType dsee_type;
} MDREqualizer;

typedef struct MDRPairedDevice
{
    uint32_t struct_size;
    uint32_t index;
    MDRBoolean connected;
    MDRBoolean playback_device;
} MDRPairedDevice;

typedef struct MDRPairedDeviceAction
{
    uint32_t struct_size;
    MDRPairedDeviceCommand command;
    const char* device_id;
    uint32_t device_id_size;
} MDRPairedDeviceAction;

typedef struct MDRPairing
{
    uint32_t struct_size;
    MDRBoolean enabled;
} MDRPairing;

typedef struct MDRGeneralSettingInfo
{
    uint32_t struct_size;
    uint32_t index;
    MDRGeneralSettingType type;
    MDRBoolean writable;
} MDRGeneralSettingInfo;

typedef struct MDRGeneralSetting
{
    uint32_t struct_size;
    uint32_t index;
    MDRBoolean boolean_value;
} MDRGeneralSetting;

typedef struct MDRAssignableControls
{
    uint32_t struct_size;
    MDRAssignableAction left;
    MDRAssignableAction right;
} MDRAssignableControls;

typedef struct MDRPower
{
    uint32_t struct_size;
    uint32_t auto_power_off_minutes;
    MDRWearingPowerMode wearing_power;
    MDRBoolean auto_pause;
    MDRBoolean head_gesture;
    MDRBoolean shutdown_requested;
} MDRPower;

typedef struct MDRVoiceGuidance
{
    uint32_t struct_size;
    MDRBoolean enabled;
    int8_t volume;
} MDRVoiceGuidance;

typedef struct MDRConnectionMode
{
    uint32_t struct_size;
    MDRAudioPriority audio_priority;
} MDRConnectionMode;

typedef struct MDRSafeListening
{
    uint32_t struct_size;
    uint8_t sound_pressure;
    MDRBoolean preview;
} MDRSafeListening;

/**
 * Synchronous observer for complete packed MDR wire frames. The frame is valid
 * only during the callback and must be copied if it is retained.
 */
typedef void (*MDRPacketCallback)(
    void* user_data,
    MDRPacketDirection direction,
    const unsigned char* frame,
    int frame_size
);

#ifdef __cplusplus
extern "C" {
#endif

/* Lifecycle and processing. */
MDRResult mdrHeadphonesOpen(MDRConnection* connection, MDRHeadphones** out_headphones);
void mdrHeadphonesClose(MDRHeadphones* headphones);
MDRResult mdrHeadphonesGetStatus(MDRHeadphones* headphones, MDRHeadphonesStatus* out_status);
MDRResult mdrHeadphonesStart(MDRHeadphones* headphones, MDROperation operation);

/** Processes pending I/O without blocking and queues any resulting events. */
MDRResult mdrHeadphonesPoll(MDRHeadphones* headphones);

/** Removes the oldest queued event, or returns MDR_RESULT_ERROR_NOT_FOUND. */
MDRResult mdrHeadphonesNextEvent(MDRHeadphones* headphones, MDREvent* out_event);

/**
 * Observes raw packets independently of semantic events. Passing NULL disables
 * observation.
 */
void mdrHeadphonesSetPacketCallback(
    MDRHeadphones* headphones,
    MDRPacketCallback callback,
    void* user_data
);

/* Capability and caller-owned UTF-8 text access. */
MDRResult mdrHeadphonesGetFeature(
    MDRHeadphones* headphones,
    MDRFeature feature,
    MDRFeatureAvailability* out_availability
);
MDRResult mdrHeadphonesGetText(
    MDRHeadphones* headphones,
    MDRText text,
    uint32_t index,
    char* buffer,
    uint32_t* inout_size
);

/* Identity, battery, and playback. */
MDRResult mdrHeadphonesGetIdentity(MDRHeadphones* headphones, MDRIdentity* out_identity);
MDRResult mdrHeadphonesGetBatteries(
    MDRHeadphones* headphones,
    MDRBattery* batteries,
    uint32_t* inout_count
);
MDRResult mdrHeadphonesGetPlayback(
    MDRHeadphones* headphones,
    MDRStateView view,
    MDRPlayback* out_playback
);
MDRResult mdrHeadphonesSetPlayback(MDRHeadphones* headphones, const MDRPlayback* playback);
MDRResult mdrHeadphonesPlayback(
    MDRHeadphones* headphones,
    const MDRPlaybackCommand* command
);

/* Sound controls. */
MDRResult mdrHeadphonesGetNoiseControl(
    MDRHeadphones* headphones,
    MDRStateView view,
    MDRNoiseControl* out_noise_control
);
MDRResult mdrHeadphonesSetNoiseControl(
    MDRHeadphones* headphones,
    const MDRNoiseControl* noise_control
);
MDRResult mdrHeadphonesGetSpeakToChat(
    MDRHeadphones* headphones,
    MDRStateView view,
    MDRSpeakToChat* out_speak_to_chat
);
MDRResult mdrHeadphonesSetSpeakToChat(
    MDRHeadphones* headphones,
    const MDRSpeakToChat* speak_to_chat
);
MDRResult mdrHeadphonesGetListening(
    MDRHeadphones* headphones,
    MDRStateView view,
    MDRListening* out_listening
);
MDRResult mdrHeadphonesSetListening(
    MDRHeadphones* headphones,
    const MDRListening* listening
);
MDRResult mdrHeadphonesGetEqualizer(
    MDRHeadphones* headphones,
    MDRStateView view,
    MDREqualizer* out_equalizer
);
MDRResult mdrHeadphonesSetEqualizer(
    MDRHeadphones* headphones,
    const MDREqualizer* equalizer
);
MDRResult mdrHeadphonesGetEqualizerBands(
    MDRHeadphones* headphones,
    MDRStateView view,
    int8_t* bands,
    uint32_t* inout_count
);
MDRResult mdrHeadphonesSetEqualizerBands(
    MDRHeadphones* headphones,
    const int8_t* bands,
    uint32_t count
);

/* Paired devices and pairing. Device names/IDs use MDR_TEXT_* with index. */
MDRResult mdrHeadphonesGetPairedDevices(
    MDRHeadphones* headphones,
    MDRPairedDevice* devices,
    uint32_t* inout_count
);
MDRResult mdrHeadphonesSetPairedDevice(
    MDRHeadphones* headphones,
    const MDRPairedDeviceAction* action
);
MDRResult mdrHeadphonesGetPairing(
    MDRHeadphones* headphones,
    MDRStateView view,
    MDRPairing* out_pairing
);
MDRResult mdrHeadphonesSetPairing(MDRHeadphones* headphones, const MDRPairing* pairing);

/* General settings and assignable controls. */
MDRResult mdrHeadphonesGetGeneralSettingInfo(
    MDRHeadphones* headphones,
    MDRGeneralSettingInfo* settings,
    uint32_t* inout_count
);
MDRResult mdrHeadphonesGetGeneralSetting(
    MDRHeadphones* headphones,
    MDRStateView view,
    uint32_t index,
    MDRGeneralSetting* out_setting
);
MDRResult mdrHeadphonesSetGeneralSetting(
    MDRHeadphones* headphones,
    const MDRGeneralSetting* setting
);
MDRResult mdrHeadphonesGetAssignableControls(
    MDRHeadphones* headphones,
    MDRStateView view,
    MDRAssignableControls* out_controls
);
MDRResult mdrHeadphonesSetAssignableControls(
    MDRHeadphones* headphones,
    const MDRAssignableControls* controls
);

/* Power, wearing behavior, voice guidance, and related system settings. */
MDRResult mdrHeadphonesGetPower(
    MDRHeadphones* headphones,
    MDRStateView view,
    MDRPower* out_power
);
MDRResult mdrHeadphonesSetPower(MDRHeadphones* headphones, const MDRPower* power);
MDRResult mdrHeadphonesGetVoiceGuidance(
    MDRHeadphones* headphones,
    MDRStateView view,
    MDRVoiceGuidance* out_voice_guidance
);
MDRResult mdrHeadphonesSetVoiceGuidance(
    MDRHeadphones* headphones,
    const MDRVoiceGuidance* voice_guidance
);
MDRResult mdrHeadphonesGetConnectionMode(
    MDRHeadphones* headphones,
    MDRStateView view,
    MDRConnectionMode* out_mode
);
MDRResult mdrHeadphonesSetConnectionMode(
    MDRHeadphones* headphones,
    const MDRConnectionMode* mode
);
MDRResult mdrHeadphonesGetSafeListening(
    MDRHeadphones* headphones,
    MDRStateView view,
    MDRSafeListening* out_safe_listening
);
MDRResult mdrHeadphonesSetSafeListening(
    MDRHeadphones* headphones,
    const MDRSafeListening* safe_listening
);

/*
 * Deprecated compatibility declarations. These retain the existing source
 * surface while new consumers use the structured API above.
 */
MDR_DEPRECATED("use mdrHeadphonesOpen")
MDRHeadphones* mdrHeadphonesCreate(MDRConnection* connection);
MDR_DEPRECATED("use mdrHeadphonesClose")
void mdrHeadphonesDestroy(MDRHeadphones* headphones);
MDR_DEPRECATED("use mdrHeadphonesPoll and mdrHeadphonesNextEvent")
int mdrHeadphonesPollEvents(MDRHeadphones* headphones);
MDR_DEPRECATED("use mdrHeadphonesGetStatus")
int mdrHeadphonesRequestIsReady(MDRHeadphones* headphones);
MDR_DEPRECATED("use mdrHeadphonesStart with MDR_OPERATION_INITIALIZE")
int mdrHeadphonesRequestInitV2(MDRHeadphones* headphones);
MDR_DEPRECATED("use mdrHeadphonesStart with MDR_OPERATION_SYNC")
int mdrHeadphonesRequestSyncV2(MDRHeadphones* headphones);
MDR_DEPRECATED("use mdrHeadphonesStart with MDR_OPERATION_APPLY")
int mdrHeadphonesRequestCommitV2(MDRHeadphones* headphones);
MDR_DEPRECATED("use mdrHeadphonesGetStatus")
int mdrHeadphonesIsDirty(MDRHeadphones* headphones);
MDR_DEPRECATED("use mdrHeadphonesGetStatus")
int mdrHeadphonesIsReady(MDRHeadphones* headphones);
MDR_DEPRECATED("use mdrHeadphonesGetText with MDR_TEXT_LAST_ERROR")
const char* mdrHeadphonesGetLastError(MDRHeadphones* headphones);

#ifdef __cplusplus
}
#endif
