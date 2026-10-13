#include <algorithm>
#include <ctime>
#include "Details.hpp"
// NOLINTBEGIN
namespace mdr
{
    using namespace v2;

    bool MDRHeadphones::AcceptInitResponseV2(MDRDataType type, Span<const UInt8> command)
    {
        if (type != MDRDataType::DATA_MDR || command.empty() ||
            command[0] != static_cast<UInt8>(t1::Command::GENERAL_SETTING_RET_CAPABILITY))
            return true;
        if (command.size() < 4 || command[3] > static_cast<UInt8>(t1::GsStringFormat::ENUM_NAME))
            return false;
        size_t cursor = 4;
        auto SkipString = [&]()
        {
            if (cursor == command.size()) return false;
            const size_t length = command[cursor++];
            if (length > command.size() - cursor) return false;
            cursor += length;
            return true;
        };
        if (!SkipString() || !SkipString()) return false;
        if (command[2] == static_cast<UInt8>(t1::GsSettingType::BOOLEAN_TYPE)) return true;
        if (command[2] != static_cast<UInt8>(t1::GsSettingType::LIST_TYPE) || cursor == command.size())
            return false;
        const UInt8 count = command[cursor++];
        if (count < 1 || count > 64) return false;
        for (UInt8 index = 0; index < count; ++index)
        {
            if (cursor == command.size() || command[cursor++] > static_cast<UInt8>(t1::GsStringFormat::ENUM_NAME) ||
                !SkipString() || !SkipString())
                return false;
        }
        return true;
    }

    // Every query is serialized with its matching response, including RET-before-ACK.
    // These fixed wire prefixes are the original Factory.valueOf payloads. New KMP-only
    // inquiries are proven by Android 13.3.0 bg0/C5896d and its byte-writer codecs.
#define V2_QUERY(Table, Get, Ret, Inquiry, Detail, Deadline, ...) \
    do { \
        const UInt8 request[] = {static_cast<UInt8>(Table::Command::Get), static_cast<UInt8>(Inquiry) __VA_OPT__(,) __VA_ARGS__}; \
        constexpr auto dataType = MDRTraits<Table::ConnectGetSupportFunction>::kDataType; \
        const auto responseType = ResponseType(dataType, static_cast<UInt8>(Table::Command::Ret), static_cast<UInt8>(Inquiry), Detail); \
        const auto queryDeadline = (Deadline); \
        auto remaining = [&]() { return std::max(0, static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(queryDeadline - std::chrono::steady_clock::now()).count())); }; \
        responseResult = MDR_RESULT_ERROR_TIMEOUT; \
        if (remaining() == 0) break; \
        BeginAwait(responseType); \
        int ackResult = MDR_RESULT_ERROR_TIMEOUT; \
        for (int retries = 0; retries < mACKRetryCount && remaining() > 0; ++retries) { \
            const int ackTimeout = std::min(mACKRetryTimeoutMS, remaining()); \
            if (ackTimeout <= 0) break; \
            SendCommandImpl({request, sizeof(request)}, dataType, mSeqNumber); \
            ackResult = co_await Await(AWAIT_ACK, ackTimeout); \
            if (ackResult == MDR_RESULT_OK) break; \
        } \
        const int responseTimeout = remaining(); \
        if (ackResult != MDR_RESULT_OK || responseTimeout <= 0) { \
            responseResult = ackResult != MDR_RESULT_OK ? ackResult : MDR_RESULT_ERROR_TIMEOUT; \
            CancelAwait(responseType, responseResult); \
        } else { \
            responseResult = co_await Await(responseType, responseTimeout); \
        } \
    } while (false)
#define DISC(Table, Get, Ret, Inquiry, Detail, ...) \
    do { V2_QUERY(Table, Get, Ret, Inquiry, Detail, discoveryDeadline __VA_OPT__(,) __VA_ARGS__); \
         if (responseResult != MDR_RESULT_OK) co_return SetLastError(responseResult, "Timeout waiting for MDR V2 capability response"); } while (false)
#define LIVE(Table, Get, Ret, Inquiry, Detail, ...) \
    { V2_QUERY(Table, Get, Ret, Inquiry, Detail, std::chrono::steady_clock::now() + std::chrono::seconds(2) __VA_OPT__(,) __VA_ARGS__); \
      if (responseResult != MDR_RESULT_OK) break; }

    MDRTask MDRHeadphones::RequestInitV2()
    {
        auto& state = mDetailsV2;
        if (mProtocolFamily != ProtocolFamily::V2)
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Device does not use MDR V2");
        state.mProtocol = {};
        state.mSupport = {};
        state.mAutoPowerOffCandidates.clear();
        state.mVoiceGuidanceOnOffSupported = {};
        state.mAssignableHasCustomizableActions = {};
        state.mGsDiscoveryOrderSize = 0;
        state.mEqCanUseUltMode = false;
        state.mEqCustomizable = false;
        state.mEqErrorCodeMode = false;
        state.mEqErrorCodeCount = 0;
        state.mSvaReceivedLength = 0;
        state.mSvaDataContinue = t2::DataContinue::NOT_CONTINUE_DATA;
        const auto discoveryDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(30);
        int responseResult;
        DISC(t1, CONNECT_GET_PROTOCOL_INFO, CONNECT_RET_PROTOCOL_INFO, t1::ConnectInquiredType::FIXED_VALUE, -1);
        static constexpr int acceptedVersions[] = {16777216, 33554432, 33554433, 33558528, 50331649, 50331648, 50331650, 50331652, 50331653, 50331651, 50331654, 50331655, 50331656, 50331657, 50331664, 50331665, 50331666, 50331667, 50331668, 50331669, 50331670, 50331671, 50331672, 50331673, 50331680, 50335745, 50335746, 50335747, 50335748, 50339840, 50339841, 50339842, 50339843, 50339844, 50339845, 50339846, 50339847, 50339848, 50339849, 50339858, 50339859, 50339860, 50339861, 50339862, 50339863, 50339864, 50339865, 50343936, 50343937, 50343938, 50343939, 50343940, 50343941, 50343942, 50343943, 50343944, 50343945, 50343952, 50343953, 50343954, 50343955, 50343956, 50343957, 50343958, 50343959, 50343960, 50343961, 50343968, 50343969, 50343971, 50343972, 50343973, 50343974, 50343986, 50343987, 50343991};
        const int version = state.mProtocol.version;
        if (std::ranges::find(acceptedVersions, version) == std::end(acceptedVersions))
            co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Unacceptable MDR V2 protocol version");
        if (version == 60397977 || (version & 0xFF000000) == 0x04000000)
            DISC(t1, CONNECT_GET_CAPABILITY_INFO, CONNECT_RET_CAPABILITY_INFO, 0x01, -1, 0x01);
        else
            DISC(t1, CONNECT_GET_CAPABILITY_INFO, CONNECT_RET_CAPABILITY_INFO, t1::ConnectInquiredType::FIXED_VALUE, -1);
        DISC(t1, CONNECT_GET_DEVICE_INFO, CONNECT_RET_DEVICE_INFO, t1::DeviceInfoType::MODEL_NAME, -1);
        DISC(t1, CONNECT_GET_DEVICE_INFO, CONNECT_RET_DEVICE_INFO, t1::DeviceInfoType::FW_VERSION, -1);
        DISC(t1, CONNECT_GET_DEVICE_INFO, CONNECT_RET_DEVICE_INFO, t1::DeviceInfoType::SERIES_AND_COLOR_INFO, -1);
        if (version < 50335748 || version == 50339859 || version == 50339860)
            DISC(t1, CONNECT_GET_DEVICE_INFO, CONNECT_RET_DEVICE_INFO, t1::DeviceInfoType::INSTRUCTION_GUIDE, -1);
        if (state.mProtocol.hasTable1)
            DISC(t1, CONNECT_GET_SUPPORT_FUNCTION, CONNECT_RET_SUPPORT_FUNCTION, t1::ConnectInquiredType::FIXED_VALUE, -1);
        if (state.mProtocol.hasTable2)
            DISC(t2, CONNECT_GET_SUPPORT_FUNCTION, CONNECT_RET_SUPPORT_FUNCTION, t2::ConnectInquiredType::FIXED_VALUE, -1);
        if (version > 50339860 && !state.mSupport.contains(t1::FunctionType::TUTORIAL_CONTENTS_SELECT_ON_CONCIERGE))
            DISC(t1, CONNECT_GET_DEVICE_INFO, CONNECT_RET_DEVICE_INFO, t1::DeviceInfoType::INSTRUCTION_GUIDE, -1);
        if (state.mSupport.contains(t1::FunctionType::TWS_SUPPORTS_A2DP_LEA_UNI_LEA_BROAD_WITH_CTKD))
            DISC(t1, LEA_GET_CAPABILITY, LEA_RET_CAPABILITY, t1::LEAInquiredType::TWS_SUPPORTS_A2DP_LEA_UNI_LEA_BROAD_WITH_CTKD, -1);
        if (state.mSupport.contains(t1::FunctionType::HBS_SUPPORTS_A2DP_LEA_UNI_LEA_BROAD_WITH_CTKD))
            DISC(t1, LEA_GET_CAPABILITY, LEA_RET_CAPABILITY, t1::LEAInquiredType::HBS_SUPPORTS_A2DP_LEA_UNI_LEA_BROAD_WITH_CTKD, -1);
        if (state.mSupport.contains(t1::FunctionType::TWS_SUPPORTS_LEA_UNI_LEA_BROAD))
            DISC(t1, LEA_GET_CAPABILITY, LEA_RET_CAPABILITY, t1::LEAInquiredType::TWS_SUPPORTS_LEA_UNI_LEA_BROAD, -1);
        if (state.mSupport.contains(t1::FunctionType::QUICK_ACCESS_CANT_BE_USED_WITH_LEA_CONNECTION))
            DISC(t1, LEA_GET_CAPABILITY, LEA_RET_CAPABILITY, t1::LEAInquiredType::QUICK_ACCESS_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
        if (state.mSupport.contains(t1::FunctionType::VOICE_ASSISTANT_SETTINGS_CANT_BE_USED_WITH_LEA_CONNECTION))
            DISC(t1, LEA_GET_CAPABILITY, LEA_RET_CAPABILITY, t1::LEAInquiredType::VOICE_ASSISTANT_SETTINGS_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
        if (state.mSupport.contains(t2::FunctionType::LE_AUDIO_SWITCH_SUPPORTED_COMPATIBILITY))
            DISC(t2, LEA_GET_CAPABILITY, LEA_RET_CAPABILITY, t2::LEAInquiredType::LE_AUDIO_SWITCH_SUPPORTED_COMPATIBILITY, -1);
        if (state.mSupport.contains(t2::FunctionType::LE_AUDIO_CONNECTION_MODE))
            DISC(t2, LEA_GET_CAPABILITY, LEA_RET_CAPABILITY, t2::LEAInquiredType::LE_AUDIO_CONNECTION_MODE_WITH_BT_RECONNECTION, -1);
        if (state.mSupport.contains(t2::FunctionType::PAS_SUPPORTS_A2DP_LEA_UNI_LEA_BROAD_WITH_CTKD))
            DISC(t2, LEA_GET_CAPABILITY, LEA_RET_CAPABILITY, t2::LEAInquiredType::PAS_SUPPORTS_A2DP_LEA_UNI_LEA_BROAD_WITH_CTKD, -1);
        if (state.mSupport.contains(t2::FunctionType::DJ_CONTROL))
        {
            DISC(t2, PARTY_GET_CAPABILITY, PARTY_RET_CAPABILITY, t2::PartyInquiredType::DJ_CONTROL, -1);
        }
        else
        {
            if (state.mSupport.contains(t2::FunctionType::DJ_CONTROL_WITH_STATUS_DISABLE_REASON))
            {
                DISC(t2, PARTY_GET_CAPABILITY, PARTY_RET_CAPABILITY, t2::PartyInquiredType::DJ_CONTROL_WITH_STATUS_DISABLE_REASON, -1);
            }
        }
        if (state.mSupport.contains(t1::FunctionType::PRESET_EQ))
        {
            DISC(t1, EQEBB_GET_CAPABILITY, EQEBB_RET_CAPABILITY, t1::EqEbbInquiredType::PRESET_EQ, -1, static_cast<UInt8>(t1::DisplayLanguage::ENGLISH));
        }
        if (state.mSupport.contains(t1::FunctionType::PRESET_EQ_NON_CUSTOMIZABLE))
        {
            DISC(t1, EQEBB_GET_CAPABILITY, EQEBB_RET_CAPABILITY, t1::EqEbbInquiredType::PRESET_EQ_NONCUSTOMIZABLE, -1, static_cast<UInt8>(t1::DisplayLanguage::ENGLISH));
        }
        if (state.mSupport.contains(t1::FunctionType::PRESET_EQ_AND_ULT_MODE))
        {
            DISC(t1, EQEBB_GET_CAPABILITY, EQEBB_RET_CAPABILITY, t1::EqEbbInquiredType::PRESET_EQ_AND_ULT_MODE, -1, static_cast<UInt8>(t1::DisplayLanguage::ENGLISH));
        }
        if (state.mSupport.contains(t1::FunctionType::EBB))
        {
            DISC(t1, EQEBB_GET_CAPABILITY, EQEBB_RET_CAPABILITY, t1::EqEbbInquiredType::EBB, -1, static_cast<UInt8>(t1::DisplayLanguage::ENGLISH));
        }
        if (state.mSupport.contains(t1::FunctionType::SOUND_EFFECT))
        {
            DISC(t1, EQEBB_GET_CAPABILITY, EQEBB_RET_CAPABILITY, t1::EqEbbInquiredType::SOUND_EFFECT, -1, static_cast<UInt8>(t1::DisplayLanguage::ENGLISH));
        }
        if (state.mSupport.contains(t1::FunctionType::CUSTOMIZABLE_SOUND_EFFECT))
        {
            DISC(t1, EQEBB_GET_CAPABILITY, EQEBB_RET_CAPABILITY, t1::EqEbbInquiredType::CUSTOMIZABLE_SOUND_EFFECT_SELECT, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::CUSTOM_EQ))
        {
            DISC(t1, EQEBB_GET_CAPABILITY, EQEBB_RET_CAPABILITY, t1::EqEbbInquiredType::CUSTOM_EQ, -1, static_cast<UInt8>(t1::DisplayLanguage::ENGLISH));
        }
        if (state.mSupport.contains(t1::FunctionType::PRESET_EQ_AND_ERRORCODE))
        {
            DISC(t1, EQEBB_GET_CAPABILITY, EQEBB_RET_CAPABILITY, t1::EqEbbInquiredType::PRESET_EQ_AND_ERRORCODE, -1, static_cast<UInt8>(t1::DisplayLanguage::ENGLISH));
        }
        if (state.mSupport.contains(t1::FunctionType::ULT_SOUND_EFFECT_ASSIGN))
        {
            DISC(t1, EQEBB_GET_CAPABILITY, EQEBB_RET_CAPABILITY, t1::EqEbbInquiredType::ULT_BTN_SOUND_EFFECT_ASSIGN, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::LR_BALANCE_CORRECTION))
        {
            DISC(t1, EQEBB_GET_CAPABILITY, EQEBB_RET_CAPABILITY, 0x51, -1, static_cast<UInt8>(t1::DisplayLanguage::ENGLISH));
        }
        if (state.mSupport.contains(t1::FunctionType::TURN_KEY_EQ_CONTROLLER))
        {
            DISC(t1, EQEBB_GET_CAPABILITY, EQEBB_RET_CAPABILITY, 0x37, -1, static_cast<UInt8>(t1::DisplayLanguage::ENGLISH));
        }
        if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_ONOFF))
        {
            DISC(t1, NCASM_GET_CAPABILITY, NCASM_RET_CAPABILITY, t1::NcAsmInquiredType::NC_ON_OFF_AND_ASM_ON_OFF, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            DISC(t1, NCASM_GET_CAPABILITY, NCASM_RET_CAPABILITY, t1::NcAsmInquiredType::NC_ON_OFF_AND_ASM_SEAMLESS, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING_DUAL_SINGLE_OFF_AND_AMBIENT_SOUND_MODE_ONOFF))
        {
            DISC(t1, NCASM_GET_CAPABILITY, NCASM_RET_CAPABILITY, t1::NcAsmInquiredType::NC_MODE_SWITCH_AND_ASM_ON_OFF, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING_DUAL_SINGLE_OFF_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            DISC(t1, NCASM_GET_CAPABILITY, NCASM_RET_CAPABILITY, t1::NcAsmInquiredType::NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AUTO_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            DISC(t1, NCASM_GET_CAPABILITY, NCASM_RET_CAPABILITY, t1::NcAsmInquiredType::MODE_NC_ASM_AUTO_NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_SINGLE_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            DISC(t1, NCASM_GET_CAPABILITY, NCASM_RET_CAPABILITY, t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_SINGLE_NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            DISC(t1, NCASM_GET_CAPABILITY, NCASM_RET_CAPABILITY, t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::MODE_NC_NCSS_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_WITH_TEST_MODE))
        {
            DISC(t1, NCASM_GET_CAPABILITY, NCASM_RET_CAPABILITY, t1::NcAsmInquiredType::MODE_NC_NCSS_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
        {
            DISC(t1, NCASM_GET_CAPABILITY, NCASM_RET_CAPABILITY, t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_CONTROL_MODE_SELECT))
        {
            DISC(t1, NCASM_GET_CAPABILITY, NCASM_RET_CAPABILITY, t1::NcAsmInquiredType::NC_AMB_TOGGLE, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_MODE_ONOFF))
        {
            DISC(t1, NCASM_GET_CAPABILITY, NCASM_RET_CAPABILITY, t1::NcAsmInquiredType::ASM_ON_OFF, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            DISC(t1, NCASM_GET_CAPABILITY, NCASM_RET_CAPABILITY, t1::NcAsmInquiredType::ASM_SEAMLESS, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::NC_OPTIMIZER_PERSONAL_BAROMETRIC))
        {
            DISC(t1, OPT_GET_CAPABILITY, OPT_RET_CAPABILITY, t1::OptInquiredType::NC_OPTIMIZER_PERSONAL_BAROMETRIC, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::NC_OPTIMIZER_PERSONAL))
        {
            DISC(t1, OPT_GET_CAPABILITY, OPT_RET_CAPABILITY, t1::OptInquiredType::NC_OPTIMIZER_PERSONAL, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::NC_OPTIMIZER_BAROMETRIC))
        {
            DISC(t1, OPT_GET_CAPABILITY, OPT_RET_CAPABILITY, t1::OptInquiredType::NC_OPTIMIZER_BAROMETRIC, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::TV_SOUND_BOOSTER))
        {
            DISC(t1, OPT_GET_CAPABILITY, OPT_RET_CAPABILITY, t1::OptInquiredType::TV_SOUND_BOOSTER, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT))
        {
            DISC(t1, PLAY_GET_CAPABILITY, PLAY_RET_CAPABILITY, t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE))
        {
            DISC(t1, PLAY_GET_CAPABILITY, PLAY_RET_CAPABILITY, t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER_WITH_FUNCTION_CHANGE))
        {
            DISC(t1, PLAY_GET_CAPABILITY, PLAY_RET_CAPABILITY, t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT_AND_MUTE))
        {
            DISC(t1, PLAY_GET_CAPABILITY, PLAY_RET_CAPABILITY, t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::SAR))
        DISC(t1, SAR_AUTO_PLAY_GET_CAPABILITY, SAR_AUTO_PLAY_RET_CAPABILITY, t1::SARAutoPlayInquiredType::SAR, -1);
        if (state.mSupport.contains(t1::FunctionType::AUTO_PLAY))
        DISC(t1, SAR_AUTO_PLAY_GET_CAPABILITY, SAR_AUTO_PLAY_RET_CAPABILITY, t1::SARAutoPlayInquiredType::AUTO_PLAY, -1);
        if (state.mSupport.contains(t1::FunctionType::INTEGRATED_AUTO_PLAY))
        DISC(t1, SAR_AUTO_PLAY_GET_CAPABILITY, SAR_AUTO_PLAY_RET_CAPABILITY, t1::SARAutoPlayInquiredType::INTEGRATED_AUTO_PLAY, -1);
        if (state.mSupport.contains(t1::FunctionType::GATT_CONNECTABLE))
        DISC(t1, SAR_AUTO_PLAY_GET_CAPABILITY, SAR_AUTO_PLAY_RET_CAPABILITY, t1::SARAutoPlayInquiredType::GATT_CONNECTABLE, -1);
        if (state.mSupport.contains(t1::FunctionType::UPSCALING_AUTO_OFF))
        {
            DISC(t1, AUDIO_GET_CAPABILITY, AUDIO_RET_CAPABILITY, t1::AudioInquiredType::UPSCALING, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::UPSCALING_AUTO_OFF_WITH_STATUS_DISABLE_REASON))
        {
            DISC(t1, AUDIO_GET_CAPABILITY, AUDIO_RET_CAPABILITY, t1::AudioInquiredType::UPSCALING_AUTO_OFF_WITH_STATUS_DISABLE_REASON, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::BGM_MODE_SMALL_MIDDLE_LARGE))
        DISC(t1, AUDIO_GET_CAPABILITY, AUDIO_RET_CAPABILITY, t1::AudioInquiredType::BGM_MODE, -1);
        if (state.mSupport.contains(t1::FunctionType::BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE))
        {
            DISC(t1, AUDIO_GET_CAPABILITY, AUDIO_RET_CAPABILITY, t1::AudioInquiredType::BGM_MODE_AND_ERRORCODE, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::UPMIX_CINEMA))
        DISC(t1, AUDIO_GET_CAPABILITY, AUDIO_RET_CAPABILITY, t1::AudioInquiredType::UPMIX_CINEMA, -1);
        if (state.mSupport.contains(t1::FunctionType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO))
        DISC(t1, AUDIO_GET_CAPABILITY, AUDIO_RET_CAPABILITY, t1::AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO, -1);
        if (state.mSupport.contains(t1::FunctionType::VOICE_CONTENTS))
        DISC(t1, AUDIO_GET_CAPABILITY, AUDIO_RET_CAPABILITY, t1::AudioInquiredType::VOICE_CONTENTS, -1);
        if (state.mSupport.contains(t1::FunctionType::SOUND_LEAKAGE_REDUCTION))
        DISC(t1, AUDIO_GET_CAPABILITY, AUDIO_RET_CAPABILITY, t1::AudioInquiredType::SOUND_LEAKAGE_REDUCTION, -1);
        if (state.mSupport.contains(t1::FunctionType::UPMIX_SERIES))
        DISC(t1, AUDIO_GET_CAPABILITY, AUDIO_RET_CAPABILITY, t1::AudioInquiredType::UPMIX_SERIES, -1);
        if (state.mSupport.contains(t1::FunctionType::LISTENING_OPTION_ASSIGN_CUSTOMIZABLE))
        DISC(t1, AUDIO_GET_CAPABILITY, AUDIO_RET_CAPABILITY, t1::AudioInquiredType::LISTENING_OPTION_ASSIGN_CUSTOMIZABLE, -1);
        if (state.mSupport.contains(t1::FunctionType::AUDIO_360_UPMIX_OPTIMIZATION))
        DISC(t1, AUDIO_GET_CAPABILITY, AUDIO_RET_CAPABILITY, 0x0C, -1);
        if (state.mSupport.contains(t1::FunctionType::AUDIO_360_UPMIX_HEADTRACKING))
        DISC(t1, AUDIO_GET_CAPABILITY, AUDIO_RET_CAPABILITY, 0x0D, -1);
        if (state.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE_TYPE1))
        {
            DISC(t1, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t1::SystemInquiredType::SMART_TALKING_MODE_TYPE1, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE_TYPE2))
        {
            DISC(t1, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::ASSIGNABLE_SETTING))
        {
            DISC(t1, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t1::SystemInquiredType::ASSIGNABLE_SETTINGS, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::ASSIGNABLE_SETTING_WITH_LIMITATION))
        {
            DISC(t1, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t1::SystemInquiredType::ASSIGNABLE_SETTINGS_WITH_LIMITATION, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::VOICE_ASSISTANT_SETTINGS))
        DISC(t1, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t1::SystemInquiredType::VOICE_ASSISTANT_SETTINGS, -1);
        if (state.mSupport.contains(t1::FunctionType::WEARING_STATUS_DETECTOR))
        DISC(t1, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t1::SystemInquiredType::WEARING_STATUS_DETECTOR, -1);
        if (state.mSupport.contains(t1::FunctionType::EARPIECE_SELECTION))
        DISC(t1, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t1::SystemInquiredType::EARPIECE_SELECTION, -1);
        if (state.mSupport.contains(t1::FunctionType::QUICK_ACCESS))
        DISC(t1, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t1::SystemInquiredType::QUICK_ACCESS, -1);
        for (UInt8 index = 0; index < state.mGsDiscoveryOrderSize; ++index)
        {
            const auto type = state.mGsDiscoveryOrder[index];
            DISC(t1, GENERAL_SETTING_GET_CAPABILITY, GENERAL_SETTING_RET_CAPABILITY, type, -1, static_cast<UInt8>(t1::DisplayLanguage::ENGLISH));
        }
        if (state.mSupport.contains(t1::FunctionType::BLE_SETUP))
        DISC(t1, COMMON_GET_CAPABILITY, COMMON_RET_CAPABILITY, t1::CommonInquiredType::BLE_SETUP, -1);
        if (state.mSupport.contains(t1::FunctionType::PHONE_AND_CONNECTED_DEVICE_INFOMATION_FOR_CLASSIC))
        DISC(t1, COMMON_GET_CAPABILITY, COMMON_RET_CAPABILITY, t1::CommonInquiredType::SMART_PHONE_AND_CONNECTED_DEVICE_INFORMATION, -1);
        if (state.mSupport.contains(t1::FunctionType::DISPLAY_FW_VERSION))
        DISC(t1, COMMON_GET_CAPABILITY, COMMON_RET_CAPABILITY, t1::CommonInquiredType::DISPLAY_FW_VERSION, -1);
        if (state.mSupport.contains(t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
        {
        DISC(t2, PERI_GET_CAPABILITY, PERI_RET_CAPABILITY, t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT, -1);
        }
        if ( (state.mSupport.contains(t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT)) || (state.mSupport.contains(t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE)) )
        {
        DISC(t2, PERI_GET_CAPABILITY, PERI_RET_CAPABILITY, t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE, -1);
        }
        if (state.mSupport.contains(t2::FunctionType::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH))
        {
        DISC(t2, VOICE_GUIDANCE_GET_CAPABILITY, VOICE_GUIDANCE_RET_CAPABILITY, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH, -1);
        }
        if ( (state.mSupport.contains(t2::FunctionType::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH)) || (state.mSupport.contains(t2::FunctionType::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT)) )
        {
        DISC(t2, VOICE_GUIDANCE_GET_CAPABILITY, VOICE_GUIDANCE_RET_CAPABILITY, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH, -1);
        DISC(t2, VOICE_GUIDANCE_GET_EXTENDED_PARAM, VOICE_GUIDANCE_RET_EXTENDED_PARAM, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH, -1);
        }
        if (state.mSupport.contains(t2::FunctionType::VOICE_GUIDANCE_SETTING_SUPPORT_LANGUAGE_SWITCH))
        {
        DISC(t2, VOICE_GUIDANCE_GET_CAPABILITY, VOICE_GUIDANCE_RET_CAPABILITY, t2::VoiceGuidanceInquiredType::SUPPORT_LANGUAGE_SWITCH, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF))
        {
        DISC(t1, POWER_GET_CAPABILITY, POWER_RET_CAPABILITY, t1::PowerInquiredType::AUTO_POWER_OFF, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
        {
        DISC(t1, POWER_GET_CAPABILITY, POWER_RET_CAPABILITY, t1::PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::BATTERY_SAFE_MODE))
        {
        DISC(t1, POWER_GET_CAPABILITY, POWER_RET_CAPABILITY, t1::PowerInquiredType::BATTERY_SAFE_MODE, -1);
        }
        if (state.mSupport.contains(t1::FunctionType::STAMINA))
        DISC(t1, POWER_GET_CAPABILITY, POWER_RET_CAPABILITY, t1::PowerInquiredType::STAMINA, -1);
        if (state.mSupport.contains(t2::FunctionType::AUTO_STANDBY))
        {
        DISC(t2, POWER_GET_CAPABILITY, POWER_RET_CAPABILITY, t2::PowerInquiredType::AUTO_STANDBY, -1);
        }
        if (state.mSupport.contains(t2::FunctionType::CARING_CHARGE_WITH_THRESHOLD))
        {
        DISC(t2, POWER_GET_CAPABILITY, POWER_RET_CAPABILITY, t2::PowerInquiredType::CARING_CHARGE_WITH_THRESHOLD, -1);
        }
        if (state.mSupport.contains(t2::FunctionType::USB_SUBMERSION))
        {
        DISC(t2, POWER_GET_CAPABILITY, POWER_RET_CAPABILITY, t2::PowerInquiredType::USB_SUBMERSION, -1);
        }
        if (state.mSupport.contains(t2::FunctionType::USB_OVERHEAT_DETECTION))
        {
        DISC(t2, POWER_GET_CAPABILITY, POWER_RET_CAPABILITY, 0x03, -1);
        }
        if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_HBS_1))
        {
        DISC(t2, SAFE_LISTENING_GET_CAPABILITY, SAFE_LISTENING_RET_CAPABILITY, t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_1, -1);
        }
        if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_1))
        {
        DISC(t2, SAFE_LISTENING_GET_CAPABILITY, SAFE_LISTENING_RET_CAPABILITY, t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_1, -1);
        }
        if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_HBS_2))
        {
        DISC(t2, SAFE_LISTENING_GET_CAPABILITY, SAFE_LISTENING_RET_CAPABILITY, t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_2, -1);
        }
        if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_2))
        {
        DISC(t2, SAFE_LISTENING_GET_CAPABILITY, SAFE_LISTENING_RET_CAPABILITY, t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_2, -1);
        }
        if (state.mSupport.contains(t2::FunctionType::MAX_VOLUME_LEVEL_LIMIT))
        {
        DISC(t2, SAFE_LISTENING_GET_CAPABILITY, SAFE_LISTENING_RET_CAPABILITY, t2::SafeListeningInquiredType::MAX_VOL_LV_LIMIT, -1);
        }
        if (state.mSupport.contains(t2::FunctionType::REPEAT_TAP_TRAINING_MODE))
        DISC(t2, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t2::SystemInquiredType::REPEAT_TAP_TRAINING_MODE, -1);
        if ( (state.mSupport.contains(t2::FunctionType::WEARING_POSITION)) || (state.mSupport.contains(t2::FunctionType::WEARING_POSITION_WITHOUT_FITTING_SUPPORTER)) )
        DISC(t2, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t2::SystemInquiredType::WEARING_DEVICE_INFORMATION, -1);
        if (state.mSupport.contains(t2::FunctionType::SONY_VOICE_ASSISTANT))
        {
        state.mSvaReceivedLength = 0;
        for (;;)
        {
            DISC(t2, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t2::SystemInquiredType::SONY_VOICE_ASSISTANT_COMMAND, -1, static_cast<UInt8>(state.mSvaReceivedLength >> 8), static_cast<UInt8>(state.mSvaReceivedLength));
            state.mSvaReceivedLength += state.mSvaPartLength;
            if (state.mSvaDataContinue != t2::DataContinue::HAS_CONTINUE_DATA) break;
        }
        DISC(t2, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t2::SystemInquiredType::SONY_VOICE_ASSISTANT_SETTING_MTK_TRANSFER_SUPPORT_LANGUAGE_SWITCH, -1);
        DISC(t2, SYSTEM_GET_EXTENDED_PARAM, SYSTEM_RET_EXTENDED_PARAM, t2::SystemInquiredType::SONY_VOICE_ASSISTANT_SETTING_MTK_TRANSFER_SUPPORT_LANGUAGE_SWITCH, -1);
        }
        if (state.mSupport.contains(t2::FunctionType::ILLUMINATION))
        DISC(t2, PARTY_GET_CAPABILITY, PARTY_RET_CAPABILITY, t2::PartyInquiredType::ILLUMINATION, -1);
        if (state.mSupport.contains(t2::FunctionType::KARAOKE))
        {
        DISC(t2, PARTY_GET_CAPABILITY, PARTY_RET_CAPABILITY, t2::PartyInquiredType::KARAOKE, -1);
        }
        else
        {
        if (state.mSupport.contains(t2::FunctionType::KARAOKE_WITH_STATUS_DISABLE_REASON))
        {
            DISC(t2, PARTY_GET_CAPABILITY, PARTY_RET_CAPABILITY, t2::PartyInquiredType::KARAOKE_WITH_STATUS_DISABLE_REASON, -1);
        }
        }
        if (state.mSupport.contains(t2::FunctionType::LINK_AUTO_SWITCH_FOR_SPEAKER))
        DISC(t2, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t2::SystemInquiredType::LINK_AUTO_SWITCH_FOR_SPEAKER, -1);
        if (state.mSupport.contains(t2::FunctionType::MIC_ON_OFF_BY_HEADPHONE_OPERATION))
        DISC(t2, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t2::SystemInquiredType::MIC_ON_OFF_BY_HEADPHONE_OPERATION, -1);
        if (state.mSupport.contains(t2::FunctionType::FUNCTION_CHANGE))
        DISC(t2, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t2::SystemInquiredType::FUNCTION_CHANGE, -1);
        if (state.mSupport.contains(t2::FunctionType::USB_BROWSER))
        DISC(t1, PLAY_GET_CAPABILITY, PLAY_RET_CAPABILITY, t1::PlayInquiredType::PLAY_MODE, -1);
        if (state.mSupport.contains(t2::FunctionType::USB_BROWSER))
        DISC(t2, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t2::SystemInquiredType::USB_BROWSER, -1);
        if (state.mSupport.contains(t2::FunctionType::LIGHTING_MODE))
        DISC(t2, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t2::SystemInquiredType::LIGHTING_MODE, -1);
        if (state.mSupport.contains(t2::FunctionType::VOICE_ASSISTANT_WITH_SPECIFIC_SETUP_LINK_SUPPORT))
        DISC(t2, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t2::SystemInquiredType::VOICE_ASSISTANT_WITH_SPECIFIC_SETUP_LINK_SUPPORT, -1);
        if (state.mSupport.contains(t2::FunctionType::LIGHTING_DEFAULT_COLOR))
        DISC(t2, SYSTEM_GET_CAPABILITY, SYSTEM_RET_CAPABILITY, t2::SystemInquiredType::LIGHTING_DEFAULT_COLOR_COLOR_TYPE, -1);
        t1::UpdtInquiredType firmwareType = t1::UpdtInquiredType::OUT_OF_RANGE;
        if (state.mSupport.contains(t1::FunctionType::FW_UPDATE_MTK_TRANSFER_WITHOUT_DISCONNECTION))
            firmwareType = t1::UpdtInquiredType::FW_UPDATE_MTK_TRANSFER_WO_DISCONNECTION;
        else if (state.mSupport.contains(t1::FunctionType::FW_UPDATE_MTK_TRANSFER_WITHOUT_DISCONNECTION_AUTO_UPDATE))
            firmwareType = t1::UpdtInquiredType::FW_UPDATE_MTK_TRANSFER_WO_DISCONNECTION_AUTO_UPDATE;
        else if (state.mSupport.contains(t1::FunctionType::FW_UPDATE_MTK_TRANSFER_WITH_REPAIR_MODE))
            firmwareType = t1::UpdtInquiredType::FW_UPDATE_MTK_TRANSFER_WITH_REPAIR_MODE;
        else if (state.mSupport.contains(t1::FunctionType::FW_UPDATE_MTK_TRANSFER_WITH_AC_CONNECTION_CHECK))
            firmwareType = t1::UpdtInquiredType::FW_UPDATE_MTK_TRANSFER_WITH_AC_CONNECTION_CHECK;
        else if (state.mSupport.contains(t1::FunctionType::FW_UPDATE_USING_MC_APP))
            firmwareType = t1::UpdtInquiredType::FW_UPDATE_USING_MC_APP;
        else if (state.mSupport.contains(t1::FunctionType::FW_UPDATE_TANDEM))
            firmwareType = t1::UpdtInquiredType::FW_UPDATE_TANDEM_PART1;
        if (firmwareType != t1::UpdtInquiredType::OUT_OF_RANGE)
        {
            DISC(t1, UPDT_GET_CAPABILITY, UPDT_RET_CAPABILITY, firmwareType, -1);
            DISC(t1, UPDT_GET_PARAM, UPDT_RET_PARAM, firmwareType, -1);
        }


        // Initial live reload follows InformationHolderListTableSet2.create order.
        if (state.mSupport.contains(t1::FunctionType::CONNECTION_ESTABLISHED_TIME))
        {
            do
            {
                const std::time_t now = std::time(nullptr);
                std::tm utc{};
#ifdef _WIN32
                gmtime_s(&utc, &now);
#else
                gmtime_r(&now, &utc);
#endif
                auto utcAsLocal = utc;
                utcAsLocal.tm_isdst = -1;
                const int offset = static_cast<int>(std::difftime(now, std::mktime(&utcAsLocal))) / 60;
                const auto responseType = ResponseType(MDRDataType::DATA_MDR, static_cast<UInt8>(t1::Command::COMMON_NTFY_PARAM),
                                                       static_cast<UInt8>(t1::CommonInquiredType::CONNECTION_ESTABLISHED_TIME));
                BeginAwait(responseType);
                const auto queryDeadline = std::chrono::steady_clock::now() + std::chrono::seconds(2);
                auto remaining = [&]() { return std::max(0, static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(queryDeadline - std::chrono::steady_clock::now()).count())); };
                const t1::CommonSetParamConnectionEstablishedTime request{
                    .unixTime = static_cast<UInt64>(now), .utcYear = static_cast<Int16>(utc.tm_year + 1900),
                    .utcMonth = static_cast<UInt8>(utc.tm_mon + 1), .utcDay = static_cast<UInt8>(utc.tm_mday),
                    .utcHour = static_cast<UInt8>(utc.tm_hour), .utcMinute = static_cast<UInt8>(utc.tm_min),
                    .utcSecond = static_cast<UInt8>(utc.tm_sec), .timeZoneHour = static_cast<UInt8>(offset / 60),
                    .timeZoneMinute = static_cast<UInt8>(offset % 60)
                };
                int ackResult = MDR_RESULT_ERROR_TIMEOUT;
                for (int retries = 0; retries < mACKRetryCount; ++retries)
                {
                    const int ackTimeout = std::min(mACKRetryTimeoutMS, remaining());
                    if (ackTimeout <= 0) break;
                    const int sendResult = SendCommandImpl(request);
                    if (sendResult != MDR_RESULT_OK)
                    {
                        ackResult = sendResult;
                        break;
                    }
                    ackResult = co_await Await(AWAIT_ACK, ackTimeout);
                    if (ackResult == MDR_RESULT_OK) break;
                }
                const int responseTimeout = remaining();
                if (ackResult != MDR_RESULT_OK || responseTimeout <= 0)
                {
                    CancelAwait(responseType, ackResult != MDR_RESULT_OK ? ackResult : MDR_RESULT_ERROR_TIMEOUT);
                    break;
                }
                responseResult = co_await Await(responseType, responseTimeout);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::FW_UPDATE_MTK_TRANSFER_WITHOUT_DISCONNECTION) || state.mSupport.contains(t1::FunctionType::FW_UPDATE_MTK_TRANSFER_WITH_REPAIR_MODE) || state.mSupport.contains(t1::FunctionType::FW_UPDATE_MTK_TRANSFER_WITH_AC_CONNECTION_CHECK) || state.mSupport.contains(t1::FunctionType::FW_UPDATE_USING_MC_APP) || state.mSupport.contains(t1::FunctionType::FW_UPDATE_TANDEM))
        {
            do
            {
                LIVE(t1, UPDT_GET_PARAM, UPDT_RET_PARAM, firmwareType, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::FW_UPDATE_MTK_TRANSFER_WITHOUT_DISCONNECTION_AUTO_UPDATE))
        {
            do
            {
                LIVE(t1, UPDT_GET_PARAM, UPDT_RET_PARAM, t1::UpdtInquiredType::FW_UPDATE_MTK_TRANSFER_WO_DISCONNECTION_AUTO_UPDATE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT) || state.mSupport.contains(t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE) || state.mSupport.contains(t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
        {
            do
            {
                const auto type = (state.mSupport.contains(t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT) || state.mSupport.contains(t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE)) ? t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE : t2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT;
                LIVE(t2, PERI_GET_STATUS, PERI_RET_STATUS, type, -1);
                LIVE(t2, PERI_GET_PARAM, PERI_RET_PARAM, type, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::SOURCE_SWITCH_CONTROL))
        {
            do
            {
                LIVE(t2, PERI_GET_PARAM, PERI_RET_PARAM, t2::PeripheralInquiredType::SOURCE_SWITCH_CONTROL, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::MUSIC_HAND_OVER_SETTING))
        {
            do
            {
                LIVE(t2, PERI_GET_PARAM, PERI_RET_PARAM, t2::PeripheralInquiredType::MUSIC_HAND_OVER_SETTING, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH))
        {
            do
            {
                if (state.mVoiceGuidanceOnOffSupported[1])
                LIVE(t2, VOICE_GUIDANCE_GET_STATUS, VOICE_GUIDANCE_RET_STATUS, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH, static_cast<int>(t2::VoiceGuidanceStatusType::ON_OFF), static_cast<UInt8>(t2::VoiceGuidanceStatusType::ON_OFF));
                LIVE(t2, VOICE_GUIDANCE_GET_STATUS, VOICE_GUIDANCE_RET_STATUS, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH, static_cast<int>(t2::VoiceGuidanceStatusType::LANGUAGE), static_cast<UInt8>(t2::VoiceGuidanceStatusType::LANGUAGE));
                LIVE(t2, VOICE_GUIDANCE_GET_PARAM, VOICE_GUIDANCE_RET_PARAM, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH, -1);
                LIVE(t2, VOICE_GUIDANCE_GET_EXTENDED_PARAM, VOICE_GUIDANCE_RET_EXTENDED_PARAM, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH))
        {
            do
            {
                if (state.mVoiceGuidanceOnOffSupported[0])
                LIVE(t2, VOICE_GUIDANCE_GET_STATUS, VOICE_GUIDANCE_RET_STATUS, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH, static_cast<int>(t2::VoiceGuidanceStatusType::ON_OFF), static_cast<UInt8>(t2::VoiceGuidanceStatusType::ON_OFF));
                LIVE(t2, VOICE_GUIDANCE_GET_STATUS, VOICE_GUIDANCE_RET_STATUS, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH, static_cast<int>(t2::VoiceGuidanceStatusType::LANGUAGE), static_cast<UInt8>(t2::VoiceGuidanceStatusType::LANGUAGE));
                LIVE(t2, VOICE_GUIDANCE_GET_PARAM, VOICE_GUIDANCE_RET_PARAM, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::VOICE_GUIDANCE_SETTING_ONLY_ON_OFF_SWITCH))
        {
            do
            {
                LIVE(t2, VOICE_GUIDANCE_GET_STATUS, VOICE_GUIDANCE_RET_STATUS, t2::VoiceGuidanceInquiredType::ONLY_ON_OFF_SETTING, -1);
                LIVE(t2, VOICE_GUIDANCE_GET_PARAM, VOICE_GUIDANCE_RET_PARAM, t2::VoiceGuidanceInquiredType::ONLY_ON_OFF_SETTING, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::VOICE_GUIDANCE_SETTING_SUPPORT_LANGUAGE_SWITCH))
        {
            do
            {
                if (state.mVoiceGuidanceOnOffSupported[2])
                LIVE(t2, VOICE_GUIDANCE_GET_STATUS, VOICE_GUIDANCE_RET_STATUS, t2::VoiceGuidanceInquiredType::SUPPORT_LANGUAGE_SWITCH, static_cast<int>(t2::VoiceGuidanceStatusType::ON_OFF), static_cast<UInt8>(t2::VoiceGuidanceStatusType::ON_OFF));
                LIVE(t2, VOICE_GUIDANCE_GET_STATUS, VOICE_GUIDANCE_RET_STATUS, t2::VoiceGuidanceInquiredType::SUPPORT_LANGUAGE_SWITCH, static_cast<int>(t2::VoiceGuidanceStatusType::LANGUAGE), static_cast<UInt8>(t2::VoiceGuidanceStatusType::LANGUAGE));
                LIVE(t2, VOICE_GUIDANCE_GET_PARAM, VOICE_GUIDANCE_RET_PARAM, t2::VoiceGuidanceInquiredType::SUPPORT_LANGUAGE_SWITCH, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT))
        {
            do
            {
                if (state.mVoiceGuidanceOnOffSupported[1])
                LIVE(t2, VOICE_GUIDANCE_GET_STATUS, VOICE_GUIDANCE_RET_STATUS, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH, static_cast<int>(t2::VoiceGuidanceStatusType::ON_OFF), static_cast<UInt8>(t2::VoiceGuidanceStatusType::ON_OFF));
                LIVE(t2, VOICE_GUIDANCE_GET_STATUS, VOICE_GUIDANCE_RET_STATUS, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH, static_cast<int>(t2::VoiceGuidanceStatusType::LANGUAGE), static_cast<UInt8>(t2::VoiceGuidanceStatusType::LANGUAGE));
                LIVE(t2, VOICE_GUIDANCE_GET_PARAM, VOICE_GUIDANCE_RET_PARAM, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH, -1);
                LIVE(t2, VOICE_GUIDANCE_GET_PARAM, VOICE_GUIDANCE_RET_PARAM, t2::VoiceGuidanceInquiredType::VOLUME, -1);
                LIVE(t2, VOICE_GUIDANCE_GET_EXTENDED_PARAM, VOICE_GUIDANCE_RET_EXTENDED_PARAM, t2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::VOICE_GUIDANCE_VOLUME_SETTING_MTK_FIXED_TO_5_STEPS))
        {
            do
            {
                LIVE(t2, VOICE_GUIDANCE_GET_STATUS, VOICE_GUIDANCE_RET_STATUS, t2::VoiceGuidanceInquiredType::VOLUME_SETTING_FIXED_TO_5_STEPS, -1);
                LIVE(t2, VOICE_GUIDANCE_GET_PARAM, VOICE_GUIDANCE_RET_PARAM, t2::VoiceGuidanceInquiredType::VOLUME_SETTING_FIXED_TO_5_STEPS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::VOICE_GUIDANCE_BATTERY_LEVEL_VOICE))
        {
            do
            {
                LIVE(t2, VOICE_GUIDANCE_GET_STATUS, VOICE_GUIDANCE_RET_STATUS, t2::VoiceGuidanceInquiredType::BATTERY_LV_VOICE, -1);
                LIVE(t2, VOICE_GUIDANCE_GET_PARAM, VOICE_GUIDANCE_RET_PARAM, t2::VoiceGuidanceInquiredType::BATTERY_LV_VOICE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::VOICE_GUIDANCE_POWER_ON_OFF_SOUND))
        {
            do
            {
                LIVE(t2, VOICE_GUIDANCE_GET_STATUS, VOICE_GUIDANCE_RET_STATUS, t2::VoiceGuidanceInquiredType::POWER_ONOFF_SOUND, -1);
                LIVE(t2, VOICE_GUIDANCE_GET_PARAM, VOICE_GUIDANCE_RET_PARAM, t2::VoiceGuidanceInquiredType::POWER_ONOFF_SOUND, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::VOICE_GUIDANCE_SOUND_EFFECT_ULT_BEEP_ON_OFF))
        {
            do
            {
                LIVE(t2, VOICE_GUIDANCE_GET_STATUS, VOICE_GUIDANCE_RET_STATUS, t2::VoiceGuidanceInquiredType::SOUNDEFFECT_ULT_BEEP_ONOFF, -1);
                LIVE(t2, VOICE_GUIDANCE_GET_PARAM, VOICE_GUIDANCE_RET_PARAM, t2::VoiceGuidanceInquiredType::SOUNDEFFECT_ULT_BEEP_ONOFF, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::QUICK_ACCESS_CANT_BE_USED_WITH_LEA_CONNECTION))
        {
            do
            {
                LIVE(t1, LEA_GET_STATUS, LEA_RET_STATUS, t1::LEAInquiredType::QUICK_ACCESS_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::LINK_AUTO_SWITCH_FOR_HEADSETS))
        {
            do
            {
                LIVE(t2, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t2::SystemInquiredType::LINK_AUTO_SWITCH_FOR_HEADSETS, -1);
                LIVE(t2, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t2::SystemInquiredType::LINK_AUTO_SWITCH_FOR_HEADSETS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::PAIRING_DEVICE_MANAGEMENT_CANT_BE_USED_WITH_LEA_CONNECTION))
        {
            do
            {
                LIVE(t1, LEA_GET_STATUS, LEA_RET_STATUS, t1::LEAInquiredType::PAIRING_DEVICE_MANAGEMENT_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::SOUND_AR_CANT_BE_USED_WITH_LEA_CONNECTION))
        {
            do
            {
                LIVE(t1, LEA_GET_STATUS, LEA_RET_STATUS, t1::LEAInquiredType::SOUND_AR_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::GATT_CONNECTABLE_CANT_BE_USED_WITH_LEA_CONNECTION))
        {
            do
            {
                LIVE(t1, LEA_GET_STATUS, LEA_RET_STATUS, t1::LEAInquiredType::GATT_CONNECTABLE_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::SOUND_AR_OPTIMIZATION_CANT_BE_USED_WITH_LEA_CONNECTION))
        {
            do
            {
                LIVE(t1, LEA_GET_STATUS, LEA_RET_STATUS, t1::LEAInquiredType::SOUND_AR_OPTIMIZATION_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::HEAD_TRACKER_CANT_BE_USED_WITH_LEA_CONNECTION))
        {
            do
            {
                LIVE(t1, LEA_GET_STATUS, LEA_RET_STATUS, t1::LEAInquiredType::HEAD_TRACKER_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::CONNECTION_MODE_CANT_BE_USED_WITH_LEA_CONNECTION))
        {
            do
            {
                LIVE(t1, LEA_GET_STATUS, LEA_RET_STATUS, t1::LEAInquiredType::CONNECTION_MODE_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::VOICE_ASSISTANT_SETTINGS_CANT_BE_USED_WITH_LEA_CONNECTION))
        {
            do
            {
                LIVE(t1, LEA_GET_STATUS, LEA_RET_STATUS, t1::LEAInquiredType::VOICE_ASSISTANT_SETTINGS_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::VOICE_ASSISTANT_WAKE_WORD_CANT_BE_USED_WITH_LEA_CONNECTION))
        {
            do
            {
                LIVE(t1, LEA_GET_STATUS, LEA_RET_STATUS, t1::LEAInquiredType::VOICE_ASSISTANT_WAKE_WORD_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::BGM_MODE_CANT_BE_USED_WITH_LEA_CONNECTION))
        {
            do
            {
                LIVE(t1, LEA_GET_STATUS, LEA_RET_STATUS, t1::LEAInquiredType::BGM_MODE_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::LINK_AUTO_SWITCH_CANT_BE_USED_WITH_LEA_CONNECTION))
        {
            do
            {
                LIVE(t2, LEA_GET_STATUS, LEA_RET_STATUS, t2::LEAInquiredType::LINK_AUTO_SWITCH_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::QUICK_ACCESS_CANT_BE_USED_WITH_LEA_CONNECTION))
        {
            do
            {
                LIVE(t1, LEA_GET_STATUS, LEA_RET_STATUS, t1::LEAInquiredType::QUICK_ACCESS_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE))
        {
            do
            {
                LIVE(t1, LEA_GET_STATUS, LEA_RET_STATUS, t1::LEAInquiredType::BGM_MODE_CANT_BE_USED_WITH_LEA_CONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.containsEqualizer())
        {
            do
            {
                t1::EqEbbInquiredType type{};
                EqPresetInquiredType(state, type);
                LIVE(t1, EQEBB_GET_STATUS, EQEBB_RET_STATUS, type, -1);
                LIVE(t1, EQEBB_GET_PARAM, EQEBB_RET_PARAM, type, -1);
                LIVE(t1, EQEBB_GET_EXTENDED_INFO, EQEBB_RET_EXTENDED_INFO, type, -1);
                if (!state.mEqCanUseUltMode && state.mEqErrorCodeMode)
                {
                    LIVE(t1, EQEBB_GET_STATUS, EQEBB_RET_STATUS, type, -1);
                    if (state.mEqErrorCodeCount != 0)
                        LIVE(t1, EQEBB_GET_STATUS, EQEBB_RET_STATUS, type, -1);
                }
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::EBB))
        {
            do
            {
                LIVE(t1, EQEBB_GET_STATUS, EQEBB_RET_STATUS, t1::EqEbbInquiredType::EBB, -1);
                LIVE(t1, EQEBB_GET_PARAM, EQEBB_RET_PARAM, t1::EqEbbInquiredType::EBB, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::SOUND_EFFECT))
        {
            do
            {
                LIVE(t1, EQEBB_GET_STATUS, EQEBB_RET_STATUS, t1::EqEbbInquiredType::SOUND_EFFECT, -1);
                LIVE(t1, EQEBB_GET_PARAM, EQEBB_RET_PARAM, t1::EqEbbInquiredType::SOUND_EFFECT, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::CUSTOMIZABLE_SOUND_EFFECT))
        {
            do
            {
                LIVE(t1, EQEBB_GET_PARAM, EQEBB_RET_PARAM, t1::EqEbbInquiredType::CUSTOMIZABLE_SOUND_EFFECT_SELECT, -1);
                LIVE(t1, EQEBB_GET_EXTENDED_INFO, EQEBB_RET_EXTENDED_INFO, t1::EqEbbInquiredType::CUSTOMIZABLE_SOUND_EFFECT_SELECT, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::CUSTOM_EQ))
        {
            do
            {
                LIVE(t1, EQEBB_GET_STATUS, EQEBB_RET_STATUS, t1::EqEbbInquiredType::CUSTOM_EQ, -1);
                LIVE(t1, EQEBB_GET_PARAM, EQEBB_RET_PARAM, t1::EqEbbInquiredType::CUSTOM_EQ, -1);
                LIVE(t1, EQEBB_GET_EXTENDED_INFO, EQEBB_RET_EXTENDED_INFO, t1::EqEbbInquiredType::CUSTOM_EQ, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::ULT_SOUND_EFFECT_ASSIGN))
        {
            do
            {
                LIVE(t1, EQEBB_GET_PARAM, EQEBB_RET_PARAM, t1::EqEbbInquiredType::ULT_BTN_SOUND_EFFECT_ASSIGN, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING_ONOFF))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::NC_ON_OFF, -1);
                LIVE(t1, NCASM_GET_PARAM, NCASM_RET_PARAM, t1::NcAsmInquiredType::NC_ON_OFF, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_ONOFF))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::NC_ON_OFF_AND_ASM_ON_OFF, -1);
                LIVE(t1, NCASM_GET_PARAM, NCASM_RET_PARAM, t1::NcAsmInquiredType::NC_ON_OFF_AND_ASM_ON_OFF, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::NC_ON_OFF_AND_ASM_SEAMLESS, -1);
                LIVE(t1, NCASM_GET_PARAM, NCASM_RET_PARAM, t1::NcAsmInquiredType::NC_ON_OFF_AND_ASM_SEAMLESS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING_DUAL_SINGLE_OFF_AND_AMBIENT_SOUND_MODE_ONOFF))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::NC_MODE_SWITCH_AND_ASM_ON_OFF, -1);
                LIVE(t1, NCASM_GET_PARAM, NCASM_RET_PARAM, t1::NcAsmInquiredType::NC_MODE_SWITCH_AND_ASM_ON_OFF, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::NOISE_CANCELLING_DUAL_SINGLE_OFF_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
                LIVE(t1, NCASM_GET_PARAM, NCASM_RET_PARAM, t1::NcAsmInquiredType::NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AUTO_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::MODE_NC_ASM_AUTO_NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
                LIVE(t1, NCASM_GET_PARAM, NCASM_RET_PARAM, t1::NcAsmInquiredType::MODE_NC_ASM_AUTO_NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_SINGLE_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_SINGLE_NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
                LIVE(t1, NCASM_GET_PARAM, NCASM_RET_PARAM, t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_SINGLE_NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
                LIVE(t1, NCASM_GET_PARAM, NCASM_RET_PARAM, t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::MODE_NC_NCSS_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_WITH_TEST_MODE))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::MODE_NC_NCSS_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
                LIVE(t1, NCASM_GET_PARAM, NCASM_RET_PARAM, t1::NcAsmInquiredType::MODE_NC_NCSS_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::MODE_NC_NCSS_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_WITH_TEST_MODE))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::NC_TEST_MODE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA, -1);
                LIVE(t1, NCASM_GET_PARAM, NCASM_RET_PARAM, t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_CONTROL_MODE_SELECT))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::NC_AMB_TOGGLE, -1);
                LIVE(t1, NCASM_GET_PARAM, NCASM_RET_PARAM, t1::NcAsmInquiredType::NC_AMB_TOGGLE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_MODE_ONOFF))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::ASM_ON_OFF, -1);
                LIVE(t1, NCASM_GET_PARAM, NCASM_RET_PARAM, t1::NcAsmInquiredType::ASM_ON_OFF, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            do
            {
                LIVE(t1, NCASM_GET_STATUS, NCASM_RET_STATUS, t1::NcAsmInquiredType::ASM_SEAMLESS, -1);
                LIVE(t1, NCASM_GET_PARAM, NCASM_RET_PARAM, t1::NcAsmInquiredType::ASM_SEAMLESS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::NC_OPTIMIZER_PERSONAL_BAROMETRIC) || state.mSupport.contains(t1::FunctionType::NC_OPTIMIZER_PERSONAL) || state.mSupport.contains(t1::FunctionType::NC_OPTIMIZER_BAROMETRIC))
        {
            do
            {
                const auto type = state.mSupport.contains(t1::FunctionType::NC_OPTIMIZER_BAROMETRIC) ? t1::OptInquiredType::NC_OPTIMIZER_BAROMETRIC : state.mSupport.contains(t1::FunctionType::NC_OPTIMIZER_PERSONAL) ? t1::OptInquiredType::NC_OPTIMIZER_PERSONAL : t1::OptInquiredType::NC_OPTIMIZER_PERSONAL_BAROMETRIC;
                LIVE(t1, OPT_GET_STATUS, OPT_RET_STATUS, type, -1);
                LIVE(t1, OPT_GET_PARAM, OPT_RET_PARAM, type, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::TV_SOUND_BOOSTER))
        {
            do
            {
                LIVE(t1, OPT_GET_STATUS, OPT_RET_STATUS, t1::OptInquiredType::TV_SOUND_BOOSTER, -1);
                LIVE(t1, OPT_GET_PARAM, OPT_RET_PARAM, t1::OptInquiredType::TV_SOUND_BOOSTER, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT))
        {
            do
            {
                LIVE(t1, PLAY_GET_STATUS, PLAY_RET_STATUS, t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT, -1);
                LIVE(t1, PLAY_GET_PARAM, PLAY_RET_PARAM, t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT, -1);
                LIVE(t1, PLAY_GET_PARAM, PLAY_RET_PARAM, t1::PlayInquiredType::MUSIC_VOLUME, -1);
                LIVE(t1, PLAY_GET_PARAM, PLAY_RET_PARAM, t1::PlayInquiredType::CALL_VOLUME, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE))
        {
            do
            {
                LIVE(t1, PLAY_GET_STATUS, PLAY_RET_STATUS, t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE, -1);
                LIVE(t1, PLAY_GET_PARAM, PLAY_RET_PARAM, t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT_AND_FUNCTION_CHANGE, -1);
                LIVE(t1, PLAY_GET_PARAM, PLAY_RET_PARAM, t1::PlayInquiredType::MUSIC_VOLUME, -1);
                LIVE(t1, PLAY_GET_PARAM, PLAY_RET_PARAM, t1::PlayInquiredType::CALL_VOLUME, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT_AND_MUTE))
        {
            do
            {
                LIVE(t1, PLAY_GET_STATUS, PLAY_RET_STATUS, t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT, -1);
                LIVE(t1, PLAY_GET_PARAM, PLAY_RET_PARAM, t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT, -1);
                LIVE(t1, PLAY_GET_PARAM, PLAY_RET_PARAM, t1::PlayInquiredType::MUSIC_VOLUME_WITH_MUTE, -1);
                LIVE(t1, PLAY_GET_PARAM, PLAY_RET_PARAM, t1::PlayInquiredType::CALL_VOLUME_WITH_MUTE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::PLAYBACK_CONTROLLER_WITH_FUNCTION_CHANGE))
        {
            do
            {
                LIVE(t1, PLAY_GET_STATUS, PLAY_RET_STATUS, t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE, -1);
                LIVE(t1, PLAY_GET_PARAM, PLAY_RET_PARAM, t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_FUNCTION_CHANGE, -1);
                LIVE(t1, PLAY_GET_PARAM, PLAY_RET_PARAM, t1::PlayInquiredType::MUSIC_VOLUME, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::GATT_CONNECTABLE))
        {
            do
            {
                LIVE(t1, SAR_AUTO_PLAY_GET_STATUS, SAR_AUTO_PLAY_RET_STATUS, t1::SARAutoPlayInquiredType::GATT_CONNECTABLE, -1);
                LIVE(t1, SAR_AUTO_PLAY_GET_PARAM, SAR_AUTO_PLAY_RET_PARAM, t1::SARAutoPlayInquiredType::GATT_CONNECTABLE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::SAR_OPTIMIZATION_COMPASS_ACCEL_TYPE))
        {
            do
            {
                LIVE(t1, SAR_AUTO_PLAY_GET_STATUS, SAR_AUTO_PLAY_RET_STATUS, t1::SARAutoPlayInquiredType::SAR_OPTIMIZATION_COMPASS_ACCEL_TYPE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::SAR_OPTIMIZATION_ACCEL_TYPE))
        {
            do
            {
                LIVE(t1, SAR_AUTO_PLAY_GET_STATUS, SAR_AUTO_PLAY_RET_STATUS, t1::SARAutoPlayInquiredType::SAR_OPTIMIZATION_ACCEL_TYPE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::HEAD_TRACKER_COMPASS_ACCEL_TYPE))
        {
            do
            {
                LIVE(t1, SAR_AUTO_PLAY_GET_STATUS, SAR_AUTO_PLAY_RET_STATUS, t1::SARAutoPlayInquiredType::SAR_OPTIMIZATION_COMPASS_ACCEL_TYPE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::HEAD_TRACKER_ACCEL_TYPE))
        {
            do
            {
                LIVE(t1, SAR_AUTO_PLAY_GET_STATUS, SAR_AUTO_PLAY_RET_STATUS, t1::SARAutoPlayInquiredType::SAR_OPTIMIZATION_ACCEL_TYPE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
        {
            do
            {
                LIVE(t1, AUDIO_GET_STATUS, AUDIO_RET_STATUS, t1::AudioInquiredType::CONNECTION_MODE, -1);
                LIVE(t1, AUDIO_GET_PARAM, AUDIO_RET_PARAM, t1::AudioInquiredType::CONNECTION_MODE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::CONNECTION_MODE_SOUND_QUALITY_SOUND_WITH_LDAC_STATUS_QUALITY_CONNECTION_QUALITY))
        {
            do
            {
                LIVE(t1, AUDIO_GET_STATUS, AUDIO_RET_STATUS, t1::AudioInquiredType::CONNECTION_MODE_WITH_LDAC_STATUS, -1);
                LIVE(t1, AUDIO_GET_PARAM, AUDIO_RET_PARAM, t1::AudioInquiredType::CONNECTION_MODE_WITH_LDAC_STATUS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::UPSCALING_AUTO_OFF) || state.mSupport.contains(t1::FunctionType::UPSCALING_AUTO_OFF_WITH_STATUS_DISABLE_REASON))
        {
            do
            {
                const auto type = state.mSupport.contains(t1::FunctionType::UPSCALING_AUTO_OFF) ? t1::AudioInquiredType::UPSCALING : t1::AudioInquiredType::UPSCALING_AUTO_OFF_WITH_STATUS_DISABLE_REASON;
                LIVE(t1, AUDIO_GET_STATUS, AUDIO_RET_STATUS, type, -1);
                LIVE(t1, AUDIO_GET_PARAM, AUDIO_RET_PARAM, type, -1);
            } while (false);
        }
        if (state.mSupport.containsBGMMode())
        {
            do
            {
                const auto type = BGMInquiredType(state);
                LIVE(t1, AUDIO_GET_PARAM, AUDIO_RET_PARAM, type, -1);
                LIVE(t1, AUDIO_GET_STATUS, AUDIO_RET_STATUS, type, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::UPMIX_CINEMA))
        {
            do
            {
                LIVE(t1, AUDIO_GET_PARAM, AUDIO_RET_PARAM, t1::AudioInquiredType::UPMIX_CINEMA, -1);
                LIVE(t1, AUDIO_GET_STATUS, AUDIO_RET_STATUS, t1::AudioInquiredType::UPMIX_CINEMA, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO))
        {
            do
            {
                LIVE(t1, AUDIO_GET_STATUS, AUDIO_RET_STATUS, t1::AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO, -1);
                LIVE(t1, AUDIO_GET_PARAM, AUDIO_RET_PARAM, t1::AudioInquiredType::CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::VOICE_CONTENTS))
        {
            do
            {
                LIVE(t1, AUDIO_GET_PARAM, AUDIO_RET_PARAM, t1::AudioInquiredType::VOICE_CONTENTS, -1);
                LIVE(t1, AUDIO_GET_STATUS, AUDIO_RET_STATUS, t1::AudioInquiredType::VOICE_CONTENTS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::SOUND_LEAKAGE_REDUCTION))
        {
            do
            {
                LIVE(t1, AUDIO_GET_PARAM, AUDIO_RET_PARAM, t1::AudioInquiredType::SOUND_LEAKAGE_REDUCTION, -1);
                LIVE(t1, AUDIO_GET_STATUS, AUDIO_RET_STATUS, t1::AudioInquiredType::SOUND_LEAKAGE_REDUCTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::UPMIX_SERIES))
        {
            do
            {
                LIVE(t1, AUDIO_GET_PARAM, AUDIO_RET_PARAM, t1::AudioInquiredType::UPMIX_SERIES, -1);
                LIVE(t1, AUDIO_GET_STATUS, AUDIO_RET_STATUS, t1::AudioInquiredType::UPMIX_SERIES, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::LISTENING_OPTION_ASSIGN_CUSTOMIZABLE))
        {
            do
            {
                LIVE(t1, AUDIO_GET_PARAM, AUDIO_RET_PARAM, t1::AudioInquiredType::LISTENING_OPTION_ASSIGN_CUSTOMIZABLE, -1);
                LIVE(t1, AUDIO_GET_STATUS, AUDIO_RET_STATUS, t1::AudioInquiredType::LISTENING_OPTION_ASSIGN_CUSTOMIZABLE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::VIBRATOR_ON_OFF))
        {
            do
            {
                LIVE(t1, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t1::SystemInquiredType::VIBRATOR, -1);
                LIVE(t1, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t1::SystemInquiredType::VIBRATOR, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::AUTO_VOLUME))
        {
            do
            {
                LIVE(t1, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t1::SystemInquiredType::AUTO_VOLUME, -1);
                LIVE(t1, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t1::SystemInquiredType::AUTO_VOLUME, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::FACE_TAP_TEST_MODE))
        {
            do
            {
                LIVE(t1, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t1::SystemInquiredType::FACE_TAP_TEST_MODE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::POWER_SAVING_MODE_ON_OFF))
        {
            do
            {
                LIVE(t1, POWER_GET_STATUS, POWER_RET_STATUS, t1::PowerInquiredType::POWER_SAVE_MODE, -1);
                LIVE(t1, POWER_GET_PARAM, POWER_RET_PARAM, t1::PowerInquiredType::POWER_SAVE_MODE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF))
        {
            do
            {
                LIVE(t1, POWER_GET_STATUS, POWER_RET_STATUS, t1::PowerInquiredType::AUTO_POWER_OFF, -1);
                LIVE(t1, POWER_GET_PARAM, POWER_RET_PARAM, t1::PowerInquiredType::AUTO_POWER_OFF, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
        {
            do
            {
                LIVE(t1, POWER_GET_STATUS, POWER_RET_STATUS, t1::PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION, -1);
                LIVE(t1, POWER_GET_PARAM, POWER_RET_PARAM, t1::PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::BATTERY_SAFE_MODE))
        {
            do
            {
                LIVE(t1, POWER_GET_PARAM, POWER_RET_PARAM, t1::PowerInquiredType::BATTERY_SAFE_MODE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::CARING_CHARGE))
        {
            do
            {
                LIVE(t1, POWER_GET_STATUS, POWER_RET_STATUS, t1::PowerInquiredType::CARING_CHARGE, -1);
                LIVE(t1, POWER_GET_PARAM, POWER_RET_PARAM, t1::PowerInquiredType::CARING_CHARGE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::CARING_CHARGE_WITH_THRESHOLD))
        {
            do
            {
                LIVE(t2, POWER_GET_STATUS, POWER_RET_STATUS, t2::PowerInquiredType::CARING_CHARGE_WITH_THRESHOLD, -1);
                LIVE(t2, POWER_GET_PARAM, POWER_RET_PARAM, t2::PowerInquiredType::CARING_CHARGE_WITH_THRESHOLD, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::AUTO_STANDBY))
        {
            do
            {
                LIVE(t2, POWER_GET_STATUS, POWER_RET_STATUS, t2::PowerInquiredType::AUTO_STANDBY, -1);
                LIVE(t2, POWER_GET_PARAM, POWER_RET_PARAM, t2::PowerInquiredType::AUTO_STANDBY, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE_TYPE1))
        {
            do
            {
                LIVE(t1, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t1::SystemInquiredType::SMART_TALKING_MODE_TYPE1, -1);
                LIVE(t1, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t1::SystemInquiredType::SMART_TALKING_MODE_TYPE1, -1);
                LIVE(t1, SYSTEM_GET_EXT_PARAM, SYSTEM_RET_EXT_PARAM, t1::SystemInquiredType::SMART_TALKING_MODE_TYPE1, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::SMART_TALKING_MODE_TYPE2))
        {
            do
            {
                LIVE(t1, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2, -1);
                LIVE(t1, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2, -1);
                LIVE(t1, SYSTEM_GET_EXT_PARAM, SYSTEM_RET_EXT_PARAM, t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::ASSIGNABLE_SETTING))
        {
            do
            {
                LIVE(t1, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t1::SystemInquiredType::ASSIGNABLE_SETTINGS, -1);
                LIVE(t1, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t1::SystemInquiredType::ASSIGNABLE_SETTINGS, -1);
                if (state.mAssignableHasCustomizableActions[0])
                LIVE(t1, SYSTEM_GET_EXT_PARAM, SYSTEM_RET_EXT_PARAM, t1::SystemInquiredType::ASSIGNABLE_SETTINGS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::VOICE_ASSISTANT_SETTINGS))
        {
            do
            {
                LIVE(t1, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t1::SystemInquiredType::VOICE_ASSISTANT_SETTINGS, -1);
                LIVE(t1, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t1::SystemInquiredType::VOICE_ASSISTANT_SETTINGS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::WEARING_STATUS_DETECTOR))
        {
            do
            {
                LIVE(t1, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t1::SystemInquiredType::WEARING_STATUS_DETECTOR, -1);
                LIVE(t1, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t1::SystemInquiredType::WEARING_STATUS_DETECTOR, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::EARPIECE_SELECTION))
        {
            do
            {
                LIVE(t1, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t1::SystemInquiredType::EARPIECE_SELECTION, -1);
                LIVE(t1, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t1::SystemInquiredType::EARPIECE_SELECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::QUICK_ACCESS))
        {
            do
            {
                LIVE(t1, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t1::SystemInquiredType::QUICK_ACCESS, -1);
                LIVE(t1, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t1::SystemInquiredType::QUICK_ACCESS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::ASSIGNABLE_SETTING_WITH_LIMITATION))
        {
            do
            {
                LIVE(t1, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t1::SystemInquiredType::ASSIGNABLE_SETTINGS_WITH_LIMITATION, -1);
                LIVE(t1, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t1::SystemInquiredType::ASSIGNABLE_SETTINGS_WITH_LIMITATION, -1);
                if (state.mAssignableHasCustomizableActions[1])
                LIVE(t1, SYSTEM_GET_EXT_PARAM, SYSTEM_RET_EXT_PARAM, t1::SystemInquiredType::ASSIGNABLE_SETTINGS_WITH_LIMITATION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::HEAD_GESTURE_ON_OFF_TRAINING))
        {
            do
            {
                LIVE(t1, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t1::SystemInquiredType::HEAD_GESTURE_ON_OFF, -1);
                LIVE(t1, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t1::SystemInquiredType::HEAD_GESTURE_ON_OFF, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::HEAD_GESTURE_ON_OFF_TRAINING))
        {
            do
            {
                LIVE(t1, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t1::SystemInquiredType::HEAD_GESTURE_TRAINING, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::CONNECTION_STATUS))
        {
            do
            {
                LIVE(t1, COMMON_GET_STATUS, COMMON_RET_STATUS, t1::CommonInquiredType::CONNECTION_STATUS, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::BATTERY_LEVEL_INDICATOR))
        {
            do
            {
                LIVE(t1, POWER_GET_STATUS, POWER_RET_STATUS, t1::PowerInquiredType::BATTERY, -1);
            } while (false);
        }
        else if (state.mSupport.contains(t1::FunctionType::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR))
        {
            do
            {
                LIVE(t1, POWER_GET_STATUS, POWER_RET_STATUS, t1::PowerInquiredType::LEFT_RIGHT_BATTERY, -1);
            } while (false);
        }
        else if (state.mSupport.contains(t1::FunctionType::BATTERY_LEVEL_WITH_THRESHOLD))
        {
            do
            {
                LIVE(t1, POWER_GET_STATUS, POWER_RET_STATUS, t1::PowerInquiredType::BATTERY_WITH_THRESHOLD, -1);
            } while (false);
        }
        else if (state.mSupport.contains(t1::FunctionType::LR_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            do
            {
                LIVE(t1, POWER_GET_STATUS, POWER_RET_STATUS, t1::PowerInquiredType::LR_BATTERY_WITH_THRESHOLD, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::CRADLE_BATTERY_LEVEL_INDICATOR))
        {
            do
            {
                LIVE(t1, POWER_GET_STATUS, POWER_RET_STATUS, t1::PowerInquiredType::CRADLE_BATTERY, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            do
            {
                LIVE(t1, POWER_GET_STATUS, POWER_RET_STATUS, t1::PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::UPSCALING_INDICATOR))
        {
            do
            {
                LIVE(t1, COMMON_GET_STATUS, COMMON_RET_STATUS, t1::CommonInquiredType::UPSCALING_EFFECT, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::CODEC_INDICATOR))
        {
            do
            {
                LIVE(t1, COMMON_GET_STATUS, COMMON_RET_STATUS, t1::CommonInquiredType::AUDIO_CODEC, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::DEVICE_SPECIAL_MODE))
        {
            do
            {
                LIVE(t1, COMMON_GET_STATUS, COMMON_RET_STATUS, t1::CommonInquiredType::DEVICE_SPECIAL_MODE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING_1))
        {
            do
            {
                LIVE(t1, GENERAL_SETTING_GET_STATUS, GENERAL_SETTING_RET_STATUS, t1::GsInquiredType::GENERAL_SETTING1, -1);
                LIVE(t1, GENERAL_SETTING_GET_PARAM, GENERAL_SETTING_RET_PARAM, t1::GsInquiredType::GENERAL_SETTING1, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING_2))
        {
            do
            {
                LIVE(t1, GENERAL_SETTING_GET_STATUS, GENERAL_SETTING_RET_STATUS, t1::GsInquiredType::GENERAL_SETTING2, -1);
                LIVE(t1, GENERAL_SETTING_GET_PARAM, GENERAL_SETTING_RET_PARAM, t1::GsInquiredType::GENERAL_SETTING2, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING_3))
        {
            do
            {
                LIVE(t1, GENERAL_SETTING_GET_STATUS, GENERAL_SETTING_RET_STATUS, t1::GsInquiredType::GENERAL_SETTING3, -1);
                LIVE(t1, GENERAL_SETTING_GET_PARAM, GENERAL_SETTING_RET_PARAM, t1::GsInquiredType::GENERAL_SETTING3, -1);
            } while (false);
        }
        if (state.mSupport.contains(t1::FunctionType::GENERAL_SETTING_4))
        {
            do
            {
                LIVE(t1, GENERAL_SETTING_GET_STATUS, GENERAL_SETTING_RET_STATUS, t1::GsInquiredType::GENERAL_SETTING4, -1);
                LIVE(t1, GENERAL_SETTING_GET_PARAM, GENERAL_SETTING_RET_PARAM, t1::GsInquiredType::GENERAL_SETTING4, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_HBS_1) || state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_1) || state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_HBS_2) || state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_2))
        {
            do
            {
                const auto type = state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_2) ? t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_2 : state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_HBS_2) ? t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_2 : state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_1) ? t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_1 : t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_1;
                LIVE(t2, SAFE_LISTENING_GET_PARAM, SAFE_LISTENING_RET_PARAM, type, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::MAX_VOLUME_LEVEL_LIMIT))
        {
            do
            {
                LIVE(t2, SAFE_LISTENING_GET_STATUS, SAFE_LISTENING_RET_STATUS, t2::SafeListeningInquiredType::MAX_VOL_LV_LIMIT, -1);
                LIVE(t2, SAFE_LISTENING_GET_PARAM, SAFE_LISTENING_RET_PARAM, t2::SafeListeningInquiredType::MAX_VOL_LV_LIMIT, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::LE_AUDIO_CONNECTION_MODE))
        {
            do
            {
                LIVE(t2, LEA_GET_STATUS, LEA_RET_STATUS, t2::LEAInquiredType::LE_AUDIO_CONNECTION_MODE_WITH_BT_RECONNECTION, -1);
                LIVE(t2, LEA_GET_PARAM, LEA_RET_PARAM, t2::LEAInquiredType::LE_AUDIO_CONNECTION_MODE_WITH_BT_RECONNECTION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::DJ_CONTROL) || state.mSupport.contains(t2::FunctionType::DJ_CONTROL_WITH_STATUS_DISABLE_REASON))
        {
            do
            {
                const auto type = state.mSupport.contains(t2::FunctionType::DJ_CONTROL) ? t2::PartyInquiredType::DJ_CONTROL : t2::PartyInquiredType::DJ_CONTROL_WITH_STATUS_DISABLE_REASON;
                LIVE(t2, PARTY_GET_STATUS, PARTY_RET_STATUS, type, -1);
                LIVE(t2, PARTY_GET_PARAM, PARTY_RET_PARAM, type, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::WEARING_STATUS_CHECKER))
        {
            do
            {
                LIVE(t2, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t2::SystemInquiredType::WEARING_STATUS_CHECKER, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::REPEAT_TAP_TRAINING_MODE))
        {
            do
            {
                LIVE(t2, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t2::SystemInquiredType::REPEAT_TAP_TRAINING_MODE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::WEARING_POSITION) || state.mSupport.contains(t2::FunctionType::WEARING_POSITION_WITHOUT_FITTING_SUPPORTER))
        {
            do
            {
                LIVE(t2, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t2::SystemInquiredType::WEARING_POSITION_JUDGMENT_BY_SENSOR, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::AUTO_VOLUME_WITH_LIMITATION))
        {
            do
            {
                LIVE(t2, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t2::SystemInquiredType::AUTO_VOLUME_WITH_LIMITATION, -1);
                LIVE(t2, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t2::SystemInquiredType::AUTO_VOLUME_WITH_LIMITATION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::SONY_VOICE_ASSISTANT))
        {
            do
            {
                LIVE(t2, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t2::SystemInquiredType::SONY_VOICE_ASSISTANT_SETTING_MTK_TRANSFER_SUPPORT_LANGUAGE_SWITCH, -1);
                LIVE(t2, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t2::SystemInquiredType::SONY_VOICE_ASSISTANT_SETTING_MTK_TRANSFER_SUPPORT_LANGUAGE_SWITCH, -1);
                LIVE(t2, SYSTEM_GET_EXTENDED_PARAM, SYSTEM_RET_EXTENDED_PARAM, t2::SystemInquiredType::SONY_VOICE_ASSISTANT_SETTING_MTK_TRANSFER_SUPPORT_LANGUAGE_SWITCH, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::LINK_AUTO_SWITCH_FOR_SPEAKER))
        {
            do
            {
                LIVE(t2, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t2::SystemInquiredType::LINK_AUTO_SWITCH_FOR_SPEAKER, -1);
                LIVE(t2, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t2::SystemInquiredType::LINK_AUTO_SWITCH_FOR_SPEAKER, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::ILLUMINATION))
        {
            do
            {
                LIVE(t2, PARTY_GET_STATUS, PARTY_RET_STATUS, t2::PartyInquiredType::ILLUMINATION, -1);
                LIVE(t2, PARTY_GET_PARAM, PARTY_RET_PARAM, t2::PartyInquiredType::ILLUMINATION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::FUNCTION_CHANGE))
        {
            do
            {
                LIVE(t2, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t2::SystemInquiredType::FUNCTION_CHANGE, -1);
                LIVE(t2, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t2::SystemInquiredType::FUNCTION_CHANGE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::USB_BROWSER))
        {
            do
            {
                LIVE(t1, PLAY_GET_STATUS, PLAY_RET_STATUS, t1::PlayInquiredType::PLAY_MODE, -1);
                LIVE(t1, PLAY_GET_PARAM, PLAY_RET_PARAM, t1::PlayInquiredType::PLAY_MODE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::LIGHTING_MODE))
        {
            do
            {
                LIVE(t2, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t2::SystemInquiredType::LIGHTING_MODE, -1);
                LIVE(t2, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t2::SystemInquiredType::LIGHTING_MODE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::LIGHTING_DEFAULT_COLOR))
        {
            do
            {
                LIVE(t2, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t2::SystemInquiredType::LIGHTING_DEFAULT_COLOR_COLOR_TYPE, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::LIGHTING_DEFAULT_COLOR))
        {
            do
            {
                LIVE(t2, SYSTEM_GET_PARAM, SYSTEM_RET_PARAM, t2::SystemInquiredType::LIGHTING_DEFAULT_COLOR_CUSTOM_COLOR, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::USB_BROWSER))
        {
            do
            {
                LIVE(t2, SYSTEM_GET_STATUS, SYSTEM_RET_STATUS, t2::SystemInquiredType::USB_BROWSER, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::USB_SUBMERSION))
        {
            do
            {
                LIVE(t2, POWER_GET_STATUS, POWER_RET_STATUS, t2::PowerInquiredType::USB_SUBMERSION, -1);
            } while (false);
        }
        if (state.mSupport.contains(t2::FunctionType::KARAOKE) || state.mSupport.contains(t2::FunctionType::KARAOKE_WITH_STATUS_DISABLE_REASON))
        {
            do
            {
                const auto type = state.mSupport.contains(t2::FunctionType::KARAOKE) ? t2::PartyInquiredType::KARAOKE : t2::PartyInquiredType::KARAOKE_WITH_STATUS_DISABLE_REASON;
                LIVE(t2, PARTY_GET_STATUS, PARTY_RET_STATUS, type, -1);
                LIVE(t2, PARTY_GET_PARAM, PARTY_RET_PARAM, type, -1);
                LIVE(t2, PARTY_GET_PARAM, PARTY_RET_PARAM, t2::PartyInquiredType::LIVE_KARAOKE, -1);
            } while (false);
        }
        mInitialized = true;
        co_return MDR_EVENT_INITIALIZE_COMPLETE;
    }
#undef LIVE
#undef DISC
#undef V2_QUERY

    MDRTask MDRHeadphones::RequestSyncV2()
    {
        auto& state = mDetailsV2;
        /* Single Battery */
        if (state.mSupport.contains(t1::FunctionType::BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::BATTERY});
        }
        else if (state.mSupport.contains(t1::FunctionType::BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::BATTERY_WITH_THRESHOLD});
        }

        /* L + R Battery */
        if (state.mSupport.contains(t1::FunctionType::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::LEFT_RIGHT_BATTERY});
        }
        else if (state.mSupport.contains(t1::FunctionType::LR_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::LR_BATTERY_WITH_THRESHOLD});
        }

        /* Case Battery */
        if (state.mSupport.contains(t1::FunctionType::CRADLE_BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::CRADLE_BATTERY});
        }
        else if (state.mSupport.contains(t1::FunctionType::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(t1::PowerGetStatus, {.type = t1::PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD});
        }

        /* Sound Pressure */
        if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_HBS_1))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.inquiredType = t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_1});
        }
        else if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_HBS_2))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.inquiredType = t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_2});
        }
        else if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_1))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.inquiredType = t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_1});
        }
        else if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_2))
        {
            SendCommandACK(t2::SafeListeningGetExtendedParam,
                           {.inquiredType = t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_2});
        }
        /* Playback metadata. Track changes are not always notified. */
        if (state.mSupport.containsPlaybackController())
        {
            SendCommandACK(t1::GetPlayParam,
                           { .type = PlaybackInquiredType(state) });
            SendCommandACK(t1::GetPlayParam, { .type = MusicVolumeInquiredType(state) });
            SendCommandACK(t1::GetPlayStatus,
                           { .type = PlaybackInquiredType(state) });
        }

        /* Wearing status (proximity sensor) */
        if (state.wearingSensorPossible())
            SendCommandACK(t2::SystemGetStatus, {.type = t2::SystemInquiredType::WEARING_STATUS_CHECKER});

        co_return MDR_EVENT_SYNC_COMPLETE;
    }

    void MDRHeadphones::SnapshotPropertiesV2()
    {
        auto& state = mDetailsV2;
        state.mShutdown.submit();
        state.mNcAsmEnabled.submit();
        state.mNcAsmFocusOnVoice.submit();
        state.mNcAsmAmbientLevel.submit();
        state.mNcAsmChangingAsmLevel.submit();
        state.mNcAsmButtonFunction.submit();
        state.mNcAsmMode.submit();
        state.mNcAsmAutoAsmEnabled.submit();
        state.mNcAsmNoiseAdaptiveSensitivity.submit();
        state.mPowerAutoOff.submit();
        state.mPowerAutoOffWearingDetection.submit();
        state.mPlayVolume.submit();
        state.mPlayControl.submit();
        state.mGsParamBool[0].submit();
        state.mGsParamBool[1].submit();
        state.mGsParamBool[2].submit();
        state.mGsParamBool[3].submit();
        state.mUpscalingEnabled.submit();
        state.mAudioPriorityMode.submit();
        state.mBGMModeEnabled.submit();
        state.mBGMModeRoomSize.submit();
        state.mUpmixCinemaEnabled.submit();
        state.mVoiceContentsEnabled.submit();
        state.mSoundLeakageReductionEnabled.submit();
        state.mAutoPauseEnabled.submit();
        state.mAssignableSettingsPresets.submit();
        state.mSpeakToChatEnabled.submit();
        state.mSpeakToChatDetectSensitivity.submit();
        state.mSpeakToModeOutTime.submit();
        state.mHeadGestureEnabled.submit();
        state.mEqAvailable.submit();
        state.mEqPresetId.submit();
        state.mEqClearBass.submit();
        state.mEqConfig.submit();
        state.mVoiceGuidanceEnabled.submit();
        state.mVoiceGuidanceVolume.submit();
        state.mPairingMode.submit();
        state.mMultipointDeviceMac.submit();
        state.mSourceSwitchControlEnabled.submit();
        state.mPairedDeviceDisconnectMac.submit();
        state.mPairedDeviceConnectMac.submit();
        state.mPairedDeviceUnpairMac.submit();
        state.mSafeListeningPreviewMode.submit();
    }

    MDRTask MDRHeadphones::RequestCommitV2()
    {
        auto& state = mDetailsV2;
        SnapshotPropertiesV2();

        /* Shutdown */
        if (state.mShutdown.pending())
        {
            using namespace t1;
            if (state.mSupport.contains(FunctionType::POWER_OFF) && state.mShutdown.submitted)
            {
                SendCommandACK(PowerSetStatusPowerOff);
                state.mShutdown.override(false);
            }
            else
                state.mShutdown.override(false);
        }
        /* NC/ASM */
        if (state.mNcAsmAmbientLevel.pending() || state.mNcAsmChangingAsmLevel.pending() ||
            state.mNcAsmEnabled.pending() || state.mNcAsmMode.pending() || state.mNcAsmFocusOnVoice.pending() ||
            state.mNcAsmAutoAsmEnabled.pending() || state.mNcAsmNoiseAdaptiveSensitivity.pending())
        {
            using namespace t1;
            if (state.mSupport.contains(
                FunctionType::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
            {
                NcAsmSetParamModeNcDualModeSwitchAsmSeamlessNa res;
                res.command = Command::NCASM_SET_PARAM;
                res.valueChangeStatus = state.mNcAsmChangingAsmLevel.submitted
                    ? ValueChangeStatus::UNDER_CHANGING : ValueChangeStatus::CHANGED;
                res.ncAsmTotalEffect = state.mNcAsmEnabled.submitted ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.ncAsmMode = state.mNcAsmMode.submitted;
                res.ambientSoundMode = state.mNcAsmFocusOnVoice.submitted
                    ? AmbientSoundMode::VOICE : AmbientSoundMode::NORMAL;
                res.ambientSoundLevelValue = state.mNcAsmAmbientLevel.submitted;
                res.ncAsmOnOffValue = state.mNcAsmAutoAsmEnabled.submitted
                    ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.noiseAdaptiveSensitivitySettings = state.mNcAsmNoiseAdaptiveSensitivity.submitted;
                SendCommandACK(NcAsmSetParamModeNcDualModeSwitchAsmSeamlessNa, res);
            }
            else if (state.mSupport.contains(FunctionType::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
            {
                NcAsmSetParamAsmSeamless res;
                res.command = Command::NCASM_SET_PARAM;
                res.valueChangeStatus = state.mNcAsmChangingAsmLevel.submitted
                    ? ValueChangeStatus::UNDER_CHANGING : ValueChangeStatus::CHANGED;
                res.ncAsmTotalEffect = state.mNcAsmEnabled.submitted ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.ambientSoundMode = state.mNcAsmFocusOnVoice.submitted
                    ? AmbientSoundMode::VOICE : AmbientSoundMode::NORMAL;
                res.ambientSoundLevelValue = state.mNcAsmAmbientLevel.submitted;
                SendCommandACK(NcAsmSetParamAsmSeamless, res);
            }
            else
            {
                NcAsmSetParamModeNcDualModeSwitchAsmSeamless res;
                res.command = Command::NCASM_SET_PARAM;
                res.valueChangeStatus = state.mNcAsmChangingAsmLevel.submitted
                    ? ValueChangeStatus::UNDER_CHANGING : ValueChangeStatus::CHANGED;
                res.ncAsmTotalEffect = state.mNcAsmEnabled.submitted ? NcAsmOnOffValue::ON : NcAsmOnOffValue::OFF;
                res.ncAsmMode = state.mNcAsmMode.submitted;
                res.ambientSoundMode = state.mNcAsmFocusOnVoice.submitted
                    ? AmbientSoundMode::VOICE : AmbientSoundMode::NORMAL;
                res.ambientSoundLevelValue = state.mNcAsmAmbientLevel.submitted;
                SendCommandACK(NcAsmSetParamModeNcDualModeSwitchAsmSeamless, res);
            }
            state.mNcAsmAmbientLevel.commit(), state.mNcAsmChangingAsmLevel.commit(), state.mNcAsmEnabled.commit(),
            state.mNcAsmMode.commit(), state.mNcAsmFocusOnVoice.commit(), state.mNcAsmAutoAsmEnabled.commit(),
            state.mNcAsmNoiseAdaptiveSensitivity.commit();
        }

        /* NC/AMB Mode */
        if (state.mNcAsmButtonFunction.pending())
        {
            using namespace t1;
            if (state.mSupport.contains(FunctionType::AMBIENT_SOUND_CONTROL_MODE_SELECT))
            {
                NcAsmSetParamNcAmbToggle res;
                res.command = Command::NCASM_SET_PARAM;
                res.function = state.mNcAsmButtonFunction.submitted;
                SendCommandACK(NcAsmSetParamNcAmbToggle, res);
            }
            state.mNcAsmButtonFunction.commit();
        }
        /* Volume */
        if (state.mPlayVolume.pending())
        {
            using namespace t1;
            SetPlayParamPlaybackControllerVolume res;
            res.command = Command::PLAY_SET_PARAM;
            res.type = MusicVolumeInquiredType(state);
            res.volumeValue = state.mPlayVolume.submitted;
            SendCommandACK(SetPlayParamPlaybackControllerVolume, res);
            state.mPlayVolume.commit();
        }
        /* Play Control */
        if (state.mPlayControl.pending())
        {
            using namespace t1;
            SetPlayStatusPlaybackController res;
            res.command = Command::PLAY_SET_STATUS;
            res.type = PlaybackInquiredType(state);
            res.status = EnableDisable::ENABLE;
            res.control = state.mPlayControl.submitted;
            SendCommandACK(SetPlayStatusPlaybackController, res);
            // A bit of a special case. We reset the value to something else
            // so simply setting 'desired' repeatedly works as intended
            state.mPlayControl.override(PlaybackControl::KEY_OFF);
        }

        /* Multipoint Switch */
        if (state.mMultipointDeviceMac.pending())
        {
            using namespace t2;
            PeripheralSetExtendedParamSourceSwitchControl res;
            if (state.mMultipointDeviceMac.submitted.length() != 17)
                state.mMultipointDeviceMac.override("");
            else
            {
                std::copy_n(state.mMultipointDeviceMac.submitted.begin(), 17, res.targetBdAddress.begin());
                SendCommandACK(PeripheralSetExtendedParamSourceSwitchControl, res);
                state.mMultipointDeviceMac.commit();
            }
        }

        /* Source Switch Control */
        if (state.mSourceSwitchControlEnabled.pending())
        {
            using namespace t2;
            if (state.mSupport.contains(t2::FunctionType::SOURCE_SWITCH_CONTROL))
            {
                PeripheralSetParamSourceSwitchControl res;
                res.value = state.mSourceSwitchControlEnabled.submitted ? 1 : 0;
                SendCommandACK(PeripheralSetParamSourceSwitchControl, res);
                state.mSourceSwitchControlEnabled.commit();
            }
            else
                state.mSourceSwitchControlEnabled.override(true);
        }

        /* Connection Ops */
        {
            using namespace t2;
            PeripheralInquiredType type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT;
            if (
                state.mSupport.contains(
                    t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT)
                ||
                state.mSupport.contains(
                    t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE)
            )
            {
                type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE;
            }
            else if (state.mSupport.contains(t2::FunctionType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
            {
                type = PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT;
            }
            else
            {
                // Unsupported. Ignore the rest.
                state.mPairedDeviceConnectMac.override("");
                state.mPairedDeviceDisconnectMac.override("");
                state.mPairedDeviceUnpairMac.override("");
            }
            PeripheralSetExtendedParamParingDeviceManagementCommon res;
            res.inquiredType = type;
            if (state.mPairedDeviceConnectMac.pending())
            {
                if (state.mPairedDeviceConnectMac.submitted.length() != 17)
                    state.mPairedDeviceConnectMac.override("");
                else
                {
                    res.connectivityActionType = ConnectivityActionType::CONNECT;
                    std::copy_n(state.mPairedDeviceConnectMac.submitted.begin(), 17, res.btDeviceAddress.begin());
                    SendCommandACK(PeripheralSetExtendedParamParingDeviceManagementCommon, res);
                    state.mPairedDeviceConnectMac.override("");
                }
            }
            if (state.mPairedDeviceDisconnectMac.pending())
            {
                if (state.mPairedDeviceDisconnectMac.submitted.length() != 17)
                    state.mPairedDeviceDisconnectMac.override("");
                else
                {
                    res.connectivityActionType = ConnectivityActionType::DISCONNECT;
                    std::copy_n(state.mPairedDeviceDisconnectMac.submitted.begin(), 17, res.btDeviceAddress.begin());
                    SendCommandACK(PeripheralSetExtendedParamParingDeviceManagementCommon, res);
                    state.mPairedDeviceDisconnectMac.override("");
                }
            }
            if (state.mPairedDeviceUnpairMac.pending())
            {
                if (state.mPairedDeviceUnpairMac.submitted.length() != 17)
                    state.mPairedDeviceUnpairMac.override("");
                else
                {
                    res.connectivityActionType = ConnectivityActionType::UNPAIR;
                    std::copy_n(state.mPairedDeviceUnpairMac.submitted.begin(), 17, res.btDeviceAddress.begin());
                    SendCommandACK(PeripheralSetExtendedParamParingDeviceManagementCommon, res);
                    state.mPairedDeviceUnpairMac.override("");
                }
            }
        

            /* Pairing Mode */
            if (state.mPairingMode.pending())
            {
                using namespace t2;
                PeripheralSetStatusParingDeviceManagementCommon res;
                res.inquiredType = type;
                res.btMode = state.mPairingMode.submitted
                    ? PeripheralBluetoothMode::INQUIRY_SCAN_MODE
                    : PeripheralBluetoothMode::NORMAL_MODE;
                res.enableDisableStatus = EnableDisable::ENABLE;
                SendCommandACK(PeripheralSetStatusParingDeviceManagementCommon, res);
                state.mPairingMode.commit();
            }
        }

        /* STC */
        if (state.mSpeakToChatEnabled.pending())
        {
            using namespace t1;
            if (state.mSupport.contains(FunctionType::SMART_TALKING_MODE_TYPE2))
            {
                SystemSetParamSmartTalking res;
                res.command = Command::SYSTEM_SET_PARAM;
                res.type = SystemInquiredType::SMART_TALKING_MODE_TYPE2;
                res.onOffValue = state.mSpeakToChatEnabled.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                res.previewModeOnOffValue = OnOffSettingValue::OFF;
                SendCommandACK(SystemSetParamSmartTalking, res);
            }
            state.mSpeakToChatEnabled.commit();
        }

        if (state.mSpeakToChatDetectSensitivity.pending() || state.mSpeakToModeOutTime.pending())
        {
            using namespace t1;
            if (state.mSupport.contains(FunctionType::SMART_TALKING_MODE_TYPE2))
            {
                SystemSetExtParamSmartTalkingModeType2 res;
                res.command = Command::SYSTEM_SET_EXT_PARAM;
                res.detectSensitivity = state.mSpeakToChatDetectSensitivity.submitted;
                res.modeOffTime = state.mSpeakToModeOutTime.submitted;
                SendCommandACK(SystemSetExtParamSmartTalkingModeType2, res);
            }
            state.mSpeakToChatDetectSensitivity.commit(), state.mSpeakToModeOutTime.commit();
        }

        /* Listening Mode */
        const bool bgmPending = state.mBGMModeEnabled.pending() || state.mBGMModeRoomSize.pending();
        const bool cinemaPending = state.mUpmixCinemaEnabled.pending();
        const bool voiceContentsPending = state.mVoiceContentsEnabled.pending();
        const bool soundLeakageReductionPending = state.mSoundLeakageReductionEnabled.pending();
        if (bgmPending || cinemaPending || voiceContentsPending || soundLeakageReductionPending)
        {
            using namespace t1;

            // Committed before sending, or a read mid-switch sees every mode off (Standard).
            if (bgmPending)
                state.mBGMModeEnabled.commit(), state.mBGMModeRoomSize.commit();
            state.mUpmixCinemaEnabled.commit();
            state.mVoiceContentsEnabled.commit();
            state.mSoundLeakageReductionEnabled.commit();

            for (int activating = 0; activating < 2; ++activating)
            {
                if (bgmPending && state.mBGMModeEnabled.submitted == (activating != 0) &&
                    state.mSupport.containsBGMMode())
                {
                    AudioSetParamBGMMode res;
                    res.command = Command::AUDIO_SET_PARAM;
                    res.type = BGMInquiredType(state);
                    res.onOffSettingValue = state.mBGMModeEnabled.submitted
                        ? OnOffSettingValue::ON
                        : OnOffSettingValue::OFF;
                    res.targetRoomSize = state.mBGMModeRoomSize.submitted;
                    SendCommandACK(AudioSetParamBGMMode, res);
                }
                if (cinemaPending && state.mUpmixCinemaEnabled.submitted == (activating != 0) &&
                    state.mSupport.contains(FunctionType::UPMIX_CINEMA))
                {
                    AudioSetParamUpmixCinema res;
                    res.command = Command::AUDIO_SET_PARAM;
                    res.onOffSettingValue = state.mUpmixCinemaEnabled.submitted
                        ? OnOffSettingValue::ON
                        : OnOffSettingValue::OFF;
                    SendCommandACK(AudioSetParamUpmixCinema, res);
                }
                if (voiceContentsPending && state.mVoiceContentsEnabled.submitted == (activating != 0) &&
                    state.mSupport.contains(FunctionType::VOICE_CONTENTS))
                {
                    AudioSetParamVoiceContents res;
                    res.onOffSettingValue = state.mVoiceContentsEnabled.submitted
                        ? OnOffSettingValue::ON
                        : OnOffSettingValue::OFF;
                    SendCommandACK(AudioSetParamVoiceContents, res);
                }
                if (soundLeakageReductionPending &&
                    state.mSoundLeakageReductionEnabled.submitted == (activating != 0) &&
                    state.mSupport.contains(FunctionType::SOUND_LEAKAGE_REDUCTION))
                {
                    AudioSetParamSoundLeakageReduction res;
                    res.onOffSettingValue = state.mSoundLeakageReductionEnabled.submitted
                        ? OnOffSettingValue::ON
                        : OnOffSettingValue::OFF;
                    SendCommandACK(AudioSetParamSoundLeakageReduction, res);
                }
            }
            EqEbbInquiredType eqType{};
            if (EqPresetInquiredType(state, eqType))
                SendCommandACK(EqEbbGetStatus, {.type = eqType});
            if (state.mSupport.contains(FunctionType::UPSCALING_AUTO_OFF))
                SendCommandACK(AudioGetStatus, {.type = AudioInquiredType::UPSCALING});
        }

        /* EQ */
        if (state.mEqPresetId.pending())
        {
            using namespace t1;
            EqEbbInquiredType type{};
            if (!EqPresetInquiredType(state, type))
                co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Preset equalizer is unavailable");
            if (type == EqEbbInquiredType::PRESET_EQ_AND_ULT_MODE)
            {
                EqEbbSetParamEqAndUltMode res;
                res.type = type;
                res.parameter.presetId = state.mEqPresetId.submitted;
                res.parameter.eqUltModeStatus = state.mEqUltMode;
                SendCommandACK(EqEbbSetParamEqAndUltMode, res);
            }
            else
            {
                EqEbbSetParamEq res;
                res.type = type;
                res.parameter.presetId = state.mEqPresetId.submitted;
                SendCommandACK(EqEbbSetParamEq, res);
            }
            state.mEqPresetId.commit();
            // Ask for a equalizer param update afterwards
            SendCommandACK(EqEbbGetParam, {.type = type});
        }
        const bool eqBandsPending = state.mEqConfig.pending() || state.mEqClearBass.pending();
        const bool eqBandsAsked = state.mEqConfig.dirty() || state.mEqClearBass.dirty();
        if (eqBandsPending && !eqBandsAsked)
        {
            state.mEqConfig.override(state.mEqConfig.current);
            state.mEqClearBass.override(state.mEqClearBass.current);
        }
        else if (eqBandsPending)
        {
            using namespace t1;
            EqEbbSetParamEq res;
            res.command = Command::EQEBB_SET_PARAM;
            if (!EqPresetInquiredType(state, res.type))
                co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Preset equalizer is unavailable");
            res.parameter.presetId = state.mEqPresetId.submitted;
            const int eqBands = state.mEqConfig.submitted.size();
            if (eqBands == 0)
            {
                state.mEqConfig.commit(), state.mEqClearBass.commit();
            }
            else
            {
                auto& bands = state.mEqConfig.submitted;
                if (eqBands == 5)
                {
                    res.parameter.bandSteps.value = Vector<UInt8>{{
                        static_cast<UInt8>(state.mEqClearBass.submitted + 10),
                        static_cast<UInt8>(bands[0] + 10),
                        static_cast<UInt8>(bands[1] + 10),
                        static_cast<UInt8>(bands[2] + 10),
                        static_cast<UInt8>(bands[3] + 10),
                        static_cast<UInt8>(bands[4] + 10),
                    }};
                }
                else if (eqBands == 10)
                    res.parameter.bandSteps.value = Vector<UInt8>{{
                        static_cast<UInt8>(bands[0] + 6),
                        static_cast<UInt8>(bands[1] + 6),
                        static_cast<UInt8>(bands[2] + 6),
                        static_cast<UInt8>(bands[3] + 6),
                        static_cast<UInt8>(bands[4] + 6),
                        static_cast<UInt8>(bands[5] + 6),
                        static_cast<UInt8>(bands[6] + 6),
                        static_cast<UInt8>(bands[7] + 6),
                        static_cast<UInt8>(bands[8] + 6),
                        static_cast<UInt8>(bands[9] + 6),
                    }};
                else
                    co_return SetLastError(MDR_RESULT_ERROR_INVALID_ARGUMENT, "mEqConfig size must be 0, 5, or 10");
                if (res.type == EqEbbInquiredType::PRESET_EQ_AND_ULT_MODE)
                {
                    EqEbbSetParamEqAndUltMode ult;
                    ult.type = res.type;
                    ult.parameter.presetId = res.parameter.presetId;
                    ult.parameter.eqUltModeStatus = state.mEqUltMode;
                    ult.parameter.bandSteps = std::move(res.parameter.bandSteps);
                    SendCommandACK(EqEbbSetParamEqAndUltMode, ult);
                }
                else
                    SendCommandACK(EqEbbSetParamEq, res);
                state.mEqConfig.commit();
                state.mEqClearBass.commit();
                // Ask for a equalizer param update afterwards
                SendCommandACK(EqEbbGetParam, {.type = res.type});
            }
        }

        /* Connection Quality */
        if (state.mAudioPriorityMode.pending())
        {
            using namespace t1;
            if (state.mSupport.contains(FunctionType::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
            {
                AudioSetParamConnection res;
                res.command = Command::AUDIO_SET_PARAM;
                // The struct defaults to CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO.
                res.type = AudioInquiredType::CONNECTION_MODE;
                res.settingValue = state.mAudioPriorityMode.submitted;
                SendCommandACK(AudioSetParamConnection, res);
            }
            state.mAudioPriorityMode.commit();
        }

        /* DSEE */
        if (state.mUpscalingEnabled.pending())
        {
            using namespace t1;
            if (state.mSupport.contains(FunctionType::UPSCALING_AUTO_OFF))
            {
                AudioSetParamUpscaling res;
                res.command = Command::AUDIO_SET_PARAM;
                res.type = AudioInquiredType::UPSCALING;
                res.settingValue = state.mUpscalingEnabled.submitted
                    ? UpscalingTypeAutoOff::AUTO : UpscalingTypeAutoOff::OFF;
                SendCommandACK(AudioSetParamUpscaling, res);
            }
            state.mUpscalingEnabled.commit();
        }

        /* Assignable controls */
        if (state.mAssignableSettingsPresets.pending())
        {
            using namespace t1;
            SystemInquiredType type{};
            if (AssignableSettingsInquiredType(state, type))
            {
                if (type == SystemInquiredType::ASSIGNABLE_SETTINGS_WITH_LIMITATION)
                {
                    SystemSetParamAssignableSettingsWithLimit res;
                    res.presetList.value = state.mAssignableSettingsPresets.submitted;
                    SendCommandACK(SystemSetParamAssignableSettingsWithLimit, res);
                }
                else
                {
                    SystemSetParamAssignableSettings res;
                    res.presetList.value = state.mAssignableSettingsPresets.submitted;
                    SendCommandACK(SystemSetParamAssignableSettings, res);
                }
            }
            state.mAssignableSettingsPresets.commit();
        }

        /* Head Gesture */
        if (state.mHeadGestureEnabled.pending())
        {
            using namespace t1;
            if (state.mSupport.contains(FunctionType::HEAD_GESTURE_ON_OFF_TRAINING))
            {
                SystemSetParamCommon res;
                res.command = Command::SYSTEM_SET_PARAM;
                res.type = SystemInquiredType::HEAD_GESTURE_ON_OFF;
                res.settingValue = state.mHeadGestureEnabled.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SystemSetParamCommon, res);
            }
            state.mHeadGestureEnabled.commit();
        }

        /* Auto Power Off */
        if (state.mPowerAutoOff.pending())
        {
            using namespace t1;
            if (state.mSupport.contains(FunctionType::AUTO_POWER_OFF))
            {
                PowerSetParamAutoPowerOff res;
                res.command = Command::POWER_SET_PARAM;
                res.currentPowerOffElements = state.mPowerAutoOff.submitted;
                res.lastSelectPowerOffElements = AutoPowerOffElements::POWER_OFF_IN_5_MIN;
                SendCommandACK(PowerSetParamAutoPowerOff, res);
            }
            state.mPowerAutoOff.commit();
        }
        if (state.mPowerAutoOffWearingDetection.pending())
        {
            using namespace t1;
            if (state.mSupport.contains(FunctionType::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
            {
                PowerSetParamAutoPowerOffWithWearingDetection res;
                res.command = Command::POWER_SET_PARAM;
                res.currentPowerOffElements = state.mPowerAutoOffWearingDetection.submitted;
                res.lastSelectPowerOffElements = AutoPowerOffWearingDetectionElements::POWER_OFF_IN_5_MIN;
                SendCommandACK(PowerSetParamAutoPowerOffWithWearingDetection, res);
            }
            state.mPowerAutoOffWearingDetection.commit();
        }

        /* Pause when device is removed */
        if (state.mAutoPauseEnabled.pending())
        {
            using namespace t1;
            if (state.mSupport.contains(FunctionType::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF))
            {
                SystemSetParamCommon res;
                res.command = Command::SYSTEM_SET_PARAM;
                res.type = SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING;
                res.settingValue = state.mAutoPauseEnabled.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SystemSetParamCommon, res);
            }
            state.mAutoPauseEnabled.commit();
        }

        /* Voice Guidance */
        if (state.mVoiceGuidanceEnabled.pending())
        {
            using namespace t2;
            if (!state.voiceGuidanceOnOffPossible())
                co_return SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "Voice-guidance on/off switching is unavailable");
            const auto type = VoiceGuidanceSettingInquiredType(state);
            const auto value = state.mVoiceGuidanceEnabled.submitted ? OnOffSettingValue::ON : OnOffSettingValue::OFF;
            if (type == VoiceGuidanceInquiredType::SUPPORT_LANGUAGE_SWITCH)
            {
                VoiceGuidanceSetParamSettingSupportLangSwitch res;
                res.voiceGuidanceInquiredType = type;
                res.settingValue = value;
                res.languageValue = state.mVoiceGuidanceLanguage;
                SendCommandACK(VoiceGuidanceSetParamSettingSupportLangSwitch, res);
            }
            else
            {
                VoiceGuidanceSetParamSettingMtk res;
                res.inquiredType = type;
                res.settingValue = value;
                SendCommandACK(VoiceGuidanceSetParamSettingMtk, res);
            }
            state.mVoiceGuidanceEnabled.commit();
        }

        /* Voice Guidance */
        if (state.mVoiceGuidanceVolume.pending())
        {
            if (state.mSupport.containsVoiceGuidanceVolume())
            {
                using namespace t2;
                VoiceGuidanceSetParamVolume res;
                res.inquiredType = VoiceGuidanceVolumeInquiredType(state);
                res.volumeValue = state.mVoiceGuidanceVolume.submitted;
                res.feedbackSound = OnOffSettingValue::ON;
                SendCommandACK(VoiceGuidanceSetParamVolume, res);
            }
            state.mVoiceGuidanceVolume.commit();
        }

        /* General Settings */
        {
            using namespace t1;
            if (state.mGsParamBool[0].pending())
            {
                if (state.mSupport.contains(FunctionType::GENERAL_SETTING_1))
                {
                    GsSetParamBoolean res;
                    res.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.type = GsInquiredType::GENERAL_SETTING1;
                    res.value = state.mGsParamBool[0].submitted ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsSetParamBoolean, res);
                }
                state.mGsParamBool[0].commit();
            }
            if (state.mGsParamBool[1].pending())
            {
                if (state.mSupport.contains(FunctionType::GENERAL_SETTING_2))
                {
                    GsSetParamBoolean res;
                    res.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.type = GsInquiredType::GENERAL_SETTING2;
                    res.value = state.mGsParamBool[1].submitted ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsSetParamBoolean, res);
                }
                state.mGsParamBool[1].commit();
            }
            if (state.mGsParamBool[2].pending())
            {
                if (state.mSupport.contains(FunctionType::GENERAL_SETTING_3))
                {
                    GsSetParamBoolean res;
                    res.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.type = GsInquiredType::GENERAL_SETTING3;
                    res.value = state.mGsParamBool[2].submitted ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsSetParamBoolean, res);
                }
                state.mGsParamBool[2].commit();
            }
            if (state.mGsParamBool[3].pending())
            {
                if (state.mSupport.contains(FunctionType::GENERAL_SETTING_4))
                {
                    GsSetParamBoolean res;
                    res.command = Command::GENERAL_SETTING_SET_PARAM;
                    res.type = GsInquiredType::GENERAL_SETTING4;
                    res.value = state.mGsParamBool[3].submitted ? GsSettingValue::ON : GsSettingValue::OFF;
                    SendCommandACK(GsSetParamBoolean, res);
                }
                state.mGsParamBool[3].commit();
            }
        }

        /* Safe Listening */
        if (state.mSafeListeningPreviewMode.pending())
        {
            using namespace t2;
            if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_HBS_1))
            {
                SafeListeningSetParamSL res;
                res.inquiredType = SafeListeningInquiredType::SAFE_LISTENING_HBS_1;
                res.previewMode = OnOffSettingValue::OFF;
                res.safeListeningMode = state.mSafeListeningPreviewMode.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SafeListeningSetParamSL, res);
            }
            else if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_HBS_2))
            {
                SafeListeningSetParamSL res;
                res.inquiredType = SafeListeningInquiredType::SAFE_LISTENING_HBS_2;
                res.previewMode = OnOffSettingValue::OFF;
                res.safeListeningMode = state.mSafeListeningPreviewMode.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SafeListeningSetParamSL, res);
            }
            else if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_1))
            {
                SafeListeningSetParamSL res;
                res.inquiredType = SafeListeningInquiredType::SAFE_LISTENING_TWS_1;
                res.previewMode = OnOffSettingValue::OFF;
                res.safeListeningMode = state.mSafeListeningPreviewMode.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SafeListeningSetParamSL, res);
            }
            else if (state.mSupport.contains(t2::FunctionType::SAFE_LISTENING_TWS_2))
            {
                SafeListeningSetParamSL res;
                res.inquiredType = SafeListeningInquiredType::SAFE_LISTENING_TWS_2;
                res.previewMode = OnOffSettingValue::OFF;
                res.safeListeningMode = state.mSafeListeningPreviewMode.submitted
                    ? OnOffSettingValue::ON
                    : OnOffSettingValue::OFF;
                SendCommandACK(SafeListeningSetParamSL, res);
            }
            state.mSafeListeningPreviewMode.commit();
        }
        co_return MDR_EVENT_APPLY_COMPLETE;
    }

    MDRTask MDRHeadphones::RequestAlertResponseV2(int action)
    {
        auto& state = mDetailsV2;
        if (!state.mAlertAwaitingResponse)
            co_return SetLastError(MDR_RESULT_ERROR_NOT_FOUND, "There is no pending alert response.");

        state.mAlertAwaitingResponse = false;

        using namespace t1;
        AlertSetParamFixedMessage res;
        res.type = AlertInquiredType::FIXED_MESSAGE;
        res.messageType = state.mLastAlertMessage;
        res.actionType = action == MDR_ALERT_ACTION_POSITIVE ? AlertAction::POSITIVE
                                                             : AlertAction::NEGATIVE;
        SendCommandACK(AlertSetParamFixedMessage, res);
        co_return MDR_EVENT_APPLY_COMPLETE;
    }

    int MDRHeadphones::HandleProtocolInfoV2(Span<const UInt8> command)
    {
        auto& state = mDetailsV2;
        if (command.size() != sizeof(v2::t1::ConnectRetProtocolInfo))
            return SetLastError(MDR_RESULT_ERROR_MALFORMED_PAYLOAD, "MDR V2 protocol info size is not valid");
        const auto result = (v2::t1::ConnectRetProtocolInfo::Deserialize)(command.data(), command.size());
        if (!result)
            return SetLastError(
                result.error,
                result.errMessage ? result.errMessage : "Unable to deserialize MDR V2 protocol info");
        state.mProtocol = {
            .version = result.value.protocolVersion,
            .hasTable1 = result.value.supportTable1Value == v2::EnableDisable::ENABLE,
            .hasTable2 = result.value.supportTable2Value == v2::EnableDisable::ENABLE
        };
        Awake(AWAIT_PROTOCOL_INFO);
        return MDR_EVENT_IDENTITY_CHANGED;
    }

    bool MDRHeadphones::IsDirtyV2() const
    {
        const auto& state = mDetailsV2;
        return state.mShutdown.dirty() || state.mNcAsmEnabled.dirty() ||
            state.mNcAsmFocusOnVoice.dirty() || state.mNcAsmAmbientLevel.dirty() || state.mNcAsmChangingAsmLevel.dirty() ||
            state.mNcAsmButtonFunction.dirty() || state.mNcAsmMode.dirty() ||
            state.mNcAsmAutoAsmEnabled.dirty() || state.mNcAsmNoiseAdaptiveSensitivity.dirty() ||
            state.mPowerAutoOff.dirty() || state.mPowerAutoOffWearingDetection.dirty() ||
            state.mPlayVolume.dirty() || state.mPlayControl.dirty() ||
            state.mGsParamBool[0].dirty() || state.mGsParamBool[1].dirty() ||
            state.mGsParamBool[2].dirty() || state.mGsParamBool[3].dirty() ||
            state.mUpscalingEnabled.dirty() || state.mAudioPriorityMode.dirty() ||
            state.mBGMModeEnabled.dirty() || state.mBGMModeRoomSize.dirty() ||
            state.mUpmixCinemaEnabled.dirty() || state.mVoiceContentsEnabled.dirty() ||
            state.mSoundLeakageReductionEnabled.dirty() || state.mAutoPauseEnabled.dirty() ||
            state.mAssignableSettingsPresets.dirty() ||
            state.mSpeakToChatEnabled.dirty() || state.mSpeakToChatDetectSensitivity.dirty() ||
            state.mSpeakToModeOutTime.dirty() || state.mHeadGestureEnabled.dirty() ||
            state.mEqAvailable.dirty() || state.mEqPresetId.dirty() ||
            state.mEqClearBass.dirty() || state.mEqConfig.dirty() ||
            state.mVoiceGuidanceEnabled.dirty() || state.mVoiceGuidanceVolume.dirty() ||
            state.mPairingMode.dirty() || state.mMultipointDeviceMac.dirty() ||
            state.mSafeListeningPreviewMode.dirty() || state.mSourceSwitchControlEnabled.dirty() ||
            state.mPairedDeviceConnectMac.dirty() || state.mPairedDeviceDisconnectMac.dirty() ||
            state.mPairedDeviceUnpairMac.dirty();
    }

#pragma endregion
}
// NOLINTEND
