#include <mdr-c/Headphones.h>

#include <algorithm>
#include <cstddef>
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
    int gFailures = 0;

    void Check(bool condition, std::string_view message)
    {
        if (condition)
            return;
        std::cerr << "FAIL: " << message << '\n';
        ++gFailures;
    }

    struct MockTransport
    {
        std::vector<uint8_t> rx;
        std::vector<uint8_t> tx;
        size_t offset{};
        MDRConnection connection{
            this,
            Connect,
            Disconnect,
            Receive,
            Send,
            Poll,
            GetDevices,
            FreeDevices,
            GetLastError,
        };

        void Load(std::span<const uint8_t> bytes)
        {
            rx.assign(bytes.begin(), bytes.end());
            offset = 0;
        }

        static MDRResult Connect(void*, const char*, const char*)
        {
            return MDR_RESULT_OK;
        }

        static void Disconnect(void*)
        {
        }

        static MDRResult Receive(void* user, char* destination, int size, int* received)
        {
            auto& self = *static_cast<MockTransport*>(user);
            *received = 0;
            if (self.offset == self.rx.size())
                return MDR_RESULT_INPROGRESS;

            const size_t count = std::min(
                static_cast<size_t>(size),
                self.rx.size() - self.offset
            );
            std::copy_n(
                self.rx.begin() + static_cast<ptrdiff_t>(self.offset),
                count,
                destination
            );
            self.offset += count;
            *received = static_cast<int>(count);
            return MDR_RESULT_OK;
        }

        static MDRResult Send(void* user, const char* source, int size, int* sent)
        {
            auto& self = *static_cast<MockTransport*>(user);
            self.tx.insert(self.tx.end(), source, source + size);
            *sent = size;
            return MDR_RESULT_OK;
        }

        static MDRResult Poll(void*, int)
        {
            return MDR_RESULT_OK;
        }

        static MDRResult GetDevices(void*, MDRDeviceInfo** devices, int* count)
        {
            *devices = nullptr;
            *count = 0;
            return MDR_RESULT_OK;
        }

        static MDRResult FreeDevices(void*, MDRDeviceInfo** devices)
        {
            *devices = nullptr;
            return MDR_RESULT_OK;
        }

        static const char* GetLastError(void*)
        {
            return "mock transport";
        }
    };

    std::string GetLastError(MDRHeadphones* headphones)
    {
        uint32_t size = 0;
        if (
            mdrHeadphonesGetText(
                headphones, MDR_TEXT_LAST_ERROR, 0, nullptr, &size
            ) != MDR_RESULT_OK
        )
        {
            return {};
        }
        std::vector<char> buffer(size);
        if (
            mdrHeadphonesGetText(
                headphones, MDR_TEXT_LAST_ERROR, 0, buffer.data(), &size
            ) != MDR_RESULT_OK
        )
        {
            return {};
        }
        return buffer.data();
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

    // The library now requires the protocol family at construction time, so infer it from the
    // capture: the CONNECT_RET_PROTOCOL_INFO reply (V1 payload = 4 bytes, V2 payload = 8 bytes).
    MDRProtocolVersion DetectProtocolVersion(const std::vector<std::filesystem::path>& packets)
    {
        for (const auto& path : packets)
        {
            const auto frame = ReadPacket(path);
            // Frames are bounded by '>' (SOF) ... '<' (EOF).
            const auto sof = std::ranges::find(frame, static_cast<uint8_t>(0x3Eu));
            if (sof == frame.end())
                continue;
            const auto eof = std::ranges::find(sof + 1, frame.end(), static_cast<uint8_t>(0x3Cu));
            if (eof == frame.end() || sof + 1 >= eof)
                continue;
            // Unescape the framed payload ('=' sentinel introduces 0x2C/0x2D/0x2E for <, =, >).
            std::vector<uint8_t> inner;
            for (auto it = sof + 1; it != eof; ++it)
            {
                if (*it == 0x3Du)
                {
                    if (++it == eof) break;
                    switch (*it)
                    {
                    case 0x2Cu: inner.push_back(0x3Cu); break;
                    case 0x2Du: inner.push_back(0x3Du); break;
                    case 0x2Eu: inner.push_back(0x3Eu); break;
                    default: inner.push_back(*it); break;
                    }
                }
                else
                    inner.push_back(*it);
            }
            // inner = [type, seq, sizeBE(4), data..., checksum]
            if (inner.size() < 7)
                continue;
            if (inner[0] != 0x0Cu) // MDRDataType::DATA_MDR
                continue;
            const int32_t size = (inner[2] << 24u) | (inner[3] << 16u) |
                                 (inner[4] << 8u) | inner[5];
            if (size < 1)
                continue;
            const size_t dataOffset = 6;
            const size_t dataEnd = dataOffset + static_cast<size_t>(size);
            if (dataEnd + 1 > inner.size())
                continue;
            uint8_t sum = 0;
            for (size_t i = 0; i < dataEnd; ++i)
                sum += inner[i];
            if (sum != inner[dataEnd])
                continue;
            if (inner[dataOffset] != 0x01u) // Command::CONNECT_RET_PROTOCOL_INFO
                continue;
            const size_t payloadSize = dataEnd - dataOffset;
            if (payloadSize == 8u)
                return MDR_PROTOCOL_V2;
            if (payloadSize == 4u)
                return MDR_PROTOCOL_V1;
        }
        return MDR_PROTOCOL_V2;
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
            mdrHeadphonesCreate(
                MDR_ABI_VERSION, &transport.connection, DetectProtocolVersion(packets), &headphones
            ) == MDR_RESULT_OK,
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
