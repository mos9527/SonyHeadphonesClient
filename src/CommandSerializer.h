#pragma once
#include "Constants.h"
#include "ByteMagic.h"
#include <cstddef>
#include <vector>
#include <stdexcept>
#include "Exceptions.h"
#include <cassert>

constexpr int MINIMUM_VOICE_FOCUS_STEP = 2;
constexpr unsigned int ASM_LEVEL_DISABLED = -1;

namespace CommandSerializer
{

	//escape special chars

	Buffer _escapeSpecials(const Buffer& src);
	Buffer _unescapeSpecials(const Buffer& src);
	unsigned char _sumChecksum(const unsigned char* src, size_t size);
	unsigned char _sumChecksum(const Buffer& src);
	//Package a serialized command according to the protocol
	/*
	References:
	* DataType
	* CommandBluetoothSender.sendCommandWithRetries
	* BluetoothSenderWrapper.sendCommandViaBluetooth
	* 
	* Serialized data format: <START_MARKER>ESCAPE_SPECIALS(<DATA_TYPE><SEQ_NUMBER><BIG ENDIAN 4 BYTE SIZE OF UNESCAPED DATA><DATA><1 BYTE CHECKSUM>)<END_MARKER>
	*/
	Buffer packageDataForBt(const Buffer& src, DATA_TYPE dataType, unsigned int seqNumber);

	Buffer serializeNcAndAsmSetting(
		char version, bool notify, NC_ASM_EFFECT ncAsmEffect, NC_ASM_SETTING_TYPE ncAsmSettingType,
		ASM_ID voicePassthrough, char asmLevel, bool autoAsm, AUTO_ASM_SENSITIVITY autoAsmSensitivity);
	Buffer serializeAutoPowerOffSetting(bool autoPowerOff);
	Buffer serializeAutoPauseSetting(bool autoPause);
	Buffer serializeVoiceGuidanceEnabledSetting(bool enabled);
	Buffer serializeVoiceGuidanceVolumeSetting(char volume);
	Buffer serializeVolumeSetting(char volume);
	Buffer serializeMultipointSwitch(const char* macString);
	Buffer serializePlayControl(PLAYBACK_CONTROL control);
	Buffer serializePowerOff();
	Buffer serializeMpToggle2(bool enabled);
	Buffer serializeMpToggle(bool enabled);
	Buffer serializeSpeakToChatConfig(char sensitivity, char timeout);
	Buffer serializeSpeakToChatEnabled(bool enabled);
	Buffer serializeListeningModeBgmSetting(bool bgmModeActive, ListeningModeBgmDistanceMode distanceMode);
	Buffer serializeListeningModeNonBgmSetting(ListeningMode nonBgmMode);
	Buffer serializeEqualizerSetting(unsigned char preset, char bass, std::vector<int> const& bands);
	Buffer serializeEqualizerSetting(unsigned char preset);
	Buffer serializeTouchSensorAssignment(TOUCH_SENSOR_FUNCTION funcL, TOUCH_SENSOR_FUNCTION funcR);
	Buffer serializeTouchSensorControlPanelEnabled(bool enabled);
	Buffer serializeNcAmbButtonMode(NcAmbButtonMode mode);
	Buffer serializeOnCallVoiceCaptureSetting(bool enabled);
	// POD Wrapper for any Buffer (of messages) that contains the command payload (which may also be size 0,i.e. ACKs)
	struct CommandMessage
	{
		Buffer messageBytes{};

		CommandMessage() = default;

		CommandMessage(Buffer const& buf) : messageBytes(CommandSerializer::_unescapeSpecials(buf)) {
			assert(verify());
		};
		CommandMessage(Buffer&& buf) : messageBytes(CommandSerializer::_unescapeSpecials(buf)) {
			assert(verify());
		};
		CommandMessage(DATA_TYPE dataType, Buffer const& buffer, unsigned char seqNumber) {
			messageBytes = CommandSerializer::packageDataForBt(buffer, dataType, seqNumber);
		}

		inline const DATA_TYPE getDataType() const { return static_cast<DATA_TYPE>(messageBytes[1]); }
		inline const unsigned char getSeqNumber() const { return messageBytes[2]; }
		inline const int getSize() const { return bytesToIntBE(&messageBytes[3]); }
		inline const unsigned char getChkSum() const { return messageBytes[7 + getSize()]; }

		inline Buffer::const_iterator begin() const { return messageBytes.begin() + 7; }
		inline Buffer::const_iterator end() const { return begin() + getSize(); }
		inline const Buffer::value_type operator[](int i) const { return *(begin() + i); }

		inline Buffer const& getMessage() const { return messageBytes; }

		inline const unsigned char calcChkSum() const { return CommandSerializer::_sumChecksum(messageBytes.data() + 1, messageBytes.size() - 3); }
		inline const bool verify() const { return messageBytes.size() >= 7 && getChkSum() == calcChkSum(); }
	};
}

