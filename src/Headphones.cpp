#include "Headphones.h"
#include "CommandSerializer.h"

#include <stdexcept>

Headphones::Headphones(BluetoothWrapper& conn) : _conn(conn), _recvFuture("receive"), _requestFuture("request"), _sendCommandFuture("send cmd")
{
	recvAsync();
}

bool Headphones::isChanged()
{
	return !(
		asmEnabled.isFulfilled() && asmFoucsOnVoice.isFulfilled() && asmLevel.isFulfilled() &&
		miscVoiceGuidanceVol.isFulfilled() &&
		volume.isFulfilled() &&
		mpDeviceMac.isFulfilled() && mpEnabled.isFulfilled() &&
		stcEnabled.isFulfilled() && stcLevel.isFulfilled() && stcTime.isFulfilled() &&
		eqConfig.isFulfilled() && eqPreset.isFulfilled() &&
		touchLeftFunc.isFulfilled() && touchRightFunc.isFulfilled() &&
		voiceCapEnabled.isFulfilled()
	);
}

void Headphones::waitForAck(int timeout)
{
    if (_conn.isConnected()) {
        std::unique_lock lck(_ackMtx);
        _ackCV.wait_for(lck, std::chrono::seconds(timeout));
    }
}

void Headphones::setChanges()
{
	if (!(asmEnabled.isFulfilled() && asmFoucsOnVoice.isFulfilled() && asmLevel.isFulfilled()))
	{
		this->_conn.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
			asmEnabled.desired ? NC_ASM_EFFECT::ON : NC_ASM_EFFECT::OFF,
			asmLevel.desired > 0 ? NC_ASM_SETTING_TYPE::AMBIENT_SOUND : NC_ASM_SETTING_TYPE::NOISE_CANCELLING,
			asmFoucsOnVoice.desired ? ASM_ID::VOICE : ASM_ID::NORMAL,
			std::max(asmLevel.desired, 1)
		));
		waitForAck();

		std::lock_guard guard(this->_propertyMtx);
		asmEnabled.fulfill();
		asmLevel.fulfill();
		asmFoucsOnVoice.fulfill();
	}

	if (!(miscVoiceGuidanceVol.isFulfilled()))
	{
		this->_conn.sendCommand(
			CommandSerializer::serializeVoiceGuidanceSetting(
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
		this->_conn.sendCommand({
			static_cast<uint8_t>(COMMAND_TYPE::MULTIPOINT_DEVICE_GET),
			0x02
		});
		waitForAck();


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

	if ((!eqPreset.isFulfilled())){
		this->_conn.sendCommand(
			CommandSerializer::serializeEqualizerSetting(eqPreset.desired),
			DATA_TYPE::DATA_MDR
		);
		waitForAck();
		eqPreset.fulfill();
		// Ask for a equalizer param update afterwards
		_conn.sendCommand({
			static_cast<uint8_t>(COMMAND_TYPE::EQUALIZER_GET),
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

	if ((!touchLeftFunc.isFulfilled()) || (!touchRightFunc.isFulfilled())) {
			
		this->_conn.sendCommand(
			CommandSerializer::serializeTouchSensorAssignment(touchLeftFunc.desired, touchRightFunc.desired),
			DATA_TYPE::DATA_MDR
		);
		waitForAck();
		touchLeftFunc.fulfill();
		touchRightFunc.fulfill();
	}

	if (!voiceCapEnabled.isFulfilled()) {
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
	static_cast<uint8_t>(COMMAND_TYPE::INIT_REQUEST),
		0x00
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Playback Metadata */
	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::PLAYBACK_STATUS_GET),
		0x01 // Metadata
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::PLAYBACK_STATUS_GET),
		0x20 // Playback Volume
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
	static_cast<uint8_t>(COMMAND_TYPE::PLAYBACK_STATUS_CONTROL_GET),
		0x01 // Play/Pause
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* NC/ASM Params */
	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::NCASM_PARAM_GET),
		0x17
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Connected Devices */
	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::CONNECTED_DEVIECES_GET),
		0x02
	}, DATA_TYPE::DATA_MDR_NO2);
	waitForAck();

	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::MULTIPOINT_ETC_ENABLE_GET),
		static_cast<uint8_t>(0xD2) // Multipoint enabled
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::MULTIPOINT_ETC_ENABLE_GET),
		static_cast<uint8_t>(0xD1) // Voice Capture enabled
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Speak to chat */
	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::AUTOMATIC_POWER_OFF_BUTTON_MODE_GET),
		0x0c // Speak to chat enabled
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::SPEAK_TO_CHAT_GET),
		0x0c // Speak to chat config
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Touch Sensor */
	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::AUTOMATIC_POWER_OFF_BUTTON_MODE_GET),
		0x03, // Touch sensor function
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Equalizer */
	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::EQUALIZER_GET),
		0x00 // Equalizer
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Misc Params */
	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::VOICEGUIDANCE_PARAM_GET),
		0x20 // Voice Guidance Volume
		}, DATA_TYPE::DATA_MDR_NO2);
	waitForAck();

#ifdef _DEBUG
	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::MISC_DATA_GET),
		0x00, // Some data in JSON format will be sent afterward...
		0x00  // See _handleMessage for info
		}, DATA_TYPE::DATA_MDR);
	waitForAck();
#endif

	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::MISC_DATA_GET),
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
	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::BATTERY_LEVEL_GET),
		0x01 // DUAL
	}, DATA_TYPE::DATA_MDR);

	waitForAck();

	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::BATTERY_LEVEL_GET),
		0x02 // CASE
	}, DATA_TYPE::DATA_MDR);

	waitForAck();

	/* Playback */
	_conn.sendCommand({
		static_cast<uint8_t>(COMMAND_TYPE::PLAYBACK_SND_PRESSURE_GET),
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
	_conn.sendCommand(
		CommandSerializer::serializePowerOff(),
		DATA_TYPE::DATA_MDR
	);
	waitForAck();
}

HeadphonesEvent Headphones::_handleInitResponse(const HeadphonesMessage& msg) {
    hasInit = true;
    return HeadphonesEvent::Initialized;
}

HeadphonesEvent Headphones::_handleNcAsmParam(const HeadphonesMessage& msg) {
    // see serializeNcAndAsmSetting
    asmEnabled.overwrite(msg[3]);
    asmFoucsOnVoice.overwrite(msg[5]);
    if (msg[4])
        asmLevel.overwrite(msg[6]);
    else
        asmLevel.overwrite(0);
    return HeadphonesEvent::NcAsmParamUpdate;
}

HeadphonesEvent Headphones::_handleBatteryLevelRet(const HeadphonesMessage& msg) {
    switch (msg[1]) {
    case 1:
        statBatteryL.overwrite(msg[2]);
        statBatteryR.overwrite(msg[4]);
        return HeadphonesEvent::BatteryLevelUpdate;
    case 2:
        statBatteryCase.overwrite(msg[2]);
        return HeadphonesEvent::BatteryLevelUpdate;
    default:
        return HeadphonesEvent::MessageUnhandled;
    }
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
    default:
        return HeadphonesEvent::MessageUnhandled;
    }
}

HeadphonesEvent Headphones::_handlePlaybackSndPressureRet(const HeadphonesMessage& msg) {
    switch (msg[1]) {
    case 0x03:
        playback.sndPressure = static_cast<char>(msg[2]);
        return HeadphonesEvent::SoundPressureUpdate;
    default:
        return HeadphonesEvent::MessageUnhandled;
    }
}

HeadphonesEvent Headphones::_handleVoiceGuidanceParam(const HeadphonesMessage& msg) {
    switch (msg[1]) {
    case 0x20:
        miscVoiceGuidanceVol.overwrite(msg[2]);
        return HeadphonesEvent::VoiceGuidanceVolumeUpdate;
    default:
        return HeadphonesEvent::MessageUnhandled;
    }
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
    default:
        return HeadphonesEvent::MessageUnhandled;
    }
}

HeadphonesEvent Headphones::_handleMultipointEtcEnable(const HeadphonesMessage& msg) {
    int sub = static_cast<int>(msg[1]) & 0xff;
    switch (sub) {
    case 0xD1:
        voiceCapEnabled.overwrite(!(bool)msg[3]);
        return HeadphonesEvent::VoiceCaptureEnabledUpdate;
    case 0xD2:
        mpEnabled.overwrite(!(bool)msg[3]);
        return HeadphonesEvent::MultipointEnabledUpdate;
    default:
        return HeadphonesEvent::MessageUnhandled;
    }
}

HeadphonesEvent Headphones::_handleAutomaticPowerOffButtonMode(const HeadphonesMessage& msg) {
    switch (msg[1]) {
    case 0x0c:
        stcEnabled.overwrite(!(bool)msg[2]);
        return HeadphonesEvent::SpeakToChatEnabledUpdate;
    case 0x03:
        touchLeftFunc.overwrite(static_cast<TOUCH_SENSOR_FUNCTION>(msg[3]));
        touchRightFunc.overwrite(static_cast<TOUCH_SENSOR_FUNCTION>(msg[4]));
        return HeadphonesEvent::TouchFunctionUpdate;
    default:
        return HeadphonesEvent::MessageUnhandled;
    }
}

HeadphonesEvent Headphones::_handleSpeakToChat(const HeadphonesMessage& msg) {
    switch (msg[1]) {
    case 0x0c:
        stcLevel.overwrite(msg[2]);
        stcTime.overwrite(msg[3]);
        return HeadphonesEvent::SpeakToChatParamUpdate;
    default:
        return HeadphonesEvent::MessageUnhandled;
    }
}

HeadphonesEvent Headphones::_handleEqualizer(const HeadphonesMessage& msg) {
    // [RET/NOTIFY 00 a2 06] 0a/bass 0a/band1 0a/band2 0a/band3 0a/band4 0a/band5
    // values have +10 offset
	eqPreset.overwrite(msg[2]);
	if (msg[3] == 0x06){    
        eqConfig.overwrite(EqualizerConfig(
            msg[4] - 10,
            std::vector<int>{
                msg[5] - 10,
                msg[6] - 10,
                msg[7] - 10,
                msg[8] - 10,
                msg[9] - 10,
            }
        ));        
    }
	return HeadphonesEvent::EqualizerParamUpdate;
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
    case DATA_TYPE::DATA_MDR_NO2:
    {
        COMMAND_TYPE command = static_cast<COMMAND_TYPE>(msg[0]);
        switch (command)
        {
        case COMMAND_TYPE::INIT_RESPONSE:
            result = _handleInitResponse(msg);
            break;
        case COMMAND_TYPE::NCASM_PARAM_RET:
        case COMMAND_TYPE::NCASM_PARAM_NOTIFY:
            result = _handleNcAsmParam(msg);
            break;
        case COMMAND_TYPE::BATTERY_LEVEL_RET:
            result = _handleBatteryLevelRet(msg);
            break;
        case COMMAND_TYPE::PLAYBACK_STATUS_RET:
        case COMMAND_TYPE::PLAYBACK_STATUS_NOTIFY:
            result = _handlePlaybackStatus(msg);
            break;
        case COMMAND_TYPE::PLAYBACK_SND_PRESSURE_RET:
            result = _handlePlaybackSndPressureRet(msg);
            break;
        case COMMAND_TYPE::VOICEGUIDANCE_PARAM_RET:
        case COMMAND_TYPE::VOICEGUIDANCE_PARAM_NOTIFY:
            result = _handleVoiceGuidanceParam(msg);
            break;
        case COMMAND_TYPE::MULTIPOINT_DEVICE_RET:
        case COMMAND_TYPE::MULTIPOINT_DEVICE_NOTIFY:
            result = _handleMultipointDevice(msg);
            break;
        case COMMAND_TYPE::CONNECTED_DEVIECES_RET:
        case COMMAND_TYPE::CONNECTED_DEVIECES_NOTIFY:
            result = _handleConnectedDevices(msg);
            break;
        case COMMAND_TYPE::PLAYBACK_STATUS_CONTROL_RET:
        case COMMAND_TYPE::PLAYBACK_STATUS_CONTROL_NOTIFY:
            result = _handlePlaybackStatusControl(msg);
            break;
        case COMMAND_TYPE::MULTIPOINT_ETC_ENABLE_RET:
        case COMMAND_TYPE::MULTIPOINT_ETC_ENABLE_NOITIFY:
            result = _handleMultipointEtcEnable(msg);
            break;
        case COMMAND_TYPE::AUTOMATIC_POWER_OFF_BUTTON_MODE_NOTIFY:
        case COMMAND_TYPE::AUTOMATIC_POWER_OFF_BUTTON_MODE_RET:
            result = _handleAutomaticPowerOffButtonMode(msg);
            break;
        case COMMAND_TYPE::SPEAK_TO_CHAT_RET:
        case COMMAND_TYPE::SPEAK_TO_CHAT_NOTIFY:
            result = _handleSpeakToChat(msg);
            break;
        case COMMAND_TYPE::EQUALIZER_RET:
        case COMMAND_TYPE::EQUALIZER_NOTIFY:
            result = _handleEqualizer(msg);
            break;
        case COMMAND_TYPE::MISC_DATA_RET:
            result = _handleMiscDataRet(msg);
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
}

Headphones::~Headphones()
{
	if (_conn.isConnected())
		disconnect();
    _ackCV.notify_all();
}

