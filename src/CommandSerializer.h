#pragma once
#include "Constants.h"
#include "ByteMagic.h"
#include <cstddef>
#include <vector>
#include <stdexcept>
#include "Exceptions.h"
#include <cassert>
#include "ProtocolV2T1.h"
#include "ProtocolV2T2.h"

constexpr int MINIMUM_VOICE_FOCUS_STEP = 2;
constexpr unsigned int ASM_LEVEL_DISABLED = -1;

namespace CommandSerializer
{

	//escape special chars

	Buffer _escapeSpecials(const BufferSpan& src);
	Buffer _unescapeSpecials(const BufferSpan& src);
	unsigned char _sumChecksum(const unsigned char* src, size_t size);
	unsigned char _sumChecksum(const BufferSpan& src);
	//Package a serialized command according to the protocol
	/*
	References:
	* DataType
	* CommandBluetoothSender.sendCommandWithRetries
	* BluetoothSenderWrapper.sendCommandViaBluetooth
	* 
	* Serialized data format: <START_MARKER>ESCAPE_SPECIALS(<DATA_TYPE><SEQ_NUMBER><BIG ENDIAN 4 BYTE SIZE OF UNESCAPED DATA><DATA><1 BYTE CHECKSUM>)<END_MARKER>
	*/
	Buffer packageDataForBt(const uint8_t* data, size_t len, DATA_TYPE dataType, unsigned int seqNumber);

	Buffer serializeMpToggle2(bool enabled);
	Buffer serializeEqualizerSetting(unsigned char preset, char bass, std::vector<int> const& bands);
	Buffer serializeEqualizerSetting(unsigned char preset);

	// POD Wrapper for any Buffer (of messages) that contains the command payload (which may also be size 0,i.e. ACKs)
	struct CommandMessage
	{
		Buffer messageBytes{};

		CommandMessage() = default;

		CommandMessage(BufferSpan const& buf) : messageBytes(_unescapeSpecials(buf)) {
			assert(verify());
		}
		CommandMessage(BufferSpan&& buf) : messageBytes(_unescapeSpecials(buf)) {
			assert(verify());
		}
		CommandMessage(DATA_TYPE dataType, const uint8_t* data, size_t len, unsigned char seqNumber)
			: messageBytes(packageDataForBt(data, len, dataType, seqNumber))
		{}

		DATA_TYPE getDataType() const { return static_cast<DATA_TYPE>(messageBytes[1]); }
		unsigned char getSeqNumber() const { return messageBytes[2]; }
		int getSize() const { return bytesToIntBE(&messageBytes[3]); }
		unsigned char getChkSum() const { return messageBytes[7 + getSize()]; }

		Buffer::const_iterator begin() const { return messageBytes.begin() + 7; }
		Buffer::const_iterator end() const { return begin() + getSize(); }
		Buffer::value_type operator[](int i) const { return *(begin() + i); }

		const uint8_t* data() const { return messageBytes.data() + 7; }
		operator BufferSpan() const { return BufferSpan(data(), getSize()); }

		template <typename TPayload, typename... TArgs>
		const TPayload* as(TArgs&&... args) const {
			if constexpr (TPayload::VARIABLE_SIZE_NEEDS_SERIALIZATION)
				static_assert(sizeof(TPayload) == 0, "Please use TPayload::deserialize() for this payload");
			if (!TPayload::isValid(*this, std::forward<TArgs>(args)...))
				throw std::runtime_error("Invalid incoming payload");
			return reinterpret_cast<const TPayload*>(data());
		}

		Buffer const& getMessage() const { return messageBytes; }

		unsigned char calcChkSum() const { return _sumChecksum(messageBytes.data() + 1, messageBytes.size() - 3); }
		bool verify() const { return messageBytes.size() >= 7 && getChkSum() == calcChkSum(); }
	};
}

