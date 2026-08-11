#include "MockTransport.hpp"

#include <mdr-c/Headphones.h>

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace
{
    using mdrtest::GetLastError;
    using mdrtest::MockTransport;

    int gFailures = 0;

    void Check(bool condition, std::string_view message)
    {
        if (condition)
            return;
        std::cerr << "FAIL: " << message << '\n';
        ++gFailures;
    }

    std::vector<uint8_t> ReadPacket(const std::filesystem::path& path)
    {
        std::ifstream input(path, std::ios::binary);
        return {
            std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>(),
        };
    }

    bool Replay(
        std::span<const uint8_t> frame,
        MDRHeadphones* headphones,
        MockTransport& transport,
        MDREvent& event
    )
    {
        transport.Load(frame);
        for (size_t attempt = 0; attempt < frame.size() + 4; ++attempt)
        {
            event = MDR_EVENT_NONE;
            if (mdrHeadphonesPoll(headphones, &event) != MDR_RESULT_OK)
                return false;
            if (event != MDR_EVENT_NONE)
                return true;
        }
        return false;
    }

    void ReplayDirectory(const std::filesystem::path& root)
    {
        std::error_code error;
        const bool isDirectory = std::filesystem::is_directory(root, error);
        Check(
            isDirectory && !error,
            "replay path is a directory: " + root.string()
        );
        if (error || !isDirectory)
            return;

        std::vector<std::filesystem::path> packets;
        std::filesystem::directory_iterator iterator(root, error);
        const std::filesystem::directory_iterator end;
        while (!error && iterator != end)
        {
            const auto& entry = *iterator;
            const std::string filename = entry.path().filename().string();
            if (
                entry.is_regular_file(error)
                && entry.path().extension() == ".bin"
                && filename.find("-rx.") != std::string::npos
            )
            {
                packets.push_back(entry.path());
            }
            iterator.increment(error);
        }
        Check(!error, "replay directory traversal succeeds");
        Check(!packets.empty(), "replay directory contains RX .bin packets");
        std::ranges::sort(packets);

        MockTransport transport;
        MDRHeadphones* headphones = nullptr;
        Check(
            mdrHeadphonesCreate(MDR_ABI_VERSION, &transport.connection, &headphones) == MDR_RESULT_OK,
            "opaque headphones session opens"
        );
        if (headphones == nullptr)
            return;

        size_t replayed = 0;
        for (const auto& path : packets)
        {
            const std::string filename = path.filename().string();
            const bool isAck = filename.find(".type-ACK-") != std::string::npos;
            const bool isData = filename.find(".type-DATA_MDR") != std::string::npos;
            Check(
                isAck || isData,
                "packet is replayable RX data or ACK: " + path.string()
            );
            if (!isAck && !isData)
                continue;

            const auto frame = ReadPacket(path);
            MDREvent event = MDR_EVENT_NONE;
            const bool producedEvent = Replay(frame, headphones, transport, event);
            const bool dispatched = producedEvent;
            Check(dispatched, "packet dispatches: " + path.string());
            if (!dispatched)
            {
                std::cerr
                    << "  event: " << event
                    << ", ready: " << mdrHeadphonesIsReady(headphones)
                    << ", dirty: " << mdrHeadphonesIsDirty(headphones)
                    << ", initialized: " << mdrHeadphonesIsInitialized(headphones)
                    << ", last error: " << GetLastError(headphones)
                    << '\n';
            }
            ++replayed;
        }

        MDRModel identity{};
        Check(
            mdrHeadphonesGetModel(headphones, &identity) == MDR_RESULT_OK,
            "replayed session identity is readable"
        );
        Check(
            identity.protocol_version != 0,
            "capture selects an MDR protocol backend"
        );

        mdrHeadphonesDestroy(headphones);
        std::cout << "Replayed " << replayed << " packet(s) from "
                  << root.string() << '\n';
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: mdr_replay_tests <packet-directory>\n";
        return 2;
    }

    ReplayDirectory(argv[1]);
    if (gFailures)
        std::cerr << gFailures << " test assertion(s) failed\n";
    return gFailures ? 1 : 0;
}
