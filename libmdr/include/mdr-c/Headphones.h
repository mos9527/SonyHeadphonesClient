#pragma once

#include <stddef.h>
#include <stdint.h>
#include "Base.h"
#include "Connection.h"

typedef struct MDRHeadphones MDRHeadphones;

typedef uint32_t MDRProtocolVersion;
#define MDR_PROTOCOL_V1 ((MDRProtocolVersion)1u)
#define MDR_PROTOCOL_V2 ((MDRProtocolVersion)2u)

typedef uint32_t MDRBoolean;
#define MDR_FALSE ((MDRBoolean)0u)
#define MDR_TRUE ((MDRBoolean)1u)

typedef uint32_t MDRFeatureAvailability;
#define MDR_AVAILABILITY_UNKNOWN ((MDRFeatureAvailability)0u)
#define MDR_AVAILABILITY_UNAVAILABLE ((MDRFeatureAvailability)1u)
#define MDR_AVAILABILITY_AVAILABLE ((MDRFeatureAvailability)2u)

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
#define MDR_FEATURE_SOURCE_SWITCH_CONTROL ((MDRFeature)29u)

typedef uint32_t MDREvent;
#define MDR_EVENT_NONE ((MDREvent)0u)
#define MDR_EVENT_IDENTITY_CHANGED ((MDREvent)1u)
#define MDR_EVENT_BATTERY_CHANGED ((MDREvent)2u)
#define MDR_EVENT_PLAYBACK_CHANGED ((MDREvent)3u)
#define MDR_EVENT_NOISE_CONTROL_CHANGED ((MDREvent)4u)
#define MDR_EVENT_SPEAK_TO_CHAT_CHANGED ((MDREvent)5u)
#define MDR_EVENT_LISTENING_MODE_CHANGED ((MDREvent)6u)
#define MDR_EVENT_EQUALIZER_CHANGED ((MDREvent)7u)
#define MDR_EVENT_PAIRED_DEVICES_CHANGED ((MDREvent)8u)
#define MDR_EVENT_PAIRING_CHANGED ((MDREvent)9u)
#define MDR_EVENT_GENERAL_SETTINGS_CHANGED ((MDREvent)10u)
#define MDR_EVENT_ASSIGNABLE_CONTROLS_CHANGED ((MDREvent)11u)
#define MDR_EVENT_POWER_CHANGED ((MDREvent)12u)
#define MDR_EVENT_VOICE_GUIDANCE_CHANGED ((MDREvent)13u)
#define MDR_EVENT_CONNECTION_MODE_CHANGED ((MDREvent)14u)
#define MDR_EVENT_SAFE_LISTENING_CHANGED ((MDREvent)15u)
#define MDR_EVENT_INITIALIZE_COMPLETE ((MDREvent)16u)
#define MDR_EVENT_SYNC_COMPLETE ((MDREvent)17u)
#define MDR_EVENT_APPLY_COMPLETE ((MDREvent)18u)
#define MDR_EVENT_ALERT ((MDREvent)19u)
#define MDR_EVENT_INTERACTION ((MDREvent)20u)
#define MDR_EVENT_DEVICE_MESSAGE ((MDREvent)21u)
#define MDR_EVENT_UNHANDLED ((MDREvent)22u)

typedef uint32_t MDRPacketDirection;
#define MDR_PACKET_DIRECTION_RX ((MDRPacketDirection)0u)
#define MDR_PACKET_DIRECTION_TX ((MDRPacketDirection)1u)

typedef uint32_t MDRText;
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

typedef uint32_t MDRAudioCodec;
#define MDR_AUDIO_CODEC_UNKNOWN ((MDRAudioCodec)0u)
#define MDR_AUDIO_CODEC_SBC ((MDRAudioCodec)1u)
#define MDR_AUDIO_CODEC_AAC ((MDRAudioCodec)2u)
#define MDR_AUDIO_CODEC_LDAC ((MDRAudioCodec)3u)
#define MDR_AUDIO_CODEC_APTX ((MDRAudioCodec)4u)
#define MDR_AUDIO_CODEC_APTX_HD ((MDRAudioCodec)5u)
#define MDR_AUDIO_CODEC_LC3 ((MDRAudioCodec)6u)
#define MDR_AUDIO_CODEC_OTHER ((MDRAudioCodec)255u)

typedef uint32_t MDRBatteryPart;
#define MDR_BATTERY_MAIN ((MDRBatteryPart)0u)
#define MDR_BATTERY_LEFT ((MDRBatteryPart)1u)
#define MDR_BATTERY_RIGHT ((MDRBatteryPart)2u)
#define MDR_BATTERY_CASE ((MDRBatteryPart)3u)

typedef uint32_t MDRChargingState;
#define MDR_CHARGING_UNKNOWN ((MDRChargingState)0u)
#define MDR_CHARGING_NO ((MDRChargingState)1u)
#define MDR_CHARGING_YES ((MDRChargingState)2u)
#define MDR_CHARGING_COMPLETE ((MDRChargingState)3u)

typedef uint32_t MDRPlaybackStatus;
#define MDR_PLAYBACK_UNKNOWN ((MDRPlaybackStatus)0u)
#define MDR_PLAYBACK_STOPPED ((MDRPlaybackStatus)1u)
#define MDR_PLAYBACK_PLAYING ((MDRPlaybackStatus)2u)
#define MDR_PLAYBACK_PAUSED ((MDRPlaybackStatus)3u)

typedef uint32_t MDRPlaybackAction;
#define MDR_PLAYBACK_PLAY ((MDRPlaybackAction)1u)
#define MDR_PLAYBACK_PAUSE ((MDRPlaybackAction)2u)
#define MDR_PLAYBACK_NEXT ((MDRPlaybackAction)3u)
#define MDR_PLAYBACK_PREVIOUS ((MDRPlaybackAction)4u)

typedef uint32_t MDRSourceSwitchControlResult;
#define MDR_SOURCE_SWITCH_CONTROL_SUCCESS ((MDRSourceSwitchControlResult)0u)
#define MDR_SOURCE_SWITCH_CONTROL_FAILED ((MDRSourceSwitchControlResult)1u)
#define MDR_SOURCE_SWITCH_CONTROL_FAILED_ON_CALL ((MDRSourceSwitchControlResult)2u)
#define MDR_SOURCE_SWITCH_CONTROL_FAILED_NOT_CONNECTED ((MDRSourceSwitchControlResult)3u)
#define MDR_SOURCE_SWITCH_CONTROL_FAILED_VOICE_ASSISTANT ((MDRSourceSwitchControlResult)4u)

typedef uint32_t MDRNoiseMode;
#define MDR_NOISE_MODE_OFF ((MDRNoiseMode)0u)
#define MDR_NOISE_MODE_CANCELLING ((MDRNoiseMode)1u)
#define MDR_NOISE_MODE_AMBIENT ((MDRNoiseMode)2u)

typedef uint32_t MDRAdaptiveSensitivity;
#define MDR_ADAPTIVE_SENSITIVITY_UNKNOWN ((MDRAdaptiveSensitivity)0u)
#define MDR_ADAPTIVE_SENSITIVITY_LOW ((MDRAdaptiveSensitivity)1u)
#define MDR_ADAPTIVE_SENSITIVITY_STANDARD ((MDRAdaptiveSensitivity)2u)
#define MDR_ADAPTIVE_SENSITIVITY_HIGH ((MDRAdaptiveSensitivity)3u)

typedef uint32_t MDRNoiseButtonMode;
#define MDR_NOISE_BUTTON_NONE ((MDRNoiseButtonMode)0u)
#define MDR_NOISE_BUTTON_NOISE_AMBIENT_OFF ((MDRNoiseButtonMode)1u)
#define MDR_NOISE_BUTTON_NOISE_AMBIENT ((MDRNoiseButtonMode)2u)
#define MDR_NOISE_BUTTON_NOISE_OFF ((MDRNoiseButtonMode)3u)
#define MDR_NOISE_BUTTON_AMBIENT_OFF ((MDRNoiseButtonMode)4u)

typedef uint32_t MDRSpeechSensitivity;
#define MDR_SPEECH_SENSITIVITY_UNKNOWN ((MDRSpeechSensitivity)0u)
#define MDR_SPEECH_SENSITIVITY_AUTO ((MDRSpeechSensitivity)1u)
#define MDR_SPEECH_SENSITIVITY_LOW ((MDRSpeechSensitivity)2u)
#define MDR_SPEECH_SENSITIVITY_HIGH ((MDRSpeechSensitivity)3u)

typedef uint32_t MDRSpeakTimeout;
#define MDR_SPEAK_TIMEOUT_UNKNOWN ((MDRSpeakTimeout)0u)
#define MDR_SPEAK_TIMEOUT_SHORT ((MDRSpeakTimeout)1u)
#define MDR_SPEAK_TIMEOUT_MEDIUM ((MDRSpeakTimeout)2u)
#define MDR_SPEAK_TIMEOUT_LONG ((MDRSpeakTimeout)3u)
#define MDR_SPEAK_TIMEOUT_MANUAL ((MDRSpeakTimeout)4u)

typedef uint32_t MDRListeningMode;
#define MDR_LISTENING_STANDARD ((MDRListeningMode)0u)
#define MDR_LISTENING_BACKGROUND_MUSIC ((MDRListeningMode)1u)
#define MDR_LISTENING_CINEMA ((MDRListeningMode)2u)

typedef uint32_t MDRRoomSize;
#define MDR_ROOM_UNKNOWN ((MDRRoomSize)0u)
#define MDR_ROOM_SMALL ((MDRRoomSize)1u)
#define MDR_ROOM_MEDIUM ((MDRRoomSize)2u)
#define MDR_ROOM_LARGE ((MDRRoomSize)3u)

typedef uint32_t MDREqualizerPreset;
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

typedef uint32_t MDRDSEEType;
#define MDR_DSEE_UNKNOWN ((MDRDSEEType)0u)
#define MDR_DSEE_STANDARD ((MDRDSEEType)1u)
#define MDR_DSEE_HX ((MDRDSEEType)2u)
#define MDR_DSEE_HX_AI ((MDRDSEEType)3u)
#define MDR_DSEE_ULTIMATE ((MDRDSEEType)4u)

typedef uint32_t MDRPairedDeviceCommand;
#define MDR_PAIRED_DEVICE_CONNECT ((MDRPairedDeviceCommand)1u)
#define MDR_PAIRED_DEVICE_DISCONNECT ((MDRPairedDeviceCommand)2u)
#define MDR_PAIRED_DEVICE_SELECT_PLAYBACK ((MDRPairedDeviceCommand)3u)
#define MDR_PAIRED_DEVICE_UNPAIR ((MDRPairedDeviceCommand)4u)

typedef uint32_t MDRGeneralSettingType;
#define MDR_GENERAL_SETTING_UNKNOWN ((MDRGeneralSettingType)0u)
#define MDR_GENERAL_SETTING_BOOLEAN ((MDRGeneralSettingType)1u)

typedef uint32_t MDRAssignableAction;
#define MDR_ASSIGNABLE_NONE ((MDRAssignableAction)0u)
#define MDR_ASSIGNABLE_PLAYBACK ((MDRAssignableAction)1u)
#define MDR_ASSIGNABLE_NOISE_CONTROL ((MDRAssignableAction)2u)
#define MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS ((MDRAssignableAction)3u)
#define MDR_ASSIGNABLE_TRACK_CONTROL ((MDRAssignableAction)4u)
#define MDR_ASSIGNABLE_VOICE_ASSISTANT ((MDRAssignableAction)5u)
#define MDR_ASSIGNABLE_QUICK_ACCESS ((MDRAssignableAction)6u)

typedef uint32_t MDRWearingPowerMode;
#define MDR_WEARING_POWER_UNAVAILABLE ((MDRWearingPowerMode)0u)
#define MDR_WEARING_POWER_DISABLED ((MDRWearingPowerMode)1u)
#define MDR_WEARING_POWER_WHEN_REMOVED ((MDRWearingPowerMode)2u)

typedef uint32_t MDRAudioPriority;
#define MDR_AUDIO_PRIORITY_UNKNOWN ((MDRAudioPriority)0u)
#define MDR_AUDIO_PRIORITY_QUALITY ((MDRAudioPriority)1u)
#define MDR_AUDIO_PRIORITY_STABILITY ((MDRAudioPriority)2u)


typedef struct MDRModel
{
    uint32_t protocol_version;
    MDRAudioCodec audio_codec;
    uint8_t model_color;
} MDRModel;

typedef struct MDRBattery
{
    MDRBatteryPart part;
    MDRBoolean present;
    uint8_t level_percent;
    uint8_t update_threshold_percent;
    MDRChargingState charging;
} MDRBattery;

typedef struct MDRPlayback
{
    MDRPlaybackStatus status;
    uint8_t volume;
} MDRPlayback;

typedef struct MDRPlaybackCommand
{
    MDRPlaybackAction action;
} MDRPlaybackCommand;

typedef struct MDRNoiseControl
{
    MDRNoiseMode mode;
    uint8_t ambient_level;
    MDRBoolean changing_asm_level;
    MDRBoolean focus_on_voice;
    MDRNoiseButtonMode button_mode;
    MDRBoolean adaptive_ambient;
    MDRAdaptiveSensitivity adaptive_sensitivity;
} MDRNoiseControl;

typedef struct MDRSpeakToChat
{
    MDRBoolean enabled;
    MDRSpeechSensitivity sensitivity;
    MDRSpeakTimeout timeout;
} MDRSpeakToChat;

typedef struct MDRListening
{
    MDRListeningMode mode;
    MDRRoomSize background_room;
} MDRListening;

typedef struct MDREqualizer
{
    MDREqualizerPreset preset;
    int8_t clear_bass;
    uint32_t band_count;
    MDRBoolean dsee_enabled;
    MDRDSEEType dsee_type;
} MDREqualizer;

typedef struct MDRPairedDevice
{
    MDRBoolean connected;
    MDRBoolean playback_device;
    char macAddress[18];
    char name[128];
} MDRPairedDevice;

typedef struct MDRPairedDeviceAction
{
    MDRPairedDeviceCommand command;
    const char* device_id;
    uint32_t device_id_size;
} MDRPairedDeviceAction;

typedef struct MDRPairing
{
    MDRBoolean enabled;
} MDRPairing;

typedef struct MDRGeneralSettingInfo
{
    uint32_t index;
    MDRGeneralSettingType type;
    MDRBoolean writable;
} MDRGeneralSettingInfo;

typedef struct MDRGeneralSetting
{
    uint32_t index;
    MDRBoolean boolean_value;
} MDRGeneralSetting;

typedef struct MDRAssignableControls
{
    MDRAssignableAction left;
    MDRAssignableAction right;
} MDRAssignableControls;

typedef struct MDRPower
{
    uint32_t auto_power_off_minutes;
    MDRWearingPowerMode wearing_power;
    MDRBoolean auto_pause;
    MDRBoolean head_gesture;
    MDRBoolean shutdown_requested;
} MDRPower;

typedef struct MDRVoiceGuidance
{
    MDRBoolean enabled;
    int8_t volume;
} MDRVoiceGuidance;

typedef struct MDRConnectionMode
{
    MDRAudioPriority audio_priority;
} MDRConnectionMode;

typedef struct MDRSafeListening
{
    uint8_t sound_pressure;
    MDRBoolean preview;
} MDRSafeListening;

// See @ref mdrHeadphonesSetPacketCallback
typedef void (*MDRPacketCallback)(void* user_data, MDRPacketDirection direction, const unsigned char* frame,
                                  int frame_size);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a new MDRHeadphones instance bound to an existing @ref MDRConnection.
 *
 * @param abiVersion Always pass @ref MDR_ABI_VERSION. This is the one handshake between the header
 *                   you compiled against and the library you ended up linked to; every struct in
 *                   this header has a fixed layout for a given value of it.
 * @param connection The @ref MDRConnection carrying the transport for the device. It must already be
 *                   connected (see mdrConnectionConnect) and remain valid for the lifetime of the
 *                   created instance.
 * @param protocolVersion The protocol family used by the headphones reachable via @p connection.
 *                        Note that a physical @ref MDRConnection implies a known protocol family.
 *                        - For Bluetooth Classic (RFCOMM) devices, matching UUID per protocol family is required for
 * proper SDP service discovery.
 *                        - For BLE devices, there's currently no exception in that they are always @ref MDR_PROTOCOL_V2
 * devices.
 * @param[out] ppHeadphones Receives the created instance on success. Untouched on failure.
 *
 * @return @ref MDR_RESULT_OK on success, or @ref MDR_RESULT_ERROR_ABI_MISMATCH if this library does
 *         not implement @p abiVersion, in which case no instance is created and every other entry
 *         point is unsafe to call.
 */
MDR_API MDRResult mdrHeadphonesCreate(uint32_t abiVersion, MDRConnection* connection,
                                      MDRProtocolVersion protocolVersion, MDRHeadphones** ppHeadphones);
/**
 * @brief Frees the @ref MDRHeadphones instance.
 */
MDR_API void mdrHeadphonesDestroy(MDRHeadphones* headphones);
/**
 * @brief Returns true if the @ref MDRHeadphones instance has been initialized via @ref mdrHeadphonesRequestInit.
 */
MDR_API MDRBoolean mdrHeadphonesIsInitialized(const MDRHeadphones* headphones);
/**
 * @brief Returns true if new ...Request calls can be made.
 *        If any in-flight request is pending/incomplete, this will return false, and subsequent ...Request calls will
 *        fail with @ref MDR_RESULT_INPROGRESS.
 */
MDR_API MDRBoolean mdrHeadphonesIsReady(const MDRHeadphones* headphones);
/**
 * @brief Returns true if there are pending changes made through the ...Set calls that have not yet been committed to
 *        the device via @ref mdrHeadphonesRequestCommit.
 */
MDR_API MDRBoolean mdrHeadphonesIsDirty(const MDRHeadphones* headphones);
/**
 * @brief Request initialization. This MUST be called prior to other operations.
 *        See also @ref mdrHeadphonesIsInitialized, @ref mdrHeadphonesIsReady, and @ref mdrHeadphonesIsDirty.
 */
MDR_API MDRResult mdrHeadphonesRequestInit(MDRHeadphones* headphones);
/**
 * @brief Request pulling latest states from the device. This includes e.g. battery levels and some other states that
 * may change without being notified by the headphones themselves.
 */
MDR_API MDRResult mdrHeadphonesRequestFetch(MDRHeadphones* headphones);
/**
 * @brief Commits any pending changes made via the ...Set calls. Changes are ONLY applied to the devices
 *        after this call is complete, and only then @ref mdrHeadphonesIsDirty will return false
 * @note  You may call this function at the end of your frame/event loop. Calling this function
 *        when @ref mdrHeadphonesIsDirty returns false is a no-op.
 */
MDR_API MDRResult mdrHeadphonesRequestCommit(MDRHeadphones* headphones);

/**
 * Processes pending I/O without blocking and reports one semantic event.
 * MDR_EVENT_NONE is returned when no command was received.
 */
MDR_API MDRResult mdrHeadphonesPoll(MDRHeadphones* headphones, MDREvent* out_event);

/**
 * Observes raw packets independently of semantic events. Passing NULL disables
 * observation.
 */
MDR_API void mdrHeadphonesSetPacketCallback(MDRHeadphones* headphones, MDRPacketCallback callback, void* user_data);

/* Capability and caller-owned UTF-8 text access. */
MDR_API MDRResult mdrHeadphonesGetFeature(MDRHeadphones* headphones, MDRFeature feature,
                                          MDRFeatureAvailability* out_availability);
MDR_API MDRResult mdrHeadphonesGetText(MDRHeadphones* headphones, MDRText text, uint32_t index, char* buffer,
                                       uint32_t* inout_size);

/* Model, battery, and playback. */
MDR_API MDRResult mdrHeadphonesGetModel(MDRHeadphones* headphones, MDRModel* out_identity);
MDR_API MDRResult mdrHeadphonesGetBatteries(MDRHeadphones* headphones, MDRBattery* batteries, uint32_t* inout_count);
MDR_API MDRResult mdrHeadphonesGetPlayback(MDRHeadphones* headphones, MDRPlayback* out_playback);
MDR_API MDRResult mdrHeadphonesSetPlayback(MDRHeadphones* headphones, const MDRPlayback* playback);
MDR_API MDRResult mdrHeadphonesPlayback(MDRHeadphones* headphones, const MDRPlaybackCommand* command);

/* Sound controls. */
MDR_API MDRResult mdrHeadphonesGetNoiseControl(MDRHeadphones* headphones, MDRNoiseControl* out_noise_control);
MDR_API MDRResult mdrHeadphonesSetNoiseControl(MDRHeadphones* headphones, const MDRNoiseControl* noise_control);
MDR_API MDRResult mdrHeadphonesGetSpeakToChat(MDRHeadphones* headphones, MDRSpeakToChat* out_speak_to_chat);
MDR_API MDRResult mdrHeadphonesSetSpeakToChat(MDRHeadphones* headphones, const MDRSpeakToChat* speak_to_chat);
MDR_API MDRResult mdrHeadphonesGetListening(MDRHeadphones* headphones, MDRListening* out_listening);
MDR_API MDRResult mdrHeadphonesSetListening(MDRHeadphones* headphones, const MDRListening* listening);
MDR_API MDRResult mdrHeadphonesGetEqualizer(MDRHeadphones* headphones, MDREqualizer* out_equalizer);
MDR_API MDRResult mdrHeadphonesSetEqualizer(MDRHeadphones* headphones, const MDREqualizer* equalizer);
MDR_API MDRResult mdrHeadphonesGetEqualizerBands(MDRHeadphones* headphones, int8_t* bands, uint32_t* inout_count);
MDR_API MDRResult mdrHeadphonesSetEqualizerBands(MDRHeadphones* headphones, const int8_t* bands, uint32_t count);

/* Paired devices and pairing. Device names/IDs use MDR_TEXT_* with index. */
MDR_API MDRResult mdrHeadphonesGetPairedDevices(MDRHeadphones* headphones, MDRPairedDevice* devices,
                                                uint32_t* inout_count);
MDR_API MDRResult mdrHeadphonesSetPairedDevice(MDRHeadphones* headphones, const MDRPairedDeviceAction* action);
MDR_API MDRResult mdrHeadphonesGetPairing(MDRHeadphones* headphones, MDRPairing* out_pairing);
MDR_API MDRResult mdrHeadphonesSetPairing(MDRHeadphones* headphones, const MDRPairing* pairing);

/**
 * @brief Whether the headphones may switch playback to the other multipoint device on their own.
 *
 * When enabled, switching in-between multipoint devices is allowed. Otherwise active playback changes
 * will NOT cause the headphones to switch to the other multipoint device.
 * Requires @ref MDR_FEATURE_SOURCE_SWITCH_CONTROL.
 *
 * @note Contribution by @jkolo in https://github.com/mos9527/SonyHeadphonesClient/pull/57
 */
MDR_API MDRResult mdrHeadphonesGetSourceSwitchControl(MDRHeadphones* headphones, MDRBoolean* out_enabled);
MDR_API MDRResult mdrHeadphonesSetSourceSwitchControl(MDRHeadphones* headphones, MDRBoolean enabled);

/**
 * @brief Outcome the headphones reported for the last source switch control request.
 *
 * This is only guaranteed to be valid after a @ref mdrHeadphonesSetSourceSwitchControl call AND a
 * @ref MDR_EVENT_PAIRED_DEVICES_CHANGED event.
 *
 * The result is otherwise undefined.
 *
 * @note Contribution by @jkolo in https://github.com/mos9527/SonyHeadphonesClient/pull/57
 */
MDR_API MDRResult mdrHeadphonesGetSourceSwitchControlResult(MDRHeadphones* headphones,
                                                            MDRSourceSwitchControlResult* out_result);

/* General settings and assignable controls. */
MDR_API MDRResult mdrHeadphonesGetGeneralSettingInfo(MDRHeadphones* headphones, MDRGeneralSettingInfo* settings,
                                                     uint32_t* inout_count);
MDR_API MDRResult mdrHeadphonesGetGeneralSetting(MDRHeadphones* headphones, uint32_t index,
                                                 MDRGeneralSetting* out_setting);
MDR_API MDRResult mdrHeadphonesSetGeneralSetting(MDRHeadphones* headphones, const MDRGeneralSetting* setting);
MDR_API MDRResult mdrHeadphonesGetAssignableControls(MDRHeadphones* headphones, MDRAssignableControls* out_controls);
MDR_API MDRResult mdrHeadphonesSetAssignableControls(MDRHeadphones* headphones, const MDRAssignableControls* controls);

/* Power, wearing behavior, voice guidance, and related system settings. */
MDR_API MDRResult mdrHeadphonesGetPower(MDRHeadphones* headphones, MDRPower* out_power);
MDR_API MDRResult mdrHeadphonesSetPower(MDRHeadphones* headphones, const MDRPower* power);
MDR_API MDRResult mdrHeadphonesGetVoiceGuidance(MDRHeadphones* headphones, MDRVoiceGuidance* out_voice_guidance);
MDR_API MDRResult mdrHeadphonesSetVoiceGuidance(MDRHeadphones* headphones, const MDRVoiceGuidance* voice_guidance);
MDR_API MDRResult mdrHeadphonesGetConnectionMode(MDRHeadphones* headphones, MDRConnectionMode* out_mode);
MDR_API MDRResult mdrHeadphonesSetConnectionMode(MDRHeadphones* headphones, const MDRConnectionMode* mode);
MDR_API MDRResult mdrHeadphonesGetSafeListening(MDRHeadphones* headphones, MDRSafeListening* out_safe_listening);
MDR_API MDRResult mdrHeadphonesSetSafeListening(MDRHeadphones* headphones, const MDRSafeListening* safe_listening);

#ifdef __cplusplus
}
#endif
