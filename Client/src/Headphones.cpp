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
		eqConfig.isFulfilled() &&
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
				static_cast<char>(miscVoiceGuidanceVol.desired)
			), DATA_TYPE::DATA_MDR_NO2
		);
		waitForAck();

		this->miscVoiceGuidanceVol.fulfill();
	}

	if (!(volume.isFulfilled()))
	{
		this->_conn.sendCommand(
			CommandSerializer::serializeVolumeSetting(
				static_cast<char>(volume.desired)
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
			static_cast<char>(COMMAND_TYPE::MULTIPOINT_DEVICE_GET),
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

	if ((!eqConfig.isFulfilled())) {
		this->_conn.sendCommand(
			CommandSerializer::serializeEqualizerSetting(eqConfig.desired.bassLevel, eqConfig.desired.bands),
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
	static_cast<char>(COMMAND_TYPE::INIT_REQUEST),
		0x00
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Playback Metadata */
	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::PLAYBACK_STATUS_GET),
		0x01 // Metadata
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::PLAYBACK_STATUS_GET),
		0x20 // Playback Volume
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
	static_cast<char>(COMMAND_TYPE::PLAYBACK_STATUS_CONTROL_GET),
		0x01 // Play/Pause
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* NC/ASM Params */
	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::NCASM_PARAM_GET),
		0x17
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Connected Devices */
	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::CONNECTED_DEVIECES_GET),
		0x02
	}, DATA_TYPE::DATA_MDR_NO2);
	waitForAck();

	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::MULTIPOINT_ETC_ENABLE_GET),
		static_cast<char>(0xD2) // Multipoint enabled
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::MULTIPOINT_ETC_ENABLE_GET),
		static_cast<char>(0xD1) // Voice Capture enabled
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Speak to chat */
	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::AUTOMATIC_POWER_OFF_BUTTON_MODE_GET),
		0x0c // Speak to chat enabled
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::SPEAK_TO_CHAT_GET),
		0x0c // Speak to chat config
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Touch Sensor */
	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::AUTOMATIC_POWER_OFF_BUTTON_MODE_GET),
		0x03, // Touch sensor function
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Equalizer */
	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::EQUALIZER_GET),
		0x00 // Equalizer
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Misc Params */
	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::VOICEGUIDANCE_PARAM_GET),
		0x20 // Voice Guidance Volume
		}, DATA_TYPE::DATA_MDR_NO2);
	waitForAck();

	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::MISC_DATA_GET),
		0x00, // Some data will be sent afterwards...		
		0x00  // See _handleMessage for info
		}, DATA_TYPE::DATA_MDR);
	waitForAck();

	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::MISC_DATA_GET),
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
		static_cast<char>(COMMAND_TYPE::BATTERY_LEVEL_GET),
		0x01 // DUAL
	}, DATA_TYPE::DATA_MDR);

	waitForAck();

	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::BATTERY_LEVEL_GET),
		0x02 // CASE
	}, DATA_TYPE::DATA_MDR);

	waitForAck();

	/* Playback */
	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::PLAYBACK_SND_PRESSURE_GET),
		0x03 // Sound Pressure(?)
	}, DATA_TYPE::DATA_MDR_NO2);

	waitForAck();
}

void Headphones::recvAsync()
{
    if (_conn.isConnected())
        _recvFuture.setFromAsync([this]() -> std::optional<CommandSerializer::CommandMessage> {
            auto& conn = this->getConn();
            CommandSerializer::CommandMessage cmd;
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

HeadphonesEvent Headphones::_handleMessage(CommandSerializer::CommandMessage const& msg)
{	
	HeadphonesEvent event;
	bool dirty = false;
	switch (msg.getDataType())
	{		
		case DATA_TYPE::ACK:			
			_ackCV.notify_one();
			break;
		case DATA_TYPE::DATA_MDR:
		case DATA_TYPE::DATA_MDR_NO2:
		{
			auto cmd = static_cast<COMMAND_TYPE>(msg[0]);
#if _DEBUG
			std::cout << "[message] cmd: 0x" << std::hex << static_cast<int>(cmd) << '\n';
#endif
			switch (cmd)
			{
			case COMMAND_TYPE::INIT_RESPONSE:
				hasInit = true;
				break;
			case COMMAND_TYPE::NCASM_PARAM_RET:
			case COMMAND_TYPE::NCASM_PARAM_NOTIFY:
				// see serializeNcAndAsmSetting
				asmEnabled.overwrite(msg[3]);
				asmFoucsOnVoice.overwrite(msg[5]);
				if (msg[4])
					asmLevel.overwrite(msg[6]);
				else
					asmLevel.overwrite(0);
				break;
			case COMMAND_TYPE::BATTERY_LEVEL_RET:
				switch (msg[1])
				{
				case 1:
					statBatteryL.overwrite(msg[2]);
					statBatteryR.overwrite(msg[4]);
					break;
				case 2:
					statBatteryCase.overwrite(msg[2]);
					break;
				default:
					break;
				}
				break;
			case COMMAND_TYPE::PLAYBACK_STATUS_RET:
			case COMMAND_TYPE::PLAYBACK_STATUS_NOTIFY:
			{
				auto type = msg[1];
				switch (type)
				{
				case 0x01:
				{
					event.type = HeadphonesEvent::PlaybackMetadataUpdate;

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
					break;
				}
				case 0x20:
					event.type = HeadphonesEvent::PlaybackVolumeUpdate;

					volume.overwrite(msg[2]);
					break;
				default:
					dirty = true;
					break;
				}
				break;
			}
			case COMMAND_TYPE::PLAYBACK_SND_PRESSURE_RET:
				switch (msg[1]) {
				case 0x03:
					playback.sndPressure = msg[2];
					break;
				default:
					dirty = true;
					break;
				}
				break;
			case COMMAND_TYPE::VOICEGUIDANCE_PARAM_RET:
			case COMMAND_TYPE::VOICEGUIDANCE_PARAM_NOTIFY:
				switch (msg[1])
				{
				case 0x20:					
					miscVoiceGuidanceVol.overwrite(msg[2]);
					break;
				default:
					dirty = true;
					break;
				}
				break;
			case COMMAND_TYPE::MULTIPOINT_DEVICE_RET:
			case COMMAND_TYPE::MULTIPOINT_DEVICE_NOTIFY:
				event.type = HeadphonesEvent::MultipointSwitch;

				mpDeviceMac.overwrite(std::string(msg.begin() + 3, msg.end()));
				break;
			case COMMAND_TYPE::CONNECTED_DEVIECES_RET:
			case COMMAND_TYPE::CONNECTED_DEVIECES_NOTIFY:
			{
				if (msg[3] == 0x00)
					break;

				event.type = HeadphonesEvent::ConnectedDeviceUpdate;

				connectedDevices.clear();
				pairedDevices.clear();

				int total = msg[1];
				int connected = msg[2];
				auto it = msg.begin() + 3;
				for (int i = 0; i < total; i++)
				{
					auto mac = std::string(it, it + 6 * 3 - 1);
					it += 6 * 3 - 1;
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
			}
				break;
			case COMMAND_TYPE::PLAYBACK_STATUS_CONTROL_RET:
			case COMMAND_TYPE::PLAYBACK_STATUS_CONTROL_NOTIFY:
			{
				event.type = HeadphonesEvent::PlaybackPlayPauseUpdate;

				switch (static_cast<PLAYBACK_CONTROL_RESPONSE>(msg[3]))
				{
					case PLAYBACK_CONTROL_RESPONSE::PLAY:
						playPause.overwrite(true);
						break;
					case PLAYBACK_CONTROL_RESPONSE::PAUSE:
						playPause.overwrite(false);
						break;
					default:
						break;
				}
				break;
			}
				break;
			case COMMAND_TYPE::MULTIPOINT_ETC_ENABLE_RET:
			case COMMAND_TYPE::MULTIPOINT_ETC_ENABLE_NOITIFY:
			{
				int sub = static_cast<int>(msg[1]) & 0xff;
				switch (sub)
				{
				case 0xD1:
					voiceCapEnabled.overwrite(!(bool)msg[3]);
					break;
				case 0xD2:
					mpEnabled.overwrite(!(bool)msg[3]);
					break;
				default:
					break;
				}
				break;
			}
			case COMMAND_TYPE::AUTOMATIC_POWER_OFF_BUTTON_MODE_NOTIFY:
			case COMMAND_TYPE::AUTOMATIC_POWER_OFF_BUTTON_MODE_RET:
				switch (msg[1])
				{
				case 0x0c:
					stcEnabled.overwrite(!(bool)msg[2]);
					break;
				case 0x03:
					touchLeftFunc.overwrite(static_cast<TOUCH_SENSOR_FUNCTION>(msg[3]));
					touchRightFunc.overwrite(static_cast<TOUCH_SENSOR_FUNCTION>(msg[4]));
					break;
				default:
					dirty = true;
					break;
				}
				break;
			case COMMAND_TYPE::SPEAK_TO_CHAT_RET:
			case COMMAND_TYPE::SPEAK_TO_CHAT_NOTIFY:
				switch (msg[1])
				{
				case 0x0c:
					stcLevel.overwrite(msg[2]);
					stcTime.overwrite(msg[3]);
					break;
				default:
					dirty = true;
					break;
				}
				break;
			case COMMAND_TYPE::EQUALIZER_RET:
			case COMMAND_TYPE::EQUALIZER_NOTIFY:
				// [RET/NOTIFY 00 a2 06] 0a/bass 0a/band1 0a/band2 0a/band3 0a/band4 0a/band5
				// values have +10 offset
				switch (msg[3])
				{
				case 0x06:
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
					break;
				default:
					break;
				}
				break;
			case COMMAND_TYPE::MISC_DATA_RET:
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
					event.type = HeadphonesEvent::HeadphoneInteractionEvent;
					event.message = std::string(it, it + len);
				}
				break;
				case 0x00:
				{
					// NOTE: These are sent immediately after MISC_DATA_GET 0x01
					// and won't be sent preiodically afterwards
					// NOTE: These seem to always conatin JSON data
					std::string str(msg.begin() + 4, msg.end());
					event.type = HeadphonesEvent::JSONMessage;
					event.message = str;
					std::cout << "[message] " << str << '\n';
				}
				break;
				default:
					dirty = true;
				}
				break;
			}
			default:
				dirty = true;
				break;
			}
			_conn.sendAck(msg.getSeqNumber());
			break;
		}
		default:
			dirty = true;
	}
	if (dirty) {
		std::cout << "[message] message not handled. \n";
		std::cout << "[message] type: " << static_cast<int>(msg.getDataType()) << '\n';
		std::cout << "[message] (hex) \n";
		for (int byte : msg) std::cout << std::setfill('0') << std::setw(2) << std::hex << (byte & 0xff) << ' ';
		std::cout << '\n';
		std::cout << "[message] (plaintext) \n";
		for (char c : msg) std::cout << c;
		std::cout << '\n';
	}
	return event;
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
	return {};
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

