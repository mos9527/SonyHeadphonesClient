#include "Headphones.h"
#include "CommandSerializer.h"

#include <stdexcept>

Headphones::Headphones(BluetoothWrapper& conn) : _conn(conn)
{
}

void Headphones::setAmbientSoundControl(bool val)
{
	std::lock_guard guard(this->_propertyMtx);
	this->_ambientSoundControl.desired = val;
}

bool Headphones::getAmbientSoundControl()
{
	return this->_ambientSoundControl.current;
}

bool Headphones::isFocusOnVoiceAvailable()
{
	return this->_ambientSoundControl.current && this->_asmLevel.current > MINIMUM_VOICE_FOCUS_STEP;
}

void Headphones::setFocusOnVoice(bool val)
{
	std::lock_guard guard(this->_propertyMtx);
	this->_focusOnVoice.desired = val;
}

bool Headphones::getFocusOnVoice()
{
	return this->_focusOnVoice.current;
}

bool Headphones::isSetAsmLevelAvailable()
{
	return this->_ambientSoundControl.current;
}

void Headphones::setAsmLevel(int val)
{
	std::lock_guard guard(this->_propertyMtx);
	this->_asmLevel.desired = val;
}

int Headphones::getAsmLevel()
{
	return this->_asmLevel.current;
}

void Headphones::setSurroundPosition(SOUND_POSITION_PRESET val)
{
	std::lock_guard guard(this->_propertyMtx);
	this->_surroundPosition.desired = val;
}

SOUND_POSITION_PRESET Headphones::getSurroundPosition()
{
	return this->_surroundPosition.current;
}

void Headphones::setVptType(int val)
{
	std::lock_guard guard(this->_propertyMtx);
	this->_vptType.desired = val;
}

int Headphones::getVptType()
{
	return this->_vptType.current;
}

void Headphones::setVoiceGuidanceVolume(int val)
{
	std::lock_guard guard(this->_propertyMtx);
	this->_voiceGuidanceVolume.desired = val;
}

int Headphones::getVoiceGuidanceVolume()
{
	return this->_voiceGuidanceVolume.current;
}

bool Headphones::isChanged()
{
	return !(this->_ambientSoundControl.isFulfilled() && this->_asmLevel.isFulfilled() && this->_focusOnVoice.isFulfilled() && this->_surroundPosition.isFulfilled() && this->_vptType.isFulfilled() && this->_voiceGuidanceVolume.isFulfilled());
}

int Headphones::setChanges()
{
	int numCommands = 0;
	if (!(this->_ambientSoundControl.isFulfilled() && this->_focusOnVoice.isFulfilled() && this->_asmLevel.isFulfilled()))
	{
		this->_conn.sendCommand(CommandSerializer::serializeNcAndAsmSetting(
			this->_ambientSoundControl.desired ? NC_ASM_EFFECT::ON : NC_ASM_EFFECT::OFF,
			this->_asmLevel.desired > 0 ? NC_ASM_SETTING_TYPE::AMBIENT_SOUND : NC_ASM_SETTING_TYPE::NOISE_CANCELLING,
			this->_focusOnVoice.desired ? ASM_ID::VOICE : ASM_ID::NORMAL,
			std::max(this->_asmLevel.desired, 1)
		));
		
		std::lock_guard guard(this->_propertyMtx);
		this->_ambientSoundControl.fulfill();
		this->_asmLevel.fulfill();
		this->_focusOnVoice.fulfill();
		numCommands++;
	}
	if (!(this->_voiceGuidanceVolume.isFulfilled()))
	{
		this->_conn.sendCommand(
			CommandSerializer::serializeVoiceGuidanceSetting(
				static_cast<char>(this->_voiceGuidanceVolume.desired)
			), DATA_TYPE::DATA_MDR_NO2 // 0x0e
		);
		this->_voiceGuidanceVolume.fulfill();
		numCommands++;
	}

	// XXX
	if (!(this->_vptType.isFulfilled() && this->_surroundPosition.isFulfilled())) {
		VPT_INQUIRED_TYPE command;
		unsigned char preset;

		if (this->_vptType.desired != 0) {
			command = VPT_INQUIRED_TYPE::VPT;
			preset = static_cast<unsigned char>(this->_vptType.desired);
		}
		else if (this->_surroundPosition.desired != SOUND_POSITION_PRESET::OFF) {
			command = VPT_INQUIRED_TYPE::SOUND_POSITION;
			preset = static_cast<unsigned char>(this->_surroundPosition.desired);
		}
		else {
			// Just used that one because it seems like it disables both
			if (this->_surroundPosition.current != SOUND_POSITION_PRESET::OFF) {
				command = VPT_INQUIRED_TYPE::SOUND_POSITION;
				preset = static_cast<unsigned char>(SOUND_POSITION_PRESET::OFF);
			}
			else if (this->_vptType.current != 0) {
				command = VPT_INQUIRED_TYPE::VPT;
				preset = 0;
			}
			else {
				throw std::logic_error("it's impossible that both values were changed to zero and were also previously zero");
			}
		}

		this->_conn.sendCommand(CommandSerializer::serializeVPTSetting(command, preset));

		std::lock_guard guard(this->_propertyMtx);
		this->_vptType.fulfill();
		this->_surroundPosition.fulfill();
		numCommands++;
	}

	return numCommands;
}

