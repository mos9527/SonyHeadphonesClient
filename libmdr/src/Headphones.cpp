#include "Headphones.hpp"
#include <fmt/base.h>
MDRHeadphones::Awaiter MDRHeadphones::Await(AwaitType type)
{
    return Awaiter{this, type};
}

void MDRHeadphones::Awake(AwaitType type)
{
    auto await = mAwaiters[type];
    if (await)
    {
        mAwaiters[type] = nullptr;
        await.resume();
    }
}

int MDRHeadphones::Receive()
{
    char buf[kMDRMaxPacketSize];
    int recvd;
    const int r = mConn->recv(mConn->user, buf, kMDRMaxPacketSize, &recvd);
    if (r != MDR_RESULT_OK)
        return r;
    fmt::print("<< ");
    for (char* p = buf; p != buf + recvd; p++)
        fmt::print("{:02X} ", static_cast<UInt8>(*p));
    fmt::println("");
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
    fmt::print(">> ");
    for (char* p = buf; p != buf + sent; p++)
        fmt::print("{:02X} ", static_cast<UInt8>(*p));
    fmt::println("");
    mSendBuf.erase(mSendBuf.begin(), mSendBuf.begin() + sent);
    return r;
}


int MDRHeadphones::Handle(Span<const UInt8> command, MDRDataType type, MDRCommandSeqNumber seq)
{
    using enum MDRDataType;
    mSeqNumber = seq;
    switch (type)
    {
    case ACK:
        HandleAck(seq);
        break;
    case DATA_MDR:
        SendACK(seq);
        return HandleCommandV2T1(command, seq);
    case DATA_MDR_NO2:
        SendACK(seq);
        return HandleCommandV2T2(command, seq);
    default:
        break;
    }
    return MDR_HEADPHONES_NO_EVENT;
}

int MDRHeadphones::MoveNext()
{
    int taskResult;
    // Task done?
    if (ExceptionHandler([this, &taskResult] { return TaskMoveNext(taskResult); }))
        return taskResult;
    auto commandBegin = std::ranges::find(mRecvBuf, kStartMarker);
    auto commandEnd = std::ranges::find(commandBegin, mRecvBuf.end(), kEndMarker);
    if (commandBegin == mRecvBuf.end() || commandEnd == mRecvBuf.end())
        return MDR_HEADPHONES_NO_EVENT; // Incomplete
    MDRBuffer packedCommand{commandBegin, commandEnd + 1};
    MDRBuffer command;
    MDRDataType type;
    MDRCommandSeqNumber seqNum;
    MDRUnpackResult res = MDRUnpackCommand(packedCommand, command, type, seqNum);
    switch (res)
    {
    case MDRUnpackResult::OK:
        mRecvBuf.erase(mRecvBuf.begin(), commandEnd);
        return Handle(command, type, seqNum);
    case MDRUnpackResult::INCOMPLETE:
        // Incomplete. Nop.
        break;
    case MDRUnpackResult::BAD_MARKER: [[unlikely]]
    case MDRUnpackResult::BAD_CHECKSUM: [[unlikely]]
        // Unlikely. What we have now makes no sense yet markers are intact.
        mRecvBuf.erase(mRecvBuf.begin(), commandEnd);
        break;
    }
    return MDR_HEADPHONES_NO_EVENT; // No event
}

int MDRHeadphones::Invoke(MDRTask&& task)
{
    if (mTask)
        return MDR_RESULT_INPROGRESS;
    mTask = std::move(task);
    mTask.coroutine.resume();
    return MDR_RESULT_OK;
}

bool MDRHeadphones::TaskMoveNext(int& result)
{
    if (!mTask || !mTask.coroutine.done())
        return false;
    auto& [exec, next, value] = mTask.coroutine.promise();
    result = value;
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
    Awake(AWAIT_ACK);
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
const char* mdrHeadphonesEventString(int evt)
{
    switch (evt)
    {
        case MDR_HEADPHONES_NO_EVENT:
            return "No Event";
        case MDR_HEADPHONES_INITIALIZED:
            return "Initialized";
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
        return MDR_HEADPHONES_ERROR;
    r = h->Receive();
    if (r != MDR_RESULT_OK && r != MDR_RESULT_INPROGRESS)
        return MDR_HEADPHONES_ERROR;
    return h->MoveNext();
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

// Sigh.
// NOLINTBEGIN
#pragma region Tasks
MDRTask MDRHeadphones::RequestInit()
{
    SendCommandACK(v2::t1::ConnectGetProtocolInfo);
    co_await Await(AWAIT_PROTOCOL_INFO);
    MDR_CHECK(mProto.hasTable1, "Device doesn't support MDR V2 Table 1");
    SendCommandACK(v2::t1::ConnectGetCapabilityInfo);

    /* Device Info */
    SendCommandACK(v2::t1::ConnectGetDeviceInfo, {.deviceInfoType = v2::t1::DeviceInfoType::FW_VERSION});
    SendCommandACK(v2::t1::ConnectGetDeviceInfo, {.deviceInfoType = v2::t1::DeviceInfoType::MODEL_NAME});
    SendCommandACK(v2::t1::ConnectGetDeviceInfo, {.deviceInfoType = v2::t1::DeviceInfoType::SERIES_AND_COLOR_INFO});

    // Following are cached by the offical app based on the MAC address
    {
        /* Support Functions */
        SendCommandACK(v2::t1::ConnectGetSupportFunction);
        co_await Await(AWAIT_SUPPORT_FUNCTION);
        if (mProto.hasTable2)
        {
            SendCommandACK(v2::t2::ConnectGetSupportFunction);
            co_await Await(AWAIT_SUPPORT_FUNCTION);
        }
    }
    mInitialized = true;
    fmt::println("=== Table 1 Functions");
    for (UInt8 i = 0; i < 255; i++)
    {
        auto sv = static_cast<v2::MessageMdrV2FunctionType_Table1>(i);
        if (mSupportFunctions.table1Functions[i] && is_valid(sv))
            fmt::println("\t{}", sv);
    }
    fmt::println("=== Table 2 Functions");
    for (UInt8 i = 0; i < 255; i++)
    {
        auto sv = static_cast<v2::MessageMdrV2FunctionType_Table2>(i);
        if (mSupportFunctions.table2Functions[i] && is_valid(sv))
            fmt::println("\t{}", sv);
    }

    co_return MDR_HEADPHONES_INITIALIZED;
}
#pragma endregion
// NOLINTEND