#include "Headphones.h"
#include "CommandSerializer.h"

#include <stdexcept>

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

bool Headphones::isChanged()
{
	bool supportsLeftRightTouch = !supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3);
	bool supportsAutoAsm = supports(MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);
	bool supportsVoiceGuidanceVolumeAdjustment = supports(MessageMdrV2FunctionType_Table2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT);
	bool supportsConfigurableVoiceCaptureDuringCall = supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1);
	bool supportsListeningMode = supports(MessageMdrV2FunctionType_Table1::LISTENING_OPTION);
	return !(
		draggingAsmLevel.isFulfilled() && asmEnabled.isFulfilled() && asmMode.isFulfilled() &&
		asmFoucsOnVoice.isFulfilled() && asmLevel.isFulfilled() &&
		(!supportsAutoAsm || (autoAsmEnabled.isFulfilled() && autoAsmSensitivity.isFulfilled())) &&
		autoPowerOffEnabled.isFulfilled() && autoPauseEnabled.isFulfilled() && voiceGuidanceEnabled.isFulfilled() &&
		(!supportsVoiceGuidanceVolumeAdjustment || miscVoiceGuidanceVol.isFulfilled()) && volume.isFulfilled() &&
		mpDeviceMac.isFulfilled() && mpEnabled.isFulfilled() &&
		stcEnabled.isFulfilled() && stcLevel.isFulfilled() && stcTime.isFulfilled() &&
		(!supportsListeningMode || listeningModeConfig.isFulfilled()) &&
		eqConfig.isFulfilled() && eqPreset.isFulfilled() &&
		(supportsLeftRightTouch ? touchLeftFunc.isFulfilled() && touchRightFunc.isFulfilled() :
			touchSensorControlPanelEnabled.isFulfilled() && ncAmbButtonMode.isFulfilled()) &&
		(!supportsConfigurableVoiceCaptureDuringCall || voiceCapEnabled.isFulfilled())
	);
}

void Headphones::waitForAck(int timeout)
{
    if (_conn.isConnected()) {
        std::unique_lock lck(_ackMtx);
        _ackCV.wait_for(lck, std::chrono::seconds(timeout));
    }
}

void Headphones::waitForProtocolInfo(int timeout)
{
	if (_conn.isConnected()) {
		std::unique_lock lck(_propertyMtx);
		_protocolInfoCV.wait_for(lck, std::chrono::seconds(timeout));
	}
}

void Headphones::waitForSupportFunction(int timeout)
{
	if (_conn.isConnected()) {
		std::unique_lock lck(_propertyMtx);
		_supportFunctionCV.wait_for(lck, std::chrono::seconds(timeout));
	}
}

void Headphones::setChanges()
{
	bool supportsLeftRightTouch = !supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3);
	bool supportsAutoAsm = supports(MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);

	if (!(draggingAsmLevel.isFulfilled() && asmEnabled.isFulfilled() && asmMode.isFulfilled() &&
		asmFoucsOnVoice.isFulfilled() && asmLevel.isFulfilled() &&
		(!supportsAutoAsm || (autoAsmEnabled.isFulfilled() && autoAsmSensitivity.isFulfilled()))))
	{
		this->_conn.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
			supportsAutoAsm ? 0x19 : 0x17,
			!draggingAsmLevel.desired,
			asmEnabled.desired ? NC_ASM_EFFECT::ON : NC_ASM_EFFECT::OFF,
			asmMode.desired,
			asmFoucsOnVoice.desired ? ASM_ID::VOICE : ASM_ID::NORMAL,
			std::max(asmLevel.desired, 1),
			autoAsmEnabled.desired,
			autoAsmSensitivity.desired
		));
		waitForAck();

		std::lock_guard guard(this->_propertyMtx);
		draggingAsmLevel.fulfill();
		asmEnabled.fulfill();
		asmMode.fulfill();
		asmLevel.fulfill();
		asmFoucsOnVoice.fulfill();
		if (supportsAutoAsm) {
			autoAsmEnabled.fulfill();
			autoAsmSensitivity.fulfill();
		}
	}

	if (!autoPowerOffEnabled.isFulfilled())
	{
		this->_conn.sendCommand(
			CommandSerializer::serializeAutoPowerOffSetting(autoPowerOffEnabled.desired),
			DATA_TYPE::DATA_MDR
		);
		waitForAck();

		this->autoPowerOffEnabled.fulfill();
	}

	if (!autoPauseEnabled.isFulfilled())
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

	if (supports(MessageMdrV2FunctionType_Table2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT) && !miscVoiceGuidanceVol.isFulfilled())
	{
		this->_conn.sendCommand(
			CommandSerializer::serializeVoiceGuidanceVolumeSetting(
				static_cast<uint8_t>(miscVoiceGuidanceVol.desired)
			), DATA_TYPE::DATA_MDR_NO2
		);
		waitForAck();

		this->miscVoiceGuidanceVol.fulfill();
	}

	if (!(volume.isFulfilled()))
	{
		this->_conn.sendCommand(
			CommandSerializer::serializeVolumeSetting(
				static_cast<uint8_t>(volume.desired)
			), DATA_TYPE::DATA_MDR
		);
		waitForAck();

		this->volume.fulfill();
	}

	if ((!mpDeviceMac.isFulfilled()))
	{		
		this->_conn.sendCommand(
			CommandSerializer::serializeMultipointSwitch(mpDeviceMac.desired.c_str()),
			DATA_TYPE::DATA_MDR_NO2
		);
		waitForAck();

		// XXX: For some reason, multipoint switch command doesn't always work
		// ...yet appending another command after it makes it much more likely to succeed?
		/*this->_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::MULTIPOINT_DEVICE_GET), // TODO No such command 0x3A on table 1
			0x02
		});
		waitForAck();*/


		// Don't fullfill until the MULTIPOINT_DEVICE_RET/NOTIFY is received
		// as the device might not have switched yet
		// mpDeviceMac.fulfill();
	}

	if ((!mpEnabled.isFulfilled()))
	{
		bool enabled = mpEnabled.desired;
		this->_conn.sendCommand(
			CommandSerializer::serializeMpToggle(enabled),
			DATA_TYPE::DATA_MDR
		);
		waitForAck();

		this->_conn.sendCommand(
			CommandSerializer::serializeMpToggle2(enabled),
			DATA_TYPE::DATA_MDR
		);
		waitForAck();
		mpEnabled.fulfill();
	}

	if ((!stcEnabled.isFulfilled()))
	{
		this->_conn.sendCommand(
			CommandSerializer::serializeSpeakToChatEnabled(stcEnabled.desired),
			DATA_TYPE::DATA_MDR
		);
		waitForAck();
		stcEnabled.fulfill();
	}

	if ((!stcLevel.isFulfilled() || !stcTime.isFulfilled()))
	{
		this->_conn.sendCommand(
			CommandSerializer::serializeSpeakToChatConfig(stcLevel.desired, stcTime.desired),
			DATA_TYPE::DATA_MDR
		);
		waitForAck();
		stcLevel.fulfill();
		stcTime.fulfill();
	}

	if (supports(MessageMdrV2FunctionType_Table1::LISTENING_OPTION) && !listeningModeConfig.isFulfilled()) {
		if (listeningModeConfig.desired.bgmActive ||
			(listeningModeConfig.current.bgmActive && listeningModeConfig.desired.nonBgmMode == ListeningMode::Standard)) {
			this->_conn.sendCommand(
				CommandSerializer::serializeListeningModeBgmSetting(listeningModeConfig.desired.bgmActive, listeningModeConfig.desired.bgmDistanceMode),
				DATA_TYPE::DATA_MDR
			);
		} else {
			this->_conn.sendCommand(
				CommandSerializer::serializeListeningModeNonBgmSetting(listeningModeConfig.desired.nonBgmMode),
				DATA_TYPE::DATA_MDR
			);
		}
		waitForAck();
		listeningModeConfig.fulfill();
	}

	if ((!eqPreset.isFulfilled())){
		this->_conn.sendCommand(
			CommandSerializer::serializeEqualizerSetting(eqPreset.desired),
			DATA_TYPE::DATA_MDR
		);
		waitForAck();
		eqPreset.fulfill();
		// Ask for a equalizer param update afterwards
		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::EQEBB_GET_PARAM),
			0x00 // Equalizer
		}, DATA_TYPE::DATA_MDR);
		waitForAck();
	}

	if ((!eqConfig.isFulfilled())) {
		this->_conn.sendCommand(
			CommandSerializer::serializeEqualizerSetting(eqPreset.current, eqConfig.desired.bassLevel, eqConfig.desired.bands),
			DATA_TYPE::DATA_MDR
		);
		waitForAck();
		eqConfig.fulfill();
	}

	if (supportsLeftRightTouch) {
		if (!touchLeftFunc.isFulfilled() || !touchRightFunc.isFulfilled()) {
			this->_conn.sendCommand(
				CommandSerializer::serializeTouchSensorAssignment(touchLeftFunc.desired, touchRightFunc.desired),
				DATA_TYPE::DATA_MDR
			);
			waitForAck();
			touchLeftFunc.fulfill();
			touchRightFunc.fulfill();
		}
	} else {
		if (!touchSensorControlPanelEnabled.isFulfilled()) {
			this->_conn.sendCommand(
				CommandSerializer::serializeTouchSensorControlPanelEnabled(touchSensorControlPanelEnabled.desired),
				DATA_TYPE::DATA_MDR
			);
			waitForAck();
			touchSensorControlPanelEnabled.fulfill();
		}

		if (!ncAmbButtonMode.isFulfilled()) {
			this->_conn.sendCommand(
				CommandSerializer::serializeNcAmbButtonMode(ncAmbButtonMode.desired),
				DATA_TYPE::DATA_MDR
			);
			waitForAck();
			ncAmbButtonMode.fulfill();
		}
	}

	if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1) && !voiceCapEnabled.isFulfilled()) {
		this->_conn.sendCommand(
			CommandSerializer::serializeOnCallVoiceCaptureSetting(voiceCapEnabled.desired),
			DATA_TYPE::DATA_MDR
		);
		waitForAck();
		voiceCapEnabled.fulfill();
	}
}

void Headphones::requestInit()
{
	/* Init */
	// NOTE: It's observed that playback metadata NOTIFYs (see _handleMessage) is sent by the device after this...
	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::CONNECT_GET_PROTOCOL_INFO),
		static_cast<uint8_t>(THMSGV2T1ConnectInquiredType::FIXED_VALUE)
	}, DATA_TYPE::DATA_MDR);
	waitForProtocolInfo(5);

	/* Capability Info */
	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::CONNECT_GET_CAPABILITY_INFO),
		static_cast<uint8_t>(THMSGV2T1ConnectInquiredType::FIXED_VALUE)
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	// Following are cached by the app based on the MAC address
	{
		/* Firmware Info */
		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::CONNECT_GET_DEVICE_INFO),
			static_cast<uint8_t>(THMSGV2T1DeviceInfoType::MODEL_NAME)
		}, DATA_TYPE::DATA_MDR);
		waitForAck();

		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::CONNECT_GET_DEVICE_INFO),
			static_cast<uint8_t>(THMSGV2T1DeviceInfoType::FW_VERSION)
		}, DATA_TYPE::DATA_MDR);
		waitForAck();

		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::CONNECT_GET_DEVICE_INFO),
			static_cast<uint8_t>(THMSGV2T1DeviceInfoType::SERIES_AND_COLOR_INFO)
		}, DATA_TYPE::DATA_MDR);
		waitForAck();

		/* Support Functions */
		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::CONNECT_GET_SUPPORT_FUNCTION),
			static_cast<uint8_t>(THMSGV2T1ConnectInquiredType::FIXED_VALUE)
		}, DATA_TYPE::DATA_MDR);
		waitForSupportFunction(5);

		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T2Command::CONNECT_GET_SUPPORT_FUNCTION),
			static_cast<uint8_t>(THMSGV2T2ConnectInquiredType::FIXED_VALUE)
		}, DATA_TYPE::DATA_MDR_NO2);
		waitForSupportFunction(5);
	}

	/* Playback Metadata */
	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::PLAY_GET_PARAM),
		0x01 // Metadata
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::PLAY_GET_PARAM),
		0x20 // Playback Volume
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
	static_cast<uint8_t>(THMSGV2T1Command::PLAY_GET_STATUS),
		0x01 // Play/Pause
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* NC/ASM Params */
	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::NCASM_GET_PARAM),
		static_cast<uint8_t>(supports(MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION) ?
			THMSGV2T1NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA :
			THMSGV2T1NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS)
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Connected Devices */
	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T2Command::PERI_GET_PARAM),
		0x02
	}, DATA_TYPE::DATA_MDR_NO2);
	waitForAck();

	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::GENERAL_SETTING_GET_PARAM),
		static_cast<uint8_t>(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_2) // Multipoint enabled
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::GENERAL_SETTING_GET_PARAM),
		static_cast<uint8_t>(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1) // Voice Capture enabled
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Speak to chat */
	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::SYSTEM_GET_PARAM),
		0x0c // Speak to chat enabled
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::SYSTEM_GET_EXT_PARAM),
		0x0c // Speak to chat config
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	if (supports(MessageMdrV2FunctionType_Table1::LISTENING_OPTION)) {
		/* Listening Mode */
		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::AUDIO_GET_PARAM),
			0x03
		}, DATA_TYPE::DATA_MDR);
		waitForAck();

		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::AUDIO_GET_PARAM),
			0x04
		}, DATA_TYPE::DATA_MDR);
		waitForAck();
	}

	if (!supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3)) {
		/* Touch Sensor */
		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::SYSTEM_GET_PARAM),
			0x03, // Touch sensor function
		}, DATA_TYPE::DATA_MDR);
		waitForAck();
	} else {
		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::GENERAL_SETTING_GET_PARAM),
			static_cast<uint8_t>(THMSGV2T1GsInquiredType::GENERAL_SETTING3) // Touch sensor control panel enabled
		}, DATA_TYPE::DATA_MDR);
		waitForAck();

		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::NCASM_GET_PARAM),
			static_cast<uint8_t>(THMSGV2T1NcAsmInquiredType::NC_AMB_TOGGLE)
		}, DATA_TYPE::DATA_MDR);
		waitForAck();
	}

	/* Equalizer */
	if ((deviceCapabilities & DC_EqualizerAvailableCommand) != 0) {
		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::EQEBB_GET_STATUS),
			0x00 // Equalizer
		}, DATA_TYPE::DATA_MDR);
		waitForAck();
	}

	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::EQEBB_GET_PARAM),
		0x00 // Equalizer
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Misc Params */
	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::POWER_GET_PARAM),
		0x05
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::SYSTEM_GET_PARAM),
		0x01 // Pause when headphones are removed
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T2Command::VOICE_GUIDANCE_GET_PARAM),
		0x01 // Voice Guidance Enabled
	}, DATA_TYPE::DATA_MDR_NO2);
	waitForAck();

	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T2Command::VOICE_GUIDANCE_GET_PARAM),
		0x20 // Voice Guidance Volume
	}, DATA_TYPE::DATA_MDR_NO2);
	waitForAck();

#ifdef _DEBUG
	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::LOG_SET_STATUS),
		0x00, // Some data in JSON format will be sent afterward...
		0x00  // See _handleMessage for info
	}, DATA_TYPE::DATA_MDR);
	waitForAck();
#endif

	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::LOG_SET_STATUS),
		0x01, // MORE data...?! why not..
		0x00  // After this command, the triple-tap (and other app-related functions) will
		      // not trigger 'app not launched' notifications on the headset
			  // I wonder if we should handle them in this app...
	}, DATA_TYPE::DATA_MDR);
	waitForAck();
}

void Headphones::requestSync()
{
	// Some values are taken from:
	// https://github.com/Freeyourgadget/Gadgetbridge/blob/master/app/src/main/java/nodomain/freeyourgadget/gadgetbridge/service/devices/sony/headphones/protocol/impl/v1/PayloadTypeV1.java

	/* Battery */
	if (supports(MessageMdrV2FunctionType_Table1::LR_BATTERY_LEVEL_WITH_THRESHOLD)) {
		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::POWER_GET_STATUS),
			0x01 // DUAL
		}, DATA_TYPE::DATA_MDR);
		waitForAck();

		if (supports(MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD)) {
			_conn.sendCommand({
			   static_cast<uint8_t>(THMSGV2T1Command::POWER_GET_STATUS),
			   0x02 // CASE
		   }, DATA_TYPE::DATA_MDR);
			waitForAck();
		}
	} else {
		_conn.sendCommand({
			static_cast<uint8_t>(THMSGV2T1Command::POWER_GET_STATUS),
			0x00 // SINGLE
		}, DATA_TYPE::DATA_MDR);
		waitForAck();
	}

	/* Playback */
	_conn.sendCommand({
		static_cast<uint8_t>(THMSGV2T1Command::EQEBB_GET_EXTENDED_INFO),
		0x03 // Sound Pressure(?)
	}, DATA_TYPE::DATA_MDR_NO2);

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

void Headphones::requestMultipointSwitch(const char* macString)
{
	_conn.sendCommand(
		CommandSerializer::serializeMultipointSwitch(macString),
		DATA_TYPE::DATA_MDR_NO2
	);
	waitForAck();
}

void Headphones::requestPlaybackControl(PLAYBACK_CONTROL control)
{
	_conn.sendCommand(
		CommandSerializer::serializePlayControl(control),
		DATA_TYPE::DATA_MDR
	);
	waitForAck();
}

void Headphones::requestPowerOff()
{
	if (supports(MessageMdrV2FunctionType_Table1::POWER_OFF)) {
		_conn.sendCommand(
		   CommandSerializer::serializePowerOff(),
		   DATA_TYPE::DATA_MDR
	   );
		waitForAck();
	}
}

HeadphonesEvent Headphones::_handleProtocolInfo(const HeadphonesMessage& msg) {
    hasInit = true;

    protocolVersion = (msg[2] << 24) | (msg[3] << 16) | (msg[4] << 8) | msg[5];
    deviceCapabilities = DC_None;

    if (protocolVersion >= 0x03002017) { // WF-1000XM5
        deviceCapabilities |= DC_EqualizerAvailableCommand;
    }

    _protocolInfoCV.notify_all();
    return HeadphonesEvent::Initialized;
}

HeadphonesEvent Headphones::_handleCapabilityInfo(const HeadphonesMessage& msg) {
    switch (static_cast<THMSGV2T1ConnectInquiredType>(msg[1])) {
    case THMSGV2T1ConnectInquiredType::FIXED_VALUE:
        capabilityCounter.overwrite(msg[2]);
        size_t uniqueIdLen = std::min<size_t>(msg[3], 128);
        char* uniqueIdBegin = (char*)&*(msg.begin() + 4);
        uniqueId.overwrite(std::string(uniqueIdBegin, uniqueIdBegin + uniqueIdLen));
        return HeadphonesEvent::DeviceInfoUpdate;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleFirmwareVersion(const HeadphonesMessage& msg) {
    switch (static_cast<THMSGV2T1DeviceInfoType>(msg[1])) {
    case THMSGV2T1DeviceInfoType::MODEL_NAME:
        size_t modelNameLen = std::min<size_t>(msg[2], 128);
        const char* modelNameBegin = (char*)&*(msg.begin() + 3);
        modelName.overwrite(std::string(modelNameBegin, modelNameBegin + modelNameLen));
        return HeadphonesEvent::DeviceInfoUpdate;
    case THMSGV2T1DeviceInfoType::FW_VERSION:
        size_t fwVersionLen = std::min<size_t>(msg[2], 128);
        const char* fwVersionBegin = (char*)&*(msg.begin() + 3);
        fwVersion.overwrite(std::string(fwVersionBegin, fwVersionBegin + fwVersionLen));
        return HeadphonesEvent::DeviceInfoUpdate;
    case THMSGV2T1DeviceInfoType::SERIES_AND_COLOR_INFO:
        modelSeries.overwrite(static_cast<THMSGV2T1ModelSeries>(msg[2]));
        modelColor.overwrite(static_cast<ModelColor>(msg[3]));
        return HeadphonesEvent::DeviceInfoUpdate;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleT1SupportFunction(const HeadphonesMessage& msg) {
    switch (static_cast<THMSGV2T1ConnectInquiredType>(msg[1])) {
    case THMSGV2T1ConnectInquiredType::FIXED_VALUE: {
        int numCapabilities = msg[2];
        if (msg.getSize() < 3 + sizeof(MessageMdrV2SupportFunction) * numCapabilities) {
            break;
        }

        MessageMdrV2SupportFunction* begin = (MessageMdrV2SupportFunction*)&*(msg.begin() + 3);
        std::vector supportFunctions(begin, begin + numCapabilities);

        this->supportFunctions1.reset();
        std::ostringstream oss;
        oss << std::uppercase << std::setfill('0');
        bool first = true;
        for (const MessageMdrV2SupportFunction& sf : supportFunctions) {
            if ((uint8_t)sf.functionType.table1 > UINT8_MAX)
                continue;
            this->supportFunctions1.set(static_cast<size_t>(sf.functionType.table1));
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

HeadphonesEvent Headphones::_handleT2SupportFunction(const HeadphonesMessage& msg) {
    switch (static_cast<THMSGV2T2ConnectInquiredType>(msg[1])) {
    case THMSGV2T2ConnectInquiredType::FIXED_VALUE: {
        int numCapabilities = msg[2];
        if (msg.getSize() < 3 + sizeof(MessageMdrV2SupportFunction) * numCapabilities) {
            break;
        }

        MessageMdrV2SupportFunction* begin = (MessageMdrV2SupportFunction*)&*(msg.begin() + 3);
        std::vector supportFunctions(begin, begin + numCapabilities);

        this->supportFunctions2.reset();
        std::ostringstream oss;
        oss << std::uppercase << std::setfill('0');
        bool first = true;
        for (const MessageMdrV2SupportFunction& sf : supportFunctions) {
            if ((uint8_t)sf.functionType.table2 > UINT8_MAX)
                continue;
            this->supportFunctions2.set(static_cast<size_t>(sf.functionType.table2));
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

HeadphonesEvent Headphones::_handleNcAsmParam(const HeadphonesMessage& msg) {
    switch (static_cast<THMSGV2T1NcAsmInquiredType>(msg[1])) {
    case THMSGV2T1NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS:
    case THMSGV2T1NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA: // with Auto ASM
        // see serializeNcAndAsmSetting
        asmEnabled.overwrite(msg[3]);
        asmMode.overwrite(static_cast<NC_ASM_SETTING_TYPE>(msg[4]));
        asmFoucsOnVoice.overwrite(msg[5]);
        asmLevel.overwrite(msg[6]);
        if (supports(MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION) && msg[1] == 0x19) {
            autoAsmEnabled.overwrite(msg[7] != 0);
            autoAsmSensitivity.overwrite(static_cast<AUTO_ASM_SENSITIVITY>(msg[8]));
        }
        return HeadphonesEvent::NcAsmParamUpdate;
    case THMSGV2T1NcAsmInquiredType::NC_AMB_TOGGLE:
        if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3)) {
            ncAmbButtonMode.overwrite(static_cast<NcAmbButtonMode>(msg[2]));
            return HeadphonesEvent::NcAmbButtonModeUpdate;
        }
        break;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleBatteryLevelRet(const HeadphonesMessage& msg) {
    switch (msg[1]) {
    case 0: // SINGLE
        statBatteryL.overwrite(msg[2]);
        return HeadphonesEvent::BatteryLevelUpdate;
    case 1: // DUAL
        if (supports(MessageMdrV2FunctionType_Table1::LR_BATTERY_LEVEL_WITH_THRESHOLD)) {
            statBatteryL.overwrite(msg[2]);
            statBatteryR.overwrite(msg[4]);
            return HeadphonesEvent::BatteryLevelUpdate;
        }
        break;
    case 2: // CASE
        if (supports(MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD)) {
            statBatteryCase.overwrite(msg[2]);
            return HeadphonesEvent::BatteryLevelUpdate;
        }
        break;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handlePlaybackStatus(const HeadphonesMessage& msg) {
    auto type = msg[1];
    switch (type) {
    case 0x01:
    {
		auto it = msg.begin() + 3;
		auto type = *it;
		auto len = *it++;
		playback.title = std::string(it, it + len);
		it += len;
		type = *it++;
		len = *it++;
		playback.album = std::string(it, it + len);
		it += len;
		type = *it++;
		len = *it++;
		playback.artist = std::string(it, it + len);
        return HeadphonesEvent::PlaybackMetadataUpdate;
    }
    case 0x20:
        volume.overwrite(msg[2]);
        return HeadphonesEvent::PlaybackVolumeUpdate;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handlePlaybackSndPressureRet(const HeadphonesMessage& msg) {
    switch (msg[1]) {
    case 0x03:
        playback.sndPressure = static_cast<char>(msg[2]);
        return HeadphonesEvent::SoundPressureUpdate;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleAutomaticPowerOffParam(const HeadphonesMessage& msg) {
    switch (msg[1]) {
    case 0x05:
    {
        uint8_t setting = msg[2];
        if (setting == 0x10)
            autoPowerOffEnabled.overwrite(true);
        else if (setting == 0x11)
            autoPowerOffEnabled.overwrite(false);
        else
            break;
        return HeadphonesEvent::AutoPowerOffUpdate;
    }
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleVoiceGuidanceParam(const HeadphonesMessage& msg) {
    switch (msg[1]) {
    case 0x01:
        // Note: RET returns 2 bools, while NOTIFY returns only 1.
        voiceGuidanceEnabled.overwrite(!(bool)msg[2]);
        return HeadphonesEvent::VoiceGuidanceEnabledUpdate;
    case 0x20:
        if (supports(MessageMdrV2FunctionType_Table2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT)) {
            miscVoiceGuidanceVol.overwrite(static_cast<char>(msg[2]));
            return HeadphonesEvent::VoiceGuidanceVolumeUpdate;
        }
        break;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleMultipointDevice(const HeadphonesMessage& msg) {
    mpDeviceMac.overwrite(std::string(msg.begin() + 3, msg.end()));
    return HeadphonesEvent::MultipointDeviceSwitchUpdate;
}

HeadphonesEvent Headphones::_handleConnectedDevices(const HeadphonesMessage& msg) {
    if (msg[3] == 0x00)
        return HeadphonesEvent::NoChange;

    connectedDevices.clear();
    pairedDevices.clear();

    int total = msg[1];
    int connected = msg[2];
    auto it = msg.begin() + 3;
    for (int i = 0; i < total; i++) {
        auto mac = std::string(it, it + (6 * 3 - 1));
        it += (6 * 3 - 1);
        char unk0 = *it++;
        char unk1 = *it++;
        char unk2 = *it++;
        char unk3 = *it++;
        auto len = *it++;
        auto name = std::string(it, it + len);
        it += len;
        if (i < connected)
            connectedDevices[mac] = BluetoothDevice(name, mac);
        else
            pairedDevices[mac] = BluetoothDevice(name, mac);
    }
    return HeadphonesEvent::ConnectedDeviceUpdate;
}

HeadphonesEvent Headphones::_handlePlaybackStatusControl(const HeadphonesMessage& msg) {
    switch (static_cast<PLAYBACK_CONTROL_RESPONSE>(msg[3])) {
    case PLAYBACK_CONTROL_RESPONSE::PLAY:
        playPause.overwrite(true);
        return HeadphonesEvent::PlaybackPlayPauseUpdate;
    case PLAYBACK_CONTROL_RESPONSE::PAUSE:
        playPause.overwrite(false);
        return HeadphonesEvent::PlaybackPlayPauseUpdate;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleMultipointEtcEnable(const HeadphonesMessage& msg) {
    switch (static_cast<THMSGV2T1GsInquiredType>(msg[1])) {
    case THMSGV2T1GsInquiredType::GENERAL_SETTING1:
        if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1)) {
            voiceCapEnabled.overwrite(!(bool)msg[3]);
            return HeadphonesEvent::VoiceCaptureEnabledUpdate;
        }
        break;
    case THMSGV2T1GsInquiredType::GENERAL_SETTING2:
        mpEnabled.overwrite(!(bool)msg[3]);
        return HeadphonesEvent::MultipointEnabledUpdate;
    case THMSGV2T1GsInquiredType::GENERAL_SETTING3:
        if (supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3)) {
            touchSensorControlPanelEnabled.overwrite(!(bool)msg[3]);
            return HeadphonesEvent::TouchSensorControlPanelEnabledUpdate;
        }
        break;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleListeningMode(const HeadphonesMessage& msg) {
    switch (msg[1]) {
    case 0x03:
        if (supports(MessageMdrV2FunctionType_Table1::LISTENING_OPTION)) {
            listeningModeConfig.overwrite(ListeningModeConfig(
                listeningModeConfig.current.nonBgmMode,
                !msg[2],
                static_cast<ListeningModeBgmDistanceMode>(msg[3])
            ));
            return HeadphonesEvent::ListeningModeUpdate;
        }
        break;
    case 0x04:
        if (supports(MessageMdrV2FunctionType_Table1::LISTENING_OPTION)) {
            listeningModeConfig.overwrite(ListeningModeConfig(
                static_cast<ListeningMode>(msg[2]),
                listeningModeConfig.current.bgmActive,
                listeningModeConfig.current.bgmDistanceMode
            ));
            return HeadphonesEvent::ListeningModeUpdate;
        }
        break;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleAutomaticPowerOffButtonMode(const HeadphonesMessage& msg) {
    switch (msg[1]) {
    case 0x01:
        autoPauseEnabled.overwrite(!(bool)msg[2]);
        return HeadphonesEvent::AutoPauseUpdate;
    case 0x0c:
        stcEnabled.overwrite(!(bool)msg[2]);
        return HeadphonesEvent::SpeakToChatEnabledUpdate;
    case 0x03:
        if (!supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3)) {
            touchLeftFunc.overwrite(static_cast<TOUCH_SENSOR_FUNCTION>(msg[3]));
            touchRightFunc.overwrite(static_cast<TOUCH_SENSOR_FUNCTION>(msg[4]));
            return HeadphonesEvent::TouchFunctionUpdate;
        }
        break;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleSpeakToChat(const HeadphonesMessage& msg) {
    switch (msg[1]) {
    case 0x0c:
        stcLevel.overwrite(msg[2]);
        stcTime.overwrite(msg[3]);
        return HeadphonesEvent::SpeakToChatParamUpdate;
    }
    return HeadphonesEvent::MessageUnhandled;
}

HeadphonesEvent Headphones::_handleEqualizerAvailable(const HeadphonesMessage& msg) {
    switch (msg[1]) {
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

HeadphonesEvent Headphones::_handleEqualizer(const HeadphonesMessage& msg) {
    // [RET/NOTIFY 00 a2 06] 0a/bass 0a/band1 0a/band2 0a/band3 0a/band4 0a/band5
    // values have +10 offset
    eqPreset.overwrite(msg[2]);
    switch (msg[3]) {
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

HeadphonesEvent Headphones::_handleMiscDataRet(const HeadphonesMessage& msg) {
    // NOTE: Plain text data in either JSON or button names(?)
    switch (msg[1]) {
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
        std::string str(msg.begin() + 4, msg.end());
        deviceMessages.overwrite(str);
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
        THMSGV2T1Command command = static_cast<THMSGV2T1Command>(msg[0]);
        switch (command)
        {
        case THMSGV2T1Command::CONNECT_RET_PROTOCOL_INFO:
            result = _handleProtocolInfo(msg);
            break;
        case THMSGV2T1Command::CONNECT_RET_CAPABILITY_INFO:
            result = _handleCapabilityInfo(msg);
            break;
        case THMSGV2T1Command::CONNECT_RET_DEVICE_INFO:
            result = _handleFirmwareVersion(msg);
            break;
        case THMSGV2T1Command::CONNECT_RET_SUPPORT_FUNCTION:
            result = _handleT1SupportFunction(msg);
            break;
        case THMSGV2T1Command::NCASM_RET_PARAM:
        case THMSGV2T1Command::NCASM_NTFY_PARAM:
            result = _handleNcAsmParam(msg);
            break;
        case THMSGV2T1Command::POWER_RET_STATUS:
        // case COMMAND_TYPE::POWER_NTFY_STATUS: // WH-1000XM5, WH-1000XM6 send this
            result = _handleBatteryLevelRet(msg);
            break;
        case THMSGV2T1Command::PLAY_RET_PARAM:
        case THMSGV2T1Command::PLAY_NTFY_PARAM:
            result = _handlePlaybackStatus(msg);
            break;
        case THMSGV2T1Command::EQEBB_RET_EXTENDED_INFO:
            result = _handlePlaybackSndPressureRet(msg);
            break;
        case THMSGV2T1Command::POWER_RET_PARAM:
        case THMSGV2T1Command::POWER_NTFY_PARAM:
            result = _handleAutomaticPowerOffParam(msg);
            break;
        case THMSGV2T1Command::PLAY_RET_STATUS:
        case THMSGV2T1Command::PLAY_NTFY_STATUS:
            result = _handlePlaybackStatusControl(msg);
            break;
        case THMSGV2T1Command::GENERAL_SETTING_RET_PARAM:
        case THMSGV2T1Command::GENERAL_SETTING_NTNY_PARAM:
            result = _handleMultipointEtcEnable(msg);
            break;
        case THMSGV2T1Command::AUDIO_RET_PARAM:
        case THMSGV2T1Command::AUDIO_NTFY_PARAM:
            result = _handleListeningMode(msg);
            break;
        case THMSGV2T1Command::SYSTEM_RET_PARAM:
        case THMSGV2T1Command::SYSTEM_NTFY_PARAM:
            result = _handleAutomaticPowerOffButtonMode(msg);
            break;
        case THMSGV2T1Command::SYSTEM_RET_EXT_PARAM:
        case THMSGV2T1Command::SYSTEM_NTFY_EXT_PARAM:
            result = _handleSpeakToChat(msg);
            break;
        case THMSGV2T1Command::EQEBB_RET_STATUS:
        case THMSGV2T1Command::EQEBB_NTFY_STATUS:
            result = _handleEqualizerAvailable(msg);
            break;
        case THMSGV2T1Command::EQEBB_RET_PARAM:
        case THMSGV2T1Command::EQEBB_NTFY_PARAM:
            result = _handleEqualizer(msg);
            break;
        case THMSGV2T1Command::LOG_NTFY_PARAM:
            result = _handleMiscDataRet(msg);
            break;
        default:
            // Command type not recognized
            break;
        }
        _conn.sendAck(msg.getSeqNumber());
        break;
    }
    case DATA_TYPE::DATA_MDR_NO2:
    {
        THMSGV2T2Command command = static_cast<THMSGV2T2Command>(msg[0]);
        switch (command)
        {
            case THMSGV2T2Command::CONNECT_RET_SUPPORT_FUNCTION:
                result = _handleT2SupportFunction(msg);
                break;
            case THMSGV2T2Command::VOICE_GUIDANCE_RET_PARAM:
            case THMSGV2T2Command::VOICE_GUIDANCE_NTFY_PARAM:
                result = _handleVoiceGuidanceParam(msg);
                break;
            case THMSGV2T2Command::MULTIPOINT_DEVICE_RET:
            case THMSGV2T2Command::PERI_NTFY_EXTENDED_PARAM:
                result = _handleMultipointDevice(msg);
                break;
            case THMSGV2T2Command::PERI_RET_PARAM:
            case THMSGV2T2Command::PERI_NTFY_PARAM:
                result = _handleConnectedDevices(msg);
                break;
            default:
                // Command type not recognized
                break;
        }
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
	if (_recvFuture.ready()) {
		auto cmd = this->_recvFuture.get();
		if (cmd.has_value()) {
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
}

Headphones::~Headphones()
{
	if (_conn.isConnected())
		disconnect();
	_ackCV.notify_all();
}

