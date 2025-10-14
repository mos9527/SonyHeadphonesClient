#include "Headphones.h"

#include <stdexcept>

#include "CommandSerializer.h"
#include "ProtocolV2T1.h"

Headphones::Headphones(BluetoothWrapper& conn) : _conn(conn), _recvFuture("receive"), _requestFuture("request"), _sendCommandFuture("send cmd")
{
    recvAsync();
}

void Headphones::pushModalAlert(const std::string& title, const std::string& message, std::function<void(bool)> callback)
{
    ModalAlert dialog;
    dialog.id = "##dlg_" + std::to_string(nextModalAlertId++);
    dialog.title = title;
    dialog.message = message;
    dialog.onClose = std::move(callback);
    modalAlerts.push_back(std::move(dialog));
}

void Headphones::postModalAlertHandling()
{
    // Remove closed dialogs
    std::erase_if(modalAlerts, [](const ModalAlert& d) { return !d.open; });
}

bool Headphones::supports(MessageMdrV2FunctionType_Table1 functionTypeTable1) const
{
    return supportFunctions1[(size_t)functionTypeTable1];
}

bool Headphones::supports(MessageMdrV2FunctionType_Table2 functionTypeTable2) const
{
    return supportFunctions2[(size_t)functionTypeTable2];
}

bool Headphones::supportsNc() const
{
    using F1 = MessageMdrV2FunctionType_Table1;
    return supports(F1::NOISE_CANCELLING_ONOFF)
        || supports(F1::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_ONOFF)
        || supports(F1::NOISE_CANCELLING_DUAL_SINGLE_OFF_AND_AMBIENT_SOUND_MODE_ONOFF)
        || supports(F1::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || supports(F1::NOISE_CANCELLING_DUAL_SINGLE_OFF_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || supports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AUTO_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || supports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_SINGLE_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || supports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || supports(F1::MODE_NC_NCSS_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_WITH_TEST_MODE)
        || supports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
}

bool Headphones::supportsAsm() const
{
    using F1 = MessageMdrV2FunctionType_Table1;
    return supports(F1::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_ONOFF)
        || supports(F1::NOISE_CANCELLING_DUAL_SINGLE_OFF_AND_AMBIENT_SOUND_MODE_ONOFF)
        || supports(F1::NOISE_CANCELLING_ONOFF_AND_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || supports(F1::NOISE_CANCELLING_DUAL_SINGLE_OFF_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || supports(F1::AMBIENT_SOUND_MODE_ONOFF)
        || supports(F1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || supports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AUTO_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || supports(F1::AMBIENT_SOUND_CONTROL_MODE_SELECT)
        || supports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_SINGLE_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || supports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT)
        || supports(F1::MODE_NC_NCSS_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_WITH_TEST_MODE)
        || supports(F1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
}

bool Headphones::supportsSafeListening() const
{
    using F2 = MessageMdrV2FunctionType_Table2;
    return supports(F2::SAFE_LISTENING_HBS_1) || supports(F2::SAFE_LISTENING_HBS_2)
        || supports(F2::SAFE_LISTENING_TWS_1) || supports(F2::SAFE_LISTENING_TWS_2);
}

bool Headphones::supportsPairingDeviceManagement() const
{
    using F2 = MessageMdrV2FunctionType_Table2;
    return supports(F2::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT)
        || supports(F2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT)
        || supports(F2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE);
}

bool Headphones::supportsMultipoint() const
{
    using F1 = MessageMdrV2FunctionType_Table1;
    static const std::string kMultipointSetting = "MULTIPOINT_SETTING";
    return supports(F1::GENERAL_SETTING_1) && gs1c.current.info.subject == kMultipointSetting
        || supports(F1::GENERAL_SETTING_2) && gs2c.current.info.subject == kMultipointSetting
        || supports(F1::GENERAL_SETTING_3) && gs3c.current.info.subject == kMultipointSetting
        || supports(F1::GENERAL_SETTING_4) && gs4c.current.info.subject == kMultipointSetting;
}

bool Headphones::isChanged()
{
    return !(
           changingAsmLevel.isFulfilled()
        && asmEnabled.isFulfilled()
        && asmMode.isFulfilled()
        && asmFoucsOnVoice.isFulfilled()
        && asmLevel.isFulfilled()
        && autoAsmEnabled.isFulfilled()
        && autoAsmSensitivity.isFulfilled()
        && autoPowerOff.isFulfilled()
        && autoPowerOffWearingDetection.isFulfilled()
        && autoPauseEnabled.isFulfilled()
        && voiceGuidanceEnabled.isFulfilled()
        && miscVoiceGuidanceVol.isFulfilled()
        && volume.isFulfilled()
        && mpDeviceMac.isFulfilled()
        && pairingMode.isFulfilled()
        && stcEnabled.isFulfilled()
        && stcLevel.isFulfilled()
        && stcTime.isFulfilled()
        && listeningModeConfig.isFulfilled()
        && eqConfig.isFulfilled()
        && eqPreset.isFulfilled()
        && touchLeftFunc.isFulfilled()
        && touchRightFunc.isFulfilled()
        && ncAmbButtonMode.isFulfilled()
        && gs1.isFulfilled()
        && gs2.isFulfilled()
        && gs3.isFulfilled()
        && gs4.isFulfilled()
        && safeListening.preview.isFulfilled()
    );
}

void Headphones::waitForAck(int timeout)
{
    if (_conn.isConnected())
    {
        std::unique_lock lck(_ackMtx);
        _ackCV.wait_for(lck, std::chrono::seconds(timeout));
    }
}

void Headphones::waitForProtocolInfo(int timeout)
{
    if (_conn.isConnected())
    {
        std::unique_lock lck(_propertyMtx);
        _protocolInfoCV.wait_for(lck, std::chrono::seconds(timeout));
    }
}

void Headphones::waitForSupportFunction(int timeout)
{
    if (_conn.isConnected())
    {
        std::unique_lock lck(_propertyMtx);
        _supportFunctionCV.wait_for(lck, std::chrono::seconds(timeout));
    }
}

/*void Headphones::waitForResponse(int timeout)
{
    return waitForAck(timeout);
    if (_conn.isConnected())
    {
        std::unique_lock lck(_ackMtx);
        std::cv_status status = _responseCV.wait_for(lck, std::chrono::seconds(timeout));
        if (status == std::cv_status::timeout)
        {
            printf("Timed out waiting for %s\n",
                _waitForResponseDataType == DATA_TYPE::DATA_MDR ? THMSGV2T1::Command_toString(static_cast<THMSGV2T1::Command>(_waitForResponseCommandId))
                    : _waitForResponseDataType == DATA_TYPE::DATA_MDR_NO2 ? THMSGV2T2::Command_toString(static_cast<THMSGV2T2::Command>(_waitForResponseCommandId))
                        : "unknown command");
        }
    }
}*/

void Headphones::setChanges()
{
    if (!(changingAsmLevel.isFulfilled() && asmEnabled.isFulfilled() && asmMode.isFulfilled() &&
        asmFoucsOnVoice.isFulfilled() && asmLevel.isFulfilled() &&
        autoAsmEnabled.isFulfilled() && autoAsmSensitivity.isFulfilled()))
    {
        if (supports(MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            sendSet<THMSGV2T1::NcAsmParamModeNcDualModeSwitchAsmSeamless>(
                changingAsmLevel.desired,
                asmEnabled.desired ? THMSGV2T1::NcAsmOnOffValue::ON : THMSGV2T1::NcAsmOnOffValue::OFF,
                asmMode.desired,
                asmFoucsOnVoice.desired ? THMSGV2T1::AmbientSoundMode::VOICE : THMSGV2T1::AmbientSoundMode::NORMAL,
                std::max(asmLevel.desired, 1)
            );
        }
        else if (supports(MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
        {
            sendSet<THMSGV2T1::NcAsmParamModeNcDualModeSwitchAsmSeamlessNa>(
                changingAsmLevel.desired,
                asmEnabled.desired ? THMSGV2T1::NcAsmOnOffValue::ON : THMSGV2T1::NcAsmOnOffValue::OFF,
                asmMode.desired,
                asmFoucsOnVoice.desired ? THMSGV2T1::AmbientSoundMode::VOICE : THMSGV2T1::AmbientSoundMode::NORMAL,
                std::max(asmLevel.desired, 1),
                autoAsmEnabled.desired ? THMSGV2T1::NcAsmOnOffValue::ON : THMSGV2T1::NcAsmOnOffValue::OFF,
                autoAsmSensitivity.desired
            );
        }
        else if (supports(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            sendSet<THMSGV2T1::NcAsmParamAsmSeamless>(
                changingAsmLevel.desired,
                asmEnabled.desired ? THMSGV2T1::NcAsmOnOffValue::ON : THMSGV2T1::NcAsmOnOffValue::OFF,
                asmFoucsOnVoice.desired ? THMSGV2T1::AmbientSoundMode::VOICE : THMSGV2T1::AmbientSoundMode::NORMAL,
                std::max(asmLevel.desired, 1)
            );
        }
        else
        {
            // Fallback to the most universal command
            sendSet<THMSGV2T1::NcAsmParamModeNcDualModeSwitchAsmSeamless>(
                changingAsmLevel.desired,
                asmEnabled.desired ? THMSGV2T1::NcAsmOnOffValue::ON : THMSGV2T1::NcAsmOnOffValue::OFF,
                asmMode.desired,
                asmFoucsOnVoice.desired ? THMSGV2T1::AmbientSoundMode::VOICE : THMSGV2T1::AmbientSoundMode::NORMAL,
                std::max(asmLevel.desired, 1)
            );
        }

        std::lock_guard guard(this->_propertyMtx);
        changingAsmLevel.fulfill();
        asmEnabled.fulfill();
        asmMode.fulfill();
        asmLevel.fulfill();
        asmFoucsOnVoice.fulfill();
        autoAsmEnabled.fulfill();
        autoAsmSensitivity.fulfill();
    }

    if (supports(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF)
        && !autoPowerOff.isFulfilled())
    {
        sendSet<THMSGV2T1::PowerParamAutoPowerOff>(
            autoPowerOff.desired,
            /*lastSelectPowerOffElements*/ THMSGV2T1::AutoPowerOffElements::POWER_OFF_IN_5_MIN
        );
        this->autoPowerOffWearingDetection.fulfill();
    }
    else if (supports(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION)
        && !autoPowerOffWearingDetection.isFulfilled())
    {
        sendSet<THMSGV2T1::PowerParamAutoPowerOffWithWearingDetection>(
            autoPowerOffWearingDetection.desired,
            /*lastSelectPowerOffElements*/ THMSGV2T1::AutoPowerOffWearingDetectionElements::POWER_OFF_IN_5_MIN
        );
        this->autoPowerOffWearingDetection.fulfill();
    }

    if (supports(MessageMdrV2FunctionType_Table1::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF)
        && !autoPauseEnabled.isFulfilled())
    {
        sendSet<THMSGV2T1::SystemParamCommon>(
            THMSGV2T1::SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING,
            autoPauseEnabled.desired
        );

        this->autoPauseEnabled.fulfill();
    }

    if (!voiceGuidanceEnabled.isFulfilled())
    {
        sendSet<THMSGV2T2::VoiceGuidanceParamSettingMtk>(
            THMSGV2T2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH,
            voiceGuidanceEnabled.desired
        );

        this->voiceGuidanceEnabled.fulfill();
    }

    if (supports(MessageMdrV2FunctionType_Table2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT)
        && !miscVoiceGuidanceVol.isFulfilled())
    {
        sendSet<THMSGV2T2::VoiceGuidanceSetParamVolume>(
            THMSGV2T2::VoiceGuidanceInquiredType::VOLUME,
            static_cast<int8_t>(miscVoiceGuidanceVol.desired),
            /*feedbackSound*/ true
        );

        this->miscVoiceGuidanceVol.fulfill();
    }

    if (!volume.isFulfilled())
    {
        sendSet<THMSGV2T1::PlayParamPlaybackControllerVolume>(
            THMSGV2T1::PlayInquiredType::MUSIC_VOLUME,
            static_cast<uint8_t>(volume.desired)
        );

        this->volume.fulfill();
    }

    if (!mpDeviceMac.isFulfilled())
    {
        sendSet<THMSGV2T2::PeripheralSetExtendedParamSourceSwitchControl>(mpDeviceMac.desired);
    }

    if (!pairingMode.isFulfilled())
    {
        sendSet<THMSGV2T2::PeripheralStatusPairingDeviceManagementCommon>(
            THMSGV2T2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE,
            pairingMode.desired ? THMSGV2T2::PeripheralBluetoothMode::INQUIRY_SCAN_MODE : THMSGV2T2::PeripheralBluetoothMode::NORMAL_MODE,
            /*enableDisableStatus*/ true
        );
        pairingMode.fulfill();
    }

    if (supports(MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
    {
        if (!stcEnabled.isFulfilled())
        {
            sendSet<THMSGV2T1::SystemParamSmartTalking>(
                THMSGV2T1::SystemInquiredType::SMART_TALKING_MODE_TYPE2,
                stcEnabled.desired,
                /*previewModeOnOffValue*/ false
            );
            stcEnabled.fulfill();
        }

        if (!stcLevel.isFulfilled() || !stcTime.isFulfilled())
        {
            sendSet<THMSGV2T1::SystemExtParamSmartTalkingMode2>(stcLevel.desired, stcTime.desired);
            stcLevel.fulfill();
            stcTime.fulfill();
        }
    }

    if (supports(MessageMdrV2FunctionType_Table1::LISTENING_OPTION) && !listeningModeConfig.isFulfilled())
    {
        if (listeningModeConfig.desired.bgmActive ||
            (listeningModeConfig.current.bgmActive && listeningModeConfig.desired.nonBgmMode == ListeningMode::Standard))
        {
            sendSet<THMSGV2T1::AudioParamBGMMode>(
                THMSGV2T1::AudioInquiredType::BGM_MODE,
                listeningModeConfig.desired.bgmActive,
                listeningModeConfig.desired.bgmDistanceMode
            );
        }
        else
        {
            sendSet<THMSGV2T1::AudioParamUpmixCinema>(
                listeningModeConfig.desired.nonBgmMode == ListeningMode::Cinema
            );
        }
        waitForAck();
        listeningModeConfig.fulfill();
    }

    if (!eqPreset.isFulfilled())
    {
        std::span<uint8_t> noBands;
        sendSet<THMSGV2T1::EqEbbParamEq>(noBands, THMSGV2T1::EqEbbInquiredType::PRESET_EQ, eqPreset.desired);
        eqPreset.fulfill();
        // Ask for a equalizer param update afterwards
        sendGet<THMSGV2T1::EqEbbGetParam>(THMSGV2T1::EqEbbInquiredType::PRESET_EQ);
    }

    if (!eqConfig.isFulfilled())
    {
        size_t numBands = eqConfig.desired.bands.size();
        if (numBands == 5)
        {
            uint8_t bands[] = {
                static_cast<uint8_t>(eqConfig.desired.bassLevel + 10),
                static_cast<uint8_t>(eqConfig.desired.bands[0] + 10),
                static_cast<uint8_t>(eqConfig.desired.bands[1] + 10),
                static_cast<uint8_t>(eqConfig.desired.bands[2] + 10),
                static_cast<uint8_t>(eqConfig.desired.bands[3] + 10),
                static_cast<uint8_t>(eqConfig.desired.bands[4] + 10)
            };
            sendSet<THMSGV2T1::EqEbbParamEq>(bands, THMSGV2T1::EqEbbInquiredType::PRESET_EQ, eqPreset.current);
        }
        else if (numBands == 10)
        {
            uint8_t bands[] = {
                static_cast<uint8_t>(eqConfig.desired.bands[0] + 6),
                static_cast<uint8_t>(eqConfig.desired.bands[1] + 6),
                static_cast<uint8_t>(eqConfig.desired.bands[2] + 6),
                static_cast<uint8_t>(eqConfig.desired.bands[3] + 6),
                static_cast<uint8_t>(eqConfig.desired.bands[4] + 6),
                static_cast<uint8_t>(eqConfig.desired.bands[5] + 6),
                static_cast<uint8_t>(eqConfig.desired.bands[6] + 6),
                static_cast<uint8_t>(eqConfig.desired.bands[7] + 6),
                static_cast<uint8_t>(eqConfig.desired.bands[8] + 6),
                static_cast<uint8_t>(eqConfig.desired.bands[9] + 6)
            };
            sendSet<THMSGV2T1::EqEbbParamEq>(bands, THMSGV2T1::EqEbbInquiredType::PRESET_EQ, eqPreset.current);
        }
        else
        {
            throw std::runtime_error("Invalid number of bands for equalizer setting");
        }
        eqConfig.fulfill();
    }

    if (supports(MessageMdrV2FunctionType_Table1::ASSIGNABLE_SETTING)
        && (!touchLeftFunc.isFulfilled() || !touchRightFunc.isFulfilled()))
    {
        THMSGV2T1::Preset presets[] = { touchLeftFunc.desired, touchRightFunc.desired };
        sendSet<THMSGV2T1::SystemParamAssignableSettings>(presets);
        touchLeftFunc.fulfill();
        touchRightFunc.fulfill();
    }

    if (supports(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT) && !ncAmbButtonMode.isFulfilled())
    {
        sendSet<THMSGV2T1::NcAsmParamNcAmbToggle>(ncAmbButtonMode.desired);
        ncAmbButtonMode.fulfill();
    }

    if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1) && !gs1.isFulfilled())
    {
        sendSet<THMSGV2T1::GsParamBoolean>(
            THMSGV2T1::GsInquiredType::GENERAL_SETTING1,
            gs1.desired ? THMSGV2T1::GsSettingValue::ON : THMSGV2T1::GsSettingValue::OFF
        );
        gs1.fulfill();
    }
    if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_2) && !gs2.isFulfilled())
    {
        sendSet<THMSGV2T1::GsParamBoolean>(
            THMSGV2T1::GsInquiredType::GENERAL_SETTING2,
            gs2.desired ? THMSGV2T1::GsSettingValue::ON : THMSGV2T1::GsSettingValue::OFF
        );
        gs2.fulfill();
    }
    if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3) && !gs3.isFulfilled())
    {
        sendSet<THMSGV2T1::GsParamBoolean>(
            THMSGV2T1::GsInquiredType::GENERAL_SETTING3,
            gs3.desired ? THMSGV2T1::GsSettingValue::ON : THMSGV2T1::GsSettingValue::OFF
        );
        gs3.fulfill();
    }
    if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_4) && !gs4.isFulfilled())
    {
        sendSet<THMSGV2T1::GsParamBoolean>(
            THMSGV2T1::GsInquiredType::GENERAL_SETTING4,
            gs4.desired ? THMSGV2T1::GsSettingValue::ON : THMSGV2T1::GsSettingValue::OFF
        );
        gs4.fulfill();
    }

    if (!safeListening.preview.isFulfilled())
    {
        if (supports(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_1))
        {
            sendSet<THMSGV2T2::SafeListeningSetParamSL>(
                THMSGV2T2::SafeListeningInquiredType::SAFE_LISTENING_HBS_1,
                false,
                safeListening.preview.desired
            );
        }
        else if (supports(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_2))
        {
            sendSet<THMSGV2T2::SafeListeningSetParamSL>(
                THMSGV2T2::SafeListeningInquiredType::SAFE_LISTENING_HBS_2,
                false,
                safeListening.preview.desired
            );
        }
        else if (supports(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_1))
        {
            sendSet<THMSGV2T2::SafeListeningSetParamSL>(
                THMSGV2T2::SafeListeningInquiredType::SAFE_LISTENING_TWS_1,
                false,
                safeListening.preview.desired
            );
        }
        else if (supports(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_2))
        {
            sendSet<THMSGV2T2::SafeListeningSetParamSL>(
                THMSGV2T2::SafeListeningInquiredType::SAFE_LISTENING_TWS_2,
                false,
                safeListening.preview.desired
            );
        }
        safeListening.preview.fulfill();
    }
}

void Headphones::requestInit()
{
    static constexpr int kImportantDataRequestTimeout = 5;

    /* Init */
    // NOTE: It's observed that playback metadata NOTIFYs (see _handleMessage) is sent by the device after this...
    sendGet<THMSGV2T1::ConnectGetProtocolInfo>(THMSGV2T1::ConnectInquiredType::FIXED_VALUE);
    waitForProtocolInfo(kImportantDataRequestTimeout);
    if (!supportsTable1)
    {
        throw std::runtime_error("Unsupported device: does not support MDR V2 Table 1");
    }

    /* Capability Info */
    sendGet<THMSGV2T1::ConnectGetCapabilityInfo>(THMSGV2T1::ConnectInquiredType::FIXED_VALUE);

    // Following are cached by the app based on the MAC address
    {
        /* Firmware Info */
        sendGet<THMSGV2T1::ConnectGetDeviceInfo>(THMSGV2T1::DeviceInfoType::MODEL_NAME);
        sendGet<THMSGV2T1::ConnectGetDeviceInfo>(THMSGV2T1::DeviceInfoType::FW_VERSION);
        sendGet<THMSGV2T1::ConnectGetDeviceInfo>(THMSGV2T1::DeviceInfoType::SERIES_AND_COLOR_INFO);

        /* Support Functions */
        sendGet<THMSGV2T1::ConnectGetSupportFunction>(THMSGV2T1::ConnectInquiredType::FIXED_VALUE);
        waitForSupportFunction(kImportantDataRequestTimeout);
        if (supportsTable2)
        {
            sendGet<THMSGV2T2::ConnectGetSupportFunction>(THMSGV2T2::ConnectInquiredType::FIXED_VALUE);
            waitForSupportFunction(kImportantDataRequestTimeout);
        }

        /* Capabilities */
        if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1))
        {
            sendGet<THMSGV2T1::GsGetCapability>(
                THMSGV2T1::GsInquiredType::GENERAL_SETTING1, THMSGV2T1::DisplayLanguage::ENGLISH);
        }
        if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_2))
        {
            sendGet<THMSGV2T1::GsGetCapability>(
                THMSGV2T1::GsInquiredType::GENERAL_SETTING2, THMSGV2T1::DisplayLanguage::ENGLISH);
        }
        if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3))
        {
            sendGet<THMSGV2T1::GsGetCapability>(
                THMSGV2T1::GsInquiredType::GENERAL_SETTING3, THMSGV2T1::DisplayLanguage::ENGLISH);
        }
        if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_4))
        {
            sendGet<THMSGV2T1::GsGetCapability>(
                THMSGV2T1::GsInquiredType::GENERAL_SETTING4, THMSGV2T1::DisplayLanguage::ENGLISH);
        }
    }

    if (supports(MessageMdrV2FunctionType_Table1::FIXED_MESSAGE))
    {
        /* Receive alerts for certain operations like toggling multipoint */
        sendSetAndForget<THMSGV2T1::AlertSetStatusFixedMessage>(true);
    }

    /* Playback Metadata */
    sendGet<THMSGV2T1::GetPlayParam>(THMSGV2T1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT);

    /* Playback Volume */
    sendGet<THMSGV2T1::GetPlayParam>(THMSGV2T1::PlayInquiredType::MUSIC_VOLUME);

    /* Play/Pause */
    sendGet<THMSGV2T1::GetPlayStatus>(THMSGV2T1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT);

    if (supports(MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
    {
        sendGet<THMSGV2T1::NcAsmGetParam>(THMSGV2T1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS);
    }
    else if (supports(MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
    {
        sendGet<THMSGV2T1::NcAsmGetParam>(THMSGV2T1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA);
    }
    else if (supports(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
    {
        sendGet<THMSGV2T1::NcAsmGetParam>(THMSGV2T1::NcAsmInquiredType::ASM_SEAMLESS);
    }

    if (supportsTable2 && supportsPairingDeviceManagement())
    {
        /* Pairing Mode*/
        sendGet<THMSGV2T2::PeripheralGetStatus>(THMSGV2T2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE);

        /* Connected Devices */
        sendGet<THMSGV2T2::PeripheralGetParam>(THMSGV2T2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE);
    }

    if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1))
    {
        sendGet<THMSGV2T1::GsGetParam>(THMSGV2T1::GsInquiredType::GENERAL_SETTING1);
    }
    if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_2))
    {
        sendGet<THMSGV2T1::GsGetParam>(THMSGV2T1::GsInquiredType::GENERAL_SETTING2);
    }
    if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3))
    {
        sendGet<THMSGV2T1::GsGetParam>(THMSGV2T1::GsInquiredType::GENERAL_SETTING3);
    }
    if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_4))
    {
        sendGet<THMSGV2T1::GsGetParam>(THMSGV2T1::GsInquiredType::GENERAL_SETTING4);
    }

    if (supports(MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
    {
        /* Speak to chat */
        sendGet<THMSGV2T1::SystemGetParam>(THMSGV2T1::SystemInquiredType::SMART_TALKING_MODE_TYPE2); // Enabled/disabled
        sendGet<THMSGV2T1::SystemGetExtParam>(THMSGV2T1::SystemInquiredType::SMART_TALKING_MODE_TYPE2); // Configuration
    }

    if (supports(MessageMdrV2FunctionType_Table1::LISTENING_OPTION))
    {
        /* Listening Mode */
        sendGet<THMSGV2T1::AudioGetParam>(THMSGV2T1::AudioInquiredType::BGM_MODE);
        sendGet<THMSGV2T1::AudioGetParam>(THMSGV2T1::AudioInquiredType::UPMIX_CINEMA);
    }

    if (supports(MessageMdrV2FunctionType_Table1::ASSIGNABLE_SETTING))
    {
        /* Touch Sensor */
        sendGet<THMSGV2T1::SystemGetParam>(THMSGV2T1::SystemInquiredType::ASSIGNABLE_SETTINGS);
    }

    if (supports(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT))
    {
        sendGet<THMSGV2T1::NcAsmGetParam>(THMSGV2T1::NcAsmInquiredType::NC_AMB_TOGGLE);
    }

    /* Equalizer */
    if ((deviceCapabilities & DC_EqualizerAvailableCommand) != 0)
    {
        sendGet<THMSGV2T1::EqEbbGetStatus>(THMSGV2T1::EqEbbInquiredType::PRESET_EQ);
    }

    /* Equalizer */
    sendGet<THMSGV2T1::EqEbbGetParam>(THMSGV2T1::EqEbbInquiredType::PRESET_EQ);

    /* Automatic Power Off */
    if (supports(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF))
    {
        sendGet<THMSGV2T1::PowerGetParam>(THMSGV2T1::PowerInquiredType::AUTO_POWER_OFF);
    }
    else if (supports(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
    {
        sendGet<THMSGV2T1::PowerGetParam>(THMSGV2T1::PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION);
    }

    /* Pause when headphones are removed */
    sendGet<THMSGV2T1::SystemGetParam>(THMSGV2T1::SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING);

    if (supportsTable2)
    {
        /* Voice Guidance Enabled */
        sendGet<THMSGV2T2::VoiceGuidanceGetParam>(THMSGV2T2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH);

        /* Voice Guidance Volume */
        sendGet<THMSGV2T2::VoiceGuidanceGetParam>(THMSGV2T2::VoiceGuidanceInquiredType::VOLUME);
    }

#ifdef _DEBUG
    _conn.sendCommand(Buffer{
        static_cast<uint8_t>(THMSGV2T1::Command::LOG_SET_STATUS),
        0x00, // Some data in JSON format will be sent afterward...
        0x00  // See _handleMessage for info
    }, DATA_TYPE::DATA_MDR);
    waitForAck();
#endif

    _conn.sendCommand(Buffer{
        static_cast<uint8_t>(THMSGV2T1::Command::LOG_SET_STATUS),
        0x01, // MORE data...?! why not..
        0x00  // After this command, the triple-tap (and other app-related functions) will
              // not trigger 'app not launched' notifications on the headset
              // I wonder if we should handle them in this app...
    }, DATA_TYPE::DATA_MDR);
    waitForAck();
}

void Headphones::requestSync()
{
    /* Single Battery */
    if (supports(MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_INDICATOR))
    {
        sendGet<THMSGV2T1::PowerGetStatus>(THMSGV2T1::PowerInquiredType::BATTERY);
    }
    else if (supports(MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_WITH_THRESHOLD))
    {
        sendGet<THMSGV2T1::PowerGetStatus>(THMSGV2T1::PowerInquiredType::BATTERY_WITH_THRESHOLD);
    }

    /* L+R Battery */
    if (supports(MessageMdrV2FunctionType_Table1::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR))
    {
        sendGet<THMSGV2T1::PowerGetStatus>(THMSGV2T1::PowerInquiredType::LEFT_RIGHT_BATTERY);
    }
    else if (supports(MessageMdrV2FunctionType_Table1::LR_BATTERY_LEVEL_WITH_THRESHOLD))
    {
        sendGet<THMSGV2T1::PowerGetStatus>(THMSGV2T1::PowerInquiredType::LR_BATTERY_WITH_THRESHOLD);
    }

    /* Case Battery */
    if (supports(MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_INDICATOR))
    {
        sendGet<THMSGV2T1::PowerGetStatus>(THMSGV2T1::PowerInquiredType::CRADLE_BATTERY);
    }
    else if (supports(MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD))
    {
        sendGet<THMSGV2T1::PowerGetStatus>(THMSGV2T1::PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD);
    }

    if (safeListening.preview.current)
    {
        /* Sound Pressure */
        if (supports(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_1))
        {
            sendGet<THMSGV2T2::SafeListeningGetExtendedParam>(THMSGV2T2::SafeListeningInquiredType::SAFE_LISTENING_HBS_1);
        }
        else if (supports(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_2))
        {
            sendGet<THMSGV2T2::SafeListeningGetExtendedParam>(THMSGV2T2::SafeListeningInquiredType::SAFE_LISTENING_HBS_2);
        }
        else if (supports(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_1))
        {
            sendGet<THMSGV2T2::SafeListeningGetExtendedParam>(THMSGV2T2::SafeListeningInquiredType::SAFE_LISTENING_TWS_1);
        }
        else if (supports(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_2))
        {
            sendGet<THMSGV2T2::SafeListeningGetExtendedParam>(THMSGV2T2::SafeListeningInquiredType::SAFE_LISTENING_TWS_2);
        }
    }
}

void Headphones::recvAsync()
{
    if (_conn.isConnected())
        _recvFuture.setFromAsync([this]() -> std::optional<HeadphonesMessage> {
            auto& conn = this->getConn();
            HeadphonesMessage cmd;
            conn.recvCommand(cmd);
            return cmd;
        });
}

void Headphones::requestPlaybackControl(THMSGV2T1::PlaybackControl control)
{
    sendSet<THMSGV2T1::SetPlayStatusPlaybackController>(
        THMSGV2T1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT,
        /*status*/ true,
        control
    );
}

void Headphones::respondToFixedMessageAlert(THMSGV2T1::AlertMessageType messageType, THMSGV2T1::AlertAction action)
{
    sendSet<THMSGV2T1::AlertSetParamFixedMessage>(messageType, action);
}

void Headphones::requestPowerOff()
{
    if (supports(MessageMdrV2FunctionType_Table1::POWER_OFF))
    {
        sendSet<THMSGV2T1::PowerSetStatusPowerOff>(THMSGV2T1::PowerOffSettingValue::USER_POWER_OFF);
    }
}

void Headphones::disconnectDevice(const std::string& mac)
{
    if (supports(MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
    {
        sendSet<THMSGV2T2::PeripheralSetExtendedParamParingDeviceManagementCommon>(
            THMSGV2T2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT,
            THMSGV2T2::ConnectivityActionType::DISCONNECT,
            mac
        );
    }
    else if (supports(MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT)
        || supports(MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE))
    {
        sendSet<THMSGV2T2::PeripheralSetExtendedParamParingDeviceManagementCommon>(
            THMSGV2T2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE,
            THMSGV2T2::ConnectivityActionType::DISCONNECT,
            mac
        );
    }
}

void Headphones::connectDevice(const std::string& mac)
{
    if (supports(MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
    {
        sendSet<THMSGV2T2::PeripheralSetExtendedParamParingDeviceManagementCommon>(
            THMSGV2T2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT,
            THMSGV2T2::ConnectivityActionType::CONNECT,
            mac
        );
    }
    else if (supports(MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT)
        || supports(MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE))
    {
        sendSet<THMSGV2T2::PeripheralSetExtendedParamParingDeviceManagementCommon>(
            THMSGV2T2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE,
            THMSGV2T2::ConnectivityActionType::CONNECT,
            mac
        );
    }
}

void Headphones::unpairDevice(const std::string& mac)
{
    if (supports(MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT))
    {
        sendSet<THMSGV2T2::PeripheralSetExtendedParamParingDeviceManagementCommon>(
            THMSGV2T2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT,
            THMSGV2T2::ConnectivityActionType::UNPAIR,
            mac
        );
    }
    else if (supports(MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT)
        || supports(MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE))
    {
        sendSet<THMSGV2T2::PeripheralSetExtendedParamParingDeviceManagementCommon>(
            THMSGV2T2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE,
            THMSGV2T2::ConnectivityActionType::UNPAIR,
            mac
        );
    }
}

HeadphonesEvent Headphones::_handleProtocolInfo(const HeadphonesMessage& msg)
{
    hasInit = true;

    auto payload = msg.as<THMSGV2T1::ConnectRetProtocolInfo>();

    protocolVersion = payload->getProtocolVersion();
    supportsTable1 = payload->supportTable1Value;
    supportsTable2 = payload->supportTable2Value;

    deviceCapabilities = DC_None;

    if (protocolVersion >= 0x03002017) // WF-1000XM5
    {
        deviceCapabilities |= DC_EqualizerAvailableCommand;
    }

    _protocolInfoCV.notify_all();
    return HeadphonesEvent::Initialized;
}

HeadphonesEvent Headphones::_handleCapabilityInfo(const HeadphonesMessage& msg)
{
    // auto payload = msg.as<THMSGV2T1::ConnectRetCapabilityInfo>();
    switch (static_cast<THMSGV2T1::ConnectInquiredType>(msg[1]))
    {
    case THMSGV2T1::ConnectInquiredType::FIXED_VALUE:
        capabilityCounter.overwrite(msg[2]);
        size_t uniqueIdLen = std::min<size_t>(msg[3], 128);
        char* uniqueIdBegin = (char*)(msg.data() + 4);
        uniqueId.overwrite(std::string(uniqueIdBegin, uniqueIdBegin + uniqueIdLen));
        return HeadphonesEvent::DeviceInfoUpdate;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleDeviceInfo(const HeadphonesMessage& msg)
{
    auto payload = msg.as<THMSGV2T1::ConnectRetDeviceInfo>();
    switch (payload->type)
    {
    case THMSGV2T1::DeviceInfoType::MODEL_NAME:
    {
        auto payloadSub = msg.as<THMSGV2T1::ConnectRetDeviceInfoModelName>();
        modelName.overwrite(payloadSub->getModelName());
        return HeadphonesEvent::DeviceInfoUpdate;
    }
    case THMSGV2T1::DeviceInfoType::FW_VERSION:
    {
        auto payloadSub = msg.as<THMSGV2T1::ConnectRetDeviceInfoFwVersion>();
        fwVersion.overwrite(payloadSub->getFwVersion());
        return HeadphonesEvent::DeviceInfoUpdate;
    }
    case THMSGV2T1::DeviceInfoType::SERIES_AND_COLOR_INFO:
    {
        auto payloadSub = msg.as<THMSGV2T1::ConnectRetDeviceInfoSeriesAndColor>();
        modelSeries.overwrite(payloadSub->modelSeries);
        modelColor.overwrite(payloadSub->modelColor);
        return HeadphonesEvent::DeviceInfoUpdate;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleT1SupportFunction(const HeadphonesMessage& msg)
{
    auto payload = msg.as<THMSGV2T1::ConnectRetSupportFunction>();
    switch (payload->inquiredType)
    {
    case THMSGV2T1::ConnectInquiredType::FIXED_VALUE:
    {
        this->supportFunctions1.reset();
        std::ostringstream oss;
        oss << std::uppercase << std::setfill('0');
        bool first = true;
        for (const MessageMdrV2SupportFunction& sf : payload->getSupportFunctions())
        {
            this->supportFunctions1.set(static_cast<uint8_t>(sf.functionType.table1));
            if (!first) oss << '\n';
            const char* functionTypeStr = MessageMdrV2FunctionType_Table1ToString(sf.functionType.table1);
            if (functionTypeStr)
                oss << functionTypeStr;
            else
                oss << "Unknown0x" << std::hex << std::setw(2) << static_cast<int>(sf.functionType.table1);
            oss << " P:" << static_cast<int>(sf.priority);
            first = false;
        }
        supportFunctionString1.overwrite(oss.str());
        _supportFunctionCV.notify_all();
        return HeadphonesEvent::SupportFunctionUpdate;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleT2SupportFunction(const HeadphonesMessage& msg)
{
    auto payload = msg.as<THMSGV2T2::ConnectRetSupportFunction>();
    switch (payload->inquiredType)
    {
    case THMSGV2T2::ConnectInquiredType::FIXED_VALUE:
    {
        this->supportFunctions2.reset();
        std::ostringstream oss;
        oss << std::uppercase << std::setfill('0');
        bool first = true;
        for (const MessageMdrV2SupportFunction& sf : payload->getSupportFunctions())
        {
            this->supportFunctions2.set(static_cast<uint8_t>(sf.functionType.table2));
            if (!first) oss << '\n';
            const char* functionTypeStr = MessageMdrV2FunctionType_Table2ToString(sf.functionType.table2);
            if (functionTypeStr)
                oss << functionTypeStr;
            else
                oss << "Unknown0x" << std::hex << std::setw(2) << static_cast<int>(sf.functionType.table2);
            oss << " P:" << static_cast<int>(sf.priority);
            first = false;
        }
        supportFunctionString2.overwrite(oss.str());
        _supportFunctionCV.notify_all();
        return HeadphonesEvent::SupportFunctionUpdate;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleNcAsmParam(const HeadphonesMessage& msg, CommandType ct)
{
    auto payload = msg.as<THMSGV2T1::NcAsmParam>(ct);
    switch (payload->type)
    {
    case THMSGV2T1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS:
    {
        if (supports(MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            auto payloadSub = msg.as<THMSGV2T1::NcAsmParamModeNcDualModeSwitchAsmSeamless>(ct);
            asmEnabled.overwrite(payloadSub->ncAsmTotalEffect == THMSGV2T1::NcAsmOnOffValue::ON);
            asmMode.overwrite(payloadSub->ncAsmMode);
            asmFoucsOnVoice.overwrite(payloadSub->ambientSoundMode == THMSGV2T1::AmbientSoundMode::VOICE);
            asmLevel.overwrite(payloadSub->ambientSoundLevelValue);
            return HeadphonesEvent::NcAsmParamUpdate;
        }
        break;
    }
    case THMSGV2T1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA:
    {
        if (supports(MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
        {
            auto payloadSub = msg.as<THMSGV2T1::NcAsmParamModeNcDualModeSwitchAsmSeamlessNa>(ct);
            asmEnabled.overwrite(payloadSub->ncAsmTotalEffect == THMSGV2T1::NcAsmOnOffValue::ON);
            asmMode.overwrite(payloadSub->ncAsmMode);
            asmFoucsOnVoice.overwrite(payloadSub->ambientSoundMode == THMSGV2T1::AmbientSoundMode::VOICE);
            asmLevel.overwrite(payloadSub->ambientSoundLevelValue);
            autoAsmEnabled.overwrite(payloadSub->noiseAdaptiveOnOffValue == THMSGV2T1::NcAsmOnOffValue::ON);
            autoAsmSensitivity.overwrite(payloadSub->noiseAdaptiveSensitivitySettings);
            return HeadphonesEvent::NcAsmParamUpdate;
        }
        break;
    }
    case THMSGV2T1::NcAsmInquiredType::ASM_SEAMLESS:
    {
        if (supports(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            auto payloadSub = msg.as<THMSGV2T1::NcAsmParamAsmSeamless>(ct);
            asmEnabled.overwrite(payloadSub->ncAsmTotalEffect == THMSGV2T1::NcAsmOnOffValue::ON);
            asmFoucsOnVoice.overwrite(payloadSub->ambientSoundMode == THMSGV2T1::AmbientSoundMode::VOICE);
            asmLevel.overwrite(payloadSub->ambientSoundLevelValue);
            return HeadphonesEvent::NcAsmParamUpdate;
        }
        break;
    }
    case THMSGV2T1::NcAsmInquiredType::NC_AMB_TOGGLE:
    {
        if (supports(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT))
        {
            auto payloadSub = msg.as<THMSGV2T1::NcAsmParamNcAmbToggle>(ct);
            ncAmbButtonMode.overwrite(payloadSub->function);
            return HeadphonesEvent::NcAmbButtonModeUpdate;
        }
        break;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleBatteryLevelRet(const HeadphonesMessage& msg)
{
    auto payload = msg.as<THMSGV2T1::PowerRetStatus>();
    switch (payload->type)
    {
    case THMSGV2T1::PowerInquiredType::BATTERY:
    {
        if (supports(MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_INDICATOR))
        {
            auto payloadSub = msg.as<THMSGV2T1::PowerRetStatusBattery>();
            statBatteryL.overwrite(BatteryData{ payloadSub->batteryLevel, payloadSub->chargingStatus, 0xFF });
            return HeadphonesEvent::BatteryLevelUpdate;
        }
        break;
    }
    case THMSGV2T1::PowerInquiredType::LEFT_RIGHT_BATTERY:
    {
        if (supports(MessageMdrV2FunctionType_Table1::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR))
        {
            auto payloadSub = msg.as<THMSGV2T1::PowerRetStatusLeftRightBattery>();
            statBatteryL.overwrite(BatteryData{ payloadSub->leftBatteryLevel, payloadSub->leftChargingStatus, 0xFF });
            statBatteryR.overwrite(BatteryData{ payloadSub->rightBatteryLevel, payloadSub->rightChargingStatus, 0xFF });
            return HeadphonesEvent::BatteryLevelUpdate;
        }
        break;
    }
    case THMSGV2T1::PowerInquiredType::CRADLE_BATTERY:
    {
        if (supports(MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_INDICATOR))
        {
            auto payloadSub = msg.as<THMSGV2T1::PowerRetStatusCradleBattery>();
            statBatteryCase.overwrite(BatteryData{ payloadSub->batteryLevel, payloadSub->chargingStatus, 0xFF });
            return HeadphonesEvent::BatteryLevelUpdate;
        }
        break;
    }
    case THMSGV2T1::PowerInquiredType::BATTERY_WITH_THRESHOLD:
    {
        if (supports(MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_WITH_THRESHOLD))
        {
            auto payloadSub = msg.as<THMSGV2T1::PowerRetStatusBatteryThreshold>();
            statBatteryL.overwrite(BatteryData{ payloadSub->batteryLevel, payloadSub->chargingStatus, payloadSub->batteryThreshold });
            return HeadphonesEvent::BatteryLevelUpdate;
        }
        break;
    }
    case THMSGV2T1::PowerInquiredType::LR_BATTERY_WITH_THRESHOLD:
    {
        if (supports(MessageMdrV2FunctionType_Table1::LR_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            auto payloadSub = msg.as<THMSGV2T1::PowerRetStatusLeftRightBatteryThreshold>();
            statBatteryL.overwrite(BatteryData{ payloadSub->leftBatteryLevel, payloadSub->leftChargingStatus, payloadSub->leftBatteryThreshold });
            statBatteryR.overwrite(BatteryData{ payloadSub->rightBatteryLevel, payloadSub->rightChargingStatus, payloadSub->rightBatteryThreshold });
            return HeadphonesEvent::BatteryLevelUpdate;
        }
        break;
    }
    case THMSGV2T1::PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD:
    {
        if (supports(MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            auto payloadSub = msg.as<THMSGV2T1::PowerRetStatusCradleBatteryThreshold>();
            statBatteryCase.overwrite(BatteryData{ payloadSub->batteryLevel, payloadSub->chargingStatus, payloadSub->batteryThreshold });
            return HeadphonesEvent::BatteryLevelUpdate;
        }
        break;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handlePlaybackParam(const HeadphonesMessage& msg, CommandType ct)
{
    auto payload = msg.as<THMSGV2T1::PlayParam>(ct);
    switch (payload->playInquiredType)
    {
    case THMSGV2T1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT:
    {
        BufferSpan span = msg;
        auto payloadSub = THMSGV2T1::PlayParamPlaybackControllerName::deserialize(span, ct);
        playback.title = std::move(payloadSub.playbackNames[0].name);
        playback.album = std::move(payloadSub.playbackNames[1].name);
        playback.artist = std::move(payloadSub.playbackNames[2].name);
        return HeadphonesEvent::PlaybackMetadataUpdate;
    }
    case THMSGV2T1::PlayInquiredType::MUSIC_VOLUME:
    {
        auto payloadSub = msg.as<THMSGV2T1::PlayParamPlaybackControllerVolume>(ct);
        volume.overwrite(payloadSub->volumeValue);
        return HeadphonesEvent::PlaybackVolumeUpdate;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleSafeListeningNotifyParam(const HeadphonesMessage& msg)
{
    auto payload = msg.as<THMSGV2T2::SafeListeningNotifyParam>();
    switch (payload->inquiredType)
    {
    case THMSGV2T2::SafeListeningInquiredType::SAFE_LISTENING_HBS_1:
    {
        if (supports(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_1))
        {
            auto payloadSub = msg.as<THMSGV2T2::SafeListeningNotifyParamSL>();
            safeListening.preview.overwrite(payloadSub->previewMode);
            return HeadphonesEvent::SafeListeningParamUpdate;
        }
        break;
    }
    case THMSGV2T2::SafeListeningInquiredType::SAFE_LISTENING_HBS_2:
    {
        if (supports(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_2))
        {
            auto payloadSub = msg.as<THMSGV2T2::SafeListeningNotifyParamSL>();
            safeListening.preview.overwrite(payloadSub->previewMode);
            return HeadphonesEvent::SafeListeningParamUpdate;
        }
        break;
    }
    case THMSGV2T2::SafeListeningInquiredType::SAFE_LISTENING_TWS_1:
    {
        if (supports(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_1))
        {
            auto payloadSub = msg.as<THMSGV2T2::SafeListeningNotifyParamSL>();
            safeListening.preview.overwrite(payloadSub->previewMode);
            return HeadphonesEvent::SafeListeningParamUpdate;
        }
        break;
    }
    case THMSGV2T2::SafeListeningInquiredType::SAFE_LISTENING_TWS_2:
    {
        if (supports(MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_2))
        {
            auto payloadSub = msg.as<THMSGV2T2::SafeListeningNotifyParamSL>();
            safeListening.preview.overwrite(payloadSub->previewMode);
            return HeadphonesEvent::SafeListeningParamUpdate;
        }
        break;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleSafeListeningExtendedParam(const HeadphonesMessage& msg)
{
    auto payload = msg.as<THMSGV2T2::SafeListeningRetExtendedParam>();
    playback.sndPressure = payload->levelPerPeriod;
    return HeadphonesEvent::SoundPressureUpdate;
}

HeadphonesEvent Headphones::_handlePowerParam(const HeadphonesMessage& msg, CommandType ct)
{
    auto payload = msg.as<THMSGV2T1::PowerParam>(ct);
    switch (payload->type)
    {
    case THMSGV2T1::PowerInquiredType::AUTO_POWER_OFF:
    {
        if (supports(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF))
        {
            auto payloadSub = msg.as<THMSGV2T1::PowerParamAutoPowerOff>(ct);
            autoPowerOff.overwrite(payloadSub->currentPowerOffElements);
            return HeadphonesEvent::AutoPowerOffUpdate;
        }
        break;
    }
    case THMSGV2T1::PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION:
    {
        if (supports(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
        {
            auto payloadSub = msg.as<THMSGV2T1::PowerParamAutoPowerOffWithWearingDetection>(ct);
            autoPowerOffWearingDetection.overwrite(payloadSub->currentPowerOffElements);
            return HeadphonesEvent::AutoPowerOffUpdate;
        }
        break;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleVoiceGuidanceParam(const HeadphonesMessage& msg, CommandType ct)
{
    auto payload = msg.as<THMSGV2T2::VoiceGuidanceParam>(ct);
    switch (payload->inquiredType)
    {
    case THMSGV2T2::VoiceGuidanceInquiredType::MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH:
    {
        if (ct == CT_Ret)
        {
            auto payloadSub = msg.as<THMSGV2T2::VoiceGuidanceParamSettingSupportLangSwitch>(ct);
            voiceGuidanceEnabled.overwrite(payloadSub->settingValue);
        }
        else
        {
            auto payloadSub = msg.as<THMSGV2T2::VoiceGuidanceParamSettingMtk>(ct);
            voiceGuidanceEnabled.overwrite(payloadSub->settingValue);
        }
        return HeadphonesEvent::VoiceGuidanceEnabledUpdate;
    }
    case THMSGV2T2::VoiceGuidanceInquiredType::VOLUME:
    {
        if (supports(MessageMdrV2FunctionType_Table2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT))
        {
            auto payloadSub = msg.as<THMSGV2T2::VoiceGuidanceParamVolume>(ct);
            miscVoiceGuidanceVol.overwrite(payloadSub->volumeValue);
            return HeadphonesEvent::VoiceGuidanceVolumeUpdate;
        }
        break;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handlePeripheralStatus(const HeadphonesMessage& msg, CommandType ct)
{
    auto payload = msg.as<THMSGV2T2::PeripheralStatus>(ct);
    switch (payload->inquiredType)
    {
    case THMSGV2T2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE:
    {
        auto payloadSub = msg.as<THMSGV2T2::PeripheralStatusPairingDeviceManagementCommon>(ct);
        pairingMode.overwrite(payloadSub->enableDisableStatus
            && payloadSub->btMode == THMSGV2T2::PeripheralBluetoothMode::INQUIRY_SCAN_MODE);
        return HeadphonesEvent::BluetoothModeUpdate;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handlePeripheralNotifyExtendedParam(const HeadphonesMessage& msg)
{
    auto payload = msg.as<THMSGV2T2::PeripheralNotifyExtendedParam>();
    switch (payload->inquiredType)
    {
    case THMSGV2T2::PeripheralInquiredType::SOURCE_SWITCH_CONTROL:
    {
        auto payloadSub = msg.as<THMSGV2T2::PeripheralNotifyExtendedParamSourceSwitchControl>();
        mpDeviceMac.overwrite(payloadSub->getTargetBdAddress());
        return HeadphonesEvent::MultipointDeviceSwitchUpdate;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handlePeripheralParam(const HeadphonesMessage& msg, CommandType ct)
{
    auto payload = msg.as<THMSGV2T2::PeripheralParam>(ct);
    switch (payload->inquiredType)
    {
    case THMSGV2T2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT:
    {
        BufferSpan span = msg;
        auto payloadSub = THMSGV2T2::PeripheralParamPairingDeviceManagementClassicBt::deserialize(span, ct);

        connectedDevices.clear();
        pairedDevices.clear();

        // Prefill connected
        connectedDevices[1] = BluetoothDevice();
        connectedDevices[2] = BluetoothDevice();

        for (const THMSGV2T2::PeripheralDeviceInfo& device : payloadSub.deviceList)
        {
            std::string mac = device.getBtDeviceAddress();
            (device.connectedStatus > 0 ? connectedDevices[device.connectedStatus] : pairedDevices[mac]) = BluetoothDevice(device.btFriendlyName, mac);
        }

        playbackDevice = payloadSub.playbackrightDevice;
        mpDeviceMac.overwrite(connectedDevices[playbackDevice].mac);

        return HeadphonesEvent::ConnectedDeviceUpdate;
    }
    case THMSGV2T2::PeripheralInquiredType::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE:
    {
        BufferSpan span = msg;
        auto payloadSub = THMSGV2T2::PeripheralParamPairingDeviceManagementWithBluetoothClassOfDevice::deserialize(span, ct);

        connectedDevices.clear();
        pairedDevices.clear();

        // Prefill connected
        connectedDevices[1] = BluetoothDevice();
        connectedDevices[2] = BluetoothDevice();

        for (const THMSGV2T2::PeripheralDeviceInfoWithBluetoothClassOfDevice& device : payloadSub.deviceList)
        {
            std::string mac = device.getBtDeviceAddress();
            (device.connectedStatus > 0 ? connectedDevices[device.connectedStatus] : pairedDevices[mac]) = BluetoothDevice(device.btFriendlyName, mac);
        }

        playbackDevice = payloadSub.playbackrightDevice;
        mpDeviceMac.overwrite(connectedDevices[playbackDevice].mac);

        return HeadphonesEvent::ConnectedDeviceUpdate;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handlePlaybackStatus(const HeadphonesMessage& msg, CommandType ct)
{
    auto payload = msg.as<THMSGV2T1::PlayStatus>(ct);
    switch (payload->playInquiredType)
    {
    case THMSGV2T1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT:
    {
        auto payloadSub = msg.as<THMSGV2T1::PlayStatusPlaybackController>(ct);
        switch (payloadSub->playbackStatus)
        {
        case THMSGV2T1::PlaybackStatus::PLAY:
            playPause.overwrite(true);
            return HeadphonesEvent::PlaybackPlayPauseUpdate;
        case THMSGV2T1::PlaybackStatus::PAUSE:
            playPause.overwrite(false);
            return HeadphonesEvent::PlaybackPlayPauseUpdate;
        }
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleGsCapability(const HeadphonesMessage& msg)
{
    BufferSpan span = msg;
    auto payload = THMSGV2T1::GsRetCapability::deserialize(span);
    THMSGV2T1::GsInquiredType type = payload.type;
#ifdef _DEBUG
    std::cout << "Capability for " << (type == THMSGV2T1::GsInquiredType::GENERAL_SETTING1 ? "GS1" :
        type == THMSGV2T1::GsInquiredType::GENERAL_SETTING2 ? "GS2" :
        type == THMSGV2T1::GsInquiredType::GENERAL_SETTING3 ? "GS3" :
        type == THMSGV2T1::GsInquiredType::GENERAL_SETTING4 ? "GS4" : "Unknown")
        << ":" << std::endl;
    std::cout << "Setting type: " << (payload.settingType == THMSGV2T1::GsSettingType::BOOLEAN_TYPE ? "Boolean" :
        payload.settingType == THMSGV2T1::GsSettingType::LIST_TYPE ? "List" : "Unknown")
        << std::endl;
    std::cout << "Setting info:" << std::endl;
    std::cout << "  String format: " << (payload.settingInfo.stringFormat == THMSGV2T1::GsStringFormat::RAW_NAME ? "Raw Name" :
        payload.settingInfo.stringFormat == THMSGV2T1::GsStringFormat::ENUM_NAME ? "Enum Name" : "Unknown")
        << std::endl;
    std::cout << "  Subject: " << payload.settingInfo.subject << std::endl;
    std::cout << "  Summary: " << payload.settingInfo.summary << std::endl;
#endif
    switch (type)
    {
    case THMSGV2T1::GsInquiredType::GENERAL_SETTING1:
    {
        gs1c.overwrite(GsCapability{ payload.settingType, std::move(payload.settingInfo) });
        return HeadphonesEvent::DeviceInfoUpdate;
    }
    case THMSGV2T1::GsInquiredType::GENERAL_SETTING2:
    {
        gs2c.overwrite(GsCapability{ payload.settingType, std::move(payload.settingInfo) });
        return HeadphonesEvent::DeviceInfoUpdate;
    }
    case THMSGV2T1::GsInquiredType::GENERAL_SETTING3:
    {
        gs3c.overwrite(GsCapability{ payload.settingType, std::move(payload.settingInfo) });
        return HeadphonesEvent::DeviceInfoUpdate;
    }
    case THMSGV2T1::GsInquiredType::GENERAL_SETTING4:
    {
        gs4c.overwrite(GsCapability{ payload.settingType, std::move(payload.settingInfo) });
        return HeadphonesEvent::DeviceInfoUpdate;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleGeneralSettingParam(const HeadphonesMessage& msg, CommandType ct)
{
    auto payload = msg.as<THMSGV2T1::GsParam>(ct);
    switch (payload->type)
    {
    case THMSGV2T1::GsInquiredType::GENERAL_SETTING1:
    {
        if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1)
            && payload->settingType == THMSGV2T1::GsSettingType::BOOLEAN_TYPE)
        {
            auto payloadSub = msg.as<THMSGV2T1::GsParamBoolean>(ct);
            gs1.overwrite(payloadSub->settingValue == THMSGV2T1::GsSettingValue::ON);
            return HeadphonesEvent::GeneralSetting1Update;
        }
        break;
    }
    case THMSGV2T1::GsInquiredType::GENERAL_SETTING2:
    {
        if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_2)
            && payload->settingType == THMSGV2T1::GsSettingType::BOOLEAN_TYPE)
        {
            auto payloadSub = msg.as<THMSGV2T1::GsParamBoolean>(ct);
            gs2.overwrite(payloadSub->settingValue == THMSGV2T1::GsSettingValue::ON);
            return HeadphonesEvent::GeneralSetting2Update;
        }
        break;
    }
    case THMSGV2T1::GsInquiredType::GENERAL_SETTING3:
    {
        if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3)
            && payload->settingType == THMSGV2T1::GsSettingType::BOOLEAN_TYPE)
        {
            auto payloadSub = msg.as<THMSGV2T1::GsParamBoolean>(ct);
            gs3.overwrite(payloadSub->settingValue == THMSGV2T1::GsSettingValue::ON);
            return HeadphonesEvent::GeneralSetting3Update;
        }
        break;
    }
    case THMSGV2T1::GsInquiredType::GENERAL_SETTING4:
    {
        if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_4)
            && payload->settingType == THMSGV2T1::GsSettingType::BOOLEAN_TYPE)
        {
            auto payloadSub = msg.as<THMSGV2T1::GsParamBoolean>(ct);
            gs4.overwrite(payloadSub->settingValue == THMSGV2T1::GsSettingValue::ON);
            return HeadphonesEvent::GeneralSetting4Update;
        }
        break;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleAudioParam(const HeadphonesMessage& msg, CommandType ct)
{
    auto payload = msg.as<THMSGV2T1::AudioParam>(ct);
    switch (payload->type)
    {
    case THMSGV2T1::AudioInquiredType::BGM_MODE:
    {
        if (supports(MessageMdrV2FunctionType_Table1::LISTENING_OPTION))
        {
            auto payloadSub = msg.as<THMSGV2T1::AudioParamBGMMode>(ct);
            listeningModeConfig.overwrite(ListeningModeConfig(
                listeningModeConfig.current.nonBgmMode,
                payloadSub->onOffSettingValue,
                payloadSub->targetRoomSize
            ));
            return HeadphonesEvent::ListeningModeUpdate;
        }
        break;
    }
    case THMSGV2T1::AudioInquiredType::UPMIX_CINEMA:
    {
        if (supports(MessageMdrV2FunctionType_Table1::LISTENING_OPTION))
        {
            auto payloadSub = msg.as<THMSGV2T1::AudioParamUpmixCinema>(ct);
            listeningModeConfig.overwrite(ListeningModeConfig(
                payloadSub->onOffSettingValue ? ListeningMode::Cinema : ListeningMode::Standard,
                listeningModeConfig.current.bgmActive,
                listeningModeConfig.current.bgmDistanceMode
            ));
            return HeadphonesEvent::ListeningModeUpdate;
        }
        break;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleSystemParam(const HeadphonesMessage& msg, CommandType ct)
{
    auto payload = msg.as<THMSGV2T1::SystemParam>(ct);
    switch (payload->type)
    {
    case THMSGV2T1::SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING:
    {
        if (supports(MessageMdrV2FunctionType_Table1::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF))
        {
            auto payloadSub = msg.as<THMSGV2T1::SystemParamCommon>(ct);
            autoPauseEnabled.overwrite(payloadSub->settingValue);
            return HeadphonesEvent::AutoPauseUpdate;
        }
        break;
    }
    case THMSGV2T1::SystemInquiredType::SMART_TALKING_MODE_TYPE2:
    {
        if (supports(MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
        {
            auto payloadSub = msg.as<THMSGV2T1::SystemParamSmartTalking>(ct);
            stcEnabled.overwrite(payloadSub->onOffValue);
            return HeadphonesEvent::SpeakToChatEnabledUpdate;
        }
        break;
    }
    case THMSGV2T1::SystemInquiredType::ASSIGNABLE_SETTINGS:
    {
        if (supports(MessageMdrV2FunctionType_Table1::ASSIGNABLE_SETTING))
        {
            auto payloadSub = msg.as<THMSGV2T1::SystemParamAssignableSettings>(ct);
            if (payloadSub->numberOfPreset == 2)
            {
                touchLeftFunc.overwrite(payloadSub->presets[0]);
                touchRightFunc.overwrite(payloadSub->presets[1]);
                return HeadphonesEvent::TouchFunctionUpdate;
            }
        }
        break;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleSystemExtParam(const HeadphonesMessage& msg, CommandType ct)
{
    auto payload = msg.as<THMSGV2T1::SystemExtParam>(ct);
    switch (payload->type)
    {
    case THMSGV2T1::SystemInquiredType::SMART_TALKING_MODE_TYPE2:
    {
        if (supports(MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
        {
            auto payloadSub = msg.as<THMSGV2T1::SystemExtParamSmartTalkingMode2>(ct);
            stcLevel.overwrite(payloadSub->detectSensitivity);
            stcTime.overwrite(payloadSub->modeOffTime);
            return HeadphonesEvent::SpeakToChatParamUpdate;
        }
        break;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleEqEbbStatus(const HeadphonesMessage& msg, CommandType ct)
{
    auto payload = msg.as<THMSGV2T1::EqEbbStatus>(ct);
    switch (payload->type)
    {
    case THMSGV2T1::EqEbbInquiredType::PRESET_EQ:
    {
        if ((deviceCapabilities & DC_EqualizerAvailableCommand) != 0)
        {
            auto payloadSub = msg.as<THMSGV2T1::EqEbbStatusOnOff>(ct);
            eqAvailable.overwrite(payloadSub->status);
            return HeadphonesEvent::EqualizerAvailableUpdate;
        }
        break;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleEqEbbParam(const HeadphonesMessage& msg, CommandType ct)
{
    auto payload = msg.as<THMSGV2T1::EqEbbParam>(ct);
    switch (payload->type)
    {
    case THMSGV2T1::EqEbbInquiredType::PRESET_EQ:
    {
        // [RET/NOTIFY 00 a2 06] 0a/bass 0a/band1 0a/band2 0a/band3 0a/band4 0a/band5
        // values have +10 offset
        auto payloadSub = msg.as<THMSGV2T1::EqEbbParamEq>(ct);
        eqPreset.overwrite(payloadSub->presetId);
        switch (payloadSub->numberOfBandStep)
        {
        case 0:
            return HeadphonesEvent::EqualizerParamUpdate;
        case 6:
            eqConfig.overwrite(EqualizerConfig(
                payloadSub->bandSteps[0] - 10, // Clear Bass
                std::vector<int>{
                    payloadSub->bandSteps[1] - 10, // 400
                    payloadSub->bandSteps[2] - 10, // 1k
                    payloadSub->bandSteps[3] - 10, // 2.5k
                    payloadSub->bandSteps[4] - 10, // 6.3k
                    payloadSub->bandSteps[5] - 10, // 16k
                }
            ));
            return HeadphonesEvent::EqualizerParamUpdate;
        case 10:
            eqConfig.overwrite(EqualizerConfig(
                0, // Clear Bass not available
                std::vector<int>{
                    payloadSub->bandSteps[0] - 6, // 31
                    payloadSub->bandSteps[1] - 6, // 63
                    payloadSub->bandSteps[2] - 6, // 125
                    payloadSub->bandSteps[3] - 6, // 250
                    payloadSub->bandSteps[4] - 6, // 500
                    payloadSub->bandSteps[5] - 6, // 1k
                    payloadSub->bandSteps[6] - 6, // 2k
                    payloadSub->bandSteps[7] - 6, // 4k
                    payloadSub->bandSteps[8] - 6, // 8k
                    payloadSub->bandSteps[9] - 6, // 16k
                }
            ));
            return HeadphonesEvent::EqualizerParamUpdate;
        }
        break;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

struct FixedMessageAlertDefinition
{
    const char* title;
    const char* message;
};

constexpr std::array<FixedMessageAlertDefinition, 256> kFixedMessageAlertDefinitions = []{
    using enum THMSGV2T1::AlertMessageType;
    std::array<FixedMessageAlertDefinition, 256> arr{};
    arr[static_cast<size_t>(DISCONNECT_CAUSED_BY_CHANGING_MULTIPOINT)] = {
        "Reconnection is necessary",
        "The audio device are temporarily disconnected and will be reconnected again automatically."
    };
    // Add more definitions as needed
    return arr;
}();

HeadphonesEvent Headphones::_handlyAlertNotifyParam(const HeadphonesMessage& msg)
{
    auto payload = msg.as<THMSGV2T1::AlertNotifyParam>();
    switch (payload->type)
    {
    case THMSGV2T1::AlertInquiredType::FIXED_MESSAGE:
    {
        if (supports(MessageMdrV2FunctionType_Table1::FIXED_MESSAGE))
        {
            auto payloadSub = msg.as<THMSGV2T1::AlertNotifyParamFixedMessage>();
            if (payloadSub->actionType != THMSGV2T1::AlertActionType::POSITIVE_NEGATIVE)
            {
                throw std::runtime_error("Unsupported alert action type");
            }
            const FixedMessageAlertDefinition& def = kFixedMessageAlertDefinitions[static_cast<size_t>(payloadSub->messageType)];
            pushModalAlert(
                def.title ? def.title : THMSGV2T1::AlertMessageType_toString(payloadSub->messageType),
                def.message ? def.message : "",
                [this, messageType = payloadSub->messageType](bool result) -> void
                {
                    /*_requestFixedMessageMessageType = messageType;
                    _requestFixedMessageAlertAction = result ? THMSGV2T1::AlertAction::POSITIVE : THMSGV2T1::AlertAction::NEGATIVE;*/ // TODO Figure out async
                    respondToFixedMessageAlert(messageType, result ? THMSGV2T1::AlertAction::POSITIVE : THMSGV2T1::AlertAction::NEGATIVE);
                }
            );
            return HeadphonesEvent::AlertFixedMessage;
        }
        break;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleMiscDataRet(const HeadphonesMessage& msg)
{
    // NOTE: Plain text data in either JSON or button names(?)
    switch (msg[1])
    {
    case 0x01:
    {
        // NOTE: These seems to always contain button names and are null terminated
        auto it = msg.begin();
        int len = 0;
        if (msg[2]) len = msg[2], it = msg.begin() + 3; // key...
        else len = msg[3], it = msg.begin() + 4; // op...
        interactionMessage.overwrite(std::string(it, it + len));
        return HeadphonesEvent::InteractionUpdate;
    }
    case 0x00:
    {
        // NOTE: These are sent immediately after MISC_DATA_GET 0x01
        // and won't be sent preiodically afterwards
        // NOTE: These seem to always conatin JSON data
        deviceMessages.overwrite(std::string(msg.begin() + 4, msg.end()));
        return HeadphonesEvent::JSONMessage;
    }
    default:
        return HeadphonesEvent::MessageUnhandled;
    }
}

HeadphonesEvent Headphones::_handleMessage(HeadphonesMessage const& msg)
{
    HeadphonesEvent result {};
    DATA_TYPE data_type = msg.getDataType();
    rawMessage.overwrite(msg);
    switch (data_type)
    {
    case DATA_TYPE::ACK:
        _ackCV.notify_one();
        return HeadphonesEvent::NoChange;
    case DATA_TYPE::DATA_MDR:
    {
        using namespace THMSGV2T1;
        Command command = static_cast<Command>(msg[0]);
        switch (command)
        {
        case Command::CONNECT_RET_PROTOCOL_INFO:
            result = _handleProtocolInfo(msg);
            break;
        case Command::CONNECT_RET_CAPABILITY_INFO:
            result = _handleCapabilityInfo(msg);
            break;
        case Command::CONNECT_RET_DEVICE_INFO:
            result = _handleDeviceInfo(msg);
            break;
        case Command::CONNECT_RET_SUPPORT_FUNCTION:
            result = _handleT1SupportFunction(msg);
            break;
        case Command::NCASM_RET_PARAM:
            result = _handleNcAsmParam(msg, CT_Ret);
            break;
        case Command::NCASM_NTFY_PARAM:
            result = _handleNcAsmParam(msg, CT_Notify);
            break;
        case Command::POWER_RET_STATUS:
        // case COMMAND_TYPE::POWER_NTFY_STATUS: // WH-1000XM5, WH-1000XM6 send this
            result = _handleBatteryLevelRet(msg);
            break;
        case Command::PLAY_RET_PARAM:
            result = _handlePlaybackParam(msg, CT_Ret);
            break;
        case Command::PLAY_NTFY_PARAM:
            result = _handlePlaybackParam(msg, CT_Notify);
            break;
        case Command::POWER_RET_PARAM:
            result = _handlePowerParam(msg, CT_Ret);
            break;
        case Command::POWER_NTFY_PARAM:
            result = _handlePowerParam(msg, CT_Notify);
            break;
        case Command::PLAY_RET_STATUS:
            result = _handlePlaybackStatus(msg, CT_Ret);
            break;
        case Command::PLAY_NTFY_STATUS:
            result = _handlePlaybackStatus(msg, CT_Notify);
            break;
        case Command::GENERAL_SETTING_RET_CAPABILITY:
            result = _handleGsCapability(msg);
            break;
        case Command::GENERAL_SETTING_RET_PARAM:
            result = _handleGeneralSettingParam(msg, CT_Ret);
            break;
        case Command::GENERAL_SETTING_NTNY_PARAM:
            result = _handleGeneralSettingParam(msg, CT_Notify);
            break;
        case Command::AUDIO_RET_PARAM:
            result = _handleAudioParam(msg, CT_Ret);
            break;
        case Command::AUDIO_NTFY_PARAM:
            result = _handleAudioParam(msg, CT_Notify);
            break;
        case Command::SYSTEM_RET_PARAM:
            result = _handleSystemParam(msg, CT_Ret);
            break;
        case Command::SYSTEM_NTFY_PARAM:
            result = _handleSystemParam(msg, CT_Notify);
            break;
        case Command::SYSTEM_RET_EXT_PARAM:
            result = _handleSystemExtParam(msg, CT_Ret);
            break;
        case Command::SYSTEM_NTFY_EXT_PARAM:
            result = _handleSystemExtParam(msg, CT_Notify);
            break;
        case Command::EQEBB_RET_STATUS:
            result = _handleEqEbbStatus(msg, CT_Ret);
            break;
        case Command::EQEBB_NTFY_STATUS:
            result = _handleEqEbbStatus(msg, CT_Notify);
            break;
        case Command::EQEBB_RET_PARAM:
            result = _handleEqEbbParam(msg, CT_Ret);
            break;
        case Command::EQEBB_NTFY_PARAM:
            result = _handleEqEbbParam(msg, CT_Notify);
            break;
        case Command::ALERT_NTFY_PARAM:
            result = _handlyAlertNotifyParam(msg);
            break;
        case Command::LOG_NTFY_PARAM:
            result = _handleMiscDataRet(msg);
            break;
        default:
            // Command type not recognized
            break;
        }
        /*if (_waitForResponseDataType == DATA_TYPE::DATA_MDR
            && _waitForResponseCommandId == static_cast<uint8_t>(command))
        {
            _waitForResponseDataType = DATA_TYPE::UNKNOWN;
            _waitForResponseCommandId = 0xFF;
            _responseCV.notify_one();
        }*/
        _conn.sendAck(msg.getSeqNumber());
        break;
    }
    case DATA_TYPE::DATA_MDR_NO2:
    {
        using namespace THMSGV2T2;
        Command command = static_cast<Command>(msg[0]);
        switch (command)
        {
        case Command::CONNECT_RET_SUPPORT_FUNCTION:
            result = _handleT2SupportFunction(msg);
            break;
        case Command::VOICE_GUIDANCE_RET_PARAM:
            result = _handleVoiceGuidanceParam(msg, CT_Ret);
            break;
        case Command::VOICE_GUIDANCE_NTFY_PARAM:
            result = _handleVoiceGuidanceParam(msg, CT_Notify);
            break;
        case Command::PERI_RET_STATUS:
            result = _handlePeripheralStatus(msg, CT_Ret);
            break;
        case Command::PERI_NTFY_STATUS:
            result = _handlePeripheralStatus(msg, CT_Notify);
            break;
        case Command::PERI_NTFY_EXTENDED_PARAM:
            result = _handlePeripheralNotifyExtendedParam(msg);
            break;
        case Command::PERI_RET_PARAM:
            result = _handlePeripheralParam(msg, CT_Ret);
            break;
        case Command::PERI_NTFY_PARAM:
            result = _handlePeripheralParam(msg, CT_Notify);
            break;
        case Command::SAFE_LISTENING_NTFY_PARAM:
            result = _handleSafeListeningNotifyParam(msg);
            break;
        case Command::SAFE_LISTENING_RET_EXTENDED_PARAM:
            result = _handleSafeListeningExtendedParam(msg);
            break;
        default:
            // Command type not recognized
            break;
        }
        /*if (_waitForResponseDataType == DATA_TYPE::DATA_MDR_NO2
            && _waitForResponseCommandId == static_cast<uint8_t>(command))
        {
            _waitForResponseDataType = DATA_TYPE::UNKNOWN;
            _waitForResponseCommandId = 0xFF;
            _responseCV.notify_one();
        }*/
        _conn.sendAck(msg.getSeqNumber());
        break;
    }
    default:
        // Data type not recognized
        break;
    }
    return result;
}

HeadphonesEvent Headphones::poll()
{
    if (_recvFuture.ready())
    {
        auto cmd = this->_recvFuture.get();
        if (cmd.has_value())
        {
            recvAsync();
            return _handleMessage(cmd.value());
        }
    }
    return HeadphonesEvent::NoMessage;
}

void Headphones::disconnect()
{
    _conn.disconnect();
    _ackCV.notify_all();
    _protocolInfoCV.notify_all();
    _supportFunctionCV.notify_all();
    // _responseCV.notify_all();
}

Headphones::~Headphones()
{
    if (_conn.isConnected())
        disconnect();
    _ackCV.notify_all();
}
