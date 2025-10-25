#include "Command.hpp"

#include <algorithm>

namespace mdr
{
    UInt8 Checksum(Span<const UInt8> data)
    {
        UInt8 sum = 0;
        for (UInt8 byte : data)
            sum += byte;
        return sum;
    }

    MDRBuffer Escape(Span<const UInt8> data) noexcept
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

    MDRBuffer Unescape(Span<const UInt8> data) noexcept
    {
        MDRBuffer res;
        res.reserve(data.size());
        const UInt8 *pData = data.data(), *pEnd = data.data() + data.size();
        while (pData != pEnd)
        {
            UInt8 byte = *pData++;
            if (byte == kEscapedByteSentry)
            {
                if (pData == pEnd)
                    return {};
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
                    return {};
                }
            }
            else
            {
                res.emplace_back(byte);
            }
        }
        return res;
    }

    MDRBuffer MDRPackCommand(MDRDataType type, MDRCommandSeqNumber seq, Span<const UInt8> serializedData) noexcept
    {
        MDRBuffer unescaped;
        unescaped.reserve((serializedData.size() << 1u) + 7u); // Worst case scenario
        unescaped.emplace_back(static_cast<UInt8>(type));
        unescaped.emplace_back(seq);
        // Int32BE unescaped size
        {
            Int32BE dataSize{static_cast<int32_t>(serializedData.size())};
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

    MDRUnpackResult MDRUnpackCommand(Span<const UInt8> command, MDRBuffer& outData, MDRDataType& outType,
                           MDRCommandSeqNumber& outSeq) noexcept
    {

        if (command.size() < 2)
            return MDRUnpackResult::INCOMPLETE;
        if (command.front() != kStartMarker || command.back() != kEndMarker)
            return MDRUnpackResult::BAD_MARKER;
        // Skip start/end markers
        command = command.subspan(1, command.size() - 1);
        MDRBuffer unescaped = Unescape(command);
        command = unescaped;
        // Type,seq
        outType = static_cast<MDRDataType>(command[0]);
        outSeq = command[1];
        command = command.subspan(2);
        // Big-endian in
        Int32BE outSize = command[0] << 24u | command[1] << 16u | command[2] << 8u | command[3];
        Span data = command.subspan(4);
        // Data...,checksum
        UInt8 checksum = data.back();
        // Checksum incl. type,seq,data
        UInt8 curChecksum = Checksum(Span(unescaped).subspan(0, unescaped.size() - 1));
        if (checksum != curChecksum)
            return MDRUnpackResult::BAD_CHECKSUM;
        // Data...
        data = data.subspan(0, data.size() - 1);
        if (data.size() != outSize) [[unlikely]]
            return MDRUnpackResult::INCOMPLETE;
        outData.resize(data.size());
        std::ranges::copy(data, outData.begin());
        return MDRUnpackResult::OK;
    }
}
