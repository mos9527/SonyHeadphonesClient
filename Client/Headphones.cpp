#include "Headphones.h"
#include "CommandSerializer.h"

#include <stdexcept>

Headphones::Headphones(BluetoothWrapper& conn) : _conn(conn)
{
	recvAsync();
}

bool Headphones::isChanged()
{
	return !(
		asmEnabled.isFulfilled() && asmFoucsOnVoice.isFulfilled() && asmLevel.isFulfilled() &&
		miscVoiceGuidanceVol.isFulfilled() &&
		volume.isFulfilled() &&
		mpDeviceMac.isFulfilled() &&
		mpEnabled.isFulfilled()
	);
}

void Headphones::waitForAck(int timeout)
{
	std::unique_lock lck(_ackMtx);
	_ackCV.wait_for(lck, std::chrono::seconds(timeout));
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
		this->_cmdCount++;

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
		this->_cmdCount++;

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
		this->_cmdCount++;

		this->volume.fulfill();
	}

	if ((!mpDeviceMac.isFulfilled()))
	{		
		this->_conn.sendCommand(
			CommandSerializer::serializeMultipointSwitch(mpDeviceMac.desired.c_str()),
			DATA_TYPE::DATA_MDR_NO2
		);
		waitForAck();
		this->_cmdCount++;

		// XXX: For some reason, multipoint switch command doesn't always work
		// ...yet appending another command after it makes it much more likely to succeed?
		this->_conn.sendCommand({
			static_cast<char>(COMMAND_TYPE::MULTIPOINT_DEVICE_GET),
			0x02
		});
		waitForAck();
		this->_cmdCount++;

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
		this->_cmdCount++;

		this->_conn.sendCommand(
			CommandSerializer::serializeMpToggle2(enabled),
			DATA_TYPE::DATA_MDR
		);
		waitForAck();
		this->_cmdCount++;
		mpEnabled.fulfill();
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
		static_cast<char>(COMMAND_TYPE::MULTIPOINT_ENABLE_GET),
		static_cast<char>(0xD2) // Multipoint enabled
	}, DATA_TYPE::DATA_MDR);
	waitForAck();

	/* Misc Params */
	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::VOICEGUIDANCE_PARAM_GET),
		0x20 // Voice Guidance Volume
	}, DATA_TYPE::DATA_MDR_NO2);
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
	this->_cmdCount++;

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
	_recvFuture.setFromAsync([this]() -> std::optional<CommandSerializer::CommandMessage> {
		auto& conn = this->getConn();
		CommandSerializer::CommandMessage cmd;
		try
		{
			conn.recvCommand(cmd);
			return cmd;
		}
		catch (const RecoverableException& exc)
		{
			if (exc.shouldDisconnect)
			{
				return std::nullopt;
			}
			else {
				throw exc;
			}
		}
	});
}

void Headphones::requestMultipointSwitch(const char* macString)
{
	_conn.sendCommand(
		CommandSerializer::serializeMultipointSwitch(macString),
		DATA_TYPE::DATA_MDR_NO2
	);
	waitForAck();
	_cmdCount++;
}

void Headphones::requestPlaybackControl(PLAYBACK_CONTROL control)
{
	_conn.sendCommand(
		CommandSerializer::serializePlayControl(control),
		DATA_TYPE::DATA_MDR
	);
	waitForAck();
	_cmdCount++;
}

void Headphones::requestPowerOff()
{
	_conn.sendCommand(
		CommandSerializer::serializePowerOff(),
		DATA_TYPE::DATA_MDR
	);
	waitForAck();
	_cmdCount++;
}

void Headphones::_handleMessage(CommandSerializer::CommandMessage const& msg)
{	
	bool dirty = false;
	switch (msg.getDataType())
	{		
		case DATA_TYPE::ACK:			
			_ackCount++;
			_ackCV.notify_one();
			break;
		case DATA_TYPE::DATA_MDR:
		case DATA_TYPE::DATA_MDR_NO2:
		{
			auto cmd = static_cast<COMMAND_TYPE>(msg[0]);
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
				mpDeviceMac.overwrite(std::string(msg.begin() + 3, msg.end()));
				std::cout << "[multipoint] swapped to " << connectedDevices[mpDeviceMac.current].name << '\n';
				break;
			case COMMAND_TYPE::CONNECTED_DEVIECES_RET:
			case COMMAND_TYPE::CONNECTED_DEVIECES_NOTIFY:
			{
				if (msg[3] == 0x00)
					break;

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
			case COMMAND_TYPE::MULTIPOINT_ENABLE_RET:
			case COMMAND_TYPE::MULTIPOINT_ENABLE_NOITIFY:
				mpEnabled.overwrite(!(bool)(msg[3]));
				dirty = true;
				break;
			default:
				dirty = true;
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
}

void Headphones::pollMessages()
{
	if (_recvFuture.ready()) {
		auto cmd = this->_recvFuture.get();
		if (cmd.has_value()) {
			_handleMessage(cmd.value());
			recvAsync();
		}
	}
}

void Headphones::disconnect()
{
	_conn.disconnect();
}

Headphones::~Headphones()
{
	if (_conn.isConnected())
		disconnect();
}

