#include <algorithm>
#include "Details.hpp"
namespace mdr
{
    using namespace v2;
    using namespace t2;

    template <typename T>
    bool ReadInquiredType(Span<const UInt8> cmd, T& out)
    {
        if (cmd.size() < 2)
            return false;
        out = static_cast<T>(cmd[1]);
        return true;
    }

    int HandleSupportFunctionT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(ConnectRetSupportFunction, res, cmd);
        auto& state = self->mDetailsV2;
        std::ranges::fill(state.mSupport.table2Functions, false);
        bool replacedWearingPosition = false;
        for (auto fun : res.supportFunctions)
        {
            auto type = fun.functionType;
            if (!replacedWearingPosition && type == FunctionType::WEARING_POSITION &&
                (state.mProtocol.version == 0x3003002 || state.mProtocol.version == 0x3003033))
            {
                type = FunctionType::WEARING_POSITION_WITHOUT_FITTING_SUPPORTER;
                replacedWearingPosition = true;
            }
            state.mSupport.table2Functions[static_cast<UInt8>(type)] = true;
        }
        state.mSupport.provenance = DetailsV2::SupportStates::Provenance::ADVERTISED;
        return MDR_EVENT_IDENTITY_CHANGED;
    }

    int HandleVoiceGuidanceCapabilityT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        VoiceGuidanceInquiredType type{};
        if (!ReadInquiredType(cmd, type))
            return MDR_EVENT_UNHANDLED;
        auto& state = self->mDetailsV2;
        switch (type)
        {
        case VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH:
        {
            Deserialize(VoiceGuidanceRetCapabilitySettingMtkNotSupportLangSwitch, res, cmd);
            state.mVoiceGuidanceOnOffSupported[0] = res.supportOnOffSwitch == VoiceGuidanceSupportsSwitch::SUPPORT;
            return MDR_EVENT_VOICE_GUIDANCE_CHANGED;
        }
        case VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH:
        {
            Deserialize(VoiceGuidanceRetCapabilitySettingMtkSupportLangSwitch, res, cmd);
            state.mVoiceGuidanceOnOffSupported[1] = res.supportOnOffSwitch == VoiceGuidanceSupportsSwitch::SUPPORT;
            return MDR_EVENT_VOICE_GUIDANCE_CHANGED;
        }
        case VoiceGuidanceInquiredType::SUPPORT_LANGUAGE_SWITCH:
        {
            Deserialize(VoiceGuidanceRetCapabilitySettingSupportLangSwitch, res, cmd);
            state.mVoiceGuidanceOnOffSupported[2] = res.supportOnOffSwitch == VoiceGuidanceSupportsSwitch::SUPPORT;
            return MDR_EVENT_VOICE_GUIDANCE_CHANGED;
        }
        default:
            return MDR_EVENT_UNHANDLED;
        }
    }

    int HandleSystemCapabilityT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        SystemInquiredType type{};
        if (!ReadInquiredType(cmd, type))
            return MDR_EVENT_UNHANDLED;
        if (type == SystemInquiredType::SONY_VOICE_ASSISTANT_COMMAND)
        {
            Deserialize(SystemRetCapabilitySVACommand, res, cmd);
            if (res.dataContinue != DataContinue::NOT_CONTINUE_DATA && res.dataContinue != DataContinue::HAS_CONTINUE_DATA)
                return self->SetLastError(MDR_RESULT_ERROR_MALFORMED_PAYLOAD, "Invalid Sony voice-assistant continuation");
            self->mDetailsV2.mSvaPartLength = static_cast<UInt8>(res.data.size());
            self->mDetailsV2.mSvaDataContinue = res.dataContinue;
            return MDR_EVENT_NONE;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleVoiceGuidanceParamT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        VoiceGuidanceInquiredType type{};
        if (!ReadInquiredType(cmd, type))
            return MDR_EVENT_UNHANDLED;
        if (static_cast<Command>(cmd[0]) == Command::VOICE_GUIDANCE_NTFY_PARAM)
        {
            if (cmd.size() < 3)
                return self->SetLastError(MDR_RESULT_ERROR_MALFORMED_PAYLOAD, "Invalid voice-guidance notification");
            switch (type)
            {
            case VoiceGuidanceInquiredType::SUPPORT_LANGUAGE_SWITCH:
                if (cmd.size() < 4)
                    return self->SetLastError(MDR_RESULT_ERROR_MALFORMED_PAYLOAD, "Invalid voice-guidance language notification");
                self->mDetailsV2.mVoiceGuidanceLanguage = static_cast<VoiceGuidanceLanguage>(cmd[3]);
                self->mDetailsV2.mVoiceGuidanceEnabled.overwrite(cmd[2] == static_cast<UInt8>(OnOffSettingValue::ON));
                return MDR_EVENT_VOICE_GUIDANCE_CHANGED;
            case VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH:
            case VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH:
            case VoiceGuidanceInquiredType::ONLY_ON_OFF_SETTING:
                self->mDetailsV2.mVoiceGuidanceEnabled.overwrite(cmd[2] == static_cast<UInt8>(OnOffSettingValue::ON));
                return MDR_EVENT_VOICE_GUIDANCE_CHANGED;
            case VoiceGuidanceInquiredType::VOLUME:
            case VoiceGuidanceInquiredType::VOLUME_SETTING_FIXED_TO_5_STEPS:
                self->mDetailsV2.mVoiceGuidanceVolume.overwrite(static_cast<Int8>(cmd[2]));
                return MDR_EVENT_VOICE_GUIDANCE_CHANGED;
            default:
                return MDR_EVENT_UNHANDLED;
            }
        }
        using enum VoiceGuidanceInquiredType;
        switch (type)
        {
        case MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH:
        case SUPPORT_LANGUAGE_SWITCH:
        {
            Deserialize(VoiceGuidanceRetParamSettingMtkSupportLangSwitchAndSupportLangSwitch, res, cmd);
            self->mDetailsV2.mVoiceGuidanceEnabled.overwrite(res.settingValue == OnOffSettingValue::ON);
            self->mDetailsV2.mVoiceGuidanceLanguage = res.currentLanguage;
            return MDR_EVENT_VOICE_GUIDANCE_CHANGED;
        }
        case MTK_TRANSFER_WO_DISCONNECTION_NOT_SUPPORT_LANGUAGE_SWITCH:
        case ONLY_ON_OFF_SETTING:
        {
            Deserialize(VoiceGuidanceRetParamSettingMtk, res, cmd);
            self->mDetailsV2.mVoiceGuidanceEnabled.overwrite(res.settingValue == OnOffSettingValue::ON);
            return MDR_EVENT_VOICE_GUIDANCE_CHANGED;
        }
        case VOLUME:
        case VOLUME_SETTING_FIXED_TO_5_STEPS:
        {
            Deserialize(VoiceGuidanceRetParamVolume, res, cmd);
            self->mDetailsV2.mVoiceGuidanceVolume.overwrite(res.volumeValue);
            return MDR_EVENT_VOICE_GUIDANCE_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandlePeripheralStatusT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PeripheralInquiredType type{};
        if (!ReadInquiredType(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum PeripheralInquiredType;
        switch (type)
        {
        case PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE:
        case PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT:
        {
            const auto command = static_cast<Command>(cmd[0]);
            if (command == Command::PERI_NTFY_STATUS)
            {
                Deserialize(PeripheralNotifyStatusParingDeviceManagementCommon, res, cmd);
                self->mDetailsV2.mPairingMode.overwrite(res.enableDisableStatus == EnableDisable::ENABLE && res.btMode == PeripheralBluetoothMode::INQUIRY_SCAN_MODE);
            }
            else
            {
                Deserialize(PeripheralRetStatusPairingDeviceManagementCommon, res, cmd);
                self->mDetailsV2.mPairingMode.overwrite(res.enableDisableStatus == EnableDisable::ENABLE && res.btMode == PeripheralBluetoothMode::INQUIRY_SCAN_MODE);
            }
            return MDR_EVENT_PAIRING_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandlePeripheralNotifyExtendedParamT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PeripheralInquiredType type{};
        if (!ReadInquiredType(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum PeripheralInquiredType;
        switch (type)
        {
        case SOURCE_SWITCH_CONTROL:
        {
            Deserialize(PeripheralNotifyExtendedParamSourceSwitchControl, res, cmd);
            self->mDetailsV2.mSourceSwitchControlResult = res.result;
            const String target{res.targetBdAddress.begin(), res.targetBdAddress.end()};
            self->mDetailsV2.mMultipointDeviceMac.overwrite(target);
            for (auto& dev : self->mDetailsV2.mPairedDevices)
                dev.playbackDevice = dev.macAddress == target;
            return MDR_EVENT_PAIRED_DEVICES_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandlePeripheralParamT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PeripheralInquiredType type{};
        if (!ReadInquiredType(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum PeripheralInquiredType;
        switch (type)
        {
        case PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT:
        {
            const auto command = static_cast<Command>(cmd[0]);
            if (command == Command::PERI_NTFY_PARAM)
            {
                Deserialize(PeripheralNotifyParamPairingDeviceManagementClassicBt, res, cmd);
                self->mDetailsV2.mPairedDevicesPlaybackDeviceID = res.playbackrightDevice;
                self->mDetailsV2.mPairedDevices.resize(res.deviceInfo.size());
                for (size_t i = 0; i < self->mDetailsV2.mPairedDevices.size(); ++i)
                {
                    auto& mac = res.deviceInfo.value[i].btDeviceAddress;
                    self->mDetailsV2.mPairedDevices[i].macAddress = {mac.begin(), mac.end()};
                    self->mDetailsV2.mPairedDevices[i].name = res.deviceInfo.value[i].btFriendlyName.value;
                    self->mDetailsV2.mPairedDevices[i].connected = res.deviceInfo.value[i].connectedStatus != 0;
                    self->mDetailsV2.mPairedDevices[i].playbackDevice =
                        res.deviceInfo.value[i].connectedStatus == res.playbackrightDevice;
                    if (self->mDetailsV2.mPairedDevices[i].playbackDevice)
                        self->mDetailsV2.mMultipointDeviceMac.overwrite(self->mDetailsV2.mPairedDevices[i].macAddress);
                }
                return MDR_EVENT_PAIRED_DEVICES_CHANGED;
            }
            Deserialize(PeripheralRetParamPairingDeviceManagementClassicBt, res, cmd);
            self->mDetailsV2.mPairedDevicesPlaybackDeviceID = res.playbackrightDevice;
            self->mDetailsV2.mPairedDevices.resize(res.deviceInfo.size());
            for (size_t i = 0; i < self->mDetailsV2.mPairedDevices.size(); ++i)
            {
                auto& mac = res.deviceInfo.value[i].btDeviceAddress;
                self->mDetailsV2.mPairedDevices[i].macAddress = {mac.begin(), mac.end()};
                self->mDetailsV2.mPairedDevices[i].name = res.deviceInfo.value[i].btFriendlyName.value;
                self->mDetailsV2.mPairedDevices[i].connected = res.deviceInfo.value[i].connectedStatus != 0;
                self->mDetailsV2.mPairedDevices[i].playbackDevice =
                    res.deviceInfo.value[i].connectedStatus == res.playbackrightDevice;
                if (res.deviceInfo.value[i].connectedStatus == res.playbackrightDevice)
                    self->mDetailsV2.mMultipointDeviceMac.overwrite(self->mDetailsV2.mPairedDevices[i].macAddress);
            }
            return MDR_EVENT_PAIRED_DEVICES_CHANGED;
        }
        case SOURCE_SWITCH_CONTROL:
            {
                const auto command = static_cast<Command>(cmd[0]);
                if (command == Command::PERI_NTFY_PARAM)
                {
                    Deserialize(PeripheralNotifyParamSourceSwitchControl, res, cmd);
                    self->mDetailsV2.mSourceSwitchControlEnabled.overwrite(res.value1 != 0);
                    self->mDetailsV2.mSourceSwitchControlResult = res.result;
                    return MDR_EVENT_PAIRED_DEVICES_CHANGED;
                }
                Deserialize(PeripheralRetParamSourceSwitchControl, res, cmd);
                self->mDetailsV2.mSourceSwitchControlEnabled.overwrite(res.value != 0);
                return MDR_EVENT_PAIRED_DEVICES_CHANGED;
            }
        case PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE:
        {
            const auto command = static_cast<Command>(cmd[0]);
            if (command == Command::PERI_NTFY_PARAM)
            {
                Deserialize(PeripheralNotifyParamPairingDeviceManagementWithBluetoothClassOfDevice, res, cmd);
                self->mDetailsV2.mPairedDevicesPlaybackDeviceID = res.playbackrightDevice;
                self->mDetailsV2.mPairedDevices.resize(res.deviceInfo.size());
                for (size_t i = 0; i < self->mDetailsV2.mPairedDevices.size(); ++i)
                {
                    auto& mac = res.deviceInfo.value[i].btDeviceAddress;
                    self->mDetailsV2.mPairedDevices[i].macAddress = {mac.begin(), mac.end()};
                    self->mDetailsV2.mPairedDevices[i].name = res.deviceInfo.value[i].btFriendlyName.value;
                    self->mDetailsV2.mPairedDevices[i].connected = res.deviceInfo.value[i].connectedStatus != 0;
                    self->mDetailsV2.mPairedDevices[i].playbackDevice =
                        res.deviceInfo.value[i].connectedStatus == res.playbackrightDevice;
                    if (self->mDetailsV2.mPairedDevices[i].playbackDevice)
                        self->mDetailsV2.mMultipointDeviceMac.overwrite(self->mDetailsV2.mPairedDevices[i].macAddress);
                }
                return MDR_EVENT_PAIRED_DEVICES_CHANGED;
            }
            Deserialize(PeripheralRetParamPairingDeviceManagementWithBluetoothClassOfDevice, res, cmd);
            self->mDetailsV2.mPairedDevicesPlaybackDeviceID = res.playbackrightDevice;
            self->mDetailsV2.mPairedDevices.resize(res.deviceInfo.size());
            for (size_t i = 0; i < self->mDetailsV2.mPairedDevices.size(); ++i)
            {
                auto& mac = res.deviceInfo.value[i].btDeviceAddress;
                self->mDetailsV2.mPairedDevices[i].macAddress = {mac.begin(), mac.end()};
                self->mDetailsV2.mPairedDevices[i].name = res.deviceInfo.value[i].btFriendlyName.value;
                self->mDetailsV2.mPairedDevices[i].connected = res.deviceInfo.value[i].connectedStatus != 0;
                self->mDetailsV2.mPairedDevices[i].playbackDevice =
                    res.deviceInfo.value[i].connectedStatus == res.playbackrightDevice;
                if (res.deviceInfo.value[i].connectedStatus == res.playbackrightDevice)
                    self->mDetailsV2.mMultipointDeviceMac.overwrite(self->mDetailsV2.mPairedDevices[i].macAddress);
            }
            return MDR_EVENT_PAIRED_DEVICES_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleSafeListeningParamsT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        SafeListeningInquiredType type{};
        if (!ReadInquiredType(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum SafeListeningInquiredType;
        switch (type)
        {
        case SAFE_LISTENING_HBS_1:
        case SAFE_LISTENING_HBS_2:
        case SAFE_LISTENING_TWS_1:
        case SAFE_LISTENING_TWS_2:
        {
            Deserialize(SafeListeningNotifyParamSL, res, cmd);
            self->mDetailsV2.mSafeListeningPreviewMode.overwrite(res.previewMode == OnOffSettingValue::ON);
            return MDR_EVENT_SAFE_LISTENING_CHANGED;
        }
        default:
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int HandleSafeListeningExtendedParamT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        Deserialize(SafeListeningRetExtendedParam, res, cmd);
        self->mDetailsV2.mSafeListeningSoundPressure = res.levelPerPeriod;
        return MDR_EVENT_SAFE_LISTENING_CHANGED;
    }

    int HandleSystemStatusT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        SystemInquiredType type{};
        if (!ReadInquiredType(cmd, type))
            return MDR_EVENT_UNHANDLED;
        using enum SystemInquiredType;
        switch (type)
        {
        case WEARING_STATUS_CHECKER:
        {
            const auto command = static_cast<Command>(cmd[0]);
            WearingStatusCode status;
            if (command == Command::SYSTEM_NTFY_STATUS)
            {
                Deserialize(SystemNotifyStatusWearingStatusChecker, res, cmd);
                status = res.status;
            }
            else
            {
                Deserialize(SystemRetStatusWearingStatusChecker, res, cmd);
                status = res.status;
            }
            self->mDetailsV2.mWearingStatus = status;
            return MDR_EVENT_WEARING_STATUS_CHANGED;
        }
        default: break;
        }
        return MDR_EVENT_UNHANDLED;
    }

    int MDRHeadphones::HandleCommandV2T2(Span<const UInt8> cmd, MDRCommandSeqNumber)
    {
        auto* self = this;
        using enum Command;
        if (cmd.empty())
            return MDR_EVENT_UNHANDLED;
        const auto command = static_cast<Command>(cmd[0]);
        switch (command)
        {
        case CONNECT_RET_SUPPORT_FUNCTION:
            return HandleSupportFunctionT2(self, cmd);
        case VOICE_GUIDANCE_RET_CAPABILITY:
            return HandleVoiceGuidanceCapabilityT2(self, cmd);
        case SYSTEM_RET_CAPABILITY:
            return HandleSystemCapabilityT2(self, cmd);
        case VOICE_GUIDANCE_RET_PARAM:
        case VOICE_GUIDANCE_NTFY_PARAM:
            return HandleVoiceGuidanceParamT2(self, cmd);
        case PERI_RET_STATUS:
        case PERI_NTFY_STATUS:
            return HandlePeripheralStatusT2(self, cmd);
        case PERI_NTFY_EXTENDED_PARAM:
            return HandlePeripheralNotifyExtendedParamT2(self, cmd);
        case PERI_RET_PARAM:
        case PERI_NTFY_PARAM:
            return HandlePeripheralParamT2(self, cmd);
        case SAFE_LISTENING_NTFY_PARAM:
            return HandleSafeListeningParamsT2(self, cmd);
        case SAFE_LISTENING_RET_EXTENDED_PARAM:
            return HandleSafeListeningExtendedParamT2(self, cmd);
        case SYSTEM_RET_STATUS:
        case SYSTEM_NTFY_STATUS:
            return HandleSystemStatusT2(self, cmd);
        default:
            MDR_LOG_DEBUG("** Unhandled {}", command);
            break;
        }
        return MDR_EVENT_UNHANDLED;
    }
}
