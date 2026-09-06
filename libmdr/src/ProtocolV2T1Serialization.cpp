#include <mdr/ProtocolV2T1.hpp>

namespace mdr::v2::t1
{
    /***
    /* These come in:
    /* [count settingsActions] [count customizableActions] 
    /* [settingsActions...] [customizableActions...]
    /*
    /* Absolute madness. Thanks to @Amrsatrio for finding this.
    ***/
    MDRResult<size_t> AssignableSettingsPresetCapability::Write(
        const AssignableSettingsPresetCapability& data,
        UInt8** ppDstBuffer,
        size_t maxSize
    )
    {
        if (
            data.settingsActions.size() >= 256
            || data.settingsCustomizableActions.size() >= 256
            || (
                data.settingsActions.size() == 0
                && data.settingsCustomizableActions.size() == 0
            )
        )
        {
            return MDRResult<size_t>::Failure(
                MDR_RESULT_ERROR_INVALID_ARGUMENT
            );
        }

        UInt8* const begin = *ppDstBuffer;
        MDR_TRY_SIZE(
            size_t, MDRPod::Write(data.preset, ppDstBuffer, maxSize)
        );
        MDR_TRY_SIZE(
            size_t,
            MDRPod::Write(
                static_cast<UInt8>(data.settingsActions.size()),
                ppDstBuffer,
                maxSize
            )
        );
        MDR_TRY_SIZE(
            size_t,
            MDRPod::Write(
                static_cast<UInt8>(
                    data.settingsCustomizableActions.size()
                ),
                ppDstBuffer,
                maxSize
            )
        );
        for (const auto& action : data.settingsActions)
        {
            MDR_TRY_SIZE(
                size_t,
                AssignableSettingsAction::Write(
                    action, ppDstBuffer, maxSize
                )
            );
        }
        for (
            const auto& action : data.settingsCustomizableActions
        )
        {
            MDR_TRY_SIZE(
                size_t,
                AssignableSettingsCustomizableAction::Write(
                    action, ppDstBuffer, maxSize
                )
            );
        }
        return MDRResult<size_t>::Success(*ppDstBuffer - begin);
    }

    MDRResult<size_t> AssignableSettingsPresetCapability::Read(
        const UInt8** ppSrcBuffer,
        AssignableSettingsPresetCapability& out,
        size_t maxSize
    )
    {
        const UInt8* const begin = *ppSrcBuffer;
        UInt8 settingsActionCount{};
        UInt8 customizableActionCount{};
        MDR_TRY_SIZE(
            size_t, MDRPod::Read(ppSrcBuffer, out.preset, maxSize)
        );
        MDR_TRY_SIZE(
            size_t,
            MDRPod::Read(
                ppSrcBuffer, settingsActionCount, maxSize
            )
        );
        MDR_TRY_SIZE(
            size_t,
            MDRPod::Read(
                ppSrcBuffer, customizableActionCount, maxSize
            )
        );
        if (settingsActionCount == 0 && customizableActionCount == 0)
        {
            return MDRResult<size_t>::Failure(
                MDR_RESULT_ERROR_MALFORMED_PAYLOAD
            );
        }

        out.settingsActions.value.resize(settingsActionCount);
        for (auto& action : out.settingsActions.value)
        {
            MDR_TRY_SIZE(
                size_t,
                AssignableSettingsAction::Read(
                    ppSrcBuffer, action, maxSize
                )
            );
        }
        out.settingsCustomizableActions.value.resize(
            customizableActionCount
        );
        for (auto& action : out.settingsCustomizableActions.value)
        {
            MDR_TRY_SIZE(
                size_t,
                AssignableSettingsCustomizableAction::Read(
                    ppSrcBuffer, action, maxSize
                )
            );
        }
        return MDRResult<size_t>::Success(*ppSrcBuffer - begin);
    }
} // namespace mdr::v2::t1
