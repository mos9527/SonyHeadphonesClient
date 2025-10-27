#include <mdr/Headphones.hpp>
#include <algorithm>
namespace mdr
{
    using namespace v2::t2;

    int HandleSupportFunctionT2(MDRHeadphones* self, Span<const UInt8> cmd)
    {
        ConnectRetSupportFunction res;
        ConnectRetSupportFunction::Deserialize(cmd.data(), res);
        std::ranges::fill(self->mSupport.table2Functions, false);
        for (auto fun : res.supportFunctions)
            self->mSupport.table2Functions[static_cast<UInt8>(fun.table2)] = true;
        self->Awake(MDRHeadphones::AWAIT_SUPPORT_FUNCTION);
        return MDR_HEADPHONES_STATE_UPDATE;
    }

    int MDRHeadphones::HandleCommandV2T2(Span<const UInt8> cmd, MDRCommandSeqNumber seq)
    {
        using namespace v2::t2;
        using enum Command;
        CommandBase base;
        CommandBase::Deserialize(cmd.data(), base);
        switch (base.command)
        {
        case CONNECT_RET_SUPPORT_FUNCTION:
            return HandleSupportFunctionT2(this, cmd);
        default:
            fmt::println("** Unhandled {}", base.command);
            break;
        }
        return MDR_HEADPHONES_NO_EVENT;
    }
}
