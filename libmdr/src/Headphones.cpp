#include "Command.hpp"
#include "ProtocolV2T1.hpp"
#include "ProtocolV2T2.hpp"
#include "mdr/Headphones.h"

#include <deque>
using namespace mdr;

struct MDRHeadphone
{
    MDRConnection* conn;
    std::deque<UInt8> recvBuf, sendBuf;

    MDRHeadphone(MDRConnection* conn) :
        conn(conn)
    {
    }

    int Receive();
    int Send();

    /**
     * @note Queues a command payload to be sent through @ref Send
     * @note Non-blocking. Need @ref Sent to be polled periodically.
     */
    void SendCommand(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq);

    /**
     * @brief Dequeues a _complete_ command payload and spawns appropriate coroutines - and advances them _here_.
     * @note  This is a no-op if buffer is incomplete and no complete command payload can be produced.
     * @note  Non-blocking. Need @ref Receive, @ref Sent to be polled periodically.
     * @note  This is your best friend.
     */
    void MoveNext();
};

int MDRHeadphone::Receive()
{
    char buf[kMDRMaxPacketSize];
    int recvd;
    const int r = conn->recv(conn->user, buf, kMDRMaxPacketSize, &recvd);
    if (r != MDR_RESULT_OK)
        return r;
    recvBuf.insert(recvBuf.end(), buf, buf + recvd);
    return r;
}

int MDRHeadphone::Send()
{
    if (sendBuf.empty())
        return MDR_RESULT_OK;
    char buf[kMDRMaxPacketSize];
    int toSend = std::min(sendBuf.size(), kMDRMaxPacketSize), sent = 0;
    std::copy(sendBuf.begin(), sendBuf.begin() + toSend, buf);
    const int r = conn->send(conn->user, buf, toSend, &sent);
    if (r != MDR_RESULT_OK)
        return r;
    sendBuf.erase(sendBuf.begin(), sendBuf.begin() + sent);
    return r;
}

void MDRHeadphone::MoveNext()
{
    auto commandBegin = std::ranges::find(recvBuf, kStartMarker);
    auto commandEnd = std::ranges::find(commandBegin, recvBuf.end(), kEndMarker);
    if (commandBegin == recvBuf.end() || commandEnd == recvBuf.end())
        return; // Incomplete
    MDRBuffer packedCommand{commandBegin, commandEnd};
    MDRBuffer command;
    MDRDataType type;
    MDRCommandSeqNumber seqNum;
    MDRUnpackResult res = Unpack(packedCommand, command, type, seqNum);
    switch (res)
    {
    case MDRUnpackResult::OK:
        recvBuf.erase(recvBuf.begin(), commandEnd);
        // --> HandleCommand(command, type, seqNum);
        break;
    case MDRUnpackResult::INCOMPLETE:
        // Incomplete. Nop.
        break;
    case MDRUnpackResult::BAD_MARKER:
    case MDRUnpackResult::BAD_CHECKSUM:
        // Unlikely. What we have now makes no sense yet markers are intact.
        recvBuf.erase(recvBuf.begin(), commandEnd);
        break;
    }
}

void MDRHeadphone::SendCommand(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq)
{
    MDRBuffer packed = Pack(type, seq, command);
    sendBuf.insert(sendBuf.end(), packed.begin(), packed.end());
}

extern "C" {
MDRHeadphone* mdrHeadphonesCreate(MDRConnection* conn)
{
    return new MDRHeadphone(conn);
}

void mdrHeadphonesDestroy(MDRHeadphone* h)
{
    delete h;
}

int mdrHeadphonesConnect(MDRHeadphone* h, const char* macAddress, const uint8_t* uuid)
{
    return h->conn->connect(h->conn->user, macAddress, uuid);
}

int mdrHeadphonesPollEvents(MDRHeadphone* h)
{
    // Non-blocking. INPROGRESS are expected, not so much for others.
    // Failfast if that happens - the owner usually has to die.
    int r = h->Send();
    if (r != MDR_RESULT_OK && r != MDR_RESULT_INPROGRESS)
        return r;
    r = h->Receive();
    if (r != MDR_RESULT_OK && r != MDR_RESULT_INPROGRESS)
        return r;
    h->MoveNext();
    return MDR_RESULT_OK;
}
}
