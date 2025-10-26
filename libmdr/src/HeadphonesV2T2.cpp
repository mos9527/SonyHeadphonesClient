#include <algorithm>

#include "Headphones.hpp"
using namespace v2::t2;
void HandleSupportFunctionT2(MDRHeadphones* self, Span<const UInt8> cmd)
{
    ConnectRetSupportFunction res;
    ConnectRetSupportFunction::Deserialize(cmd.data(), res);
    std::ranges::fill(self->mSupportFunctions.table2Functions, 0);
    for (auto fun : res.supportFunctions)
        self->mSupportFunctions.table2Functions[static_cast<UInt8>(fun.table2)] = true;
}
void MDRHeadphones::HandleCommandV2T2(Span<const UInt8> cmd, MDRCommandSeqNumber seq)
{
    using namespace v2::t2;
    using enum Command;
    CommandBase base;
    CommandBase::Deserialize(cmd.data(), base);
    switch (base.command)
    {
    case CONNECT_RET_SUPPORT_FUNCTION:
        HandleSupportFunctionT2(this, cmd);
        return Awake(AWAIT_SUPPORT_FUNCTION);
    default:
        fmt::println("** Unhandled {}", base.command);
    }
}
