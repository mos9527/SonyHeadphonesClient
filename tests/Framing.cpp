// Receive-path tests: what the library does with frames that arrive early, fragmented
// or malformed. All frames here are synthetic - no capture data, no device required.
#include "MockTransport.hpp"

#include <mdr-c/Headphones.h>

#include <cstdint>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace
{
    using mdrtest::MockTransport;

    int gFailures = 0;

    void Check(bool condition, std::string_view message)
    {
        if (condition)
            return;
        std::cerr << "FAIL: " << message << '\n';
        ++gFailures;
    }

    // CONNECT_RET_PROTOCOL_INFO as a WF-1000XM5 answers it: MDR V2, both tables enabled.
    constexpr uint8_t kProtocolInfo[]{0x3E, 0x0C, 0x01, 0x00, 0x00, 0x00, 0x08, 0x01, 0x00,
                                      0x03, 0x00, 0x30, 0x18, 0x00, 0x00, 0x61, 0x3C};
    // Table 1 payload carrying an arbitrary non-handshake command (NCASM_RET_PARAM).
    constexpr uint8_t kTable1Frame[]{0x3E, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x01, 0x67, 0x74, 0x3C};
    // Table 2 payload carrying PERI_RET_PARAM - the notify a device pushes when its
    // paired-device state changed while nobody was listening.
    constexpr uint8_t kTable2Frame[]{0x3E, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x01, 0x37, 0x46, 0x3C};
    // Both markers, nothing in between: shortest input that clears MDRUnpackCommand's
    // size check while carrying no header at all.
    constexpr uint8_t kEmptyFrame[]{0x3E, 0x3C};
    // An escape sentry with no escapee behind it - unescaping cannot produce a header.
    constexpr uint8_t kDanglingEscape[]{0x3E, 0x3D, 0x3C};

    struct Session
    {
        MockTransport transport;
        MDRHeadphones* headphones{};

        Session() { mdrHeadphonesCreate(MDR_ABI_VERSION, &transport.connection, &headphones); }

        ~Session()
        {
            if (headphones)
                mdrHeadphonesDestroy(headphones);
        }

        Session(const Session&) = delete;
        Session& operator=(const Session&) = delete;

        // Feeds a stream and polls until it is drained. Returns the first failing poll
        // result, or MDR_RESULT_OK if the session survived the whole stream.
        MDRResult Pump(std::span<const uint8_t> bytes, MDREvent& outEvent, size_t chunk = 0)
        {
            outEvent = MDR_EVENT_NONE;
            transport.Load(bytes);
            transport.chunk = chunk;
            for (size_t attempt = 0; attempt < bytes.size() + 8; ++attempt)
            {
                MDREvent event = MDR_EVENT_NONE;
                const MDRResult result = mdrHeadphonesPoll(headphones, &event);
                if (result != MDR_RESULT_OK)
                    return result;
                if (event != MDR_EVENT_NONE)
                    outEvent = event;
            }
            return MDR_RESULT_OK;
        }
    };

    // A device whose paired-device state changed while it was unattended pushes the
    // notify as soon as we open the socket - possibly before our protocol handshake
    // completes. That is a race, not a broken device, so the session has to survive it.
    void TestFrameBeforeProtocolInfo(std::span<const uint8_t> frame, std::string_view what)
    {
        Session session;
        Check(session.headphones != nullptr, "opaque headphones session opens");
        if (!session.headphones)
            return;

        MDREvent event = MDR_EVENT_NONE;
        const MDRResult result = session.Pump(frame, event);
        Check(result == MDR_RESULT_OK,
              std::string(what) + " before protocol info keeps the session alive, last error: " +
                  mdrtest::GetLastError(session.headphones));
        Check(!session.transport.tx.empty(), std::string(what) + " before protocol info is still ACKed");

        // The handshake still has to land afterwards, otherwise we only survived by
        // wedging the receive buffer.
        const MDRResult handshake = session.Pump(kProtocolInfo, event);
        Check(handshake == MDR_RESULT_OK, "protocol info is accepted after " + std::string(what));
        Check(event == MDR_EVENT_IDENTITY_CHANGED, "protocol info dispatches after " + std::string(what));
    }

    // Nothing a device can put on the wire may push the parser past the frame it is
    // looking at, however little of a header survived the markers.
    void TestDegenerateFrame(std::span<const uint8_t> frame, std::string_view what)
    {
        Session session;
        if (!session.headphones)
            return;

        MDREvent event = MDR_EVENT_NONE;
        const MDRResult result = session.Pump(frame, event);
        Check(result == MDR_RESULT_OK,
              std::string(what) + " is discarded without failing the session, last error: " +
                  mdrtest::GetLastError(session.headphones));

        const MDRResult handshake = session.Pump(kProtocolInfo, event);
        Check(handshake == MDR_RESULT_OK, "protocol info is accepted after " + std::string(what));
        Check(event == MDR_EVENT_IDENTITY_CHANGED, "protocol info dispatches after " + std::string(what));
    }

    // RFCOMM hands over whatever arrived, so a frame can be split at any offset.
    void TestFragmentedFrame(size_t chunk)
    {
        Session session;
        if (!session.headphones)
            return;

        MDREvent event = MDR_EVENT_NONE;
        const MDRResult result = session.Pump(kProtocolInfo, event, chunk);
        const std::string suffix = " (" + std::to_string(chunk) + " byte reads)";
        Check(result == MDR_RESULT_OK, "fragmented protocol info keeps the session alive" + suffix);
        Check(event == MDR_EVENT_IDENTITY_CHANGED, "fragmented protocol info dispatches" + suffix);
    }

    // Leftovers from an aborted frame precede the next good one after a mid-stream
    // attach; the parser has to resynchronise on the next start marker.
    void TestGarbagePrefix()
    {
        Session session;
        if (!session.headphones)
            return;

        std::vector<uint8_t> stream{0x11, 0x22, 0x33, 0x3C};
        stream.insert(stream.end(), std::begin(kProtocolInfo), std::end(kProtocolInfo));

        MDREvent event = MDR_EVENT_NONE;
        const MDRResult result = session.Pump(stream, event);
        Check(result == MDR_RESULT_OK, "garbage before a frame keeps the session alive");
        Check(event == MDR_EVENT_IDENTITY_CHANGED, "frame after garbage dispatches");
    }
} // namespace

int main()
{
    TestFrameBeforeProtocolInfo(kTable1Frame, "table 1 data");
    TestFrameBeforeProtocolInfo(kTable2Frame, "table 2 data");
    TestDegenerateFrame(kEmptyFrame, "a frame with no header");
    TestDegenerateFrame(kDanglingEscape, "a frame ending in an escape sentry");
    TestFragmentedFrame(1);
    TestFragmentedFrame(5);
    TestGarbagePrefix();

    if (gFailures)
        std::cerr << gFailures << " test assertion(s) failed\n";
    else
        std::cout << "Framing tests passed\n";
    return gFailures ? 1 : 0;
}
