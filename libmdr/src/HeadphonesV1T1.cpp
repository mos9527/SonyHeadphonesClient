#include "Details.hpp"

namespace mdr
{
    int MDRHeadphones::HandleCommandV1T1(Span<const UInt8>, MDRCommandSeqNumber)
    {
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }
}
