// ReSharper disable CppParameterMayBeConstPtrOrRef
#include <ranges>
#include <fmt/base.h>
#include <algorithm>
#include <mdr/Headphones.hpp>

namespace mdr
{
    MDRHeadphones::Awaiter& MDRHeadphones::Await(AwaitType type)
    {
        return mAwaiters[type];
    }

    void MDRHeadphones::Awake(AwaitType type)
    {
        if (auto& await = mAwaiters[type])
            await.resume_now(MDR_RESULT_OK);
    }

    int MDRHeadphones::PollEvents()
    {
        int r = mdrConnectionPoll(mConn, 0);
        if (r == MDR_RESULT_OK)
        {
            // Non-blocking. INPROGRESS are expected, not so much for others.
            // Failfast if that happens - the owner usually has to die.
            r = Send();
            if (r != MDR_RESULT_OK && r != MDR_RESULT_INPROGRESS)
                return MDR_HEADPHONES_ERROR;
            r = Receive();
            if (r != MDR_RESULT_OK && r != MDR_RESULT_INPROGRESS)
                return MDR_HEADPHONES_ERROR;
        }
        else
        {
            if (r != MDR_RESULT_ERROR_TIMEOUT)
                return MDR_HEADPHONES_ERROR;
        }
        return MoveNext();
    }

    bool MDRHeadphones::IsReady() const
    {
        return !mTask;
    }

    bool MDRHeadphones::IsDirty() const
    {
        bool dirty = mShutdown.dirty() || mNcAsmEnabled.dirty() || mNcAsmFocusOnVoice.dirty() || mNcAsmAmbientLevel.
            dirty();
        dirty |= mNcAsmButtonFunction.dirty() || mNcAsmMode.dirty() || mNcAsmAutoAsmEnabled.dirty();
        dirty |= mNcAsmNoiseAdaptiveSensitivity.dirty() || mPowerAutoOff.dirty() || mPlayControl.dirty();
        dirty |= mPowerAutoOffWearingDetection.dirty() || mPlayVolume.dirty() || mGsParamBool1.dirty();
        dirty |= mGsParamBool2.dirty() || mGsParamBool3.dirty() || mGsParamBool4.dirty();
        dirty |= mUpscalingEnabled.dirty();
        dirty |= mAudioPriorityMode.dirty() || mBGMModeEnabled.dirty() || mBGMModeRoomSize.dirty();
        dirty |= mUpmixCinemaEnabled.dirty() || mAutoPauseEnabled.dirty() || mTouchFunctionLeft.dirty();
        dirty |= mTouchFunctionRight.dirty() || mSpeakToChatEnabled.dirty() || mSpeakToChatDetectSensitivity.dirty();
        dirty |= mSpeakToModeOutTime.dirty() || mHeadGestureEnabled.dirty() || mEqAvailable.dirty();
        dirty |= mEqPresetId.dirty() || mEqClearBass.dirty() || mEqConfig.dirty();
        dirty |= mVoiceGuidanceEnabled.dirty() || mVoiceGuidanceVolume.dirty() || mPairingMode.dirty();
        dirty |= mMultipointDeviceMac.dirty() || mSafeListeningPreviewMode.dirty();
        dirty |= mPairedDeviceConnectMac.dirty() || mPairedDeviceDisconnectMac.dirty() || mPairedDeviceUnpairMac.
            dirty();
        return dirty;
    }

    int MDRHeadphones::Receive()
    {
        char buf[kMDRMaxPacketSize];
        int recvd;
        const int r = mdrConnectionRecv(mConn, buf, kMDRMaxPacketSize, &recvd);
        if (r != MDR_RESULT_OK)
            return r;
#ifdef MDR_DEBUG
        fprintf(MDR_LOG_STREAM, "<< ");
        for (char* p = buf; p != buf + recvd; p++)
            fprintf(MDR_LOG_STREAM, "%X ", static_cast<UInt8>(*p));
        fprintf(MDR_LOG_STREAM, "\n");
#endif
        mRecvBuf.insert(mRecvBuf.end(), buf, buf + recvd);
        return r;
    }

    int MDRHeadphones::Send()
    {
        if (mSendBuf.empty())
            return MDR_RESULT_OK;
        char buf[kMDRMaxPacketSize];
        int toSend = std::min(mSendBuf.size(), kMDRMaxPacketSize), sent = 0;
        std::copy_n(mSendBuf.begin(), toSend, buf);
        const int r = mdrConnectionSend(mConn, buf, toSend, &sent);
        if (r != MDR_RESULT_OK)
            return r;
#ifdef MDR_DEBUG
        fprintf(MDR_LOG_STREAM, "<< ");
        for (char* p = buf; p != buf + sent; p++)
            fprintf(MDR_LOG_STREAM, "%X ", static_cast<UInt8>(*p));
        fprintf(MDR_LOG_STREAM, "\n");
#endif
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
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int MDRHeadphones::MoveNext()
    {
        // Awaiter timeouts
        {
            using namespace std::literals;
            constexpr auto kTimeout = std::chrono::milliseconds(kAwaitTimeoutMS);
            auto now = std::chrono::steady_clock::now();
            for (auto& awaiter : mAwaiters)
            {
                if (!awaiter) continue;
                auto duration = now - awaiter.tick;
                if (duration > kTimeout)
                    awaiter.resume_now(MDR_RESULT_ERROR_TIMEOUT);
            }
            int taskResult;
            try
            {
                if (TaskMoveNext(taskResult))
                    return taskResult;
            } catch (std::runtime_error& e)
            {
                mLastError = e.what();
                return MDR_HEADPHONES_ERROR;
            }
        }
        int idleCode = mTask ? MDR_HEADPHONES_INPROGRESS : MDR_HEADPHONES_IDLE;
        if (mRecvBuf.empty())
            return idleCode;
        auto commandBegin = std::ranges::find(mRecvBuf, kStartMarker);
        auto commandEnd = std::ranges::find(commandBegin, mRecvBuf.end(), kEndMarker);
        if (commandBegin == mRecvBuf.end() || commandEnd == mRecvBuf.end())
            return idleCode; // Incomplete
        MDRBuffer packedCommand{commandBegin, commandEnd + 1};
        MDRBuffer command;
        MDRDataType type;
        MDRCommandSeqNumber seqNum;
        switch (MDRUnpackCommand(packedCommand, command, type, seqNum))
        {
        case MDRUnpackResult::OK:
            mRecvBuf.erase(mRecvBuf.begin(), commandEnd);
            return Handle(command, type, seqNum);
        case MDRUnpackResult::INCOMPLETE:
            // Incomplete. Nop.
            break;
        case MDRUnpackResult::BAD_MARKER: [[unlikely]]
        case MDRUnpackResult::BAD_CHECKSUM:
            [[unlikely]]
                // Unlikely. What we have now makes no sense yet markers are intact.
                mRecvBuf.erase(mRecvBuf.begin(), commandEnd);
            break;
        }
        return idleCode;
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
}

#pragma region C Exports
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

int mdrConnectionConnect(MDRConnection* conn, const char* macAddress, const char* serviceUUID)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->connect(conn->user, macAddress, serviceUUID);
}

void mdrConnectionDisconnect(MDRConnection* conn)
{
    if (!conn)
        return;
    return conn->disconnect(conn->user);
}

int mdrConnectionRecv(MDRConnection* conn, char* dst, int size, int* pReceived)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->recv(conn->user, dst, size, pReceived);
}

int mdrConnectionSend(MDRConnection* conn, const char* src, int size, int* pSent)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->send(conn->user, src, size, pSent);
}

int mdrConnectionPoll(MDRConnection* conn, int timeout)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->poll(conn->user, timeout);
}

int mdrConnectionGetDevicesList(MDRConnection* conn, MDRDeviceInfo** ppList, int* pCount)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->getDevicesList(conn->user, ppList, pCount);
}

int mdrConnectionFreeDevicesList(MDRConnection* conn, MDRDeviceInfo** ppList)
{
    if (!conn)
        return MDR_RESULT_ERROR_NO_CONNECTION;
    return conn->freeDevicesList(conn->user, ppList);
}

const char* mdrConnectionGetLastError(MDRConnection* conn)
{
    if (!conn)
        return "FIXME: nullptr conn";
    return conn->getLastError(conn->user);
}

MDRHeadphones* mdrHeadphonesCreate(MDRConnection* conn)
{
    return reinterpret_cast<MDRHeadphones*>(new mdr::MDRHeadphones(conn));
}

void mdrHeadphonesDestroy(MDRHeadphones* h)
{
    delete reinterpret_cast<mdr::MDRHeadphones*>(h);
}

int mdrHeadphonesPollEvents(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    return h->PollEvents();
}

int mdrHeadphonesRequestIsReady(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    if (!h->IsReady())
        return MDR_RESULT_INPROGRESS;
    return MDR_RESULT_OK;
}

int mdrHeadphonesRequestInitV2(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    return h->Invoke(h->RequestInitV2());
}

int mdrHeadphonesRequestSyncV2(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    return h->Invoke(h->RequestSyncV2());
}

int mdrHeadphonesRequestCommitV2(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    return h->Invoke(h->RequestCommitV2());
}

int mdrHeadphonesIsDirty(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    if (h->IsDirty())
    {
        return MDR_RESULT_INPROGRESS;
    }
    return MDR_RESULT_OK;
}

const char* mdrHeadphonesGetLastError(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    return h->GetLastError();
}
}
#pragma endregion
