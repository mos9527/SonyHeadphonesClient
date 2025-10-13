#include "CommandSerializer.h"

constexpr uint8_t ESCAPED_BYTE_SENTRY = 0x3D;
constexpr uint8_t ESCAPED_60 = 44; // 0x3C -> 0x3D 0x2C
constexpr uint8_t ESCAPED_61 = 45; // 0x3D -> 0x3D 0x2D
constexpr uint8_t ESCAPED_62 = 46; // 0x3E -> 0x3D 0x2E
constexpr int MAX_STEPS_WH_1000_XM3 = 19;

namespace CommandSerializer
{
	Buffer _escapeSpecials(const BufferSpan& src)
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

	Buffer _unescapeSpecials(const BufferSpan& src)
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

	uint8_t _sumChecksum(const BufferSpan& src)
	{
		return _sumChecksum(src.data(), src.size());
	}

	Buffer packageDataForBt(const uint8_t* data, size_t len, DATA_TYPE dataType, unsigned int seqNumber)
	{
		//Reserve at least the size for the size, start&end markers, and the source
		Buffer toEscape;
		toEscape.reserve(len + 2 + sizeof(int));
		Buffer ret;
		ret.reserve(toEscape.capacity());
		toEscape.push_back(static_cast<uint8_t>(dataType));
		toEscape.push_back(seqNumber);
		auto retSize = intToBytesBE(static_cast<unsigned int>(len));
		//Insert data size
		toEscape.insert(toEscape.end(), retSize.begin(), retSize.end());
		//Insert command data
		toEscape.insert(toEscape.end(), data, data + len);

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
}

