#include <ranges>
#include <fmt/base.h>
#include <algorithm>
#include <mdr/Headphones.hpp>

namespace mdr
{
    MDRHeadphones::Awaiter MDRHeadphones::Await(AwaitType type)
    {
        return Awaiter{this, type};
    }

    void MDRHeadphones::Awake(AwaitType type)
    {
        if (auto await = mAwaiters[type])
        {
            mAwaiters[type] = nullptr;
            await.resume();
        }
    }

    int MDRHeadphones::PollEvents()
    {
        if (mdrConnectionPoll(mConn, 0) == MDR_RESULT_OK)
        {
            // Non-blocking. INPROGRESS are expected, not so much for others.
            // Failfast if that happens - the owner usually has to die.
            int r = Send();
            if (r != MDR_RESULT_OK && r != MDR_RESULT_INPROGRESS)
                return MDR_HEADPHONES_ERROR;
            r = Receive();
            if (r != MDR_RESULT_OK && r != MDR_RESULT_INPROGRESS)
                return MDR_HEADPHONES_ERROR;
        }
        return MoveNext();
    }

    bool MDRHeadphones::IsReady() const
    {
        return !mTask;
    }

    int MDRHeadphones::Receive()
    {
        char buf[kMDRMaxPacketSize];
        int recvd;
        const int r = mdrConnectionRecv(mConn, buf, kMDRMaxPacketSize, &recvd);
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
        std::copy_n(mSendBuf.begin(), toSend, buf);
        const int r = mdrConnectionSend(mConn, buf, toSend, &sent);
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
        return MDR_HEADPHONES_EVT_UNHANDLED;
    }

    int MDRHeadphones::MoveNext()
    {
        int taskResult;
        // Task done?
        if (ExceptionHandler([this, &taskResult] { return TaskMoveNext(taskResult); }))
            return taskResult;
        if (mRecvBuf.empty())
            return MDR_HEADPHONES_INPROGRESS;
        auto commandBegin = std::ranges::find(mRecvBuf, kStartMarker);
        auto commandEnd = std::ranges::find(commandBegin, mRecvBuf.end(), kEndMarker);
        if (commandBegin == mRecvBuf.end() || commandEnd == mRecvBuf.end())
            return MDR_HEADPHONES_INPROGRESS; // Incomplete
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
        return MDR_HEADPHONES_INPROGRESS; // No event
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

    // Sigh.
    // NOLINTBEGIN
#pragma region Tasks
    // XXX: Maybe RequestInitV2? Maybe someone with a pair of XM4s could help with V1 :D
    MDRTask MDRHeadphones::RequestInit()
    {
        SendCommandACK(v2::t1::ConnectGetProtocolInfo);
        co_await Await(AWAIT_PROTOCOL_INFO);
        MDR_CHECK(mProtocol.hasTable1, "Device doesn't support MDR V2 Table 1");
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
            if (mProtocol.hasTable2)
            {
                SendCommandACK(v2::t2::ConnectGetSupportFunction);
                co_await Await(AWAIT_SUPPORT_FUNCTION);
            }

            /* General Setting */
            v2::t1::DisplayLanguage lang = v2::t1::DisplayLanguage::ENGLISH;
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1))
            {
                SendCommandACK(v2::t1::GsGetCapability, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING1, .displayLanguage = lang
                               });
                SendCommandACK(v2::t1::GsGetParam, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING1
                               });
            }
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::GENERAL_SETTING_2))
            {
                SendCommandACK(v2::t1::GsGetCapability, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING2, .displayLanguage = lang
                               });
                SendCommandACK(v2::t1::GsGetParam, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING2
                               });
            }
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3))
            {
                SendCommandACK(v2::t1::GsGetCapability, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING3, .displayLanguage = lang
                               });
                SendCommandACK(v2::t1::GsGetParam, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING3
                               });
            }
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::GENERAL_SETTING_4))
            {
                SendCommandACK(v2::t1::GsGetCapability, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING4, .displayLanguage = lang
                               });
                SendCommandACK(v2::t1::GsGetParam, {
                               .type = v2::t1::GsInquiredType::GENERAL_SETTING4
                               });
            }

            /* DSEE */
            if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
                SendCommandACK(v2::t1::AudioGetCapability, {
                           .type = v2::t1::AudioInquiredType::UPSCALING
                           });
        }
        /* [NO ACK] Receive alerts for certain operations like toggling multipoint */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::FIXED_MESSAGE))
            SendCommandImpl<v2::t1::AlertSetStatusFixedMessage>({.status = v2::MessageMdrV2EnableDisable::ENABLE});

        /* Codec Type */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::CODEC_INDICATOR))
            SendCommandACK(v2::t1::CommonGetStatus, { .type = v2::t1::CommonInquiredType::AUDIO_CODEC });

        /* Playback Metadata */
        SendCommandACK(v2::t1::GetPlayParam,
                       { .type = v2::t1::PlayInquiredType::PLAYBACK_CONTROL_WITH_CALL_VOLUME_ADJUSTMENT });

        /* Playback Volume */
        SendCommandACK(v2::t1::GetPlayParam, { .type = v2::t1::PlayInquiredType::MUSIC_VOLUME });

        /* Play/Pause */
        if (mSupport.contains(
            v2::MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            SendCommandACK(v2::t1::NcAsmGetParam,
                           { .type = v2::t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS});
        }
        else if (mSupport.contains(
            v2::MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION))
        {
            SendCommandACK(v2::t1::NcAsmGetParam,
                           { .type = v2::t1::NcAsmInquiredType::MODE_NC_ASM_DUAL_NC_MODE_SWITCH_AND_ASM_SEAMLESS_NA});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT))
        {
            SendCommandACK(v2::t1::NcAsmGetParam, { .type = v2::t1::NcAsmInquiredType::ASM_SEAMLESS});
        }

        /* Pairing Management */
        constexpr v2::MessageMdrV2FunctionType_Table2 kPairingFunctions[] = {
            v2::MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT,
            v2::MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_BT,
            v2::MessageMdrV2FunctionType_Table2::PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE_CLASSIC_LE
        };
        if (std::ranges::any_of(kPairingFunctions, [&](auto x) { return mSupport.contains(x); }))
        {
            /* Pairing Mode */
            SendCommandACK(v2::t2::PeripheralGetStatus,
                           {.type = v2::t2::PeripheralInquiredType::
                           PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE
                           });

            /* Connected Devices */
            SendCommandACK(v2::t2::PeripheralGetParam,
                           {.type = v2::t2::PeripheralInquiredType::
                           PAIRING_DEVICE_MANAGEMENT_WITH_BLUETOOTH_CLASS_OF_DEVICE
                           });
        }

        /* Speak To Chat */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2))
        {
            SendCommandACK(v2::t1::SystemGetParam, {.type = v2::t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2});
            SendCommandACK(v2::t1::SystemGetExtParam, {.type = v2::t1::SystemInquiredType::SMART_TALKING_MODE_TYPE2});
        }

        /* Listening Mode */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::LISTENING_OPTION))
        {
            SendCommandACK(v2::t1::AudioGetParam, {.type = v2::t1::AudioInquiredType::BGM_MODE});
            SendCommandACK(v2::t1::AudioGetParam, {.type = v2::t1::AudioInquiredType::UPMIX_CINEMA});
        }

        /* Equalizer */
        SendCommandACK(v2::t1::EqEbbGetStatus, {.type = v2::t1::EqEbbInquiredType::PRESET_EQ});

        /* Connection Quality */
        if (mSupport.contains(
            v2::MessageMdrV2FunctionType_Table1::CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY))
            SendCommandACK(v2::t1::AudioGetParam, {.type = v2::t1::AudioInquiredType::CONNECTION_MODE});

        /* DSEE */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::UPSCALING_AUTO_OFF))
        {
            SendCommandACK(v2::t1::AudioGetStatus, {.type = v2::t1::AudioInquiredType::UPSCALING});
            SendCommandACK(v2::t1::AudioGetParam, {.type = v2::t1::AudioInquiredType::UPSCALING});
        }

        /* Touch Sensor */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::ASSIGNABLE_SETTING))
            SendCommandACK(v2::t1::SystemGetParam, {.type = v2::t1::SystemInquiredType::ASSIGNABLE_SETTINGS });

        /* NC/AMB Toggle */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT))
            SendCommandACK(v2::t1::NcAsmGetParam, {.type = v2::t1::NcAsmInquiredType::NC_AMB_TOGGLE });

        /* Head Gesture */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::HEAD_GESTURE_ON_OFF_TRAINING))
            SendCommandACK(v2::t1::SystemGetParam, {.type = v2::t1::SystemInquiredType::HEAD_GESTURE_ON_OFF });

        /* Auto Power Off */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF))
        {
            SendCommandACK(v2::t1::PowerGetParam, {.type = v2::t1::PowerInquiredType::AUTO_POWER_OFF});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION))
        {
            SendCommandACK(v2::t1::PowerGetParam,
                           {.type = v2::t1::PowerInquiredType::AUTO_POWER_OFF_WEARING_DETECTION});
        }

        /* Pause when headphones are removed */
        SendCommandACK(v2::t1::SystemGetParam, {.type = v2::t1::SystemInquiredType::PLAYBACK_CONTROL_BY_WEARING });

        /* Voice Guidance */
        if (mProtocol.hasTable2)
        {
            // Enabled
            SendCommandACK(v2::t2::VoiceGuidanceGetParam,
                           {
                           .type = v2::t2::VoiceGuidanceInquiredType::
                           MTK_TRANSFER_WO_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH
                           });
            // Volume
            SendCommandACK(v2::t2::VoiceGuidanceGetParam, {.type = v2::t2::VoiceGuidanceInquiredType::VOLUME});
        }

        /* LOG_SET_STATUS */
        // XXX: Figure out if there's a struct for this in the app
        constexpr UInt8 kLogSetStatusCommand[] = {
            static_cast<UInt8>(v2::t1::Command::LOG_SET_STATUS),
            0x01, 0x00
        };
        SendCommandImpl(kLogSetStatusCommand, MDRDataType::DATA_MDR, mSeqNumber);
        co_await Await(AWAIT_ACK);
        co_return MDR_HEADPHONES_TASK_INIT_OK;
    }

    MDRTask MDRHeadphones::RequestSync()
    {
        /* Single Battery */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(v2::t1::PowerGetStatus, {.type = v2::t1::PowerInquiredType::BATTERY});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(v2::t1::PowerGetStatus, {.type = v2::t1::PowerInquiredType::BATTERY_WITH_THRESHOLD});
        }

        /* L + R Battery */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(v2::t1::PowerGetStatus, {.type = v2::t1::PowerInquiredType::LEFT_RIGHT_BATTERY});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::LR_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(v2::t1::PowerGetStatus, {.type = v2::t1::PowerInquiredType::LR_BATTERY_WITH_THRESHOLD});
        }

        /* Case Battery */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_INDICATOR))
        {
            SendCommandACK(v2::t1::PowerGetStatus, {.type = v2::t1::PowerInquiredType::CRADLE_BATTERY});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD))
        {
            SendCommandACK(v2::t1::PowerGetStatus, {.type = v2::t1::PowerInquiredType::CRADLE_BATTERY_WITH_THRESHOLD});
        }

        /* Sound Pressure */
        if (mSupport.contains(v2::MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_1))
        {
            SendCommandACK(v2::t2::SafeListeningGetExtendedParam,
                           {.type = v2::t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_1});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table2::SAFE_LISTENING_HBS_2))
        {
            SendCommandACK(v2::t2::SafeListeningGetExtendedParam,
                           {.type = v2::t2::SafeListeningInquiredType::SAFE_LISTENING_HBS_2});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_1))
        {
            SendCommandACK(v2::t2::SafeListeningGetExtendedParam,
                           {.type = v2::t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_1});
        }
        else if (mSupport.contains(v2::MessageMdrV2FunctionType_Table2::SAFE_LISTENING_TWS_2))
        {
            SendCommandACK(v2::t2::SafeListeningGetExtendedParam,
                           {.type = v2::t2::SafeListeningInquiredType::SAFE_LISTENING_TWS_2});
        }
        co_return MDR_HEADPHONES_TASK_SYNC_OK;
    }
#pragma endregion
    // NOLINTEND
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
    return conn->connect(conn->user, macAddress, serviceUUID);
}

void mdrConnectionDisconnect(MDRConnection* conn)
{
    return conn->disconnect(conn->user);
}

int mdrConnectionRecv(MDRConnection* conn, char* dst, int size, int* pReceived)
{
    return conn->recv(conn->user, dst, size, pReceived);
}

int mdrConnectionSend(MDRConnection* conn, const char* src, int size, int* pSent)
{
    return conn->send(conn->user, src, size, pSent);
}

int mdrConnectionPoll(MDRConnection* conn, int timeout)
{
    return conn->poll(conn->user, timeout);
}

int mdrConnectionGetDevicesList(MDRConnection* conn, MDRDeviceInfo** ppList, int* pCount)
{
    return conn->getDevicesList(conn->user, ppList, pCount);
}

int mdrConnectionFreeDevicesList(MDRConnection* conn, MDRDeviceInfo** ppList)
{
    return conn->freeDevicesList(conn->user, ppList);
}

const char* mdrConnectionGetLastError(MDRConnection* conn)
{
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

int mdrHeadphonesRequestInit(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    return h->Invoke(h->RequestInit());
}

int mdrHeadphonesRequestSync(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    return h->Invoke(h->RequestSync());
}

const char* mdrHeadphonesGetLastError(MDRHeadphones* p)
{
    auto h = reinterpret_cast<mdr::MDRHeadphones*>(p);
    return h->GetLastError();
}
}
#pragma endregion
