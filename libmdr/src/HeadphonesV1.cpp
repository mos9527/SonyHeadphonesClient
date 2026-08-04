#include "Details.hpp"

namespace mdr
{
    MDRTask MDRHeadphones::RequestInitV1()
    {
        SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "MDR V1 initialization is not implemented");
        co_return MDR_HEADPHONES_ERROR;
    }

    MDRTask MDRHeadphones::RequestSyncV1()
    {
        SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "MDR V1 synchronization is not implemented");
        co_return MDR_HEADPHONES_ERROR;
    }

    MDRTask MDRHeadphones::RequestCommitV1()
    {
        SetLastError(MDR_RESULT_ERROR_NOT_SUPPORTED, "MDR V1 commit is not implemented");
        co_return MDR_HEADPHONES_ERROR;
    }
}
