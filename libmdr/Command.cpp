#include "Command.hpp"
namespace mdr
{
    constexpr uint8_t kEscapedByteSentry = 0x3D; // '='
    constexpr uint8_t kEscaped60 = 44; // 0x3C -> 0x3D 0x2C
    constexpr uint8_t kEscaped61 = 45; // 0x3D -> 0x3D 0x2D
    constexpr uint8_t kEscaped62 = 46; // 0x3E -> 0x3D 0x2E
    inline constexpr char kStartMarker{ 62 }; // >
    inline constexpr char kEndMarker{ 60 }; // <
    UInt8 Checksum(Span<const UInt8> data)
    {
        UInt8 sum = 0;
        for (UInt8 byte : data)
            sum += byte;
        return sum;
    }
    MDRBuffer Escape(Span<const UInt8> data)
    {
        MDRBuffer res;
        res.reserve(data.size() << 1u); // Worst case scenario
        for (UInt8 byte : data)
        {
            switch (byte)
            {
            case 60:
                res.emplace_back(kEscapedByteSentry), res.emplace_back(kEscaped60);
                break;
            case 61:
                res.emplace_back(kEscapedByteSentry), res.emplace_back(kEscaped61);
                break;
            case 62:
                res.emplace_back(kEscapedByteSentry), res.emplace_back(kEscaped62);
                break;
            default:
                res.emplace_back(byte);
                break;
            }
        }
        return res;
    }
    MDRBuffer Unescape(Span<const UInt8> data)
    {
        MDRBuffer res;
        res.reserve(data.size());
        const UInt8 *pData = data.data(), *pEnd = data.data() + data.size();
        while (pData != pEnd)
        {
            UInt8 byte = *pData++;
            if (byte == kEscapedByteSentry)
            {
                MDR_CHECK(pData != pEnd, "Invalid escape sequence at end of data");
                UInt8 next = *pData++;
                switch (next)
                {
                case kEscaped60:
                    res.emplace_back(60);
                    break;
                case kEscaped61:
                    res.emplace_back(61);
                    break;
                case kEscaped62:
                    res.emplace_back(62);
                    break;
                default:
                    MDR_CHECK(false, "Invalid escape sequence byte: {}", next);
                }
            }
            else
            {
                res.emplace_back(byte);
            }
        }
        return res;
    }
    MDRBuffer Pack(MDRDataType type, MDRCommandSeqNumber seq, Span<UInt8> serializedData)
    {
        MDR_CHECK(is_valid(type), "Invalid data type");
        MDRBuffer unescaped;
        unescaped.reserve((serializedData.size() << 1u) + 7u); // Worst case scenario
        unescaped.emplace_back(static_cast<UInt8>(type));
        unescaped.emplace_back(seq);
        // Int32BE unescaped size
        {
            Int32BE dataSize{ static_cast<int32_t>(serializedData.size()) };
            UInt8* pData = unescaped.data();
            unescaped.insert(unescaped.end(), 4u, 0u);
            MDRPod::Write(dataSize, &pData);
        }
        unescaped.insert(unescaped.end(), serializedData.begin(), serializedData.end());
        unescaped.emplace_back(Checksum(unescaped));
        MDRBuffer res;
        res.emplace_back(static_cast<UInt8>(kStartMarker));
        MDRBuffer escaped = Escape(unescaped);
        res.insert(res.end(), escaped.begin(), escaped.end());
        res.emplace_back(static_cast<UInt8>(kEndMarker));
        return res;
    }

    MDRBuffer Unpack(Span<const UInt8> packedData, MDRDataType& outType, MDRCommandSeqNumber& outSeq)
    {
        MDRBuffer res, serializedData;
        MDR_CHECK(packedData.size() >= 7u, "Packed data too small");
        MDR_CHECK(packedData[0] == static_cast<UInt8>(kStartMarker), "Invalid start marker");
        Span payload{ res.begin() + 1, res.end() - 1 };
        res = Unescape(payload);
        UInt8* pData = res.data();
        MDRPod::Read(&pData, outType);
        MDR_CHECK(is_valid(outType), "Invalid data type");
        MDRPod::Read(&pData, outSeq);
        Int32BE dataSize;
        MDRPod::Read(&pData, dataSize);
        MDR_CHECK(dataSize >= 0, "Invalid data size. Got {}", static_cast<int32_t>(dataSize));
        serializedData.resize(dataSize);
        std::memcpy(serializedData.data(), pData, dataSize);
        pData += static_cast<size_t>(dataSize);
        UInt8 checksum, calcChecksum;
        MDRPod::Read(&pData, checksum);
        MDR_CHECK(checksum == (calcChecksum=Checksum(payload)), "Invalid checksum. Got {}, expected {}", checksum, calcChecksum);
        return serializedData;
    }
}
