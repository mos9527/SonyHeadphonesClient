#include "Headphones.hpp"
#include <fmt/base.h>
MDRHeadphones::Awaiter MDRHeadphones::Await(AwaitType type)
{
    return Awaiter{this, type};
}

int MDRHeadphones::Receive()
{
    char buf[kMDRMaxPacketSize];
    int recvd;
    const int r = mConn->recv(mConn->user, buf, kMDRMaxPacketSize, &recvd);
    if (r != MDR_RESULT_OK)
        return r;
    fmt::println("-- recv");
    for (char* p = buf; p != buf + recvd; p++)
        fmt::print("{:02X} ", static_cast<UInt8>(*p));
    fmt::println("\n--");
    mRecvBuf.insert(mRecvBuf.end(), buf, buf + recvd);
    return r;
}

int MDRHeadphones::Send()
{
    if (mSendBuf.empty())
        return MDR_RESULT_OK;
    char buf[kMDRMaxPacketSize];
    int toSend = std::min(mSendBuf.size(), kMDRMaxPacketSize), sent = 0;
    std::copy(mSendBuf.begin(), mSendBuf.begin() + toSend, buf);
    const int r = mConn->send(mConn->user, buf, toSend, &sent);
    if (r != MDR_RESULT_OK)
        return r;
    fmt::println("-- send");
    for (char* p = buf; p != buf + sent; p++)
        fmt::print("{:02X} ", static_cast<UInt8>(*p));
    fmt::println("\n--");
    mSendBuf.erase(mSendBuf.begin(), mSendBuf.begin() + sent);
    return r;
}


void MDRHeadphones::Handle(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq)
{
    using enum MDRDataType;
    mSeqNumber = seq;
    switch (type)
    {
    case ACK:
        return HandleAck(seq);
    case DATA_MDR:
        SendACK(seq);
        return HandleCommandV2T1(command, seq);
    case DATA_MDR_NO2:
        SendACK(seq);
        return HandleCommandV2T2(command, seq);
    default:
        break;
    }
}

void MDRHeadphones::MoveNext()
{
    ExceptionHandler([this] { TaskMoveNext(); });
    auto commandBegin = std::ranges::find(mRecvBuf, kStartMarker);
    auto commandEnd = std::ranges::find(commandBegin, mRecvBuf.end(), kEndMarker);
    if (commandBegin == mRecvBuf.end() || commandEnd == mRecvBuf.end())
        return; // Incomplete
    MDRBuffer packedCommand{commandBegin, commandEnd + 1};
    MDRBuffer command;
    MDRDataType type;
    MDRCommandSeqNumber seqNum;
    MDRUnpackResult res = MDRUnpackCommand(packedCommand, command, type, seqNum);
    switch (res)
    {
    case MDRUnpackResult::OK:
        mRecvBuf.erase(mRecvBuf.begin(), commandEnd);
        Handle(command, type, seqNum);
        break;
    case MDRUnpackResult::INCOMPLETE:
        // Incomplete. Nop.
        break;
    case MDRUnpackResult::BAD_MARKER: [[unlikely]]
    case MDRUnpackResult::BAD_CHECKSUM: [[unlikely]]
        // Unlikely. What we have now makes no sense yet markers are intact.
        mRecvBuf.erase(mRecvBuf.begin(), commandEnd);
        break;
    }
}

int MDRHeadphones::Invoke(MDRTask&& task)
{
    if (mTask)
        return MDR_RESULT_INPROGRESS;
    mTask = std::move(task);
    mTask.coroutine.resume();
    ExceptionHandler([this] { TaskMoveNext(); });
    return MDR_RESULT_OK;
}

bool MDRHeadphones::TaskMoveNext()
{
    if (!mTask)
        return true;
    if (!mTask.coroutine.done())
        return false;
    auto& [exec, next] = mTask.coroutine.promise();
    if (exec)
        std::rethrow_exception(exec);
    mTask = {};
    return true;
}

void MDRHeadphones::SendCommandImpl(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq)
{
    MDRBuffer packed = MDRPackCommand(type, seq, command);
    mSendBuf.insert(mSendBuf.end(), packed.begin(), packed.end());
}

void MDRHeadphones::SendACK(MDRCommandSeqNumber seq)
{
    SendCommandImpl({}, MDRDataType::ACK, 1 - seq);
}

void MDRHeadphones::HandleAck(MDRCommandSeqNumber)
{
    auto& await = mAwaiters[AWAIT_ACK];
    await.resume();
    await = {};
}

extern "C" {
const char* mdrResultString(int err)
{
    switch (err)
    {
        case MDR_RESULT_OK:
            return "OK";
        case MDR_RESULT_INPROGRESS:
            return "Task in progress";
        case MDR_RESULT_ERROR_GENERAL:
            return "General error";
        case MDR_RESULT_ERROR_NOT_FOUND:
            return "Resource not found";
        case MDR_RESULT_ERROR_TIMEOUT:
            return "Timed out";
        case MDR_RESULT_ERROR_NET:
            return "Networking error";
        case MDR_RESULT_ERROR_NO_CONNECTION:
            return "No connection has been established";
        case MDR_RESULT_ERROR_BAD_ADDRESS:
            return "Invalid address information";
        default:
            return "Unknown";
    }
}
MDRHeadphones* mdrHeadphonesCreate(MDRConnection* conn)
{
    return new MDRHeadphones(conn);
}

void mdrHeadphonesDestroy(MDRHeadphones* h)
{
    delete h;
}

int mdrHeadphonesPollEvents(MDRHeadphones* h)
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

int mdrHeadphonesRequestInit(MDRHeadphones* h)
{
    return h->Invoke(h->RequestInit());
}
const char* mdrHeadphonesGetLastError(MDRHeadphones* h)
{
    return h->GetLastError();
}
}

#pragma region Tasks
MDRTask MDRHeadphones::RequestInit()
{
    fmt::println("Init in");
    co_await SendCommandACK<v2::t1::ConnectGetProtocolInfo>();
    fmt::println("PROTO ACK!");
}
#pragma endregion