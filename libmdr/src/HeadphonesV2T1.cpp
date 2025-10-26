#include <algorithm>

#include "Headphones.hpp"
using namespace v2::t1;
void HandleProtocolInfo(MDRHeadphones* self, Span<const UInt8> cmd)
{
    ConnectRetProtocolInfo res;
    ConnectRetProtocolInfo::Deserialize(cmd.data(), res);
    self->mProto = {
        .version = res.protocolVersion,
        .hasTable1 = res.supportTable1Value == v2::MessageMdrV2EnableDisable::ENABLE,
        .hasTable2 = res.supportTable1Value == v2::MessageMdrV2EnableDisable::ENABLE
    };
}

void HandleSupportFunctionT1(MDRHeadphones* self, Span<const UInt8> cmd)
{
    ConnectRetSupportFunction res;
    ConnectRetSupportFunction::Deserialize(cmd.data(), res);
    std::ranges::fill(self->mSupportFunctions.table1Functions, 0);
    for (auto fun : res.supportFunctions)
        self->mSupportFunctions.table1Functions[static_cast<UInt8>(fun.table1)] = true;
}
void MDRHeadphones::HandleCommandV2T1(Span<const UInt8> cmd, MDRCommandSeqNumber seq)
{
    CommandBase base;
    CommandBase::Deserialize(cmd.data(), base);
    using enum Command;
    switch (base.command)
    {
    case CONNECT_RET_PROTOCOL_INFO:
        HandleProtocolInfo(this, cmd);
        return Awake(AWAIT_PROTOCOL_INFO);
    case CONNECT_RET_SUPPORT_FUNCTION:
        HandleSupportFunctionT1(this, cmd);
        return Awake(AWAIT_SUPPORT_FUNCTION);
    default:
        fmt::println("** Unhandled {}", base.command);
    }
}
