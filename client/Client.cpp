#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <span>
#include <string>
#include <tuple>
#include <utility>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <SDL3/SDL.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <mdr-c/Headphones.h>
#include <mdr/Protocol.hpp>
#include "Fonts/PlexSansIcon.h"
#include "MaterialYouTheme.hpp"
#include "Recorder.hpp"
#include "Platform/Platform.hpp"
#include "I18N/Strings.hpp"

static mdr::String ImTextLabel(i18n::TextId id, const char* icon = "")
{
    return mdr::Format("{}{}{}###{}", icon, *icon ? " " : "",
                       i18n::Translate(id, clientGetAppLocale()), i18n::Index(id));
}

#define TrLable(id, ...) (ImTextLabel(id __VA_OPT__(,) __VA_ARGS__).c_str())

#ifdef MDR_CLIENT_DEBUGGER
#include "Debugger.hpp"
#endif

MDRHeadphones* gDevice;
mdr::String gHeadphonesError;
#ifdef MDR_CLIENT_DEBUGGER
bool gDebuggerOpen{};
bool gDebuggerOnlyMode{};
#endif

#pragma region Enum Names
const char* FormatAudioCodec(MDRAudioCodec codec)
{
    switch (codec)
    {
    case MDR_AUDIO_CODEC_UNKNOWN:
        return Tr(i18n::TextId::Unsettled);
    case MDR_AUDIO_CODEC_SBC:
        return Tr(i18n::TextId::CodecSbc);
    case MDR_AUDIO_CODEC_AAC:
        return Tr(i18n::TextId::CodecAac);
    case MDR_AUDIO_CODEC_LDAC:
        return Tr(i18n::TextId::CodecLdac);
    case MDR_AUDIO_CODEC_APTX:
        return Tr(i18n::TextId::CodecAptx);
    case MDR_AUDIO_CODEC_APTX_HD:
        return Tr(i18n::TextId::CodecAptxHd);
    case MDR_AUDIO_CODEC_LC3:
        return Tr(i18n::TextId::CodecLc3);
    default:
    case MDR_AUDIO_CODEC_OTHER:
        return Tr(i18n::TextId::Unknown);
    }
}

const char* FormatDseeType(MDRDSEEType type)
{
    switch (type)
    {
    case MDR_DSEE_HX:
        return Tr(i18n::TextId::DseeHx);
    case MDR_DSEE_STANDARD:
        return Tr(i18n::TextId::Dsee);
    case MDR_DSEE_HX_AI:
        return Tr(i18n::TextId::DseeHxAi);
    case MDR_DSEE_ULTIMATE:
        return Tr(i18n::TextId::DseeUltimate);
    default:
        return Tr(i18n::TextId::DseeUnknown);
    }
}

const char* FormatChargingState(MDRChargingState status)
{
    switch (status)
    {
    case MDR_CHARGING_YES:
        return Tr(i18n::TextId::Charging);
    case MDR_CHARGING_COMPLETE:
        return Tr(i18n::TextId::Charged);
    case MDR_CHARGING_NO:
        return ""; // Hidden
    default:
    case MDR_CHARGING_UNKNOWN:
        return Tr(i18n::TextId::Unknown);
    }
}

const char* FormatAdaptiveSensitivity(MDRAdaptiveSensitivity status)
{
    switch (status)
    {
    case MDR_ADAPTIVE_SENSITIVITY_STANDARD:
        return Tr(i18n::TextId::Standard);
    case MDR_ADAPTIVE_SENSITIVITY_HIGH:
        return Tr(i18n::TextId::High);
    case MDR_ADAPTIVE_SENSITIVITY_LOW:
        return Tr(i18n::TextId::Low);
    default:
        return Tr(i18n::TextId::Unknown);
    }
}

const char* FormatSpeechSensitivity(MDRSpeechSensitivity status)
{
    switch (status)
    {
    case MDR_SPEECH_SENSITIVITY_AUTO:
        return Tr(i18n::TextId::Auto);
    case MDR_SPEECH_SENSITIVITY_HIGH:
        return Tr(i18n::TextId::High);
    case MDR_SPEECH_SENSITIVITY_LOW:
        return Tr(i18n::TextId::Low);
    default:
        return Tr(i18n::TextId::Unknown);
    }
}

const char* FormatSpeakTimeout(MDRSpeakTimeout status)
{
    switch (status)
    {
    case MDR_SPEAK_TIMEOUT_SHORT:
        return Tr(i18n::TextId::TimeoutShort);
    case MDR_SPEAK_TIMEOUT_MEDIUM:
        return Tr(i18n::TextId::TimeoutStandard);
    case MDR_SPEAK_TIMEOUT_LONG:
        return Tr(i18n::TextId::TimeoutLong);
    case MDR_SPEAK_TIMEOUT_MANUAL:
        return Tr(i18n::TextId::TimeoutManual);
    default:
        return Tr(i18n::TextId::Unknown);
    }
}

const char* FormatSourceSwitchControlResult(MDRSourceSwitchControlResult result)
{
    switch (result)
    {
    case MDR_SOURCE_SWITCH_CONTROL_FAILED_ON_CALL:
        return Tr(i18n::TextId::SourceSwitchCall);
    case MDR_SOURCE_SWITCH_CONTROL_FAILED_NOT_CONNECTED:
        return Tr(i18n::TextId::SourceSwitchNotConnected);
    case MDR_SOURCE_SWITCH_CONTROL_FAILED_VOICE_ASSISTANT:
        return Tr(i18n::TextId::SourceSwitchVoiceAssistant);
    default:
        return Tr(i18n::TextId::SourceSwitchRefused);
    }
}

const char* FormatEqualizerPreset(MDREqualizerPreset id)
{
    switch (id)
    {
    case MDR_EQ_OFF:
        return Tr(i18n::TextId::Off);
    case MDR_EQ_ROCK:
        return Tr(i18n::TextId::EqRock);
    case MDR_EQ_POP:
        return Tr(i18n::TextId::EqPop);
    case MDR_EQ_JAZZ:
        return Tr(i18n::TextId::EqJazz);
    case MDR_EQ_DANCE:
        return Tr(i18n::TextId::EqDance);
    case MDR_EQ_EDM:
        return Tr(i18n::TextId::EqEdm);
    case MDR_EQ_R_AND_B_HIP_HOP:
        return Tr(i18n::TextId::EqRhythmAndBlues);
    case MDR_EQ_ACOUSTIC:
        return Tr(i18n::TextId::EqAcoustic);
    case MDR_EQ_BRIGHT:
        return Tr(i18n::TextId::EqBright);
    case MDR_EQ_EXCITED:
        return Tr(i18n::TextId::EqExcited);
    case MDR_EQ_MELLOW:
        return Tr(i18n::TextId::EqMellow);
    case MDR_EQ_RELAXED:
        return Tr(i18n::TextId::EqRelaxed);
    case MDR_EQ_VOCAL:
        return Tr(i18n::TextId::EqVocal);
    case MDR_EQ_TREBLE:
        return Tr(i18n::TextId::EqTreble);
    case MDR_EQ_BASS:
        return Tr(i18n::TextId::EqBass);
    case MDR_EQ_SPEECH:
        return Tr(i18n::TextId::EqSpeech);
    case MDR_EQ_HEAVY:
        return Tr(i18n::TextId::EqHeavy);
    case MDR_EQ_CLEAR:
        return Tr(i18n::TextId::EqClear);
    case MDR_EQ_HARD:
        return Tr(i18n::TextId::EqHard);
    case MDR_EQ_SOFT:
        return Tr(i18n::TextId::EqSoft);
    case MDR_EQ_GAMING:
        return Tr(i18n::TextId::EqGaming);
    case MDR_EQ_FPS_1:
        return Tr(i18n::TextId::EqFps1);
    case MDR_EQ_FPS_2:
        return Tr(i18n::TextId::EqFps2);
    case MDR_EQ_FPS_3:
        return Tr(i18n::TextId::EqFps3);
    case MDR_EQ_CUSTOM:
        return Tr(i18n::TextId::EqCustom);
    case MDR_EQ_USER_1:
        return Tr(i18n::TextId::EqUser1);
    case MDR_EQ_USER_2:
        return Tr(i18n::TextId::EqUser2);
    case MDR_EQ_USER_3:
        return Tr(i18n::TextId::EqUser3);
    case MDR_EQ_USER_4:
        return Tr(i18n::TextId::EqUser4);
    case MDR_EQ_USER_5:
        return Tr(i18n::TextId::EqUser5);
    default:
        return Tr(i18n::TextId::Unknown);
    }
}

const char* FormatAssignableActionKeyLocation(const MDRAssignableControl& control)
{
    switch (control.location)
    {
    case MDR_ASSIGNABLE_ACTION_KEY_LEFT:
        if (control.type == MDR_ASSIGNABLE_ACTION_KEY_TYPE_FACE_TAP)
            return Tr(i18n::TextId::LeftFaceTap);
        else if (control.type == MDR_ASSIGNABLE_ACTION_KEY_TYPE_TOUCH_SENSOR)
            return Tr(i18n::TextId::LeftTouch);
        else
            return Tr(i18n::TextId::LeftButton);
    case MDR_ASSIGNABLE_ACTION_KEY_RIGHT:
        if (control.type == MDR_ASSIGNABLE_ACTION_KEY_TYPE_FACE_TAP)
            return Tr(i18n::TextId::RightFaceTap);
        else if (control.type == MDR_ASSIGNABLE_ACTION_KEY_TYPE_TOUCH_SENSOR)
            return Tr(i18n::TextId::RightTouch);
        else
            return Tr(i18n::TextId::RightButton);
    case MDR_ASSIGNABLE_ACTION_KEY_CUSTOM:
        return Tr(i18n::TextId::CustomButton);
    case MDR_ASSIGNABLE_ACTION_KEY_C:
        return Tr(i18n::TextId::CButton);
    case MDR_ASSIGNABLE_ACTION_KEY_NC_AMB:
        return Tr(i18n::TextId::NcAmbButton);
    case MDR_ASSIGNABLE_ACTION_KEY_NC_AMBIENT:
        return Tr(i18n::TextId::NcAmbientButton);
    default:
        return Tr(i18n::TextId::Unknown);
    }
}

const char* FormatAssignableAction(MDRAssignableAction action)
{
    switch (action)
    {
    case MDR_ASSIGNABLE_NOISE_CONTROL:
        return Tr(i18n::TextId::AmbientSoundControl);
    case MDR_ASSIGNABLE_PLAYBACK:
        return Tr(i18n::TextId::PlaybackControl);
    case MDR_ASSIGNABLE_TRACK_CONTROL:
        return Tr(i18n::TextId::TrackControl);
    case MDR_ASSIGNABLE_VOICE_RECOGNITION:
        return Tr(i18n::TextId::VoiceRecognition);
    case MDR_ASSIGNABLE_GOOGLE_ASSISTANT:
        return Tr(i18n::TextId::GoogleAssistant);
    case MDR_ASSIGNABLE_AMAZON_ALEXA:
        return Tr(i18n::TextId::AmazonAlexa);
    case MDR_ASSIGNABLE_TENCENT_XIAOWEI:
        return Tr(i18n::TextId::TencentXiaowei);
    case MDR_ASSIGNABLE_MICROSOFT_CORTANA:
        return Tr(i18n::TextId::MicrosoftCortana);
    case MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS:
        return Tr(i18n::TextId::AmbientSoundQuickAccess);
    case MDR_ASSIGNABLE_QUICK_ACCESS:
        return Tr(i18n::TextId::QuickAccess);
    case MDR_ASSIGNABLE_VOLUME:
        return Tr(i18n::TextId::VolumeControl);
    case MDR_ASSIGNABLE_PLAYBACK_VOICE_ASSISTANT_LIMITATION:
        return Tr(i18n::TextId::PlaybackVoiceAssistantLimitation);
    case MDR_ASSIGNABLE_TENCENT_XIAOWEI_Q_MSC:
        return Tr(i18n::TextId::TencentXiaoweiQMsc);
    case MDR_ASSIGNABLE_TEAMS:
        return Tr(i18n::TextId::Teams);
    case MDR_ASSIGNABLE_GOOGLE_ASSISTANT_BT_CLASSIC_CAUTION:
        return Tr(i18n::TextId::GoogleAssistantClassicOnly);
    case MDR_ASSIGNABLE_AMAZON_ALEXA_BT_CLASSIC_CAUTION:
        return Tr(i18n::TextId::AmazonAlexaClassicOnly);
    case MDR_ASSIGNABLE_TENCENT_XIAOWEI_BT_CLASSIC_CAUTION:
        return Tr(i18n::TextId::TencentXiaoweiClassicOnly);
    case MDR_ASSIGNABLE_QUICK_ACCESS_BT_CLASSIC_CAUTION:
        return Tr(i18n::TextId::QuickAccessClassicOnly);
    case MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS_BT_CLASSIC_CAUTION:
        return Tr(i18n::TextId::AmbientSoundQuickAccessClassicOnly);
    case MDR_ASSIGNABLE_TENCENT_XIAOWEI_Q_MSC_BT_CLASSIC_CAUTION:
        return Tr(i18n::TextId::TencentXiaoweiQMscClassicOnly);
    case MDR_ASSIGNABLE_NOISE_CONTROL_MIC:
        return Tr(i18n::TextId::AmbientSoundMic);
    case MDR_ASSIGNABLE_LISTENING_MODE_QUICK_ACCESS:
        return Tr(i18n::TextId::ListeningModeQuickAccess);
    case MDR_ASSIGNABLE_NOISE_CONTROL_LISTENING_MODE:
        return Tr(i18n::TextId::AmbientSoundListeningMode);
    case MDR_ASSIGNABLE_CHAT_MIX:
        return Tr(i18n::TextId::ChatMix);
    case MDR_ASSIGNABLE_CUSTOM1:
        return Tr(i18n::TextId::AssignableCustom1);
    case MDR_ASSIGNABLE_CUSTOM2:
        return Tr(i18n::TextId::AssignableCustom2);
    case MDR_ASSIGNABLE_NONE:
        return Tr(i18n::TextId::NoFunction);
    default:
        return Tr(i18n::TextId::Unknown);
    }
}

const char* FormatNoiseButtonMode(MDRNoiseButtonMode function)
{
    switch (function)
    {
    case MDR_NOISE_BUTTON_NONE:
        return Tr(i18n::TextId::NoFunction);
    case MDR_NOISE_BUTTON_NOISE_AMBIENT_OFF:
        return Tr(i18n::TextId::NoiseAmbientOff);
    case MDR_NOISE_BUTTON_NOISE_AMBIENT:
        return Tr(i18n::TextId::NoiseAmbient);
    case MDR_NOISE_BUTTON_NOISE_OFF:
        return Tr(i18n::TextId::NoiseOff);
    case MDR_NOISE_BUTTON_AMBIENT_OFF:
        return Tr(i18n::TextId::AmbientOff);
    default:
        return Tr(i18n::TextId::Unknown);
    }
}

const char* FormatAutoPowerOff(uint32_t minutes)
{
    switch (minutes)
    {
    case 5:
        return Tr(i18n::TextId::PowerOff5Minutes);
    case 15:
        return Tr(i18n::TextId::PowerOff15Minutes);
    case 30:
        return Tr(i18n::TextId::PowerOff30Minutes);
    case 60:
        return Tr(i18n::TextId::PowerOff1Hour);
    case 180:
        return Tr(i18n::TextId::PowerOff3Hours);
    case 0:
        return Tr(i18n::TextId::PowerOffNever);
    default:
        return Tr(i18n::TextId::Unknown);
    }
}

const char* FormatFeatureAvailability(MDRFeatureAvailability availability)
{
    switch (availability)
    {
    case MDR_AVAILABILITY_AVAILABLE:
        return PSI_OK;
    case MDR_AVAILABILITY_UNAVAILABLE:
        return PSI_REMOVE;
    default:
        return "?";
    }
}
#pragma endregion

bool FeatureAvailable(MDRFeature feature)
{
    MDRFeatureAvailability availability = MDR_AVAILABILITY_UNKNOWN;
    return gDevice && mdrHeadphonesGetFeature(gDevice, feature, &availability) == MDR_RESULT_OK &&
        availability == MDR_AVAILABILITY_AVAILABLE;
}

mdr::String GetText(MDRText text, uint32_t index = 0)
{
    if (!gDevice)
        return {};
    uint32_t size = 0;
    if (mdrHeadphonesGetText(gDevice, text, index, nullptr, &size) != MDR_RESULT_OK || size == 0)
        return {};
    mdr::Vector<char> buffer(size);
    if (mdrHeadphonesGetText(gDevice, text, index, buffer.data(), &size) != MDR_RESULT_OK)
        return {};
    return buffer.data();
}

uint8_t GetModelColor()
{
    MDRModel identity{};
    return gDevice && mdrHeadphonesGetModel(gDevice, &identity) == MDR_RESULT_OK ? identity.model_color : 0;
}

mdr::Vector<MDRBattery> GetBatteries()
{
    mdr::Vector<MDRBattery> values(4);
    uint32_t count = static_cast<uint32_t>(values.size());
    if (!gDevice || mdrHeadphonesGetBatteries(gDevice, values.data(), &count) != MDR_RESULT_OK)
        return {};
    values.resize(count);
    return values;
}

mdr::Vector<MDRPairedDevice> GetPairedDevices()
{
    mdr::Vector<MDRPairedDevice> values(16);
    uint32_t count = static_cast<uint32_t>(values.size());
    if (!gDevice || mdrHeadphonesGetPairedDevices(gDevice, values.data(), &count) != MDR_RESULT_OK)
        return {};
    values.resize(count);
    return values;
}

mdr::Vector<MDRGeneralSettingInfo> GetGeneralSettingInfos()
{
    mdr::Vector<MDRGeneralSettingInfo> values(4);
    uint32_t count = static_cast<uint32_t>(values.size());
    if (!gDevice || mdrHeadphonesGetGeneralSettingInfo(gDevice, values.data(), &count) != MDR_RESULT_OK)
        return {};
    values.resize(count);
    return values;
}

mdr::Vector<std::pair<MDRGeneralSettingInfo, MDRGeneralSetting>>
GetGeneralSettings(const mdr::Vector<MDRGeneralSettingInfo>& infos)
{
    mdr::Vector<std::pair<MDRGeneralSettingInfo, MDRGeneralSetting>> values;
    values.reserve(infos.size());
    for (const MDRGeneralSettingInfo& info : infos)
    {
        MDRGeneralSetting setting{};
        if (!gDevice || mdrHeadphonesGetGeneralSetting(gDevice, info.index, &setting) != MDR_RESULT_OK)
            continue;
        values.emplace_back(info, setting);
    }
    return values;
}

mdr::Vector<MDRAssignableControl> GetAssignableControls()
{
    mdr::Vector<MDRAssignableControl> values(2); // Custom only, or Left+Right
    uint32_t count = static_cast<uint32_t>(values.size());
    if (!gDevice || mdrHeadphonesGetAssignableControls(gDevice, values.data(), &count) != MDR_RESULT_OK)
        return {};
    values.resize(count);
    return values;
}

mdr::Vector<MDRAssignableAction> GetAssignableControlActions(MDRAssignableActionKeyLocation key)
{
    mdr::Vector<MDRAssignableAction> values(7); // Number of MDRAssignableAction enum values
    uint32_t count = static_cast<uint32_t>(values.size());
    if (!gDevice || mdrHeadphonesGetAssignableControlActions(gDevice, key, values.data(), &count) != MDR_RESULT_OK)
        return {};
    values.resize(count);
    return values;
}

mdr::Vector<int> GetEqualizerBands()
{
    mdr::Vector<int8_t> bytes(16);
    uint32_t count = static_cast<uint32_t>(bytes.size());
    if (!gDevice || mdrHeadphonesGetEqualizerBands(gDevice, bytes.data(), &count) != MDR_RESULT_OK)
        return {};
    bytes.resize(count);
    mdr::Vector<int> values;
    values.reserve(count);
    for (const int8_t value : bytes)
        values.emplace_back(value);
    return values;
}

mdr::Vector<std::pair<MDREqualizerPreset, mdr::String>> GetEqualizerPresets()
{
    uint32_t count = 0;
    if (!gDevice || mdrHeadphonesGetEqualizerPresets(gDevice, nullptr, &count) != MDR_RESULT_OK || count == 0)
        return {};
    mdr::Vector<MDREqualizerPreset> ids(count);
    if (mdrHeadphonesGetEqualizerPresets(gDevice, ids.data(), &count) != MDR_RESULT_OK)
        return {};
    mdr::Vector<std::pair<MDREqualizerPreset, mdr::String>> presets;
    presets.reserve(count);
    for (uint32_t i = 0; i < count; ++i)
    {
        if (ids[i] == MDR_EQ_UNKNOWN)
            continue;
        presets.emplace_back(ids[i], GetText(MDR_TEXT_EQUALIZER_PRESET_NAME, i));
    }
    return presets;
}

void SetEqualizerBands(const mdr::Vector<int>& values)
{
    mdr::Vector<int8_t> bytes;
    bytes.reserve(values.size());
    for (const int value : values)
        bytes.emplace_back(static_cast<int8_t>(value));
    if (!bytes.empty())
        mdrHeadphonesSetEqualizerBands(gDevice, bytes.data(), static_cast<uint32_t>(bytes.size()));
}

struct ClientState
{
    MDRModel mModel{};
    mdr::Vector<MDRBattery> mBatteries;
    MDRPlayback mPlayback{};
    MDRNoiseControl mNoise{};
    MDRSpeakToChat mSpeakToChat{};
    MDRListening mListening{};
    MDREqualizer mEqualizer{};
    mdr::Vector<int> mEqualizerBands;
    mdr::Vector<std::pair<MDREqualizerPreset, mdr::String>> mEqualizerPresets;
    mdr::Vector<MDRPairedDevice> mPairedDevices;
    MDRPairing mPairing{};
    MDRWearingStatus mWearingStatus{};
    mdr::Vector<std::pair<MDRGeneralSettingInfo, MDRGeneralSetting>> mGeneralSettings;
    bool mModelAvailable;
    bool mNoiseAvailable;
    bool mSpeakToChatAvailable;
    bool mListeningAvailable;
    bool mEqualizerAvailable;
    bool mPairingAvailable;
    bool mWearingStatusAvailable;
    bool mPlaybackVolumeStaged;
    // Set to true to update batteries, etc for V2 and  playback vol/metadata once headphones become available.
    bool mPendingSync;
} gState;

static bool gAlertPending{};
static mdr::String gAlertMessage;

void RefreshPlaybackState()
{
    MDRPlayback playback{};
    if (mdrHeadphonesGetPlayback(gDevice, &playback) != MDR_RESULT_OK)
        return;
    gState.mPlayback.status = playback.status;
    if (gState.mPlaybackVolumeStaged && playback.volume == gState.mPlayback.volume)
        gState.mPlaybackVolumeStaged = false;
    if (!gState.mPlaybackVolumeStaged)
        gState.mPlayback.volume = playback.volume;
}

void RefreshClientState()
{
    gState = {};
    gState.mModelAvailable = mdrHeadphonesGetModel(gDevice, &gState.mModel) == MDR_RESULT_OK;
    gState.mBatteries = GetBatteries();
    RefreshPlaybackState();
    gState.mNoiseAvailable = mdrHeadphonesGetNoiseControl(gDevice, &gState.mNoise) == MDR_RESULT_OK;
    gState.mSpeakToChatAvailable = mdrHeadphonesGetSpeakToChat(gDevice, &gState.mSpeakToChat) == MDR_RESULT_OK;
    gState.mListeningAvailable = mdrHeadphonesGetListening(gDevice, &gState.mListening) == MDR_RESULT_OK;
    gState.mEqualizerAvailable = mdrHeadphonesGetEqualizer(gDevice, &gState.mEqualizer) == MDR_RESULT_OK;
    gState.mEqualizerBands = GetEqualizerBands();
    gState.mEqualizerPresets = GetEqualizerPresets();
    gState.mPairedDevices = GetPairedDevices();
    gState.mPairingAvailable = mdrHeadphonesGetPairing(gDevice, &gState.mPairing) == MDR_RESULT_OK;
    gState.mWearingStatusAvailable = FeatureAvailable(MDR_FEATURE_WEARING_STATUS) &&
        mdrHeadphonesGetWearingStatus(gDevice, &gState.mWearingStatus) == MDR_RESULT_OK;
    gState.mGeneralSettings = GetGeneralSettings(GetGeneralSettingInfos());
}

// See https://github.com/mos9527/SonyHeadphonesClient/pull/63
static bool gPauseMediaOnRemove = false;
static ClientMediaPause* gMediaPause = nullptr;

void clientSetPauseMediaOnRemove(bool enabled) { gPauseMediaOnRemove = enabled; }

// True when the headphones' auto pause will go somewhere other than this computer: another
// device, not one of our own adapters, is connected alongside us.
bool HostMediaNeedsPausing()
{
    for (const MDRPairedDevice& device : gState.mPairedDevices)
    {
        if (!device.connected)
            continue;
        int isLocal = 0;
        if (clientPlatformIsLocalBluetoothAddress(device.macAddress, &isLocal) != MDR_RESULT_OK)
            return false;
        if (!isLocal)
            return true;
    }
    return false;
}

const char* FormatWearingStatus(MDRWearingStatus status)
{
    switch (status)
    {
    case MDR_WEARING_STATUS_WORN:
        return Tr(i18n::TextId::Worn);
    case MDR_WEARING_STATUS_LEFT_REMOVED:
        return Tr(i18n::TextId::LeftRemoved);
    case MDR_WEARING_STATUS_RIGHT_REMOVED:
        return Tr(i18n::TextId::RightRemoved);
    case MDR_WEARING_STATUS_REMOVED:
        return Tr(i18n::TextId::Removed);
    default:
        return Tr(i18n::TextId::Unknown);
    }
}

void OnWearingStatusChanged()
{
    const MDRWearingStatus previous = gState.mWearingStatus;
    gState.mWearingStatusAvailable = mdrHeadphonesGetWearingStatus(gDevice, &gState.mWearingStatus) == MDR_RESULT_OK;
    if (!gPauseMediaOnRemove || !gState.mWearingStatusAvailable || gState.mWearingStatus == previous)
        return;
    if (gState.mWearingStatus == MDR_WEARING_STATUS_REMOVED)
    {
        if (!gMediaPause && HostMediaNeedsPausing())
            gMediaPause = clientPlatformMediaPause();
    }
    else if (gState.mWearingStatus == MDR_WEARING_STATUS_WORN && gMediaPause)
    {
        clientPlatformMediaResume(gMediaPause);
        gMediaPause = nullptr;
    }
}

void CloseDevice()
{
    if (!gDevice)
    {
        gAlertPending = false;
        gAlertMessage.clear();
        return;
    }
    gHeadphonesError = GetText(MDR_TEXT_LAST_ERROR);
    mdrHeadphonesSetPacketCallback(gDevice, nullptr, nullptr);
#ifdef MDR_CLIENT_DEBUGGER
    clientDebuggerDetach();
#endif
    mdrHeadphonesDestroy(gDevice);
    gDevice = nullptr;
    gState = {};
    gAlertPending = false;
    gAlertMessage.clear();
}

#pragma region ImGui Extra
#define IM_FONTSIZE_TITLE 20
#define IM_FONTSIZE_HEADING 18
#define IM_FONTSIZE_SUBHEADING 16
#define IM_FONTSIZE_BODY 15
#define IM_FONTSIZE_CAPTION 14

struct ImStylesRAII
{
    int numVars = 0, numColors = 0, numFonts = 0;
    template <typename... Args>
    void PushVar(ImGuiStyleVar idx, Args&&... args)
    {
        ImGui::PushStyleVar(idx, args...), numVars++;
    }
    template <typename... Args>
    void PushCol(ImGuiCol idx, Args&&... args)
    {
        ImGui::PushStyleColor(idx, args...), numColors++;
    }
    template <typename... Args>
    void PushFont(ImFont* font, Args&&... args)
    {
        ImGui::PushFont(font, args...), numFonts++;
    }
    void PushFont(float fontSize)
    {
        PushFont(nullptr, fontSize);
    }
    ~ImStylesRAII()
    {
        ImGui::PopStyleVar(numVars);
        ImGui::PopStyleColor(numColors);
        while (numFonts > 0)
        {
            ImGui::PopFont();
            --numFonts;
        }
    }
};

void ImHeading(const char* text, float fontSize = IM_FONTSIZE_HEADING)
{
    ImStylesRAII scope;
    scope.PushFont(fontSize);
    ImGui::SeparatorText(text);
}

bool ImHeadingTreeNode(const char* label, ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen)
{
    ImStylesRAII scope;
    scope.PushFont(IM_FONTSIZE_HEADING);
    return ImGui::TreeNodeEx(label, flags);
}

constexpr ImGuiWindowFlags kImWindowFlagsTopMost =
    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar;

// -- https://github.com/ocornut/imgui/issues/3379#issuecomment-2943903877
void ImScrollWhenDraggingOnVoid(const ImVec2& delta, ImGuiMouseButton mouse_button)
{
    using namespace ImGui;

    ImGuiContext& g = *GetCurrentContext();
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##scrolldraggingoverlay");
    KeepAliveID(id);

    // Passing 0 to ItemHoverable means it doesn't set HoveredId, which is what we want.
    if (g.ActiveId == 0 && ItemHoverable(window->Rect(), 0, g.CurrentItemFlags) &&
        IsMouseClicked(mouse_button, ImGuiInputFlags_None, id))
        SetActiveID(id, window);
    if (g.ActiveId == id && !g.IO.MouseDown[mouse_button])
        ClearActiveID();

    // Set keep underlying highlight. However, mouse not necessarily hovering same item creates a weird disconnect.
    // if (g.ActiveId == id)
    //    g.ActiveIdAllowOverlap = true;

    // if (g.ActiveId == id && delta.x != 0.0f)
    //     SetScrollX(window, window->Scroll.x + delta.x);
    if (g.ActiveId == id && delta.y != 0.0f)
        SetScrollY(window, window->Scroll.y - delta.y);
}

void ImScrollWhenDraggingAnywhere(const ImVec2& delta, ImGuiMouseButton mouse_button)
{
    ImGuiContext& g = *ImGui::GetCurrentContext();
    const bool backup_hovered_id_allow_overlap = g.HoveredIdAllowOverlap;
    g.HoveredIdAllowOverlap = true;
    ImScrollWhenDraggingOnVoid(delta, mouse_button);
    g.HoveredIdAllowOverlap = backup_hovered_id_allow_overlap; // As we know ScrollWhenDraggingOnVoid() doesn't changed
                                                               // HoveredId we can unconditionally restore.
}
// --

// Only useful if you're manipulating the DrawList which has positions
// that are _NOT_ window local
std::tuple<ImVec2, ImVec2, ImDrawList*> ImWindowDrawOffsetRegionList()
{
    ImVec2 offset = ImGui::GetCursorScreenPos();
    ImVec2 region = ImGui::GetContentRegionAvail();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    return {offset, region, drawList};
}

// Centered text.
void ImTextCentered(const char* text, float fontSize = 0.0f)
{
    ImStylesRAII scope;
    scope.PushFont(fontSize);
    ImVec2 size = ImGui::CalcTextSize(text);
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2 - size.x / 2 + ImGui::GetStyle().FramePadding.x);
    ImGui::Text("%s", text);
}

// Generate linear, monotonous ints of [0, count - 1] at interval of intervalMS
int ImBlink(int intervalMS, int count)
{
    size_t time = ImGui::GetTime() * 1000;
    time = time % (intervalMS * count);
    return time / intervalMS;
}

// Generate linear, monotonous float in range of [0, 1] at interval of intervalMS
float ImBlinkF(float intervalMS)
{
    const double interval = std::max(intervalMS, 1.0f) / 1000.0;
    return static_cast<float>(std::fmod(ImGui::GetTime(), interval) / interval);
}

void ImSpinner(float interval, float width, ImU32 color, float thickness = 4.0f, bool centerX = false,
               bool centerY = false)
{
    const auto& style = ImGui::GetStyle();
    const float availableWidth = std::max(1.0f, ImGui::GetContentRegionAvail().x);
    width = std::clamp(width, 1.0f, availableWidth);
    thickness = std::clamp(thickness, 1.0f, width);
    const float itemHeight =
        centerY ? std::max(thickness, ImGui::GetFrameHeight()) : thickness + style.FramePadding.y * 2.0f;
    ImVec2 offset = ImGui::GetCursorScreenPos();
    if (centerX)
    {
        offset.x += (availableWidth - width) * 0.5f;
        ImGui::SetCursorScreenPos(offset);
    }
    ImGui::Dummy({width, itemHeight});
    if (!ImGui::IsItemVisible())
        return;
    offset.y += (itemHeight - thickness) * 0.5f;

    ImDrawList* draw = ImGui::GetWindowDrawList();
    const float radius = thickness * 0.5f;
    const float fringe = (draw->Flags & ImDrawListFlags_AntiAliasedFill) ? draw->_FringeScale * 0.5f : 0.0f;
    const float innerRadius = std::max(0.0f, radius - fringe);
    const float outerRadius = radius + fringe;
    const float halfBand = width * 0.3f;
    const float bandCenter = offset.x - halfBand + ImBlinkF(interval) * (width + halfBand * 2.0f);
    constexpr int segments = 48;
    constexpr int capSegments = 8;
    constexpr int stripCount = segments + capSegments * 2;
    draw->PrimReserve(stripCount * 18, (stripCount + 1) * 4);
    const unsigned int vertexStart = draw->_VtxCurrentIdx;
    const ImVec2 uv = draw->_Data->TexUvWhitePixel;
    const ImU32 transparent = color & ~IM_COL32_A_MASK;
    for (int i = 0; i <= stripCount; ++i)
    {
        ImVec2 center{offset.x + radius, offset.y + radius};
        ImVec2 normal{0.0f, -1.0f};
        if (i < capSegments || i > capSegments + segments)
        {
            const bool left = i < capSegments;
            const float angle =
                left ? IM_PI - IM_PI * 0.5f * i / capSegments : IM_PI * 0.5f * (stripCount - i) / capSegments;
            normal = {std::cos(angle), -std::sin(angle)};
            if (!left)
                center.x += width - thickness;
        }
        else
            center.x += (width - thickness) * (i - capSegments) / segments;
        const ImVec2 innerTop = center + normal * innerRadius;
        const ImVec2 outerTop = center + normal * outerRadius;
        const float intensity = std::clamp(1.0f - std::abs(innerTop.x - bandCenter) / halfBand, 0.0f, 1.0f);
        const float opacity = 0.14f + 0.86f * intensity * intensity * (3.0f - 2.0f * intensity);
        const ImU32 alpha = static_cast<ImU32>(((color & IM_COL32_A_MASK) >> IM_COL32_A_SHIFT) * opacity);
        const ImU32 shaded = transparent | (alpha << IM_COL32_A_SHIFT);
        draw->PrimWriteVtx(outerTop, uv, transparent);
        draw->PrimWriteVtx(innerTop, uv, shaded);
        draw->PrimWriteVtx({innerTop.x, center.y * 2.0f - innerTop.y}, uv, shaded);
        draw->PrimWriteVtx({outerTop.x, center.y * 2.0f - outerTop.y}, uv, transparent);
    }
    for (int i = 0; i < stripCount; ++i)
    {
        const unsigned int base = vertexStart + i * 4;
        for (int row = 0; row < 3; ++row)
        {
            draw->PrimWriteIdx(static_cast<ImDrawIdx>(base + row));
            draw->PrimWriteIdx(static_cast<ImDrawIdx>(base + row + 4));
            draw->PrimWriteIdx(static_cast<ImDrawIdx>(base + row + 5));
            draw->PrimWriteIdx(static_cast<ImDrawIdx>(base + row));
            draw->PrimWriteIdx(static_cast<ImDrawIdx>(base + row + 5));
            draw->PrimWriteIdx(static_cast<ImDrawIdx>(base + row + 1));
        }
    }
}

// Fill the available horizontal region with lineTotal amount of buttons
// This is used for modal dialogues
bool ImModalButton(const char* label, int lineIndex = 0, int lineTotal = 1)
{
    assert(lineIndex < lineTotal);
    auto& style = ImGui::GetStyle();
    float padding = style.FramePadding.x;
    float width = ImGui::GetContentRegionAvail().x / lineTotal;
    if (lineIndex)
        ImGui::SameLine();
    return ImGui::Button(label, lineTotal > 1 ? ImVec2{width - padding, 0} : ImVec2{width, 0});
}

void ImSetNextWindowCentered()
{
    auto& style = ImGui::GetStyle();
    float padding = style.FramePadding.x;
    ImGui::SetNextWindowPos({0.0f, ImGui::GetContentRegionAvail().y / 2 + padding}, 0, {0.0f, 0.5f});
    ImGui::SetNextWindowSize({ImGui::GetIO().DisplaySize.x, 0});
}

// Outer size of a bordered text badge: the text plus FramePadding / 2 on every side.
ImVec2 ImBadgeSize(const char* text) { return ImGui::CalcTextSize(text) + ImGui::GetStyle().FramePadding; }

bool ImBadge(const char* text, ImVec2 pos, ImU32 borderColor, ImU32 textColor, float rounding = 0.0f,
             float thickness = 1.0f, bool* hovered = nullptr, const char* id = nullptr)
{
    const ImVec2 size = ImBadgeSize(text);
    ImGui::SetCursorScreenPos(pos);
    ImGui::PushID(id ? id : text);
    const bool clicked = ImGui::InvisibleButton("##badge", size);
    ImGui::PopID();
    if (hovered)
        *hovered = ImGui::IsItemHovered();
    ImDrawList* draw = ImGui::GetWindowDrawList();
    draw->AddRect(pos, pos + size, borderColor, rounding, ImDrawFlags_None, thickness);
    draw->AddText(pos + ImGui::GetStyle().FramePadding / 2, textColor, text);
    return clicked;
}

template <typename T, size_t Extent, typename Formatter>
bool ImComboBoxItems(const char* label, std::span<const T, Extent> items, T& selection, Formatter format)
{
    bool changed = false;
    if (ImGui::BeginCombo(label, format(selection)))
    {
        for (T const& i : items)
        {
            bool selected = i == selection;
            const mdr::String itemLabel = mdr::Format("{}###{}", format(i), static_cast<int>(i));
            if (ImGui::Selectable(itemLabel.c_str(), selected))
                selection = i, changed = true;
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    return changed;
}

bool ImEqualizer(std::span<int> bands)
{
    constexpr const char* kBand5[] = {"400", "1k", "2.5k", "6.3k", "16k"};
    constexpr const char* kBand10[] = {"31", "63", "125", "250", "500", "1k", "2k", "4k", "8k", "16k"};
    const char* const* kBands = nullptr;
    int numBands = static_cast<int>(bands.size());
    int mn = 0, mx = 0;
    if (numBands == 10)
        kBands = kBand10, mn = -6, mx = 6;
    if (numBands == 5)
        kBands = kBand5, mn = -10, mx = 10;
    if (!kBands)
    {
        ImGui::TextUnformatted(mdr::Format(fmt::runtime(Tr(i18n::TextId::EqUnavailable)), numBands).c_str());
        return false;
    }
    bool changed = false;
    auto& style = ImGui::GetStyle();
    float padding = style.FramePadding.x;
    auto [offset, region, draw] = ImWindowDrawOffsetRegionList();
    float bandWidth = region.x / numBands - padding;
    float bandHeight = std::max(region.y, 160.0f);
    if (numBands == 5)
        ImHeading(Tr(i18n::TextId::EqFiveBand), IM_FONTSIZE_SUBHEADING);
    if (numBands == 10)
        ImHeading(Tr(i18n::TextId::EqTenBand), IM_FONTSIZE_SUBHEADING);
    for (int i = 0; i < numBands; ++i)
    {
        ImGui::BeginGroup();
        ImGui::PushID(i);
        changed |= ImGui::VSliderInt("##v", ImVec2{bandWidth, bandHeight}, &bands[i], mn, mx);
        ImGui::PopID();

        float textWidth = ImGui::CalcTextSize(kBands[i]).x;
        float textOffset = (bandWidth - textWidth) * 0.5f;
        if (textOffset > 0.0f)
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
        ImGui::TextUnformatted(kBands[i]);

        ImGui::EndGroup();
        if (i != numBands - 1)
            ImGui::SameLine(0.0f, padding);
    }
    return changed;
}
#pragma endregion

#pragma region States
enum CONN_STATE
{
    CONN_STATE_NO_CONNECTION,
    CONN_STATE_CONNECTING,
    CONN_STATE_CONNECTED,
    CONN_STATE_DISCONNECTED // Passive, or from errors
} connState{};

enum DEVICE_TYPE
{
    DEVICE_TYPE_AUTO,
    DEVICE_TYPE_V2,
    DEVICE_TYPE_V1
};

struct ConnectionAttemptState
{
    static constexpr uint64_t kAttemptTimeoutMs = 10'000;

    std::string address;
    std::array<const char*, 2> services{};
    std::string lastError;
    size_t serviceCount{};
    size_t serviceIndex{};
    uint64_t deadlineMs{};
    bool ble{};
};

ConnectionAttemptState connectionAttempt;

const char* ConnectionAttemptName()
{
    if (connectionAttempt.ble)
        return Tr(i18n::TextId::BleName);
    if (connectionAttempt.serviceCount == 1)
        return std::strcmp(connectionAttempt.services[0], MDR_SERVICE_UUID_LEGACY) == 0 ? Tr(i18n::TextId::ProtocolV1) : Tr(i18n::TextId::ProtocolV2);
    return connectionAttempt.serviceIndex == 0 ? Tr(i18n::TextId::ProtocolV2) : Tr(i18n::TextId::ProtocolV1);
}

MDRProtocolVersion ConnectionProtocolVersion()
{
    if (connectionAttempt.ble)
        return MDR_PROTOCOL_V2;
    const char* service = connectionAttempt.services[connectionAttempt.serviceIndex];
    if (std::strcmp(service, MDR_SERVICE_UUID_LEGACY) == 0)
        return MDR_PROTOCOL_V1;
    return MDR_PROTOCOL_V2;
}

void CaptureConnectionError(MDRConnection* conn, MDRResult result)
{
    const char* error = mdrConnectionGetLastError(conn);
    connectionAttempt.lastError = error && *error ? error : mdrResultString(result);
}

MDRResult TryConnectionAttempt(MDRConnection* conn)
{
    while (connectionAttempt.serviceIndex < connectionAttempt.serviceCount)
    {
        const MDRResult result = mdrConnectionConnect(conn, connectionAttempt.address.c_str(),
                                                      connectionAttempt.services[connectionAttempt.serviceIndex]);
        if (result == MDR_RESULT_OK || result == MDR_RESULT_INPROGRESS)
        {
            connectionAttempt.deadlineMs = SDL_GetTicks() + ConnectionAttemptState::kAttemptTimeoutMs;
            return result;
        }

        CaptureConnectionError(conn, result);
        mdrConnectionDisconnect(conn);
        ++connectionAttempt.serviceIndex;
    }
    return MDR_RESULT_ERROR_NO_CONNECTION;
}

MDRResult AdvanceConnectionAttempt(MDRConnection* conn, MDRResult reason)
{
    CaptureConnectionError(conn, reason);
    mdrConnectionDisconnect(conn);
    ++connectionAttempt.serviceIndex;
    return TryConnectionAttempt(conn);
}

MDRResult StartConnection(MDRConnection* conn, const char* address, bool usingBLE, DEVICE_TYPE deviceType)
{
    connectionAttempt = {};
    connectionAttempt.address = address;
    connectionAttempt.ble = usingBLE;
    if (usingBLE)
    {
        connectionAttempt.services[0] = MDR_BLE_SERVICE_UUID_TANDEM_OVER_BLE_HPC;
        connectionAttempt.serviceCount = 1;
    }
    else if (deviceType == DEVICE_TYPE_AUTO)
    {
        connectionAttempt.services = {MDR_SERVICE_UUID_XM5, MDR_SERVICE_UUID_LEGACY};
        connectionAttempt.serviceCount = 2;
    }
    else
    {
        connectionAttempt.services[0] = deviceType == DEVICE_TYPE_V2 ? MDR_SERVICE_UUID_XM5 : MDR_SERVICE_UUID_LEGACY;
        connectionAttempt.serviceCount = 1;
    }
    return TryConnectionAttempt(conn);
}
#pragma endregion

void DrawDeviceDiscovery()
{
    assert(connState == CONN_STATE_NO_CONNECTION);
    ImSetNextWindowCentered();
    static bool popup = false;
    if (!popup)
        ImGui::OpenPopup("DeviceDiscovery"), popup = true;
    if (ImGui::BeginPopupModal("DeviceDiscovery", nullptr, kImWindowFlagsTopMost))
    {
        static MDRDeviceInfo* pDeviceInfo = nullptr;
        static int nDeviceInfo = 0;
        ImTextCentered(Tr(i18n::TextId::AppName), ImGui::GetContentRegionAvail().x * 0.05f);
        ImTextCentered(mdr::Format(fmt::runtime(Tr(i18n::TextId::VersionInfo)), CLIENT_VERSION,
                                   MDR_GIT_BRANCH_NAME, MDR_GIT_COMMIT_HASH, MDR_PLATFORM_OS, MDR_PLATFORM_PROCESSOR, clientGetAppLocale())
                           .c_str(), IM_FONTSIZE_CAPTION);
        // Chose, and have the GATT backend active
        static bool usingBLE = false;
        static DEVICE_TYPE deviceType = DEVICE_TYPE_AUTO;
        static int connInitResult = MDR_RESULT_INPROGRESS;
        // BLE / Classic toggle
        bool needSwitchClientPlatform = clientPlatformConnectionGet() == nullptr;
        {
            ImStylesRAII styles;
            styles.PushFont(IM_FONTSIZE_CAPTION);
            styles.PushVar(ImGuiStyleVar_FramePadding, ImVec2{});
            styles.PushVar(ImGuiStyleVar_FrameRounding, 0.0f);
            {
                ImStylesRAII styles;
                if (usingBLE)
                    styles.PushCol(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                if (ImModalButton(TrLable(i18n::TextId::Classic, PSI_BLUETOOTH), 0, 2))
                    usingBLE = false, needSwitchClientPlatform = true;
            }
            {
                ImStylesRAII styles;
                if (!usingBLE)
                    styles.PushCol(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                if (ImModalButton(TrLable(i18n::TextId::Ble, PSI_BLUETOOTH_ALT), 1, 2))
                    usingBLE = true, needSwitchClientPlatform = true;
            }
        }
        ImGui::BeginDisabled(usingBLE);
        {
            ImStylesRAII styles;
            styles.PushFont(IM_FONTSIZE_CAPTION);
            styles.PushVar(ImGuiStyleVar_FramePadding, ImVec2{});
            styles.PushVar(ImGuiStyleVar_FrameRounding, 0.0f);
            const std::array labels{ImTextLabel(i18n::TextId::Auto, PSI_PLUS_SIGN),
                                    ImTextLabel(i18n::TextId::ProtocolV2, PSI_FAST_FORWARD),
                                    ImTextLabel(i18n::TextId::ProtocolV1, PSI_FORWARD)};
            const std::array tooltips{Tr(i18n::TextId::ProtocolAutoHelp), Tr(i18n::TextId::ProtocolV2Help), Tr(i18n::TextId::ProtocolV1Help)};
            for (int i = 0; i < static_cast<int>(labels.size()); ++i)
            {
                ImStylesRAII buttonStyles;
                if (deviceType != i)
                    buttonStyles.PushCol(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                if (ImModalButton(labels[i].c_str(), i, static_cast<int>(labels.size())))
                    deviceType = static_cast<DEVICE_TYPE>(i);
                if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
                {
                    ImGui::BeginTooltip();
                    ImGui::TextUnformatted(tooltips[i]);
                    ImGui::EndTooltip();
                }
            }
        }
        ImGui::EndDisabled();
        auto RefreshDeviceList = [&]()
        {
            MDRConnection* conn = clientPlatformConnectionGet();
            if (conn) // TODO: Error modals
                mdrConnectionGetDevicesList(conn, &pDeviceInfo, &nDeviceInfo);
        };
        if (needSwitchClientPlatform)
        {
            int flags = 0;
            if (usingBLE)
                flags |= MDR_INIT_BT_BLE;
            MDRConnection* conn = clientPlatformConnectionGet();
            if (conn && pDeviceInfo)
                mdrConnectionFreeDevicesList(conn, &pDeviceInfo), pDeviceInfo = nullptr, nDeviceInfo = 0;
            CloseDevice();
            clientPlatformConnectionDestroy();
            connInitResult = clientPlatformConnectionInit(flags);
            RefreshDeviceList();
        }
        auto DrawDeviceList = [&]()
        {
            ImHeading(Tr(i18n::TextId::AvailableDevices));
            static int deviceIndex = 0;
            std::span<MDRDeviceInfo> devices{pDeviceInfo, static_cast<size_t>(nDeviceInfo)};
            if (!devices.empty())
            {
                int btnIndex = 0;
                for (const auto& device : devices)
                {
                    ImGui::PushID(device.szDeviceMacAddress);
                    ImGui::RadioButton(device.szDeviceName, &deviceIndex, btnIndex++);
                    ImGui::PopID();
                }
            }
            else
            {
                ImGui::TextWrapped(PSI_WARNING_SIGN " %s", Tr(i18n::TextId::NoDevices));
            }
            ImGui::BeginDisabled(devices.empty());
            if (ImModalButton(TrLable(i18n::TextId::Connect, PSI_LINK), 0, 2))
            {
                const int res = StartConnection(clientPlatformConnectionGet(), devices[deviceIndex].szDeviceMacAddress,
                                                usingBLE, deviceType);
                if (res != MDR_RESULT_OK && res != MDR_RESULT_INPROGRESS)
                    connState = CONN_STATE_DISCONNECTED;
                else
                    connState = CONN_STATE_CONNECTING;
            }
            ImGui::EndDisabled();
            if (ImModalButton(TrLable(i18n::TextId::Refresh, PSI_REFRESH), 1, 2))
                RefreshDeviceList();
        };
        if (connInitResult != MDR_RESULT_OK && connInitResult != MDR_RESULT_INPROGRESS)
        {
            ImTextCentered(mdr::Format(PSI_EXCLAMATION_SIGN " {}",
                                       mdr::Format(fmt::runtime(Tr(i18n::TextId::ConnectionInitFailed)), mdrResultString(connInitResult)))
                               .c_str());
        }
        DrawDeviceList();
        {
            ImStylesRAII scope;
            scope.PushFont(IM_FONTSIZE_CAPTION);
            ImGui::SeparatorText(mdr::Format(PSI_INFO_SIGN_ALT " {}", Tr(i18n::TextId::ConnectionHelp)).c_str());
            ImTextCentered(mdr::Format(PSI_WARNING_SIGN " {} " PSI_WARNING_SIGN, Tr(i18n::TextId::Disclaimer)).c_str());
        }
#ifdef MDR_CLIENT_DEBUGGER
        ImGui::Separator();
        if (ImModalButton(TrLable(i18n::TextId::ProtocolDebugger, PSI_BUG)))
            gDebuggerOpen = true;
#endif
        ImGui::EndPopup();
    }
    else
        popup = false;
}

// NOTE: Only CONN_STATE_DISCONNECTED state shows the modal
void DisconnectWithModal(const char* manualError = nullptr)
{
    MDRConnection* conn = clientPlatformConnectionGet();
    connState = CONN_STATE_DISCONNECTED;
    CloseDevice();
    if (manualError)
        gHeadphonesError = manualError;
    mdrConnectionDisconnect(conn);
}

void DrawDeviceAlert()
{
    if (!gAlertPending)
        return;

    ImSetNextWindowCentered();
    ImGui::OpenPopup("Headphones confirmation");
    if (!ImGui::BeginPopupModal("Headphones confirmation", nullptr, kImWindowFlagsTopMost))
        return;

    ImGui::TextWrapped("%s", Tr(i18n::TextId::AlertPrompt));
    if (!gAlertMessage.empty())
        ImGui::TextUnformatted(mdr::Format(fmt::runtime(Tr(i18n::TextId::AlertMessage)), gAlertMessage).c_str());
    ImGui::NewLine();

    ImGui::BeginDisabled(!mdrHeadphonesIsReady(gDevice));
    MDRAlertAction response{};
    bool answered = false;
    if (ImModalButton(TrLable(i18n::TextId::Cancel, PSI_REMOVE), 0, 2))
        response = MDR_ALERT_ACTION_NEGATIVE, answered = true;
    if (ImModalButton(TrLable(i18n::TextId::Continue, PSI_OK), 1, 2))
        response = MDR_ALERT_ACTION_POSITIVE, answered = true;
    ImGui::EndDisabled();

    if (answered)
    {
        const MDRResult result = mdrHeadphonesRequestRespondToAlert(gDevice, response);
        if (result == MDR_RESULT_OK)
        {
            gAlertPending = false;
            gAlertMessage.clear();
            ImGui::CloseCurrentPopup();
        }
        else if (result != MDR_RESULT_INPROGRESS)
        {
            ImGui::EndPopup();
            DisconnectWithModal();
            return;
        }
    }
    ImGui::EndPopup();
}

void DrawDeviceConnecting()
{
    assert(connState == CONN_STATE_CONNECTING);
    MDRConnection* conn = clientPlatformConnectionGet();
    MDRResult pollResult = mdrConnectionPoll(conn, 0);
    if (pollResult != MDR_RESULT_OK)
    {
        const bool attemptTimedOut = (pollResult == MDR_RESULT_INPROGRESS || pollResult == MDR_RESULT_ERROR_TIMEOUT) &&
            SDL_GetTicks() >= connectionAttempt.deadlineMs;
        const bool attemptFailed = pollResult != MDR_RESULT_INPROGRESS && pollResult != MDR_RESULT_ERROR_TIMEOUT;
        if (attemptTimedOut || attemptFailed)
        {
            pollResult = AdvanceConnectionAttempt(conn, attemptTimedOut ? MDR_RESULT_ERROR_TIMEOUT : pollResult);
            if (pollResult != MDR_RESULT_OK && pollResult != MDR_RESULT_INPROGRESS)
            {
                connState = CONN_STATE_DISCONNECTED;
                CloseDevice();
                MaterialYouTheme::ApplyDefault();
                return;
            }
        }
    }
    switch (pollResult)
    {
    case MDR_RESULT_OK:
        connState = CONN_STATE_CONNECTED;
        connectionAttempt.lastError.clear();
        CloseDevice();
        if (mdrHeadphonesCreate(MDR_ABI_VERSION, conn, ConnectionProtocolVersion(), &gDevice) != MDR_RESULT_OK)
        {
            DisconnectWithModal();
            return;
        }
        mdrHeadphonesSetPacketCallback(
            gDevice,
            [](void*, MDRPacketDirection direction, const unsigned char* frame, int frameSize)
            {
                clientPayloadRecorderObserve(direction, frame, frameSize);
#ifdef MDR_CLIENT_DEBUGGER
                clientDebuggerObservePacket(direction, frame, frameSize);
#endif
            },
            nullptr
        );
#ifdef MDR_CLIENT_DEBUGGER
        clientDebuggerAttach(gDevice);
#endif
        if (mdrHeadphonesRequestInit(gDevice) != MDR_RESULT_OK)
            DisconnectWithModal();

        return;
    case MDR_RESULT_ERROR_TIMEOUT:
    case MDR_RESULT_INPROGRESS:
        {
            ImSetNextWindowCentered();
            static bool popup = false;
            if (!popup)
                ImGui::OpenPopup("Connection"), popup = true;
            if (ImGui::BeginPopupModal("Connection", nullptr, kImWindowFlagsTopMost))
            {
                ImGui::NewLine();
                ImTextCentered(Tr(i18n::TextId::Connecting), IM_FONTSIZE_TITLE);
                ImTextCentered(mdr::Format(fmt::runtime(Tr(i18n::TextId::DeviceType)), ConnectionAttemptName()).c_str(), IM_FONTSIZE_CAPTION);
                ImGui::Dummy({0, 16.0f});
                ImSpinner(1400.0f, ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x * 2.0f,
                          MaterialYouTheme::ArgbToImU32(MaterialYouTheme::FixedSurfaceColors::onSurface),
                          ImGui::GetFontSize() * 0.25f, true);
                ImGui::NewLine();
                ImTextCentered(mdrConnectionGetLastError(conn));
                ImGui::NewLine();
                if (ImModalButton(TrLable(i18n::TextId::Cancel, PSI_REMOVE)))
                {
                    CloseDevice();
                    mdrConnectionDisconnect(conn);
                    connectionAttempt = {};
                    connState = CONN_STATE_NO_CONNECTION;
                }
                ImGui::EndPopup();
            }
            else
                popup = false;
            return;
        }
    default:
        {
            CaptureConnectionError(conn, pollResult);
            connState = CONN_STATE_DISCONNECTED;
            CloseDevice();
            mdrConnectionDisconnect(conn);
            MaterialYouTheme::ApplyDefault();
            break;
        }
    }
}

void DrawDeviceControlsHeader()
{
    MDRConnection* conn = clientPlatformConnectionGet();
    const mdr::String modelName = GetText(MDR_TEXT_MODEL_NAME);
    if (ImGui::BeginMenuBar())
    {
        auto& style = ImGui::GetStyle();
        /* Disconnect & Shutdown */
        if (ImGui::BeginMenu(mdr::Format(PSI_CHEVRON_DOWN " {}", modelName).c_str()))
        {
            if (ImGui::MenuItem(TrLable(i18n::TextId::Disconnect, PSI_UNLINK)))
            {
                CloseDevice();
                mdrConnectionDisconnect(conn);
                connState = CONN_STATE_NO_CONNECTION;
            }
            if (FeatureAvailable(MDR_FEATURE_SHUTDOWN))
            {
                ImGui::BeginDisabled(!mdrHeadphonesIsReady(gDevice));
                if (ImGui::MenuItem(TrLable(i18n::TextId::Shutdown, PSI_OFF)))
                {
                    MDRPower power{};
                    if (mdrHeadphonesGetPower(gDevice, &power) == MDR_RESULT_OK)
                    {
                        power.shutdown_requested = MDR_TRUE;
                        mdrHeadphonesSetPower(gDevice, &power);
                    }
                }
                ImGui::EndDisabled();
            }
#ifdef MDR_CLIENT_DEBUGGER
            ImGui::Separator();
            ImGui::MenuItem(TrLable(i18n::TextId::ProtocolDebugger, PSI_BUG), nullptr, &gDebuggerOpen);
            if (ImGui::MenuItem(TrLable(i18n::TextId::TriggerDisconnectError, PSI_BUG)))
                DisconnectWithModal(Tr(i18n::TextId::ManualDisconnectError));
#endif
            ImGui::EndMenu();
        }
        if (!gDevice)
        {
            ImGui::EndMenuBar();
            return;
        }
        const ImVec2 spinnerPos = ImGui::GetCursorScreenPos();
        const float spinnerThickness = ImGui::GetFontSize() * 0.2f;
        /* Cool Badges */
        // Title, Border Color, Text Color
        using Badge = std::tuple<const char*, ImU32, ImU32>;
        std::array<Badge, 4> badges4;
        Badge *badgeFirst = &badges4[0], *badgeLast = &badges4[0];
        /* Codec */
        if (gState.mModelAvailable && gState.mModel.audio_codec != MDR_AUDIO_CODEC_UNKNOWN)
        {
            *(badgeLast++) = {FormatAudioCodec(gState.mModel.audio_codec), ~0u, ~0u};
        }
        /* DSEE */
        if (FeatureAvailable(MDR_FEATURE_DSEE) && gState.mEqualizerAvailable && gState.mEqualizer.dsee_enabled)
        {
            *(badgeLast++) = {FormatDseeType(gState.mEqualizer.dsee_type), ~0u, ~0u};
        }
        std::span<Badge> badges{badgeFirst, static_cast<size_t>(badgeLast - badgeFirst)};
        /* Manual Sync */
        const mdr::String syncBadge = mdr::Format(PSI_REFRESH " {}", Tr(i18n::TextId::Sync));
        const char* kSyncBadge = syncBadge.c_str();
        ImStylesRAII badgeStyles;
        badgeStyles.PushFont(IM_FONTSIZE_CAPTION);
        const float spacing = style.ItemSpacing.x;
        float badgeRegionX = -spacing;
        for (auto& [s, border, text] : badges)
            badgeRegionX += ImBadgeSize(s).x + spacing;
        badgeRegionX += ImBadgeSize(kSyncBadge).x + spacing;
        const ImGuiWindow* window = ImGui::GetCurrentWindow();
        const ImRect bar = window->MenuBarRect();
        const float inset = std::max(window->WindowPadding.x, style.ItemSpacing.x);
        const float badgeY = bar.Min.y + (bar.GetHeight() - ImBadgeSize(kSyncBadge).y) * 0.5f;
        ImVec2 pos{bar.Max.x - inset - badgeRegionX, badgeY};
        pos.x = std::max(pos.x, spinnerPos.x + spacing);
        if (!mdrHeadphonesIsReady(gDevice))
        {
            const float spinnerWidth = pos.x - spinnerPos.x - inset * 2.0f;
            if (spinnerWidth >= spinnerThickness)
            {
                const float spinnerHeight = std::max(spinnerThickness, ImGui::GetFrameHeight());
                const float spinnerY = bar.Min.y + (bar.GetHeight() - spinnerHeight) * 0.5f;
                ImGui::SetCursorScreenPos({spinnerPos.x + inset, spinnerY});
                ImSpinner(1400.0f, spinnerWidth,
                          MaterialYouTheme::ArgbToImU32(MaterialYouTheme::FixedSurfaceColors::onSurface, 0.5f),
                          spinnerThickness, false, true);
            }
        }
        const float rounding = style.FrameRounding;
        for (auto& [s, border, text] : badges)
        {
            ImBadge(s, pos, border, text, rounding, 2.0f);
            pos.x += ImBadgeSize(s).x + spacing;
        }
        {
            const bool ready = mdrHeadphonesIsReady(gDevice);
            const ImU32 color = ready ? ~0u : ImGui::GetColorU32(ImGuiCol_TextDisabled);
            bool hovered = false;
            if (ImBadge(kSyncBadge, pos, color, color, rounding, 2.0f, &hovered, "sync") && ready)
                gState.mPendingSync = true;
            if (hovered)
                ImGui::SetTooltip("%s", Tr(i18n::TextId::SyncHelp));
        }
        ImGui::EndMenuBar();
    }
    // Stats
    if (ImGui::BeginTable("##Stats", 2, ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable))
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        /* Batteries */
        {
            if (ImGui::BeginTable("##Battery", 2, ImGuiTableFlags_SizingStretchProp))
            {
                for (const MDRBattery& battery : gState.mBatteries)
                {
                    if (!battery.present || !battery.update_threshold_percent)
                        continue;
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    const char* label = battery.part == MDR_BATTERY_LEFT ? Tr(i18n::TextId::BatteryLeft)
                        : battery.part == MDR_BATTERY_RIGHT              ? Tr(i18n::TextId::BatteryRight)
                        : battery.part == MDR_BATTERY_CASE               ? Tr(i18n::TextId::BatteryCase)
                                                                         : Tr(i18n::TextId::Battery);
                    ImGui::Text("%s: %u%%", label, static_cast<unsigned>(battery.level_percent));
                    ImGui::TableSetColumnIndex(1);
                    ImGui::ProgressBar(battery.level_percent / 100.0f, {-1, 0}, FormatChargingState(battery.charging));
                }
                ImGui::EndTable();
            }
            if (gState.mWearingStatusAvailable)
                ImGui::TextUnformatted(mdr::Format(fmt::runtime(Tr(i18n::TextId::Wearing)), FormatWearingStatus(gState.mWearingStatus)).c_str());
        }
        ImGui::TableSetColumnIndex(1);
        /* Now Playing */
        {
            {
                ImStylesRAII scope;
                scope.PushFont(IM_FONTSIZE_SUBHEADING);
                ImGui::Text(PSI_VOLUME_UP " %s", Tr(i18n::TextId::NowPlaying));
            }
            if (ImGui::BeginTable("##NowPlaying", 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerH))
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(Tr(i18n::TextId::Title));
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", GetText(MDR_TEXT_TRACK_TITLE).c_str());
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(Tr(i18n::TextId::Album));
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", GetText(MDR_TEXT_TRACK_ALBUM).c_str());
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(Tr(i18n::TextId::Artist));
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", GetText(MDR_TEXT_TRACK_ARTIST).c_str());
                ImGui::EndTable();
            }
        }
        ImGui::EndTable();
    }
}

void DrawDeviceControlsPlayback()
{
    ImHeading(Tr(i18n::TextId::Volume));
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    int volume = gState.mPlayback.volume;
    if (ImGui::SliderInt("##Volume", &volume, 0, 30))
    {
        MDRPlayback playback = gState.mPlayback;
        playback.volume = static_cast<uint8_t>(volume);
        if (mdrHeadphonesSetPlayback(gDevice, &playback) == MDR_RESULT_OK)
        {
            gState.mPlayback = playback;
            gState.mPlaybackVolumeStaged = true;
        }
    }
    ImHeading(Tr(i18n::TextId::Controls));
    if (ImModalButton(TrLable(i18n::TextId::Previous, PSI_STEP_BACKWARD), 0, 3))
    {
        MDRPlaybackCommand command{};
        command.action = MDR_PLAYBACK_PREVIOUS;
        mdrHeadphonesPlayback(gDevice, &command);
    }
    if (gState.mPlayback.status == MDR_PLAYBACK_PLAYING)
    {
        if (ImModalButton(TrLable(i18n::TextId::Pause, PSI_PAUSE), 1, 3))
        {
            MDRPlaybackCommand command{};
            command.action = MDR_PLAYBACK_PAUSE;
            mdrHeadphonesPlayback(gDevice, &command);
        }
    }
    else
    {
        if (ImModalButton(TrLable(i18n::TextId::Play, PSI_PLAY), 1, 3))
        {
            MDRPlaybackCommand command{};
            command.action = MDR_PLAYBACK_PLAY;
            mdrHeadphonesPlayback(gDevice, &command);
        }
    }
    if (ImModalButton(TrLable(i18n::TextId::Next, PSI_STEP_FORWARD), 2, 3))
    {
        MDRPlaybackCommand command{};
        command.action = MDR_PLAYBACK_NEXT;
        mdrHeadphonesPlayback(gDevice, &command);
    }
}

void DrawDeviceControlsSound()
{
    const bool supportNC = FeatureAvailable(MDR_FEATURE_NOISE_CANCELLING);
    const bool supportASM = FeatureAvailable(MDR_FEATURE_AMBIENT_SOUND);
    const bool supportAutoASM = FeatureAvailable(MDR_FEATURE_ADAPTIVE_AMBIENT_SOUND);
    /* NC/ASM */
    if (supportASM || supportNC)
    {
        if (ImHeadingTreeNode(TrLable(i18n::TextId::AmbientSound), ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool changed = false;

            MDRProtocolVersion protocolVersion = ConnectionProtocolVersion();
            if (protocolVersion == MDR_PROTOCOL_V1)
            {
                bool ncAsmEnabled = gState.mNoise.mode != MDR_NOISE_MODE_OFF;
                if (ImGui::Checkbox(TrLable(i18n::TextId::Enabled), &ncAsmEnabled))
                    gState.mNoise.mode = ncAsmEnabled ? MDR_NOISE_MODE_V1_ON : MDR_NOISE_MODE_OFF, changed = true;

                ImGui::BeginDisabled(!ncAsmEnabled);

                // -1: Noise Cancelling
                // 0: Wind Noise Reduction
                // 1-20: Ambient Sound
                bool sliderChanged;
                int sliderLevel = static_cast<int8_t>(gState.mNoise.ambient_level);
                if (sliderLevel == -1)
                    sliderChanged = ImGui::SliderInt("##AmbStrength", &sliderLevel, -1, 20, Tr(i18n::TextId::NoiseCancelling));
                else if (sliderLevel == 0)
                    sliderChanged = ImGui::SliderInt("##AmbStrength", &sliderLevel, -1, 20, Tr(i18n::TextId::WindNoiseReduction));
                else
                    sliderChanged = ImGui::SliderInt("##AmbStrength", &sliderLevel, -1, 20,
                                                     mdr::Format(fmt::runtime(Tr(i18n::TextId::AmbientLevel)), sliderLevel).c_str());
                if (sliderChanged)
                    gState.mNoise.ambient_level = static_cast<uint8_t>(sliderLevel), changed = true;
                gState.mNoise.changing_asm_level = sliderChanged && ImGui::IsItemActive();
                if (ImGui::IsItemDeactivatedAfterEdit())
                    changed = true;

                ImGui::BeginDisabled(sliderLevel < 1);
                bool focusOnVoice = gState.mNoise.focus_on_voice != MDR_FALSE;
                if (ImGui::Checkbox(TrLable(i18n::TextId::VoicePassthrough), &focusOnVoice))
                    gState.mNoise.focus_on_voice = focusOnVoice ? MDR_TRUE : MDR_FALSE, changed = true;
                ImGui::EndDisabled(); // sliderLevel < 1

                ImGui::EndDisabled(); // !ncAsmEnabled
            }
            else if (protocolVersion == MDR_PROTOCOL_V2)
            {
                if (supportNC)
                {
                    if (ImGui::RadioButton(TrLable(i18n::TextId::NoiseCancelling), gState.mNoise.mode == MDR_NOISE_MODE_CANCELLING))
                    {
                        gState.mNoise.mode = MDR_NOISE_MODE_CANCELLING;
                        changed = true;
                    }
                    ImGui::SameLine();
                }
                if (supportASM)
                {
                    if (ImGui::RadioButton(TrLable(i18n::TextId::AmbientSound), gState.mNoise.mode == MDR_NOISE_MODE_AMBIENT))
                    {
                        gState.mNoise.mode = MDR_NOISE_MODE_AMBIENT;
                        if (gState.mNoise.ambient_level == 0)
                            gState.mNoise.ambient_level = 20;
                        changed = true;
                    }
                    ImGui::SameLine();
                }
                if (ImGui::RadioButton(TrLable(i18n::TextId::Off), gState.mNoise.mode == MDR_NOISE_MODE_OFF))
                    gState.mNoise.mode = MDR_NOISE_MODE_OFF, changed = true;
                ImHeading(Tr(i18n::TextId::AmbientStrength), IM_FONTSIZE_SUBHEADING);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                {
                    // Only works with AMB enabled
                    ImGui::BeginDisabled(gState.mNoise.mode != MDR_NOISE_MODE_AMBIENT);
                    bool ambientChanged = false;
                    int ambientLevel = gState.mNoise.ambient_level;
                    if (ImGui::SliderInt("##AmbStrength", &ambientLevel, 1, 20))
                        gState.mNoise.ambient_level = static_cast<uint8_t>(ambientLevel),
                        ambientChanged = changed = true;
                    gState.mNoise.changing_asm_level = ambientChanged && ImGui::IsItemActive();
                    if (ImGui::IsItemDeactivatedAfterEdit())
                        changed = true;
                    if (supportAutoASM)
                    {
                        bool adaptive = gState.mNoise.adaptive_ambient != MDR_FALSE;
                        if (ImGui::Checkbox(TrLable(i18n::TextId::AutoAmbientSound), &adaptive))
                            gState.mNoise.adaptive_ambient = adaptive ? MDR_TRUE : MDR_FALSE, changed = true;
                        ImGui::BeginDisabled(!adaptive);
                        constexpr MDRAdaptiveSensitivity kSelections[] = {MDR_ADAPTIVE_SENSITIVITY_STANDARD,
                                                                          MDR_ADAPTIVE_SENSITIVITY_HIGH,
                                                                          MDR_ADAPTIVE_SENSITIVITY_LOW};
                        changed |= ImComboBoxItems(TrLable(i18n::TextId::Sensitivity), std::span{kSelections},
                                                   gState.mNoise.adaptive_sensitivity, FormatAdaptiveSensitivity);
                        ImGui::EndDisabled(); // !adaptive
                    }
                    bool focusOnVoice = gState.mNoise.focus_on_voice != MDR_FALSE;
                    if (ImGui::Checkbox(TrLable(i18n::TextId::VoicePassthrough), &focusOnVoice))
                        gState.mNoise.focus_on_voice = focusOnVoice ? MDR_TRUE : MDR_FALSE, changed = true;
                    ImGui::EndDisabled(); // gState.mNoise.mode != MDR_NOISE_MODE_AMBIENT
                }
            }

            if (changed && gState.mNoiseAvailable)
                mdrHeadphonesSetNoiseControl(gDevice, &gState.mNoise);
            ImGui::TreePop();
        }
    }
    /* STC */
    if (FeatureAvailable(MDR_FEATURE_SPEAK_TO_CHAT))
    {
        if (ImHeadingTreeNode(TrLable(i18n::TextId::SpeakToChat), ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool changed = false;
            bool enabled = gState.mSpeakToChat.enabled != MDR_FALSE;
            if (ImGui::Checkbox(TrLable(i18n::TextId::Enabled), &enabled))
                gState.mSpeakToChat.enabled = enabled ? MDR_TRUE : MDR_FALSE, changed = true;
            ImGui::BeginDisabled(!enabled);
            constexpr MDRSpeechSensitivity kSensitivity[] = {MDR_SPEECH_SENSITIVITY_AUTO, MDR_SPEECH_SENSITIVITY_HIGH,
                                                             MDR_SPEECH_SENSITIVITY_LOW};
            changed |= ImComboBoxItems(TrLable(i18n::TextId::Sensitivity), std::span{kSensitivity}, gState.mSpeakToChat.sensitivity,
                                       FormatSpeechSensitivity);
            constexpr MDRSpeakTimeout kTimeout[] = {MDR_SPEAK_TIMEOUT_SHORT, MDR_SPEAK_TIMEOUT_MEDIUM,
                                                    MDR_SPEAK_TIMEOUT_LONG, MDR_SPEAK_TIMEOUT_MANUAL};
            changed |=
                ImComboBoxItems(TrLable(i18n::TextId::ModeDuration), std::span{kTimeout}, gState.mSpeakToChat.timeout, FormatSpeakTimeout);
            ImGui::EndDisabled();
            if (changed && gState.mSpeakToChatAvailable)
                mdrHeadphonesSetSpeakToChat(gDevice, &gState.mSpeakToChat);
            ImGui::TreePop();
        }
    }
    /* Listening Mode */
    if (FeatureAvailable(MDR_FEATURE_LISTENING_MODE))
    {
        if (ImHeadingTreeNode(TrLable(i18n::TextId::ListeningMode), ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool changed = false;
            if (ImGui::RadioButton(TrLable(i18n::TextId::Standard), gState.mListening.mode == MDR_LISTENING_STANDARD))
                gState.mListening.mode = MDR_LISTENING_STANDARD, changed = true;

            const bool haveBackgroundMusic = FeatureAvailable(MDR_FEATURE_LISTENING_BACKGROUND_MUSIC);
            if (haveBackgroundMusic &&
                ImGui::RadioButton(TrLable(i18n::TextId::AmbientBackgroundMusic),
                                   gState.mListening.mode == MDR_LISTENING_BACKGROUND_MUSIC))
                gState.mListening.mode = MDR_LISTENING_BACKGROUND_MUSIC, changed = true;

            if (haveBackgroundMusic)
            {
                ImGui::Indent();
                ImGui::BeginDisabled(gState.mListening.mode != MDR_LISTENING_BACKGROUND_MUSIC);
                constexpr std::pair<MDRRoomSize, i18n::TextId> kBGMDistanceModes[] = {
                    {MDR_ROOM_SMALL, i18n::TextId::MyRoom},
                    {MDR_ROOM_MEDIUM, i18n::TextId::LivingRoom},
                    {MDR_ROOM_LARGE, i18n::TextId::Cafe},
                };
                const char* currentDistStr = Tr(i18n::TextId::Unknown);
                for (auto const& [k, v] : kBGMDistanceModes)
                    if (k == gState.mListening.background_room)
                        currentDistStr = i18n::Translate(v, clientGetAppLocale());
                if (ImGui::BeginCombo(TrLable(i18n::TextId::Distance), currentDistStr))
                {
                    for (auto const& [k, v] : kBGMDistanceModes)
                    {
                        bool is_selected = k == gState.mListening.background_room;
                        const mdr::String label = ImTextLabel(v);
                        if (ImGui::Selectable(label.c_str(), is_selected))
                            gState.mListening.background_room = k, changed = true;
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::EndDisabled();
                ImGui::Unindent();
            }

            if (FeatureAvailable(MDR_FEATURE_LISTENING_CINEMA) &&
                ImGui::RadioButton(TrLable(i18n::TextId::Cinema), gState.mListening.mode == MDR_LISTENING_CINEMA))
                gState.mListening.mode = MDR_LISTENING_CINEMA, changed = true;

            if (FeatureAvailable(MDR_FEATURE_LISTENING_VOICE_BOOST) &&
                ImGui::RadioButton(TrLable(i18n::TextId::VoiceBoost), gState.mListening.mode == MDR_LISTENING_VOICE_BOOST))
                gState.mListening.mode = MDR_LISTENING_VOICE_BOOST, changed = true;

            if (FeatureAvailable(MDR_FEATURE_LISTENING_SOUND_LEAKAGE_REDUCTION) &&
                ImGui::RadioButton(TrLable(i18n::TextId::SoundLeakageReduction),
                                   gState.mListening.mode == MDR_LISTENING_SOUND_LEAKAGE_REDUCTION))
                gState.mListening.mode = MDR_LISTENING_SOUND_LEAKAGE_REDUCTION, changed = true;

            if (changed && gState.mListeningAvailable)
                mdrHeadphonesSetListening(gDevice, &gState.mListening);
            ImGui::TreePop();
        }
    }
    /* EQ & DSEE */
    if (ImHeadingTreeNode(TrLable(i18n::TextId::EqualizerDsee), ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool changed = false;
        // Only what the device advertised: an id it never named is one it will not take.
        mdr::Vector<MDREqualizerPreset> selections;
        for (const auto& [id, name] : gState.mEqualizerPresets)
            selections.push_back(id);
        const auto formatPreset = [](MDREqualizerPreset id) -> const char*
        {
            for (const auto& [advertised, name] : gState.mEqualizerPresets)
                if (advertised == id && !name.empty())
                    return name.c_str();
            return FormatEqualizerPreset(id);
        };
        const bool equalizerUsable = !gState.mEqualizerAvailable || gState.mEqualizer.available != MDR_FALSE;
        const bool dseeUsable = !gState.mEqualizerAvailable || gState.mEqualizer.dsee_available != MDR_FALSE;
        if (!equalizerUsable || !dseeUsable)
            ImGui::TextDisabled("%s", Tr(i18n::TextId::ListeningEqUnavailable));
        ImGui::BeginDisabled(!equalizerUsable);
        // An empty list means the device has not answered yet, not that it has no presets.
        ImGui::BeginDisabled(selections.empty());
        changed |= ImComboBoxItems<MDREqualizerPreset, std::dynamic_extent>(TrLable(i18n::TextId::Preset), std::span{selections},
                                                                            gState.mEqualizer.preset, formatPreset);
        ImGui::EndDisabled();
        if (ImEqualizer(gState.mEqualizerBands))
            SetEqualizerBands(gState.mEqualizerBands);
        if (gState.mEqualizerBands.size() == 5)
        {
            ImHeading(Tr(i18n::TextId::ClearBass), IM_FONTSIZE_SUBHEADING);
            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            int clearBass = gState.mEqualizer.clear_bass;
            if (ImGui::SliderInt("##", &clearBass, -10, 10))
                gState.mEqualizer.clear_bass = static_cast<int8_t>(clearBass), changed = true;
        }
        ImGui::EndDisabled();
        ImHeading(Tr(i18n::TextId::Dsee), IM_FONTSIZE_SUBHEADING);
        ImGui::BeginDisabled(!FeatureAvailable(MDR_FEATURE_DSEE) || !dseeUsable);
        if (ImGui::RadioButton(TrLable(i18n::TextId::Off), gState.mEqualizer.dsee_enabled == MDR_FALSE))
            gState.mEqualizer.dsee_enabled = MDR_FALSE, changed = true;
        if (ImGui::RadioButton(TrLable(i18n::TextId::OnAuto), gState.mEqualizer.dsee_enabled != MDR_FALSE))
            gState.mEqualizer.dsee_enabled = MDR_TRUE, changed = true;
        ImGui::EndDisabled();
        if (changed && gState.mEqualizerAvailable)
            mdrHeadphonesSetEqualizer(gDevice, &gState.mEqualizer);
        ImGui::TreePop();
    }
}

void DrawDeviceControlsDevices()
{
    const bool supportDeviceMgmt = FeatureAvailable(MDR_FEATURE_PAIRED_DEVICE_MANAGEMENT);
    if (!supportDeviceMgmt)
        ImGui::TextUnformatted(Tr(i18n::TextId::DeviceManagementHelp));
    ImGui::BeginDisabled(!supportDeviceMgmt);
    struct DeviceView
    {
        MDRPairedDevice state;
        mdr::String mac; // Colonated MAC (17 chars); stable addressing key
        mdr::String name;
    };
    mdr::Vector<DeviceView> devices;
    for (const MDRPairedDevice& state : gState.mPairedDevices)
        devices.emplace_back(state, mdr::String{state.macAddress}, mdr::String{state.name});
    auto StageDeviceAction = [](MDRPairedDeviceCommand command, const char* mac)
    {
        MDRPairedDeviceAction action{};
        action.command = command;
        action.device_id = mac;
        action.device_id_size = static_cast<uint32_t>(std::strlen(mac));
        mdrHeadphonesSetPairedDevice(gDevice, &action);
    };
    const bool supportFix = FeatureAvailable(MDR_FEATURE_SOURCE_SWITCH_CONTROL);
    MDRBoolean switchControlEnabled = MDR_TRUE;
    if (supportFix)
        mdrHeadphonesGetSourceSwitchControl(gDevice, &switchControlEnabled);
    // Sound Connect's "Fixing playback device" is the negation of source switch control.
    const bool playbackFixed = switchControlEnabled == MDR_FALSE;
    auto DrawDeviceElement = [&](const DeviceView& device, bool selected) -> bool
    {
        ImGui::PushID(device.mac.c_str());
        ImGui::BeginGroup();
        if (device.state.playback_device)
        {
            ImGui::Text(PSI_VOLUME_DOWN " "), ImGui::SameLine();
            if (playbackFixed)
                ImGui::Text(PSI_LOCK " "), ImGui::SameLine();
        }
        bool res = ImGui::Selectable(device.name.c_str(), selected);
        if (device.state.connected && ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            StageDeviceAction(MDR_PAIRED_DEVICE_SELECT_PLAYBACK, device.mac.c_str());
        if (selected)
        {
            ImGui::Separator();
            if (device.state.connected)
            {
                const bool canFix = supportFix && device.state.playback_device;
                const int columns = canFix ? 3 : 2;
                if (ImModalButton(TrLable(i18n::TextId::Disconnect, PSI_UNLINK), 0, columns))
                    StageDeviceAction(MDR_PAIRED_DEVICE_DISCONNECT, device.mac.c_str());
                if (ImModalButton(TrLable(i18n::TextId::SwitchPlayback, PSI_VOLUME_DOWN), 1, columns))
                    StageDeviceAction(MDR_PAIRED_DEVICE_SELECT_PLAYBACK, device.mac.c_str());
                if (canFix &&
                    ImModalButton(playbackFixed ? TrLable(i18n::TextId::UnfixPlayback, PSI_UNLOCK) : TrLable(i18n::TextId::FixPlayback, PSI_LOCK), 2, columns))
                    mdrHeadphonesSetSourceSwitchControl(gDevice, playbackFixed ? MDR_TRUE : MDR_FALSE);
            }
            else
            {
                if (ImModalButton(TrLable(i18n::TextId::Connect, PSI_LINK), 0, 2))
                    StageDeviceAction(MDR_PAIRED_DEVICE_CONNECT, device.mac.c_str());
            }
            if (ImModalButton(TrLable(i18n::TextId::Unpair, PSI_BLUETOOTH_ALT), 1, 2))
                StageDeviceAction(MDR_PAIRED_DEVICE_UNPAIR, device.mac.c_str());
            // After the button rows: Unpair shares a row, so an inline message would land beside it.
            if (supportFix && device.state.connected && device.state.playback_device)
            {
                MDRSourceSwitchControlResult fixResult = MDR_SOURCE_SWITCH_CONTROL_SUCCESS;
                mdrHeadphonesGetSourceSwitchControlResult(gDevice, &fixResult);
                if (fixResult != MDR_SOURCE_SWITCH_CONTROL_SUCCESS)
                    ImGui::TextWrapped(PSI_INFO_SIGN_ALT " %s", FormatSourceSwitchControlResult(fixResult));
            }
        }
        ImGui::EndGroup();
        ImGui::PopID();
        return res;
    };
    static mdr::String connectSelectedMac;
    if (ImHeadingTreeNode(TrLable(i18n::TextId::Connected), ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& device : devices)
            if (device.state.connected && DrawDeviceElement(device, connectSelectedMac == device.mac))
                connectSelectedMac = connectSelectedMac == device.mac ? "" : device.mac;
        ImGui::TreePop();
    }
    if (ImHeadingTreeNode(TrLable(i18n::TextId::Paired), ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (auto& device : devices)
            if (!device.state.connected && DrawDeviceElement(device, connectSelectedMac == device.mac))
                connectSelectedMac = connectSelectedMac == device.mac ? "" : device.mac;
        ImGui::TreePop();
    }
    if (gState.mPairing.enabled)
    {
        ImTextCentered(Tr(i18n::TextId::Pairing), IM_FONTSIZE_HEADING);
        ImSpinner(1000.0f, ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x * 2.0f,
                  MaterialYouTheme::ArgbToImU32(MaterialYouTheme::ThemeForModelColor(GetModelColor()).primary), 2.0f,
                  true, false);
        if (ImModalButton(TrLable(i18n::TextId::Stop)))
        {
            gState.mPairing.enabled = MDR_FALSE;
            if (gState.mPairingAvailable)
                mdrHeadphonesSetPairing(gDevice, &gState.mPairing);
        }
    }
    else
    {
        if (ImModalButton(TrLable(i18n::TextId::EnterPairingMode, PSI_BLUETOOTH)))
        {
            gState.mPairing.enabled = MDR_TRUE;
            if (gState.mPairingAvailable)
                mdrHeadphonesSetPairing(gDevice, &gState.mPairing);
        }
        ImStylesRAII scope;
        scope.PushFont(IM_FONTSIZE_CAPTION);
        ImGui::TextWrapped(PSI_INFO_SIGN_ALT " %s", Tr(i18n::TextId::PairingHelp));
    }
    ImGui::EndDisabled();
}

void DrawDeviceControlsSystem()
{
    /* General Settings */
    if (ImHeadingTreeNode(TrLable(i18n::TextId::GeneralSettings), ImGuiTreeNodeFlags_DefaultOpen))
    {
        using StringPair = std::pair<const char*, i18n::TextId>;
        const auto kFormatGSString = [](const char* key, std::span<const StringPair> strings) -> const char*
        {
            auto it = std::lower_bound(strings.begin(), strings.end(), key, [](const StringPair& lhs, const char* rhs)
                                       { return strcmp(lhs.first, rhs) < 0; });
            if (it == strings.end() || strcmp(it->first, key) != 0)
                return Tr(i18n::TextId::UnknownKey);
            return i18n::Translate(it->second, clientGetAppLocale());
        };
        constexpr StringPair kGSSubjectStrings[] = {{"MULTIPOINT_SETTING", i18n::TextId::Multipoint},
                                                    {"SIDETONE_SETTING", i18n::TextId::Sidetone},
                                                    {"TOUCH_PANEL_SETTING", i18n::TextId::TouchControl}};
        constexpr StringPair kGSSummaryStrings[] = {
            {"MULTIPOINT_SETTING_SUMMARY", i18n::TextId::MultipointSummary},
            {"MULTIPOINT_SETTING_SUMMARY_LDAC_AVAILABLE", i18n::TextId::MultipointLdacSummary},
            {"SIDETONE_SETTING_SUMMARY", i18n::TextId::SidetoneSummary},
        };
        for (auto& [info, setting] : gState.mGeneralSettings)
        {
            if (info.type != MDR_GENERAL_SETTING_BOOLEAN)
                continue;
            const mdr::String subjectKey = GetText(MDR_TEXT_GENERAL_SETTING_SUBJECT, info.index);
            const mdr::String summaryKey = GetText(MDR_TEXT_GENERAL_SETTING_SUMMARY, info.index);
            const char* subject = kFormatGSString(subjectKey.c_str(), kGSSubjectStrings);
            const char* summary = kFormatGSString(summaryKey.c_str(), kGSSummaryStrings);
            bool value = setting.boolean_value != MDR_FALSE;
            ImGui::PushID(static_cast<int>(info.index));
            ImGui::BeginDisabled(subjectKey.empty() || !info.writable);
            const mdr::String subjectLabel = mdr::Format("{}###setting", subject);
            if (ImGui::Checkbox(subjectLabel.c_str(), &value))
            {
                setting.boolean_value = value ? MDR_TRUE : MDR_FALSE;
                mdrHeadphonesSetGeneralSetting(gDevice, &setting);
            }
            if (!summaryKey.empty())
            {
                ImStylesRAII scope;
                scope.PushFont(IM_FONTSIZE_CAPTION);
                ImGui::Bullet();
                ImGui::SameLine();
                ImGui::TextWrapped("%s", summary);
            }
            ImGui::EndDisabled();
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    /* Assignable Settings */
    if (FeatureAvailable(MDR_FEATURE_ASSIGNABLE_CONTROLS))
    {
        if (ImHeadingTreeNode(TrLable(i18n::TextId::AssignableControls), ImGuiTreeNodeFlags_DefaultOpen))
        {
            bool changed = false;

            auto controls = GetAssignableControls();
            for (MDRAssignableControl& control : controls)
            {
                mdr::Vector<MDRAssignableAction> actions = GetAssignableControlActions(control.location);
                std::erase(actions, MDR_ASSIGNABLE_GOOGLE_ASSISTANT);
                const mdr::String label = mdr::Format("{}###{}", FormatAssignableActionKeyLocation(control),
                                                      static_cast<int>(control.location));
                changed |= ImComboBoxItems<MDRAssignableAction, std::dynamic_extent>(
                    label.c_str(), std::span{actions}, control.action, FormatAssignableAction);
            }

            if (changed)
                mdrHeadphonesSetAssignableControls(gDevice, controls.data(), static_cast<uint32_t>(controls.size()));

            ImGui::TreePop();
        }
    }
    /* NC/ASM Button Settings */
    if (FeatureAvailable(MDR_FEATURE_NOISE_CONTROL_BUTTON) &&
        ImHeadingTreeNode(TrLable(i18n::TextId::NoiseButtonFunction), ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (gState.mNoiseAvailable)
        {
            constexpr MDRNoiseButtonMode kSelections[] = {MDR_NOISE_BUTTON_NONE, MDR_NOISE_BUTTON_NOISE_AMBIENT_OFF,
                                                          MDR_NOISE_BUTTON_NOISE_AMBIENT, MDR_NOISE_BUTTON_NOISE_OFF,
                                                          MDR_NOISE_BUTTON_AMBIENT_OFF};
            if (ImComboBoxItems(TrLable(i18n::TextId::Function), std::span{kSelections}, gState.mNoise.button_mode, FormatNoiseButtonMode))
                mdrHeadphonesSetNoiseControl(gDevice, &gState.mNoise);
        }
        ImGui::TreePop();
    }
    MDRPower power{};
    const bool havePower = mdrHeadphonesGetPower(gDevice, &power) == MDR_RESULT_OK;
    /* Head Gesture */
    if (FeatureAvailable(MDR_FEATURE_HEAD_GESTURE) && ImHeadingTreeNode(TrLable(i18n::TextId::HeadGesture), ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool enabled = power.head_gesture != MDR_FALSE;
        if (ImGui::Checkbox(TrLable(i18n::TextId::Enabled), &enabled) && havePower)
        {
            power.head_gesture = enabled ? MDR_TRUE : MDR_FALSE;
            mdrHeadphonesSetPower(gDevice, &power);
        }
        ImGui::TreePop();
    }
    /* Auto Power Off */
    if (FeatureAvailable(MDR_FEATURE_AUTO_POWER_OFF) &&
        ImHeadingTreeNode(TrLable(i18n::TextId::AutoPowerOff), ImGuiTreeNodeFlags_DefaultOpen))
    {
        constexpr uint32_t kSelections[] = {0, 5, 15, 30, 60, 180};
        bool changed =
            ImComboBoxItems(TrLable(i18n::TextId::Time), std::span{kSelections}, power.auto_power_off_minutes, FormatAutoPowerOff);
        if (FeatureAvailable(MDR_FEATURE_WEARING_DETECTION) && power.wearing_power != MDR_WEARING_POWER_UNAVAILABLE)
        {
            bool whenRemoved = power.wearing_power == MDR_WEARING_POWER_WHEN_REMOVED;
            if (ImGui::Checkbox(TrLable(i18n::TextId::PowerOffRemoved), &whenRemoved))
                power.wearing_power = whenRemoved ? MDR_WEARING_POWER_WHEN_REMOVED : MDR_WEARING_POWER_DISABLED,
                changed = true;
        }
        if (changed && havePower)
            mdrHeadphonesSetPower(gDevice, &power);
        ImGui::TreePop();
    }
    /* Auto Pause */
    if (FeatureAvailable(MDR_FEATURE_AUTO_PAUSE) &&
        ImHeadingTreeNode(TrLable(i18n::TextId::PauseRemoved), ImGuiTreeNodeFlags_DefaultOpen))
    {
        bool enabled = power.auto_pause != MDR_FALSE;
        if (ImGui::Checkbox(TrLable(i18n::TextId::Enabled), &enabled) && havePower)
        {
            power.auto_pause = enabled ? MDR_TRUE : MDR_FALSE;
            mdrHeadphonesSetPower(gDevice, &power);
        }
        ImGui::TreePop();
    }
    /* Host-side pause, driven by the proximity sensor */
    if (gState.mWearingStatusAvailable &&
        ImHeadingTreeNode(TrLable(i18n::TextId::PauseHostMedia), ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Checkbox(TrLable(i18n::TextId::Enabled), &gPauseMediaOnRemove);
        ImGui::SameLine();
        ImGui::TextDisabled("(%s, %s)", FormatWearingStatus(gState.mWearingStatus),
                            HostMediaNeedsPausing() ? Tr(i18n::TextId::AnotherDeviceConnected) : Tr(i18n::TextId::HostPauseNotNeeded));
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("%s", Tr(i18n::TextId::HostPauseHelp));
        ImGui::TreePop();
    }
    /* Voice Guidance */
    if (FeatureAvailable(MDR_FEATURE_VOICE_GUIDANCE) &&
        ImHeadingTreeNode(TrLable(i18n::TextId::VoiceGuidance), ImGuiTreeNodeFlags_DefaultOpen))
    {
        MDRVoiceGuidance voice{};
        if (mdrHeadphonesGetVoiceGuidance(gDevice, &voice) == MDR_RESULT_OK)
        {
            bool changed = false;
            bool enabled = voice.enabled != MDR_FALSE;
            if (ImGui::Checkbox(TrLable(i18n::TextId::Enabled), &enabled))
                voice.enabled = enabled ? MDR_TRUE : MDR_FALSE, changed = true;
            if (FeatureAvailable(MDR_FEATURE_VOICE_GUIDANCE_VOLUME))
            {
                ImHeading(Tr(i18n::TextId::Volume), IM_FONTSIZE_SUBHEADING);
                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                int volume = voice.volume;
                if (ImGui::SliderInt("##Volume", &volume, -2, 2))
                    voice.volume = static_cast<int8_t>(volume), changed = true;
            }
            if (changed)
                mdrHeadphonesSetVoiceGuidance(gDevice, &voice);
        }
        ImGui::TreePop();
    }
}
void DrawDeviceControlsAbout()
{
    if (ImHeadingTreeNode(TrLable(i18n::TextId::Model), ImGuiTreeNodeFlags_DefaultOpen))
    {
        if (ImGui::BeginTable("##ModelTable", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(Tr(i18n::TextId::ModelLabel));
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", GetText(MDR_TEXT_MODEL_NAME).c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(Tr(i18n::TextId::MacLabel));
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", GetText(MDR_TEXT_UNIQUE_ID).c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(Tr(i18n::TextId::FirmwareLabel));
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", GetText(MDR_TEXT_FIRMWARE_VERSION).c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(Tr(i18n::TextId::SeriesLabel));
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", GetText(MDR_TEXT_MODEL_SERIES).c_str());

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextUnformatted(Tr(i18n::TextId::ColorLabel));
            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", GetText(MDR_TEXT_MODEL_COLOR).c_str());

            ImGui::EndTable();
        }
        ImGui::TreePop();
    }
    if (ImHeadingTreeNode(TrLable(i18n::TextId::Features), ImGuiTreeNodeFlags_DefaultOpen))
    {
        struct FeatureRow
        {
            i18n::TextId name;
            MDRFeature feature;
        };
        constexpr FeatureRow kFeatures[] = {
            {i18n::TextId::Identity, MDR_FEATURE_IDENTITY},
            {i18n::TextId::SingleBattery, MDR_FEATURE_BATTERY_SINGLE},
            {i18n::TextId::LeftRightBattery, MDR_FEATURE_BATTERY_LEFT_RIGHT},
            {i18n::TextId::CaseBattery, MDR_FEATURE_BATTERY_CASE},
            {i18n::TextId::PlaybackMetadata, MDR_FEATURE_PLAYBACK_METADATA},
            {i18n::TextId::PlaybackControl, MDR_FEATURE_PLAYBACK_CONTROL},
            {i18n::TextId::PlaybackVolume, MDR_FEATURE_PLAYBACK_VOLUME},
            {i18n::TextId::NoiseCancelling, MDR_FEATURE_NOISE_CANCELLING},
            {i18n::TextId::AmbientSound, MDR_FEATURE_AMBIENT_SOUND},
            {i18n::TextId::AdaptiveAmbientSound, MDR_FEATURE_ADAPTIVE_AMBIENT_SOUND},
            {i18n::TextId::SpeakToChat, MDR_FEATURE_SPEAK_TO_CHAT},
            {i18n::TextId::ListeningMode, MDR_FEATURE_LISTENING_MODE},
            {i18n::TextId::ListeningBackgroundFeature, MDR_FEATURE_LISTENING_BACKGROUND_MUSIC},
            {i18n::TextId::ListeningCinemaFeature, MDR_FEATURE_LISTENING_CINEMA},
            {i18n::TextId::ListeningVoiceFeature, MDR_FEATURE_LISTENING_VOICE_BOOST},
            {i18n::TextId::ListeningLeakageFeature, MDR_FEATURE_LISTENING_SOUND_LEAKAGE_REDUCTION},
            {i18n::TextId::Equalizer, MDR_FEATURE_EQUALIZER},
            {i18n::TextId::Dsee, MDR_FEATURE_DSEE},
            {i18n::TextId::PairedDeviceManagement, MDR_FEATURE_PAIRED_DEVICE_MANAGEMENT},
            {i18n::TextId::PairingMode, MDR_FEATURE_PAIRING_MODE},
            {i18n::TextId::GeneralSettings, MDR_FEATURE_GENERAL_SETTINGS},
            {i18n::TextId::AssignableControls, MDR_FEATURE_ASSIGNABLE_CONTROLS},
            {i18n::TextId::NoiseControlButton, MDR_FEATURE_NOISE_CONTROL_BUTTON},
            {i18n::TextId::AutoPowerOff, MDR_FEATURE_AUTO_POWER_OFF},
            {i18n::TextId::WearingDetection, MDR_FEATURE_WEARING_DETECTION},
            {i18n::TextId::AutoPause, MDR_FEATURE_AUTO_PAUSE},
            {i18n::TextId::HeadGesture, MDR_FEATURE_HEAD_GESTURE},
            {i18n::TextId::VoiceGuidance, MDR_FEATURE_VOICE_GUIDANCE},
            {i18n::TextId::VoiceGuidanceVolume, MDR_FEATURE_VOICE_GUIDANCE_VOLUME},
            {i18n::TextId::Shutdown, MDR_FEATURE_SHUTDOWN},
            {i18n::TextId::ConnectionMode, MDR_FEATURE_CONNECTION_MODE},
            {i18n::TextId::SafeListening, MDR_FEATURE_SAFE_LISTENING},
            {i18n::TextId::WearingStatus, MDR_FEATURE_WEARING_STATUS},
        };
        if (ImGui::BeginTable("##Features", 2, ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
        {
            for (const FeatureRow& row : kFeatures)
            {
                MDRFeatureAvailability availability = MDR_AVAILABILITY_UNKNOWN;
                mdrHeadphonesGetFeature(gDevice, row.feature, &availability);
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::TextUnformatted(i18n::Translate(row.name, clientGetAppLocale()));
                ImGui::TableSetColumnIndex(1);
                ImGui::Text("%s", FormatFeatureAvailability(availability));
            }
            ImGui::EndTable();
        }
        ImGui::TreePop();
    }
}
void DrawDeviceControlsTabs()
{
    if (ImGui::BeginTabBar("##Controls"))
    {
        if (ImGui::BeginTabItem(TrLable(i18n::TextId::Playback)))
        {
            DrawDeviceControlsPlayback();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TrLable(i18n::TextId::Sound)))
        {
            DrawDeviceControlsSound();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TrLable(i18n::TextId::Devices)))
        {
            DrawDeviceControlsDevices();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TrLable(i18n::TextId::System)))
        {
            DrawDeviceControlsSystem();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem(TrLable(i18n::TextId::About)))
        {
            DrawDeviceControlsAbout();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void DrawDeviceControls()
{
    MDREvent event = MDR_EVENT_NONE;
    const MDRResult pollResult = mdrHeadphonesPoll(gDevice, &event);
    if (pollResult != MDR_RESULT_OK)
    {
        DisconnectWithModal();
        return;
    }
    switch (event)
    {
    case MDR_EVENT_INITIALIZE_COMPLETE:
        if (mdrHeadphonesRequestSync(gDevice) != MDR_RESULT_OK)
        {
            DisconnectWithModal();
            return;
        }
        break;
    case MDR_EVENT_IDENTITY_CHANGED:
        gState.mModelAvailable = mdrHeadphonesGetModel(gDevice, &gState.mModel) == MDR_RESULT_OK;
        MaterialYouTheme::ApplyForModelColor(GetModelColor());
        break;
    case MDR_EVENT_BATTERY_CHANGED:
        gState.mBatteries = GetBatteries();
        break;
    case MDR_EVENT_WEARING_STATUS_CHANGED:
        OnWearingStatusChanged();
        break;
    case MDR_EVENT_PLAYBACK_CHANGED:
        RefreshPlaybackState();
        break;
    case MDR_EVENT_NOISE_CONTROL_CHANGED:
        gState.mNoiseAvailable = mdrHeadphonesGetNoiseControl(gDevice, &gState.mNoise) == MDR_RESULT_OK;
        break;
    case MDR_EVENT_SPEAK_TO_CHAT_CHANGED:
        gState.mSpeakToChatAvailable = mdrHeadphonesGetSpeakToChat(gDevice, &gState.mSpeakToChat) == MDR_RESULT_OK;
        break;
    case MDR_EVENT_LISTENING_MODE_CHANGED:
        gState.mListeningAvailable = mdrHeadphonesGetListening(gDevice, &gState.mListening) == MDR_RESULT_OK;
        break;
    case MDR_EVENT_EQUALIZER_CHANGED:
        gState.mEqualizerAvailable = mdrHeadphonesGetEqualizer(gDevice, &gState.mEqualizer) == MDR_RESULT_OK;
        gState.mEqualizerBands = GetEqualizerBands();
        break;
    case MDR_EVENT_PAIRED_DEVICES_CHANGED:
        gState.mPairedDevices = GetPairedDevices();
        break;
    case MDR_EVENT_PAIRING_CHANGED:
        gState.mPairingAvailable = mdrHeadphonesGetPairing(gDevice, &gState.mPairing) == MDR_RESULT_OK;
        break;
    case MDR_EVENT_GENERAL_SETTINGS_CHANGED:
        gState.mGeneralSettings = GetGeneralSettings(GetGeneralSettingInfos());
        break;
    case MDR_EVENT_SYNC_COMPLETE:
        RefreshClientState();
        MaterialYouTheme::ApplyForModelColor(GetModelColor());
        break;
    case MDR_EVENT_APPLY_COMPLETE:
        RefreshPlaybackState();
        break;
    case MDR_EVENT_ALERT:
        gAlertMessage = GetText(MDR_TEXT_LAST_ALERT);
        gAlertPending = true;
        break;
    case MDR_EVENT_NEED_SYNC:
        gState.mPendingSync = true;
        break;
    }

    DrawDeviceControlsHeader();
    if (!gDevice)
        return;
    DrawDeviceAlert();
    if (!gDevice)
        return;
    ImGui::Separator();
    ImGui::BeginChild("##ControlTabs");
    ImGui::BeginDisabled(!mdrHeadphonesIsReady(gDevice));
    DrawDeviceControlsTabs();
    ImGui::EndDisabled();
    ImScrollWhenDraggingAnywhere(ImGui::GetIO().MouseDelta, ImGuiMouseButton_Left);
    ImGui::EndChild();

    if (mdrHeadphonesIsReady(gDevice))
    {
        if (mdrHeadphonesIsDirty(gDevice) && mdrHeadphonesRequestCommit(gDevice) != MDR_RESULT_OK)
            DisconnectWithModal();
        if (gState.mPendingSync)
        {
            gState.mPendingSync = false;
            if (mdrHeadphonesRequestSync(gDevice) != MDR_RESULT_OK)
                DisconnectWithModal();
        }
    }
}

void DrawDeviceDisconnect()
{
    MDRConnection* conn = clientPlatformConnectionGet();
    static bool popup = false;
    if (!popup)
    {
#ifdef MDR_CLIENT_DEBUGGER
        clientDebuggerClearExportStatus();
#endif
        MDR_LOG("[Client] Device disconnected")
        if (!connectionAttempt.lastError.empty())
            MDR_LOG("[Client] Connection: {}", connectionAttempt.lastError)
        else if (conn)
            MDR_LOG("[Client] Connection: {}", mdrConnectionGetLastError(conn))
        if (!gHeadphonesError.empty())
            MDR_LOG("[Client] Headphones: {}", gHeadphonesError)
        ImGui::OpenPopup("Disconnected"), popup = true;
    }
    ImSetNextWindowCentered();

    if (ImGui::BeginPopupModal("Disconnected", nullptr, kImWindowFlagsTopMost))
    {
        ImGui::NewLine();
        ImTextCentered(Tr(i18n::TextId::Disconnected), IM_FONTSIZE_TITLE);
        ImSpinner(2000.0f, ImGui::GetContentRegionAvail().x - ImGui::GetStyle().WindowPadding.x * 2.0f,
                  MaterialYouTheme::ArgbToImU32(MaterialYouTheme::FixedSurfaceColors::error),
                  ImGui::GetFontSize() * 0.25f, true);
        if (!connectionAttempt.lastError.empty())
            ImGui::TextWrapped("%s", mdr::Format(fmt::runtime(Tr(i18n::TextId::ConnectionError)), connectionAttempt.lastError).c_str());
        else if (conn)
            ImGui::TextWrapped("%s", mdr::Format(fmt::runtime(Tr(i18n::TextId::ConnectionError)), mdrConnectionGetLastError(conn)).c_str());
        if (!gHeadphonesError.empty())
            ImGui::TextWrapped("%s", mdr::Format(fmt::runtime(Tr(i18n::TextId::HeadphonesError)), gHeadphonesError).c_str());
#ifdef MDR_CLIENT_DEBUGGER
        ImGui::Separator();
        ImTextCentered(mdr::Format(PSI_INFO_SIGN " {}", Tr(i18n::TextId::DebuggerHelp)).c_str(), IM_FONTSIZE_CAPTION);
        const char* exportStatus = clientDebuggerGetExportStatus();
        if (*exportStatus)
            ImGui::TextWrapped("%s", mdr::Format(fmt::runtime(Tr(i18n::TextId::PacketExport)), exportStatus).c_str());
#endif
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::BeginDisabled(
#ifdef MDR_CLIENT_DEBUGGER
            !clientDebuggerHasPackets() || clientDebuggerExportInProgress()
#else
            false
#endif
        );
#ifdef MDR_CLIENT_DEBUGGER
        if (ImModalButton(TrLable(i18n::TextId::ExportLatest, PSI_SAVE), 0, 3))
            clientDebuggerExportLatestPacket();
        if (ImModalButton(TrLable(i18n::TextId::ExportZip, PSI_SAVE), 1, 3))
            clientDebuggerExportPacketCollection();
#endif
        ImGui::EndDisabled();
        if (ImModalButton(TrLable(i18n::TextId::Reconnect, PSI_LINK),
#ifdef MDR_CLIENT_DEBUGGER
                          2, 3
#else
                          0, 1
#endif
                          ))
        {
            CloseDevice();
            mdrConnectionDisconnect(conn);
            connectionAttempt = {};
            connState = CONN_STATE_NO_CONNECTION;
        }

        ImGui::EndPopup();
    }
    else
        popup = false;
}

void DrawApp()
{
    ImStylesRAII scope;
    scope.PushFont(IM_FONTSIZE_BODY);
    auto& io = ImGui::GetIO();
    auto& g = *ImGui::GetCurrentContext();
#ifdef MDR_CLIENT_DEBUGGER
    if (gDebuggerOnlyMode)
    {
        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize(io.DisplaySize);
        if (ImGui::Begin(TrLable(i18n::TextId::AppName), nullptr, kImWindowFlagsTopMost))
            ImGui::TextDisabled("%s", Tr(i18n::TextId::PacketReplayMode));
        ImGui::End();
        clientDebuggerDraw(&gDebuggerOpen, true);
        if (!gDebuggerOpen)
            gDebuggerOnlyMode = false;
        return;
    }
#endif
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGuiWindowFlags flags = kImWindowFlagsTopMost;
    switch (connState)
    {
    case CONN_STATE_CONNECTED:
        flags |= ImGuiWindowFlags_MenuBar;
        break;
    default:
        break;
    }
    if (ImGui::Begin(TrLable(i18n::TextId::AppName), nullptr, flags))
    {
        switch (connState)
        {
        case CONN_STATE_NO_CONNECTION:
#ifdef MDR_CLIENT_DEBUGGER
            if (!gDebuggerOpen)
#endif
                DrawDeviceDiscovery();
            break;
        case CONN_STATE_CONNECTING:
            DrawDeviceConnecting();
            break;
        case CONN_STATE_CONNECTED:
            DrawDeviceControls();
            break;
        case CONN_STATE_DISCONNECTED:
            DrawDeviceDisconnect();
            break;
        }
    }
    ImGui::End();
#ifdef MDR_CLIENT_DEBUGGER
    // Error modals replace the debugger popup while preserving its open state.
    // Once the error is dismissed, the debugger reopens with its packet history intact.
    if (connState != CONN_STATE_DISCONNECTED && (gDebuggerOpen || ImGui::IsPopupOpen("Debugger")))
        clientDebuggerDraw(&gDebuggerOpen);
#endif
}

#ifdef MDR_CLIENT_DEBUGGER
void clientEnterDebuggerReplayMode()
{
    CloseDevice();
    clientPlatformConnectionDestroy();
    connectionAttempt = {};
    connState = CONN_STATE_NO_CONNECTION;
    gDebuggerOnlyMode = true;
    gDebuggerOpen = true;
}
#endif

bool clientShouldExit()
{
    // Defines like IMGUI_DISABLE_OBSOLETE_FUNCTIONS changes ImGui struct sizes
    // and can lead to very, very bad results. Check them here too to ensure than this TU got the correct ones.
    IMGUI_CHECKVERSION();
    DrawApp();
    return false;
}
