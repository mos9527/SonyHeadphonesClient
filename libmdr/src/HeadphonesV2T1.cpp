#include "Headphones.hpp"

void MDRHeadphones::HandleCommandV2T1(Span<const UInt8> command, MDRCommandSeqNumber seq)
{
    using namespace v2::t1;
    CommandBase base;
    CommandBase::Deserialize(command.data(), base);
}
