#include "DebuggerDetails.hpp"

#include <algorithm>
#include <cstdio>
#include <cstring>

namespace
{
    using namespace client::debugger;

    template <typename Function>
    bool ForEachDescriptor(Function&& function)
    {
        for (const PacketDescriptor& descriptor : Getv1t1Packets())
            if (!function(descriptor))
                return false;
        for (const PacketDescriptor& descriptor : Getv1t2Packets())
            if (!function(descriptor))
                return false;
        for (const PacketDescriptor& descriptor : Getv2t1Packets())
            if (!function(descriptor))
                return false;
        for (const PacketDescriptor& descriptor : Getv2t2Packets())
            if (!function(descriptor))
                return false;
        return true;
    }

    bool CheckDefaultRoundTrips()
    {
        size_t count{};
        const bool passed = ForEachDescriptor(
            [&](const PacketDescriptor& descriptor)
            {
                ++count;
                PacketInstance source{descriptor};
                mdr::UInt8 firstBytes[mdr::kMDRMaxPacketSize];
                const auto first = descriptor.encode(source.Value(), firstBytes, sizeof(firstBytes));
                if (!first)
                {
                    std::fprintf(stderr, "Unable to encode default %s\n", descriptor.name);
                    return false;
                }

                PacketInstance decoded{descriptor};
                const auto decode = descriptor.decode(decoded.Value(), {firstBytes, first.value});
                if (!decode)
                {
                    std::fprintf(stderr, "Unable to decode default %s\n", descriptor.name);
                    return false;
                }

                mdr::UInt8 secondBytes[mdr::kMDRMaxPacketSize];
                const auto second = descriptor.encode(decoded.Value(), secondBytes, sizeof(secondBytes));
                if (!second || second.value != first.value || std::memcmp(firstBytes, secondBytes, first.value) != 0)
                {
                    std::fprintf(stderr, "Round trip changed %s\n", descriptor.name);
                    return false;
                }
                return true;
            });
        if (count == 0)
        {
            std::fprintf(stderr, "No debugger packet descriptors were generated\n");
            return false;
        }
        return passed;
    }

    bool CheckInvalidValuesRemainEditable()
    {
        const PacketDescriptor* selected{};
        ForEachDescriptor(
            [&](const PacketDescriptor& descriptor)
            {
                if (std::string_view(descriptor.name) == "mdr::v2::t1::ConnectRetProtocolInfo")
                {
                    selected = &descriptor;
                }
                return true;
            });
        if (!selected)
            return false;

        PacketInstance packet{*selected};
        mdr::UInt8 bytes[mdr::kMDRMaxPacketSize];
        auto encoded = selected->encode(packet.Value(), bytes, sizeof(bytes));
        if (!encoded || encoded.value == 0)
            return false;
        bytes[0] ^= 0x7f;

        PacketInstance invalid{*selected};
        if (!selected->decode(invalid.Value(), {bytes, encoded.value}))
            return false;
        if (selected->validate(invalid.Value()))
        {
            std::fprintf(stderr, "Invalid debugger packet unexpectedly validated\n");
            return false;
        }

        mdr::UInt8 roundTrip[mdr::kMDRMaxPacketSize];
        const auto invalidEncoded = selected->encode(invalid.Value(), roundTrip, sizeof(roundTrip));
        return invalidEncoded && invalidEncoded.value == encoded.value &&
            std::memcmp(bytes, roundTrip, encoded.value) == 0;
    }
} // namespace

int main()
{
    if (!CheckDefaultRoundTrips())
        return 1;
    if (!CheckInvalidValuesRemainEditable())
        return 2;
    return 0;
}
