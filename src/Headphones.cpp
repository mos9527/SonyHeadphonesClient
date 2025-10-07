#include "Headphones.h"

#include <stdexcept>

#include "CommandSerializer.h"
#include "ProtocolV2T1.h"

Headphones::Headphones(BluetoothWrapper& conn) : _conn(conn), _recvFuture("receive"), _requestFuture("request"), _sendCommandFuture("send cmd")
{
    recvAsync();
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

bool Headphones::supportsAutoPowerOff() const
{
    using F1 = MessageMdrV2FunctionType_Table1;
    return /*supports(F1::AUTO_POWER_OFF)
        ||*/ supports(F1::AUTO_POWER_OFF_WITH_WEARING_DETECTION);
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
        && autoPowerOffEnabled.isFulfilled()
        && autoPauseEnabled.isFulfilled()
        && voiceGuidanceEnabled.isFulfilled()
        && miscVoiceGuidanceVol.isFulfilled()
        && volume.isFulfilled()
        && mpDeviceMac.isFulfilled()
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

    if (supportsAutoPowerOff() && !autoPowerOffEnabled.isFulfilled())
    {
        sendSet<THMSGV2T1::PowerParamAutoPowerOffWithWearingDetection>(
            /*currentPowerOffElements*/ autoPowerOffEnabled.desired
                ? THMSGV2T1::AutoPowerOffWearingDetectionElements::POWER_OFF_WHEN_REMOVED_FROM_EARS
                : THMSGV2T1::AutoPowerOffWearingDetectionElements::POWER_OFF_DISABLE,
            /*lastSelectPowerOffElements*/ THMSGV2T1::AutoPowerOffWearingDetectionElements::POWER_OFF_IN_5_MIN
        );

        this->autoPowerOffEnabled.fulfill();
    }

    if (supports(MessageMdrV2FunctionType_Table1::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF)
        && !autoPauseEnabled.isFulfilled())
    {
        this->_conn.sendCommand(
            CommandSerializer::serializeAutoPauseSetting(autoPauseEnabled.desired),
            DATA_TYPE::DATA_MDR
        );
        waitForAck();

        this->autoPauseEnabled.fulfill();
    }

    if (!voiceGuidanceEnabled.isFulfilled())
    {
        this->_conn.sendCommand(
            CommandSerializer::serializeVoiceGuidanceEnabledSetting(voiceGuidanceEnabled.desired),
            DATA_TYPE::DATA_MDR_NO2
        );
        waitForAck();

        this->voiceGuidanceEnabled.fulfill();
    }

    if (supports(MessageMdrV2FunctionType_Table2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT)
        && !miscVoiceGuidanceVol.isFulfilled())
    {
        this->_conn.sendCommand(
            CommandSerializer::serializeVoiceGuidanceVolumeSetting(
                static_cast<uint8_t>(miscVoiceGuidanceVol.desired)
            ), DATA_TYPE::DATA_MDR_NO2
        );
        waitForAck();

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

        // XXX: For some reason, multipoint switch command doesn't always work
        // ...yet appending another command after it makes it much more likely to succeed?
        /*this->_conn.sendCommand(Buffer{
            static_cast<uint8_t>(THMSGV2T1::Command::MULTIPOINT_DEVICE_GET), // TODO No such command 0x3A on table 1
            0x02
        });
        waitForAck();*/


        // Don't fulfill until PERI_NTFY_EXTENDED_PARAM is received
        // as the device might not have switched yet
        // mpDeviceMac.fulfill();
    }

    if (supports(MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
    {
        if (!stcEnabled.isFulfilled())
        {
            this->_conn.sendCommand(
                CommandSerializer::serializeSpeakToChatEnabled(stcEnabled.desired),
                DATA_TYPE::DATA_MDR
            );
            waitForAck();
            stcEnabled.fulfill();
        }

        if (!stcLevel.isFulfilled() || !stcTime.isFulfilled())
        {
            this->_conn.sendCommand(
                CommandSerializer::serializeSpeakToChatConfig(stcLevel.desired, stcTime.desired),
                DATA_TYPE::DATA_MDR
            );
            waitForAck();
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
        this->_conn.sendCommand(
            CommandSerializer::serializeEqualizerSetting(eqPreset.desired),
            DATA_TYPE::DATA_MDR
        );
        waitForAck();
        eqPreset.fulfill();
        // Ask for a equalizer param update afterwards
        _conn.sendCommand(Buffer{
            static_cast<uint8_t>(THMSGV2T1::Command::EQEBB_GET_PARAM),
            0x00 // Equalizer
        }, DATA_TYPE::DATA_MDR);
        waitForAck();
    }

    if (!eqConfig.isFulfilled())
    {
        this->_conn.sendCommand(
            CommandSerializer::serializeEqualizerSetting(eqPreset.current, eqConfig.desired.bassLevel, eqConfig.desired.bands),
            DATA_TYPE::DATA_MDR
        );
        waitForAck();
        eqConfig.fulfill();
    }

    if (supports(MessageMdrV2FunctionType_Table1::ASSIGNABLE_SETTING)
        && (!touchLeftFunc.isFulfilled() || !touchRightFunc.isFulfilled()))
    {
        this->_conn.sendCommand(
            CommandSerializer::serializeTouchSensorAssignment(touchLeftFunc.desired, touchRightFunc.desired),
            DATA_TYPE::DATA_MDR
        );
        waitForAck();
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

    if (supportsTable2)
    {
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

    /* Speak to chat */
    _conn.sendCommand(Buffer{
        static_cast<uint8_t>(THMSGV2T1::Command::SYSTEM_GET_PARAM),
        0x0c // Speak to chat enabled
    }, DATA_TYPE::DATA_MDR);
    waitForAck();

    _conn.sendCommand(Buffer{
        static_cast<uint8_t>(THMSGV2T1::Command::SYSTEM_GET_EXT_PARAM),
        0x0c // Speak to chat config
    }, DATA_TYPE::DATA_MDR);
    waitForAck();

    if (supports(MessageMdrV2FunctionType_Table1::LISTENING_OPTION))
    {
        /* Listening Mode */
        sendGet<THMSGV2T1::AudioGetParam>(THMSGV2T1::AudioInquiredType::BGM_MODE);
        sendGet<THMSGV2T1::AudioGetParam>(THMSGV2T1::AudioInquiredType::UPMIX_CINEMA);
    }

    if (supports(MessageMdrV2FunctionType_Table1::ASSIGNABLE_SETTING))
    {
        /* Touch Sensor */
        _conn.sendCommand(Buffer{
            static_cast<uint8_t>(THMSGV2T1::Command::SYSTEM_GET_PARAM),
            0x03, // Touch sensor function
        }, DATA_TYPE::DATA_MDR);
        waitForAck();
    }

    if (supports(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT))
    {
        sendGet<THMSGV2T1::NcAsmGetParam>(THMSGV2T1::NcAsmInquiredType::NC_AMB_TOGGLE);
    }

    /* Equalizer */
    if ((deviceCapabilities & DC_EqualizerAvailableCommand) != 0)
    {
        _conn.sendCommand(Buffer{
            static_cast<uint8_t>(THMSGV2T1::Command::EQEBB_GET_STATUS),
            0x00 // Equalizer
        }, DATA_TYPE::DATA_MDR);
        waitForAck();
    }

    _conn.sendCommand(Buffer{
        static_cast<uint8_t>(THMSGV2T1::Command::EQEBB_GET_PARAM),
        0x00 // Equalizer
    }, DATA_TYPE::DATA_MDR);
    waitForAck();

    /* Misc Params */
    sendGet<THMSGV2T1::PowerGetParam>(THMSGV2T1::PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION);

    _conn.sendCommand(Buffer{
        static_cast<uint8_t>(THMSGV2T1::Command::SYSTEM_GET_PARAM),
        0x01 // Pause when headphones are removed
    }, DATA_TYPE::DATA_MDR);
    waitForAck();

    if (supportsTable2)
    {
        _conn.sendCommand(Buffer{
            static_cast<uint8_t>(THMSGV2T2::Command::VOICE_GUIDANCE_GET_PARAM),
            0x01 // Voice Guidance Enabled
        }, DATA_TYPE::DATA_MDR_NO2);
        waitForAck();

        _conn.sendCommand(Buffer{
            static_cast<uint8_t>(THMSGV2T2::Command::VOICE_GUIDANCE_GET_PARAM),
            0x20 // Voice Guidance Volume
        }, DATA_TYPE::DATA_MDR_NO2);
        waitForAck();
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

    waitForAck();
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

void Headphones::requestPowerOff()
{
    if (supports(MessageMdrV2FunctionType_Table1::POWER_OFF))
    {
        sendSet<THMSGV2T1::PowerSetStatusPowerOff>(THMSGV2T1::PowerOffSettingValue::USER_POWER_OFF);
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
    case THMSGV2T1::PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION:
    {
        if (supports(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
        {
            auto payloadSub = msg.as<THMSGV2T1::PowerParamAutoPowerOffWithWearingDetection>(ct);
            THMSGV2T1::AutoPowerOffWearingDetectionElements setting = payloadSub->currentPowerOffElements;
            if (setting == THMSGV2T1::AutoPowerOffWearingDetectionElements::POWER_OFF_WHEN_REMOVED_FROM_EARS)
                autoPowerOffEnabled.overwrite(true);
            else if (setting == THMSGV2T1::AutoPowerOffWearingDetectionElements::POWER_OFF_DISABLE)
                autoPowerOffEnabled.overwrite(false);
            else
                break;
            return HeadphonesEvent::AutoPowerOffUpdate;
        }
        break;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleVoiceGuidanceParam(const HeadphonesMessage& msg)
{
    switch (msg[1])
    {
    case 0x01:
        // Note: RET returns 2 bools, while NOTIFY returns only 1.
        voiceGuidanceEnabled.overwrite(!(bool)msg[2]);
        return HeadphonesEvent::VoiceGuidanceEnabledUpdate;
    case 0x20:
        if (supports(MessageMdrV2FunctionType_Table2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT))
        {
            miscVoiceGuidanceVol.overwrite(static_cast<char>(msg[2]));
            return HeadphonesEvent::VoiceGuidanceVolumeUpdate;
        }
        break;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handlePeripheralNotifyExtendedParam(const HeadphonesMessage& msg)
{
    auto payload = msg.as<THMSGV2T2::PeripheralNtfyExtendedParam>();
    switch (payload->inquiredType)
    {
    case THMSGV2T2::PeripheralInquiredType::SOURCE_SWITCH_CONTROL:
    {
        auto payloadSub = msg.as<THMSGV2T2::PeripheralNtfyExtendedParamSourceSwitchControl>();
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

HeadphonesEvent Headphones::_handleSystemParam(const HeadphonesMessage& msg)
{
    switch (msg[1])
    {
    case 0x01:
        if (supports(MessageMdrV2FunctionType_Table1::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF))
        {
            autoPauseEnabled.overwrite(!(bool)msg[2]);
            return HeadphonesEvent::AutoPauseUpdate;
        }
        break;
    case 0x0c:
        if (supports(MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
        {
            stcEnabled.overwrite(!(bool)msg[2]);
            return HeadphonesEvent::SpeakToChatEnabledUpdate;
        }
        break;
    case 0x03:
        if (supports(MessageMdrV2FunctionType_Table1::ASSIGNABLE_SETTING))
        {
            touchLeftFunc.overwrite(static_cast<TOUCH_SENSOR_FUNCTION>(msg[3]));
            touchRightFunc.overwrite(static_cast<TOUCH_SENSOR_FUNCTION>(msg[4]));
            return HeadphonesEvent::TouchFunctionUpdate;
        }
        break;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleSpeakToChat(const HeadphonesMessage& msg)
{
    switch (msg[1])
    {
    case 0x0c:
        if (supports(MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
        {
            stcLevel.overwrite(msg[2]);
            stcTime.overwrite(msg[3]);
            return HeadphonesEvent::SpeakToChatParamUpdate;
        }
        break;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleEqualizerAvailable(const HeadphonesMessage& msg)
{
    switch (msg[1])
    {
    case 0x00:
        if ((deviceCapabilities & DC_EqualizerAvailableCommand) != 0)
        {
            eqAvailable.overwrite(!msg[2]);
            return HeadphonesEvent::EqualizerAvailableUpdate;
        }
        break;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleEqualizer(const HeadphonesMessage& msg)
{
    // [RET/NOTIFY 00 a2 06] 0a/bass 0a/band1 0a/band2 0a/band3 0a/band4 0a/band5
    // values have +10 offset
    eqPreset.overwrite(msg[2]);
    switch (msg[3])
    {
    case 0x00:
        return HeadphonesEvent::EqualizerParamUpdate;
    case 0x06:
        eqConfig.overwrite(EqualizerConfig(
            msg[4] - 10, // Clear Bass
            std::vector<int>{
                msg[5] - 10, // 400
                msg[6] - 10, // 1k
                msg[7] - 10, // 2.5k
                msg[8] - 10, // 6.3k
                msg[9] - 10, // 16k
            }
        ));
        return HeadphonesEvent::EqualizerParamUpdate;
    case 0x0a:
        eqConfig.overwrite(EqualizerConfig(
            0, // Clear Bass not available
            std::vector<int>{
                msg[4] - 6, // 31
                msg[5] - 6, // 63
                msg[6] - 6, // 125
                msg[7] - 6, // 250
                msg[8] - 6, // 500
                msg[9] - 6, // 1k
                msg[10] - 6, // 2k
                msg[11] - 6, // 4k
                msg[12] - 6, // 8k
                msg[13] - 6, // 16k
            }
        ));
        return HeadphonesEvent::EqualizerParamUpdate;
    default:
        return HeadphonesEvent::MessageUnhandled;
    }
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
        case Command::SYSTEM_NTFY_PARAM:
            result = _handleSystemParam(msg);
            break;
        case Command::SYSTEM_RET_EXT_PARAM:
        case Command::SYSTEM_NTFY_EXT_PARAM:
            result = _handleSpeakToChat(msg);
            break;
        case Command::EQEBB_RET_STATUS:
        case Command::EQEBB_NTFY_STATUS:
            result = _handleEqualizerAvailable(msg);
            break;
        case Command::EQEBB_RET_PARAM:
        case Command::EQEBB_NTFY_PARAM:
            result = _handleEqualizer(msg);
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
        case Command::VOICE_GUIDANCE_NTFY_PARAM:
            result = _handleVoiceGuidanceParam(msg);
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
