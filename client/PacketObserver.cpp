#include "PacketObserver.hpp"

#include "PayloadRecorder.hpp"
#ifdef MDR_CLIENT_DEBUGGER
#include "Debugger.hpp"
#endif

namespace
{
    MDRHeadphones* gObservedHeadphones{};

    void ObservePacket(void*, MDRPacketDirection direction, const unsigned char* frame, int frameSize)
    {
        clientPayloadRecorderObserve(direction, frame, frameSize);
#ifdef MDR_CLIENT_DEBUGGER
        clientDebuggerObservePacket(direction, frame, frameSize);
#endif
    }
} // namespace

void clientPacketObserverAttach(MDRHeadphones* headphones)
{
    clientPacketObserverDetach();
    gObservedHeadphones = headphones;
    if (gObservedHeadphones)
        mdrHeadphonesSetPacketCallback(gObservedHeadphones, ObservePacket, nullptr);
#ifdef MDR_CLIENT_DEBUGGER
    clientDebuggerAttach(headphones);
#endif
}

void clientPacketObserverDetach()
{
    if (gObservedHeadphones)
        mdrHeadphonesSetPacketCallback(gObservedHeadphones, nullptr, nullptr);
#ifdef MDR_CLIENT_DEBUGGER
    clientDebuggerDetach();
#endif
    gObservedHeadphones = nullptr;
}
