#include "PayloadRecorder.hpp"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_log.h>
#include <mdr/Command.hpp>
#include <mdr/ProtocolV2T1.hpp>
#include <mdr/ProtocolV2T2.hpp>

#include <atomic>
#include <chrono>
#include <string_view>

namespace
{
    constexpr std::string_view kCapturePrefix{"mdr-packet-"};

    std::atomic_uint64_t gCaptureCounter{0};
    mdr::String gCaptureDirectory;

    bool IsCaptureFilename(std::string_view filename)
    {
        return filename.starts_with(kCapturePrefix)
            && filename.ends_with(".bin");
    }

    SDL_EnumerationResult SDLCALL RemoveExistingCapture(
        void*,
        const char* directory,
        const char* filename
    )
    {
        if (!IsCaptureFilename(filename))
            return SDL_ENUM_CONTINUE;

        const mdr::String path = mdr::String(directory) + filename;
        SDL_PathInfo info{};
        if (
            !SDL_GetPathInfo(path.c_str(), &info)
            || info.type != SDL_PATHTYPE_FILE
        )
        {
            return SDL_ENUM_CONTINUE;
        }
        return SDL_RemovePath(path.c_str())
            ? SDL_ENUM_CONTINUE
            : SDL_ENUM_FAILURE;
    }

    const char* PayloadCommandName(
        mdr::MDRDataType type,
        mdr::UInt8 command
    )
    {
        switch (type)
        {
        case mdr::MDRDataType::DATA_MDR:
        case mdr::MDRDataType::SHOT_MDR:
            return mdr::v2::t1::format_as(
                static_cast<mdr::v2::t1::Command>(command)
            );
        case mdr::MDRDataType::DATA_MDR_NO2:
        case mdr::MDRDataType::SHOT_MDR_NO2:
            return mdr::v2::t2::format_as(
                static_cast<mdr::v2::t2::Command>(command)
            );
        default:
            return "NO_COMMAND";
        }
    }

    void RecordPacket(
        void*,
        MDRPacketDirection direction,
        const unsigned char* frame,
        int frameSize
    )
    {
        if (gCaptureDirectory.empty() || !frame || frameSize <= 0)
            return;

        const mdr::Span<const mdr::UInt8> packedFrame{
            frame,
            frame + frameSize
        };
        mdr::MDRBuffer payload;
        mdr::MDRDataType type{};
        mdr::MDRCommandSeqNumber sequence{};
        const bool unpacked =
            mdr::MDRUnpackCommand(
                packedFrame, payload, type, sequence
            ) == mdr::MDRUnpackResult::OK;
        const mdr::UInt8 command =
            unpacked && !payload.empty() ? payload.front() : 0;
        const auto timestamp = std::chrono::duration_cast<
            std::chrono::milliseconds
        >(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();
        const uint64_t counter = gCaptureCounter.fetch_add(
            1, std::memory_order_relaxed
        );
        const char* directionName =
            direction == MDR_PACKET_DIRECTION_RX ? "rx" : "tx";
        const mdr::String filename = mdr::Format(
            "{}{}-{:06}-{}.type-{}-{:02x}.seq-{:02x}."
            "cmd-{}-{:02x}.bin",
            kCapturePrefix,
            timestamp,
            counter,
            directionName,
            unpacked ? mdr::format_as(type) : "INVALID",
            static_cast<mdr::UInt8>(type),
            sequence,
            unpacked && !payload.empty()
                ? PayloadCommandName(type, command)
                : "NO_COMMAND",
            command
        );
        const mdr::String path =
            gCaptureDirectory + "/" + filename;
        SDL_IOStream* output = SDL_IOFromFile(path.c_str(), "wb");
        if (!output)
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Unable to open payload capture %s: %s",
                path.c_str(),
                SDL_GetError()
            );
            return;
        }
        const size_t written = SDL_WriteIO(
            output, frame, static_cast<size_t>(frameSize)
        );
        const bool closed = SDL_CloseIO(output);
        if (written != static_cast<size_t>(frameSize) || !closed)
        {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "Unable to write payload capture %s: %s",
                path.c_str(),
                SDL_GetError()
            );
        }
    }
}

bool clientPayloadRecorderConfigure(const char* directory)
{
    if (!directory || !*directory)
    {
        gCaptureDirectory.clear();
        gCaptureCounter.store(0, std::memory_order_relaxed);
        return true;
    }
    if (!SDL_CreateDirectory(directory))
        return false;
    if (!SDL_EnumerateDirectory(
        directory,
        RemoveExistingCapture,
        nullptr
    ))
    {
        return false;
    }
    gCaptureDirectory = directory;
    gCaptureCounter.store(0, std::memory_order_relaxed);
    return true;
}

void clientPayloadRecorderObserve(
    MDRPacketDirection direction,
    const unsigned char* frame,
    int frameSize
)
{
    RecordPacket(nullptr, direction, frame, frameSize);
}
