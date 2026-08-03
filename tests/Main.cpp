#include <mdr/Headphones.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
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
        mdr::Vector<mdr::UInt8> rx;
        mdr::Vector<mdr::UInt8> tx;
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

        static int Connect(void*, const char*, const char*)
        {
            return MDR_RESULT_OK;
        }

        static void Disconnect(void*)
        {
        }

        static int Receive(
            void* user, char* destination, int size, int* received
        )
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

        static int Send(
            void* user, const char* source, int size, int* sent
        )
        {
            auto& self = *static_cast<MockTransport*>(user);
            self.tx.insert(self.tx.end(), source, source + size);
            *sent = size;
            return MDR_RESULT_OK;
        }

        static int Poll(void*, int)
        {
            return MDR_RESULT_OK;
        }

        static int GetDevices(void*, MDRDeviceInfo** devices, int* count)
        {
            *devices = nullptr;
            *count = 0;
            return MDR_RESULT_OK;
        }

        static int FreeDevices(void*, MDRDeviceInfo** devices)
        {
            *devices = nullptr;
            return MDR_RESULT_OK;
        }

        static const char* GetLastError(void*)
        {
            return "mock transport";
        }
    };

    mdr::Vector<mdr::UInt8> ReadPacket(
        const std::filesystem::path& path
    )
    {
        std::ifstream input(path, std::ios::binary);
        return {
            std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>(),
        };
    }

    int Replay(
        mdr::Span<const mdr::UInt8> frame,
        mdr::MDRHeadphones& headphones,
        MockTransport& transport
    )
    {
        transport.rx.assign(frame.begin(), frame.end());
        transport.offset = 0;
        for (size_t attempt = 0; attempt < frame.size() + 4; ++attempt)
        {
            const int event = headphones.PollEvents();
            if (event == MDR_HEADPHONES_ERROR || event > MDR_HEADPHONES_IDLE)
                return event;
        }
        return MDR_HEADPHONES_IDLE;
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
        Check(
            !packets.empty(),
            "replay directory contains RX .bin packets"
        );
        std::ranges::sort(packets);

        MockTransport transport;
        mdr::MDRHeadphones headphones(&transport.connection);
        size_t replayed = 0;
        for (const auto& path : packets)
        {
            const auto frame = ReadPacket(path);
            mdr::MDRBuffer payload;
            mdr::MDRDataType type{};
            mdr::MDRCommandSeqNumber sequence{};
            const bool unpacked =
                mdr::MDRUnpackCommand(
                    frame, payload, type, sequence
                ) == mdr::MDRUnpackResult::OK;
            Check(unpacked, "packet unpacks: " + path.string());
            if (!unpacked)
                continue;

            const bool isAck = type == mdr::MDRDataType::ACK;
            const bool isData =
                type == mdr::MDRDataType::DATA_MDR
                || type == mdr::MDRDataType::DATA_MDR_NO2;
            Check(
                isAck || isData,
                "packet is replayable RX data or ACK: " + path.string()
            );
            if (!isAck && !isData)
                continue;

            const int event = Replay(frame, headphones, transport);
            const bool dispatched = isAck
                ? event == MDR_HEADPHONES_EVT_UNHANDLED
                : event != MDR_HEADPHONES_ERROR
                    && event != MDR_HEADPHONES_EVT_UNHANDLED
                    && event > MDR_HEADPHONES_IDLE;
            Check(dispatched, "packet dispatches: " + path.string());
            if (!dispatched)
            {
                std::cerr
                    << "  type: " << mdr::format_as(type)
                    << ", sequence: " << static_cast<unsigned>(sequence)
                    << ", event: " << event
                    << ", detail: " << headphones.GetLastError() << '\n';
            }
            ++replayed;
        }

        std::cout << "Replayed " << replayed << " packet(s) from "
                  << root.string() << '\n';
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: mdr_tests <packet-directory>\n";
        return 2;
    }

    ReplayDirectory(argv[1]);
    if (gFailures)
        std::cerr << gFailures << " replay assertion(s) failed\n";
    return gFailures ? 1 : 0;
}
