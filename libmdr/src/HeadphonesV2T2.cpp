#include "Headphones.hpp"

void MDRHeadphones::HandleCommandV2T2(Span<const UInt8> command, MDRCommandSeqNumber seq)
{
    using namespace v2::t2;
    using enum Command;
    CommandBase base;
    CommandBase::Deserialize(command.data(), base);
    switch (base.command)
    {
    default:
        fmt::println("** Unhandled {}", base.command);
    }
}
