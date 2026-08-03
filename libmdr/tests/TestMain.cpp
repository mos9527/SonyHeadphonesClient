#include <mdr/Headphones.hpp>
#include <mdr/ProtocolV1T1.hpp>
#include <mdr/ProtocolV1T2.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string_view>
#include <vector>

namespace
{
    int gFailures = 0;

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
        size_t chunkSize{std::numeric_limits<size_t>::max()};
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
                {
                    static_cast<size_t>(size),
                    self.chunkSize,
                    self.rx.size() - self.offset,
                }
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

    struct PacketObserver
    {
        struct Packet
        {
            MDRPacketDirection direction{};
            mdr::Vector<mdr::UInt8> frame;
        };

        mdr::Vector<Packet> packets;

        static void Observe(
            void* userData,
            MDRPacketDirection direction,
            const unsigned char* frame,
            int frameSize
        )
        {
            auto& self = *static_cast<PacketObserver*>(userData);
            self.packets.push_back({
                direction,
                {frame, frame + frameSize},
            });
        }
    };

    template <mdr::MDRIsSerializable T>
    bool RoundTrip(const T& input, T& output)
    {
        mdr::UInt8 buffer[mdr::kMDRMaxPacketSize]{};
        const auto written = T::Serialize(input, buffer, sizeof(buffer));
        if (!written)
            return false;
        auto read = T::Deserialize(buffer, written.value);
        if (!read)
            return false;
        output = std::move(read.value);
        return true;
    }

    int Replay(
        mdr::Span<const mdr::UInt8> frame,
        size_t chunkSize,
        mdr::MDRHeadphones& headphones,
        MockTransport& transport
    )
    {
        transport.rx.assign(frame.begin(), frame.end());
        transport.offset = 0;
        transport.chunkSize = chunkSize;
        for (size_t attempt = 0; attempt < frame.size() + 4; ++attempt)
        {
            const int event = headphones.PollEvents();
            if (event == MDR_HEADPHONES_ERROR || event > MDR_HEADPHONES_IDLE)
                return event;
        }
        return MDR_HEADPHONES_IDLE;
    }

    void TestFraming()
    {
        const mdr::UInt8 payload[]{
            0x10,
            static_cast<mdr::UInt8>(mdr::kStartMarker),
            static_cast<mdr::UInt8>(mdr::kEscapedByteSentry),
            static_cast<mdr::UInt8>(mdr::kEndMarker),
            0x20,
        };
        const auto packed = mdr::MDRPackCommand(
            mdr::MDRDataType::DATA_MDR, 0x7a, payload
        );
        mdr::MDRBuffer unpacked;
        mdr::MDRDataType type{};
        mdr::MDRCommandSeqNumber sequence{};
        Check(
            mdr::MDRUnpackCommand(
                packed, unpacked, type, sequence
            ) == mdr::MDRUnpackResult::OK,
            "escaped command frame unpacks"
        );
        Check(
            std::ranges::equal(payload, unpacked),
            "framing round trip preserves payload"
        );
        Check(
            type == mdr::MDRDataType::DATA_MDR && sequence == 0x7a,
            "framing round trip preserves metadata"
        );
    }

    void TestSerialization()
    {
        mdr::v1::t1::GetDeviceInfo v1Trivial{};
        decltype(v1Trivial) v1TrivialOutput{};
        Check(
            RoundTrip(v1Trivial, v1TrivialOutput)
                && v1TrivialOutput.command == v1Trivial.command
                && v1TrivialOutput.inquiredType
                    == v1Trivial.inquiredType,
            "V1 trivial payload round trip"
        );

        mdr::v1::t2::
            NotifyPeripheralExParamPairingDeviceManagementClassicBtConnectionControl
                v1Dynamic{};
        v1Dynamic.btDeviceAddress.value = "01:23:45:67:89:AB";
        decltype(v1Dynamic) v1DynamicOutput{};
        Check(
            RoundTrip(v1Dynamic, v1DynamicOutput)
                && v1DynamicOutput.btDeviceAddress.value
                    == v1Dynamic.btDeviceAddress.value,
            "V1 dynamic payload round trip"
        );

        mdr::v2::t2::PeripheralGetParam v2Trivial{};
        decltype(v2Trivial) v2TrivialOutput{};
        Check(
            RoundTrip(v2Trivial, v2TrivialOutput)
                && v2TrivialOutput.command == v2Trivial.command
                && v2TrivialOutput.inquiredType
                    == v2Trivial.inquiredType,
            "V2 trivial payload round trip"
        );

        mdr::v2::t2::PeripheralSetExtendedParamSourceSwitchControl
            v2Dynamic{};
        v2Dynamic.targetBdAddress.value = "AB:89:67:45:23:01";
        decltype(v2Dynamic) v2DynamicOutput{};
        Check(
            RoundTrip(v2Dynamic, v2DynamicOutput)
                && v2DynamicOutput.targetBdAddress.value
                    == v2Dynamic.targetBdAddress.value,
            "V2 dynamic payload round trip"
        );
    }

    void TestDynamicDispatch()
    {
        mdr::v2::t2::PeripheralRetStatusPairingDeviceManagementCommon
            response{};
        response.inquiredType =
            mdr::v2::t2::PeripheralInquiredType::
                PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE;
        response.btMode =
            mdr::v2::t2::PeripheralBluetoothMode::INQUIRY_SCAN_MODE;
        response.enableDisableStatus =
            mdr::v2::EnableDisable::ENABLE;

        mdr::UInt8 payload[mdr::kMDRMaxPacketSize]{};
        const auto serialized = decltype(response)::Serialize(
            response, payload, sizeof(payload)
        );
        Check(bool(serialized), "dispatch fixture serializes");
        if (!serialized)
            return;
        const auto packed = mdr::MDRPackCommand(
            mdr::MDRDataType::DATA_MDR_NO2,
            0,
            {payload, serialized.value}
        );

        MockTransport transport;
        mdr::MDRHeadphones headphones(&transport.connection);
        PacketObserver observer;
        headphones.SetPacketCallback(
            PacketObserver::Observe,
            &observer
        );
        const int event = Replay(
            packed, 3, headphones, transport
        );
        Check(
            event == MDR_HEADPHONES_EVT_BLUETOOTH_MODE,
            "fragmented V2T2 frame dispatches dynamically"
        );
        Check(
            headphones.mPairingMode.current,
            "dynamic dispatch updates headphones state"
        );
        Check(
            observer.packets.size() == 2
                && observer.packets[0].direction
                    == MDR_PACKET_DIRECTION_RX
                && observer.packets[0].frame == packed
                && observer.packets[1].direction
                    == MDR_PACKET_DIRECTION_TX,
            "packet callback observes complete RX and TX frames"
        );

        headphones.PollEvents();
        mdr::MDRBuffer ackPayload;
        mdr::MDRDataType ackType{};
        mdr::MDRCommandSeqNumber ackSequence{};
        Check(
            mdr::MDRUnpackCommand(
                transport.tx, ackPayload, ackType, ackSequence
            ) == mdr::MDRUnpackResult::OK
                && ackType == mdr::MDRDataType::ACK
                && ackSequence == 1,
            "dynamic dispatch sends the expected ACK"
        );
    }

    void TestBufferedFrames()
    {
        const auto first = mdr::MDRPackCommand(
            mdr::MDRDataType::ACK, 0, {}
        );
        const auto second = mdr::MDRPackCommand(
            mdr::MDRDataType::ACK, 1, {}
        );
        MockTransport transport;
        transport.rx.insert(
            transport.rx.end(), first.begin(), first.end()
        );
        transport.rx.insert(
            transport.rx.end(), second.begin(), second.end()
        );
        mdr::MDRHeadphones headphones(&transport.connection);
        Check(
            headphones.PollEvents() == MDR_HEADPHONES_EVT_UNHANDLED,
            "first buffered frame dispatches"
        );
        Check(
            headphones.PollEvents() == MDR_HEADPHONES_EVT_UNHANDLED,
            "second buffered frame dispatches"
        );
        Check(
            transport.offset == transport.rx.size(),
            "combined transport chunk is consumed"
        );
    }

    mdr::Vector<mdr::UInt8> ReadFixture(
        const std::filesystem::path& path
    )
    {
        std::ifstream input(path, std::ios::binary);
        return {
            std::istreambuf_iterator<char>(input),
            std::istreambuf_iterator<char>(),
        };
    }

    void TestCapturedFixtures(const std::filesystem::path& root)
    {
        std::error_code error;
        std::filesystem::recursive_directory_iterator iterator(root, error);
        const std::filesystem::recursive_directory_iterator end;
        std::vector<std::filesystem::path> fixtures;
        while (!error && iterator != end)
        {
            const auto& entry = *iterator;
            const std::string filename =
                entry.path().filename().string();
            if (
                entry.is_regular_file(error)
                && entry.path().extension() == ".bin"
                && filename.find("-rx.") != std::string::npos
            )
            {
                fixtures.push_back(entry.path());
            }
            iterator.increment(error);
        }
        Check(!error, "fixture directory traversal succeeds");
        std::ranges::sort(
            fixtures,
            {},
            [](const std::filesystem::path& path)
            {
                return path.filename().string();
            }
        );

        MockTransport transport;
        mdr::MDRHeadphones headphones(&transport.connection);
        size_t replayed = 0;
        for (const auto& path : fixtures)
        {
            const auto frame = ReadFixture(path);
            mdr::MDRBuffer payload;
            mdr::MDRDataType type{};
            mdr::MDRCommandSeqNumber sequence{};
            const bool unpacked =
                mdr::MDRUnpackCommand(
                    frame, payload, type, sequence
                ) == mdr::MDRUnpackResult::OK;
            const std::string fixtureName = path.string();
            Check(
                unpacked,
                "captured fixture unpacks: " + fixtureName
            );
            if (!unpacked)
                continue;

            const bool isAck = type == mdr::MDRDataType::ACK;
            const bool isData =
                type == mdr::MDRDataType::DATA_MDR
                || type == mdr::MDRDataType::DATA_MDR_NO2;
            if (!isAck && !isData)
                continue;

            const int event = Replay(
                frame,
                std::numeric_limits<size_t>::max(),
                headphones,
                transport
            );
            const bool dispatched = isAck
                ? event == MDR_HEADPHONES_EVT_UNHANDLED
                : event != MDR_HEADPHONES_ERROR
                    && event != MDR_HEADPHONES_EVT_UNHANDLED
                    && event > MDR_HEADPHONES_IDLE;
            Check(
                dispatched,
                "captured fixture dispatches: " + fixtureName
            );
            if (!dispatched)
            {
                const mdr::UInt8 command =
                    payload.empty() ? 0 : payload.front();
                std::cerr
                    << "  type: " << mdr::format_as(type)
                    << " (0x" << std::hex
                    << static_cast<unsigned>(type) << std::dec
                    << "), sequence: "
                    << static_cast<unsigned>(sequence)
                    << ", command: "
                    << PayloadCommandName(type, command)
                    << " (0x" << std::hex
                    << static_cast<unsigned>(command) << std::dec
                    << "), event: " << event
                    << ", payload bytes: " << payload.size()
                    << ", detail: " << headphones.GetLastError()
                    << '\n';
                std::cerr << "  payload:";
                for (const mdr::UInt8 byte : payload)
                {
                    std::cerr << ' ' << std::hex
                              << std::setw(2)
                              << std::setfill('0')
                              << static_cast<unsigned>(byte);
                }
                std::cerr << std::dec << '\n';
            }
            if (isData)
                ++replayed;
        }
        std::cout << "Replayed " << replayed
                  << " captured RX data fixture(s)\n";
    }
}

int main(int argc, char** argv)
{
    if (argc > 2)
    {
        std::cerr << "Usage: mdr_tests [fixture-directory]\n";
        return 2;
    }
    std::cerr << "Playback of captured RX data fixtures from " << (argc == 2 ? argv[1] : MDR_TEST_FIXTURE_DIR) << '\n';
    TestFraming();
    TestSerialization();
    TestDynamicDispatch();
    TestBufferedFrames();
    TestCapturedFixtures(
        argc == 2
            ? std::filesystem::path(argv[1])
            : std::filesystem::path(MDR_TEST_FIXTURE_DIR)
    );
    if (gFailures)
        std::cerr << gFailures << " test assertion(s) failed\n";
    return gFailures ? 1 : 0;
}
