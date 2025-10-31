#include <mdr/Headphones.hpp>
#include <algorithm>

namespace mdr
{
    using namespace v2::t2;

    int HandleSupportFunctionT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        ConnectRetSupportFunction res;
        ConnectRetSupportFunction::Deserialize(cmd.data(), res, cmd.size());
        std::ranges::fill(self->mSupport.table2Functions, false);
        for (auto fun : res.supportFunctions)
            self->mSupport.table2Functions[static_cast<UInt8>(fun.table2)] = true;
        self->Awake(MDRHeadphones::AWAIT_SUPPORT_FUNCTION);
        return MDR_HEADPHONES_EVT_SUPPORT_FUNCTIONS;
    }

    int HandleVoiceGuidanceParamT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        VoiceGuidanceBase base;
        VoiceGuidanceBase::Deserialize(cmd.data(), base, cmd.size());
        using enum VoiceGuidanceInquiredType;
        switch (base.type)
        {
        case MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH:
        {
            VoiceGuidanceParamSettingMtk res;
            VoiceGuidanceParamSettingMtk::Deserialize(cmd.data(), res, cmd.size());
            self->mVoiceGuidanceEnabled.overwrite(res.settingValue == v2::MessageMdrV2OnOffSettingValue::ON);
            return MDR_HEADPHONES_EVT_VOICE_GUIDANCE_ENABLE;
        }
        case VOLUME:
        {
            VoiceGuidanceParamVolume res;
            VoiceGuidanceParamVolume::Deserialize(cmd.data(), res, cmd.size());
            self->mVoiceGuidanceVolume.overwrite(res.volumeValue);
            return MDR_HEADPHONES_EVT_VOICE_GUIDANCE_VOLUME;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandlePeripheralStatusT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PeripheralBase base;
        PeripheralBase::Deserialize(cmd.data(), base, cmd.size());
        using enum PeripheralInquiredType;
        switch (base.type)
        {
        case PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE:
        {
            PeripheralStatusPairingDeviceManagementCommon res;
            PeripheralStatusPairingDeviceManagementCommon::Deserialize(cmd.data(), res, cmd.size());
            self->mPairingMode.overwrite(res.enableDisableStatus == v2::MessageMdrV2EnableDisable::ENABLE && res.btMode == PeripheralBluetoothMode::INQUIRY_SCAN_MODE);
            return MDR_HEADPHONES_EVT_BLUETOOTH_MODE;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandlePeripheralNotifyExtendedParamT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PeripheralBase base;
        PeripheralBase::Deserialize(cmd.data(), base, cmd.size());
        using enum PeripheralInquiredType;
        switch (base.type)
        {
        case SOURCE_SWITCH_CONTROL:
        {
            PeripheralNotifyExtendedParamSourceSwitchControl res;
            PeripheralNotifyExtendedParamSourceSwitchControl::Deserialize(cmd.data(), res, cmd.size());
            self->mMultipointDeviceMac.overwrite(String(res.targetBdAddress.begin(), res.targetBdAddress.end()));
            return MDR_HEADPHONES_EVT_MULTIPOINT_SWITCH;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandlePeripheralParamT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        PeripheralBase base;
        PeripheralBase::Deserialize(cmd.data(), base, cmd.size());
        using enum PeripheralInquiredType;
        switch (base.type)
        {
        case PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT:
        {
            PeripheralParamPairingDeviceManagementClassicBt res;
            PeripheralParamPairingDeviceManagementClassicBt::Deserialize(cmd.data(), res, cmd.size());
            self->mPairedDevices.resize(res.deviceList.size());
            for (size_t i = 0; i < self->mPairedDevices.size(); ++i)
            {
                auto& mac = res.deviceList.value[i].btDeviceAddress;
                self->mPairedDevices[i].macAddress = {mac.begin(), mac.end()};
                self->mPairedDevices[i].name = res.deviceList.value[i].btFriendlyName.value;
                self->mPairedDevices[i].connected = res.deviceList.value[i].connectedStatus;
            }
            self->mPairedDevicesPlaybackDeviceIndex = res.playbackDevice;
            self->mMultipointDeviceMac.overwrite(
                self->mPairedDevices[self->mPairedDevicesPlaybackDeviceIndex].macAddress);
            return MDR_HEADPHONES_EVT_CONNECTED_DEVICES;
        }
        case PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE:
        {
            PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice res;
            PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice::Deserialize(cmd.data(), res, cmd.size());
            self->mPairedDevices.resize(res.deviceList.size());
            for (size_t i = 0; i < self->mPairedDevices.size(); ++i)
            {
                auto& mac = res.deviceList.value[i].btDeviceAddress;
                self->mPairedDevices[i].macAddress = {mac.begin(), mac.end()};
                self->mPairedDevices[i].name = res.deviceList.value[i].btFriendlyName.value;
                self->mPairedDevices[i].connected = res.deviceList.value[i].connectedStatus;
            }
            self->mPairedDevicesPlaybackDeviceIndex = res.playbackDevice;
            self->mMultipointDeviceMac.overwrite(
                self->mPairedDevices[self->mPairedDevicesPlaybackDeviceIndex].macAddress);
            return MDR_HEADPHONES_EVT_CONNECTED_DEVICES;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleSafeListeningParamsT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        SafeListeningNotifyParam base;
        SafeListeningNotifyParam::Deserialize(cmd.data(), base, cmd.size());
        using enum SafeListeningInquiredType;
        switch (base.type)
        {
        case SAFE_LISTENING_HBS_1:
        case SAFE_LISTENING_HBS_2:
        case SAFE_LISTENING_TWS_1:
        case SAFE_LISTENING_TWS_2:
        {
            SafeListeningNotifyParamSL res;
            SafeListeningNotifyParamSL::Deserialize(cmd.data(), res, cmd.size());
            self->mSafeListeningPreviewMode.overwrite(res.previewMode == v2::MessageMdrV2EnableDisable::ENABLE);
            return MDR_HEADPHONES_EVT_SAFE_LISTENING_PARAM;
        }
        default:
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int HandleSafeListeningExtendedParamT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        SafeListeningRetExtendedParam res;
        SafeListeningRetExtendedParam::Deserialize(cmd.data(), res, cmd.size());
        self->mSafeListeningSoundPressure = res.levelPerPeriod;
        return MDR_HEADPHONES_EVT_SOUND_PRESSURE;
    }

    int MDRHeadphones::HandleCommandV2T2(Span<const UInt8> cmd, MDRCommandSeqNumber seq)
    {
        using namespace v2::t2;
        using enum Command;
        CommandBase base;
        CommandBase::Deserialize(cmd.data(), base, cmd.size());
        switch (base.command)
        {
        case CONNECT_RET_SUPPORT_FUNCTION:
            return HandleSupportFunctionT2(this, cmd);
        case VOICE_GUIDANCE_RET_PARAM:
        case VOICE_GUIDANCE_NTFY_PARAM:
            return HandleVoiceGuidanceParamT2(this, cmd);
        case PERI_RET_STATUS:
        case PERI_NTFY_STATUS:
            return HandlePeripheralStatusT2(this, cmd);
        case PERI_NTFY_EXTENDED_PARAM:
            return HandlePeripheralNotifyExtendedParamT2(this, cmd);
        case PERI_RET_PARAM:
        case PERI_NTFY_PARAM:
            return HandlePeripheralParamT2(this, cmd);
        case SAFE_LISTENING_NTFY_PARAM:
            return HandleSafeListeningParamsT2(this, cmd);
        case SAFE_LISTENING_RET_EXTENDED_PARAM:
            return HandleSafeListeningExtendedParamT2(this, cmd);
        default:
            fmt::println("** Unhandled {}", base.command);
            break;
        }
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }
}
