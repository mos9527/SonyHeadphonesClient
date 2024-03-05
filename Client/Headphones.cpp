#include "Headphones.h"
#include "CommandSerializer.h"

#include <stdexcept>

Headphones::Headphones(BluetoothWrapper& conn) : _conn(conn)
{
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

void Headphones::handleMessage(CommandSerializer::CommandMessage const& msg)
{
	std::cout << "[message] type: " << static_cast<int>(msg.getDataType()) << '\n';
	switch (msg.getDataType())
	{
		case DATA_TYPE::ACK:			
			_ackCount++;
			break;
		case DATA_TYPE::DATA_MDR:
		case DATA_TYPE::DATA_MDR_NO2:
			std::cout << "[message] responding ACK\n";
			_conn.sendAck(msg.getSeqNumber());
			break;
		default:
			break;
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

