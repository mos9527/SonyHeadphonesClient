#include "CommandSerializer.h"

constexpr uint8_t ESCAPED_BYTE_SENTRY = 0x3D;
constexpr uint8_t ESCAPED_60 = 44; // 0x3C -> 0x3D 0x2C
constexpr uint8_t ESCAPED_61 = 45; // 0x3D -> 0x3D 0x2D
constexpr uint8_t ESCAPED_62 = 46; // 0x3E -> 0x3D 0x2E
constexpr int MAX_STEPS_WH_1000_XM3 = 19;

namespace CommandSerializer
{
	Buffer _escapeSpecials(const Buffer& src)
	{
		Buffer ret;
		ret.reserve(src.size());

		for (auto&& b : src)
		{
			switch (b)
			{
			case 60:
				ret.push_back(ESCAPED_BYTE_SENTRY);
				ret.push_back(ESCAPED_60);
				break;

			case 61:
				ret.push_back(ESCAPED_BYTE_SENTRY);
				ret.push_back(ESCAPED_61);
				break;

			case 62:
				ret.push_back(ESCAPED_BYTE_SENTRY);
				ret.push_back(ESCAPED_62);
				break;

			default:
				ret.push_back(b);
				break;
			}
		}

		return ret;
	}

	Buffer _unescapeSpecials(const Buffer& src)
	{
		Buffer ret;
		ret.reserve(src.size());

		for (size_t i = 0; i < src.size(); i++)
		{
			auto currByte = src[i];
			if (currByte == ESCAPED_BYTE_SENTRY)
			{
				if (i == src.size() - 1)
				{
					throw std::runtime_error("No data left for escaped byte data");
				}
				i = i + 1;
				switch (src[i])
				{
				case ESCAPED_60:
					ret.push_back(60);
					break;

				case ESCAPED_61:
					ret.push_back(61);
					break;

				case ESCAPED_62:
					ret.push_back(62);
					break;

				default:
					throw std::runtime_error("Unexpected escaped byte");
					break;
				}
			}
			else
			{
				ret.push_back(currByte);
			}
		}

		return ret;
	}

	uint8_t _sumChecksum(const uint8_t* src, size_t size)
	{
		uint8_t accumulator = 0;
		for (size_t i = 0; i < size; i++)
		{
			accumulator += src[i];
		}
		return accumulator;
	}

	uint8_t _sumChecksum(const Buffer& src)
	{
		return _sumChecksum(src.data(), src.size());
	}

	Buffer packageDataForBt(const Buffer& src, DATA_TYPE dataType, unsigned int seqNumber)
	{
		//Reserve at least the size for the size, start&end markers, and the source
		Buffer toEscape;
		toEscape.reserve(src.size() + 2 + sizeof(int));
		Buffer ret;
		ret.reserve(toEscape.capacity());
		toEscape.push_back(static_cast<uint8_t>(dataType));
		toEscape.push_back(seqNumber);
		auto retSize = intToBytesBE(static_cast<unsigned int>(src.size()));
		//Insert data size
		toEscape.insert(toEscape.end(), retSize.begin(), retSize.end());
		//Insert command data
		toEscape.insert(toEscape.end(), src.begin(), src.end());

		auto checksum = _sumChecksum(toEscape);
		toEscape.push_back(checksum);
		toEscape = _escapeSpecials(toEscape);

		
		ret.push_back(START_MARKER);
		ret.insert(ret.end(), toEscape.begin(), toEscape.end());
		ret.push_back(END_MARKER);


		// CommandMessage will be chunked if it's larger than MAX_BLUETOOTH_MESSAGE_SIZE, just crash to deal with it for now
		if (ret.size() > MAX_BLUETOOTH_MESSAGE_SIZE)
		{
			throw std::runtime_error("Exceeded the max bluetooth message size, and I can't handle chunked messages");
		}

		return ret;
	}

	Buffer serializeNcAndAsmSetting(
		char version, bool finalSetting, NC_ASM_EFFECT ncAsmEffect, NC_ASM_SETTING_TYPE ncAsmSettingType,
		ASM_ID voicePassthrough, char asmLevel, bool autoAsm, AUTO_ASM_SENSITIVITY autoAsmSensitivity)
	{
		// 0x68 | 0x17 | [Not Dragging ASM Slider?] | [NC & ASM On?] | [NC:0 ASM:1] | [Voice Passthrough?] | [ASM Level]
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::NCASM_PARAM_SET)); // 0x68
		ret.push_back(version);
		ret.push_back(finalSetting ? 0x01 : 0x00);
		ret.push_back(static_cast<uint8_t>(ncAsmEffect));
		ret.push_back(static_cast<uint8_t>(ncAsmSettingType));
		ret.push_back(static_cast<uint8_t>(voicePassthrough));
		ret.push_back(asmLevel);
		if (version >= 0x19) {
			// [Auto ASM?] | [Standard:0 High:1 Low:2]
			ret.push_back(autoAsm ? 0x01 : 0x00);
			ret.push_back(static_cast<uint8_t>(autoAsmSensitivity));
		}
		return ret;
	}

	Buffer serializeVoiceGuidanceSetting(char volume) {
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::VOICEGUIDANCE_PARAM_SET));
		ret.push_back(0x20);
		ret.push_back(volume); // Guidance Volume
		ret.push_back(0x00);
		return ret;
	}
	Buffer serializeVolumeSetting(char volume)
	{
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::PLAYBACK_STATUS_SET));
		ret.push_back(0x20);
		ret.push_back(volume); // Volume
		return ret;
	}
	Buffer serializeMultipointSwitch(const char* macString)
	{
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::MULTIPOINT_DEVICE_SET));
		ret.push_back(0x01);
		ret.insert(ret.end(), macString, macString + 6 * 3 - 1); // Mac string. e.g. XX:XX:XX:XX:XX:XX
		return ret;
	}
	Buffer serializePlayControl(PLAYBACK_CONTROL control)
	{
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::PLAYBACK_STATUS_CONTROL_SET));
		ret.push_back(0x01);
		ret.push_back(0x00);
		ret.push_back(static_cast<uint8_t>(control));
		return ret;
	}

	Buffer serializePowerOff()
	{
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::POWER_OFF));
		ret.push_back(0x03);
		ret.push_back(0x01);
		return ret;
	}


	Buffer serializeMpToggle(bool enabled)
	{
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::MULTIPOINT_ETC_ENABLE_SET));
		ret.push_back(static_cast<uint8_t>(0xD2));
		ret.push_back(0x00);
		ret.push_back(!enabled); // 1 (on->off) 0 (off->on)
		return ret;
	}

	// from https://github.com/Freeyourgadget/Gadgetbridge/blob/master/app/src/main/java/nodomain/freeyourgadget/gadgetbridge/service/devices/sony/headphones/protocol/impl/v3/SonyProtocolImplV3.java
	Buffer serializeSpeakToChatConfig(char sensitivity, char timeout)
	{
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::SPEAK_TO_CHAT_SET));
		ret.push_back(0x0c);
		ret.push_back(sensitivity);
		ret.push_back(timeout);
		return ret;
	}

	Buffer serializeMpToggle2(bool enabled)
	{
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::MULTIPOINT_ENABLE_SET_2));
		ret.push_back(0x00);
		ret.push_back(0x07);
		ret.push_back(0x01);
		return ret;
	}

	// from https://github.com/Freeyourgadget/Gadgetbridge/blob/master/app/src/main/java/nodomain/freeyourgadget/gadgetbridge/service/devices/sony/headphones/protocol/impl/v3/SonyProtocolImplV3.java
	Buffer serializeSpeakToChatEnabled(bool enabled)
	{
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::AUTOMATIC_POWER_OFF_BUTTON_MODE_SET));
		ret.push_back(0x0c);
		ret.push_back(!enabled);
		ret.push_back(0x01);
		return ret;
	}

	// from https://github.com/Freeyourgadget/Gadgetbridge/blob/master/app/src/main/java/nodomain/freeyourgadget/gadgetbridge/service/devices/sony/headphones/protocol/impl/v3/SonyProtocolImplV3.java
	//      https://github.com/Plutoberth/SonyHeadphonesClient/commit/66d8e52aad4ffd08aa78e811a23f67a5bad07d9a
	Buffer serializeEqualizerSetting(uint8_t preset, char bass, std::vector<int> const& bands)
	{
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::EQUALIZER_SET));
		ret.push_back(0x00);
		ret.push_back(preset);
		ret.push_back(0x06); // data size
		ret.push_back(bass + 10);
		ret.push_back(bands[0] + 10);
		ret.push_back(bands[1] + 10);
		ret.push_back(bands[2] + 10);
		ret.push_back(bands[3] + 10);
		ret.push_back(bands[4] + 10);
		return ret;
	}
	Buffer serializeEqualizerSetting(uint8_t preset)
	{
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::EQUALIZER_SET));
		ret.push_back(0x00);
		ret.push_back(preset);
		ret.push_back(0x00); // data size		
		return ret;
	}
	Buffer serializeTouchSensorAssignment(TOUCH_SENSOR_FUNCTION funcL, TOUCH_SENSOR_FUNCTION funcR)
	{
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::AUTOMATIC_POWER_OFF_BUTTON_MODE_SET));
		ret.push_back(0x03);
		ret.push_back(0x02);
		ret.push_back(static_cast<uint8_t>(funcL));
		ret.push_back(static_cast<uint8_t>(funcR));
		return ret;
	}
	Buffer serializeOnCallVoiceCaptureSetting(bool enabled)
	{
		Buffer ret;
		ret.push_back(static_cast<uint8_t>(COMMAND_TYPE::MULTIPOINT_ETC_ENABLE_SET));
		ret.push_back(0xD1);
		ret.push_back(0x00);
		ret.push_back(!enabled);
		return ret;
	}
}

