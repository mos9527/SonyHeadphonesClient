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
		miscVoiceGuidanceVol.isFulfilled()
	);
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
			), DATA_TYPE::DATA_MDR_NO2 // 0x0e
		);
		this->_cmdCount++;

		this->miscVoiceGuidanceVol.fulfill();
	}
}

void Headphones::waitForAck()
{
	std::unique_lock lck(_ackMtx);
	_ackCV.wait(lck);
}

void Headphones::requestSync()
{
	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::BATTERY_LEVEL_REQUEST),
		0x01 // DUAL
	}, DATA_TYPE::DATA_MDR);
	this->_cmdCount++;

	waitForAck();

	_conn.sendCommand({
		static_cast<char>(COMMAND_TYPE::BATTERY_LEVEL_REQUEST),
		0x02 // CASE
	}, DATA_TYPE::DATA_MDR);

	waitForAck();
}

void Headphones::recvAsync()
{
	_recvFuture.setFromAsync([=]() -> std::optional<CommandSerializer::CommandMessage> {
		auto& conn = getConn();
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

void Headphones::_handleMessage(CommandSerializer::CommandMessage const& msg)
{
	std::cout << "[message] type: " << static_cast<int>(msg.getDataType()) << '\n';	
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
			std::cout << "[message] command: " << std::hex << (int)cmd << "\n[message] ";
			for (int byte : msg) std::cout << std::hex << (byte & 0xff) << ' ';
			std::cout << '\n';
			switch (cmd)
			{
			case COMMAND_TYPE::NCASM_NOTIFY_PARAM:
				// see serializeNcAndAsmSetting
				asmEnabled.overwrite(msg[3]);
				asmFoucsOnVoice.overwrite(msg[5]);
				if (msg[4])
					asmLevel.overwrite(msg[6]);
				else
					asmLevel.overwrite(0);
				break;
			case COMMAND_TYPE::BATTERY_LEVEL_REPLY:
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
			default:
				break;
			}
			_conn.sendAck(msg.getSeqNumber());
			break;
		}
		default:
			break;
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

