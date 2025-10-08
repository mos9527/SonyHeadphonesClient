#pragma once

#include "BluetoothWrapper.h"
#include "Constants.h"
#include "SingleInstanceFuture.h"

#include <bitset>
#include <iostream>
#include <map>
#include <mutex>
#include <variant>

template <class T>
struct Property
{
    T current;
    T desired;

    bool pendingRequest = false;

    void flagPending()
    {
        this->pendingRequest = true;
    }

    bool isPending() const
    {
        return this->pendingRequest;
    }

    void fulfill()
    {
        this->current = this->desired;
        this->pendingRequest = false;
    }

    bool isFulfilled()
    {
        return this->desired == this->current;
    }

    void overwrite(const T& value)
    {
        current = value;
        desired = value;
        this->pendingRequest = false;
    };
};

template <class T>
struct ReadonlyProperty
{
    T current;

    template <typename U>
    void overwrite(U&& value)
    {
        current = std::forward<U>(value);
    }
};

enum class HeadphonesEvent
{
    MessageUnhandled = -1,

    NoMessage,
    NoChange,

    JSONMessage,
    Initialized,

    DeviceInfoUpdate,
    SupportFunctionUpdate,

    NcAsmParamUpdate,
    NcAmbButtonModeUpdate,
    BatteryLevelUpdate,

    PlaybackMetadataUpdate,
    PlaybackVolumeUpdate,
    PlaybackPlayPauseUpdate,

    SoundPressureUpdate,
    AutoPowerOffUpdate,
    AutoPauseUpdate,
    VoiceGuidanceEnabledUpdate,
    VoiceGuidanceVolumeUpdate,

    SpeakToChatParamUpdate,
    SpeakToChatEnabledUpdate,

    ListeningModeUpdate,

    TouchFunctionUpdate,

    EqualizerAvailableUpdate,
    EqualizerParamUpdate,

    MultipointDeviceSwitchUpdate,

    GeneralSetting1Update,
    GeneralSetting2Update,
    GeneralSetting3Update,
    GeneralSetting4Update,

    SafeListeningParamUpdate,

    ConnectedDeviceUpdate,

    InteractionUpdate,
};

using HeadphonesMessage = CommandSerializer::CommandMessage;

enum DeviceCapabilities
{
    DC_None = 0x0,

    // Since WF-1000XM5
    DC_EqualizerAvailableCommand = 0x1,
};

HEADPHONES_DEFINE_ENUM_FLAG_OPERATORS(DeviceCapabilities);

class Headphones
{
public:
    /* The built-in EQ. All values should integers of range -10~+10 */
    struct EqualizerConfig
    {
        int bassLevel{};
        std::vector<int> bands;
        EqualizerConfig() : bassLevel(0), bands(0) {}
        EqualizerConfig(int bass, const std::vector<int>& bands) : bassLevel(bass), bands(bands) {}
        bool operator==(const EqualizerConfig& other) const
        {
            return bassLevel == other.bassLevel && bands == other.bands;
        }
    };

    /* New Listening Mode in WH-1000XM6 */
    struct ListeningModeConfig
    {
        ListeningMode nonBgmMode; // 03 XX
        bool bgmActive; // 04 >XX< ?? (inverted)
        THMSGV2T1::RoomSize bgmDistanceMode; // 04 ?? >XX<

        ListeningModeConfig()
            : nonBgmMode(ListeningMode::Standard)
            , bgmActive(false)
            , bgmDistanceMode(THMSGV2T1::RoomSize::SMALL)
        {}

        ListeningModeConfig(ListeningMode nonBgm, bool bgmActive, THMSGV2T1::RoomSize bgmDistance)
            : nonBgmMode(nonBgm)
            , bgmActive(bgmActive)
            , bgmDistanceMode(bgmDistance)
        {}

        ListeningMode getEffectiveMode() const
        {
            if (bgmActive)
                return ListeningMode::BGM;
            else
                return nonBgmMode;
        }

        bool operator==(const ListeningModeConfig& other) const
        {
            return nonBgmMode == other.nonBgmMode && bgmActive == other.bgmActive && bgmDistanceMode == other.bgmDistanceMode;
        }
    };

    Headphones(BluetoothWrapper& conn);

    ReadonlyProperty<HeadphonesMessage> rawMessage;

    // Capability counter and MAC address
    ReadonlyProperty<uint8_t> capabilityCounter{};
    ReadonlyProperty<std::string> uniqueId{};

    // Device model name from firmware
    ReadonlyProperty<std::string> modelName{};

    // Firmware version string
    ReadonlyProperty<std::string> fwVersion{};

    // Series and color
    ReadonlyProperty<THMSGV2T1::ModelSeries> modelSeries{};
    ReadonlyProperty<ModelColor> modelColor{};

    // Support functions
    std::bitset<256> supportFunctions1{};
    std::bitset<256> supportFunctions2{};
    ReadonlyProperty<std::string> supportFunctionString1{};
    ReadonlyProperty<std::string> supportFunctionString2{};

    // Is NC or Ambient sound enabled?
    Property<bool> asmEnabled{};

    // NC or Ambient sound mode?
    Property<THMSGV2T1::NcAsmMode> asmMode{};

    // Is Foucs On Voice enabled?
    Property<bool> asmFoucsOnVoice{};

    // Ambient sound level. 0 ~ 20.
    // 0 shouldn't be a possible value on the app.
    Property<int> asmLevel{};
    Property<THMSGV2T1::ValueChangeStatus> changingAsmLevel{ THMSGV2T1::ValueChangeStatus::CHANGED, THMSGV2T1::ValueChangeStatus::CHANGED };

    // Is auto ambient sound enabled? (WH-1000XM6 onwards)
    Property<bool> autoAsmEnabled{};

    // Auto ambient sound sensitivity. 0 ~ 2. (WH-1000XM6 onwards)
    Property<THMSGV2T1::NoiseAdaptiveSensitivity> autoAsmSensitivity{};

    // Is automatic power off enabled?
    Property<bool> autoPowerOffEnabled{};

    // Is "Pause when headphones are removed" enabled?
    Property<bool> autoPauseEnabled{};

    // Is voice guidance enabled?
    Property<bool> voiceGuidanceEnabled{};

    // Volume for voice guidance. -2 ~ 2
    Property<int> miscVoiceGuidanceVol{};

    // Battery levels 0 ~ 100
    struct BatteryData
    {
        uint8_t level{};
        THMSGV2T1::BatteryChargingStatus chargingStatus{};
        uint8_t thresh{ 0xFF };
    };
    ReadonlyProperty<BatteryData> statBatteryL{};
    ReadonlyProperty<BatteryData> statBatteryR{};
    ReadonlyProperty<BatteryData> statBatteryCase{};

    // Volume. 0 ~ 30
    Property<int> volume{};

    // Play/Pause. true for play, false for pause
    ReadonlyProperty<bool> playPause{};

    // Plaintext messages
    ReadonlyProperty<std::string> deviceMessages{};

    // Headphone interaction message. Avalialbe after InteractionUpdate
    ReadonlyProperty<std::string> interactionMessage{};

    // Connected devices
    std::map<uint8_t, BluetoothDevice> connectedDevices;

    // Paired devices that are not connected
    std::map<std::string, BluetoothDevice> pairedDevices;

    uint8_t playbackDevice;

    struct GsCapability
    {
        THMSGV2T1::GsSettingType type;
        THMSGV2T1::GsSettingInfo info;
    };
    ReadonlyProperty<GsCapability> gs1c{}, gs2c{}, gs3c{}, gs4c{};
    Property<bool> gs1{}, gs2{}, gs3{}, gs4{}; // FIXME uint8_t to allow both bool and list types

    // Playback
    struct
    {
        std::string title;
        std::string album;
        std::string artist;
        int sndPressure{};
    } playback;

    // Safe listening
    struct
    {
        Property<bool> preview{};
    } safeListening;

    // Multipoint
    Property<std::string> mpDeviceMac{};

    // Speak to chat
    Property<bool> stcEnabled{};

    // AUTO:0 HIGH:1 LOW:2
    Property<THMSGV2T1::DetectSensitivity> stcLevel{};

    // SHORT:0 STANDARD:1 LONG:2 OFF(Does not close automatically):3
    Property<THMSGV2T1::ModeOutTime> stcTime{};

    // Listening mode
    Property<ListeningModeConfig> listeningModeConfig{};

    // Equalizer
    ReadonlyProperty<bool> eqAvailable{};
    Property<int> eqPreset;
    Property<EqualizerConfig> eqConfig;

    // [WF only] Touch sensor function
    Property<THMSGV2T1::Preset> touchLeftFunc{}, touchRightFunc{};

    // [WH only] [NC/AMB] Button Setting
    Property<THMSGV2T1::Function> ncAmbButtonMode{};

    // Protocol version
    uint32_t protocolVersion{};
    bool supportsTable1{};
    bool supportsTable2{};
    DeviceCapabilities deviceCapabilities{};

    bool supports(MessageMdrV2FunctionType_Table1 functionTypeTable1) const;
    bool supports(MessageMdrV2FunctionType_Table2 functionTypeTable2) const;
    bool supportsNc() const;
    bool supportsAsm() const;
    bool supportsSafeListening() const;
    bool supportsAutoPowerOff() const;

    bool isChanged();
    void setChanges();

    void waitForAck(int timeout = 1);
    void waitForProtocolInfo(int timeout);
    void waitForSupportFunction(int timeout);
    // void waitForResponse(int timeout = 5);

    template <typename TPayload, typename... TArgs>
    void sendGet(TArgs&&... args);

    template <typename TPayload, typename... TArgs>
    void sendSet(TArgs&&... args);

    template <typename TPayload, typename... TArgs>
    void sendMdrCommandWithType(CommandType ct, TArgs&&... args);

    template <typename TPayload>
    void sendMdrCommandWithTypeHelper(CommandType ct, const BufferSpan& span);

    void requestInit();
    void requestSync();
    void requestPlaybackControl(THMSGV2T1::PlaybackControl control);
    void requestPowerOff();

    void recvAsync();
    BluetoothWrapper& getConn() { return _conn; }

    /*
    Asynchornously poll for incoming messages and (optionally) returns any event
    that has been triggered by the message.
    This function is non-blocking and thread-safe.
    */
    HeadphonesEvent poll();

    bool isConnected() const { return _conn.isConnected(); }
    void disconnect();

    ~Headphones();

    SingleInstanceFuture<std::optional<HeadphonesMessage>> _recvFuture;
    SingleInstanceFuture<void> _sendCommandFuture;
    SingleInstanceFuture<void> _requestFuture;

private:
    std::mutex _propertyMtx, _ackMtx;
    BluetoothWrapper& _conn;
    std::condition_variable _ackCV;
    std::condition_variable _protocolInfoCV;
    std::condition_variable _supportFunctionCV;
    /*std::condition_variable _responseCV;
    DATA_TYPE _waitForResponseDataType = DATA_TYPE::UNKNOWN;
    uint8_t _waitForResponseCommandId = 0xFF;*/

    // Helper functions for _handleMessage
    HeadphonesEvent _handleProtocolInfo(const HeadphonesMessage& msg);
    HeadphonesEvent _handleCapabilityInfo(const HeadphonesMessage& msg);
    HeadphonesEvent _handleDeviceInfo(const HeadphonesMessage& msg);
    HeadphonesEvent _handleT1SupportFunction(const HeadphonesMessage& msg);
    HeadphonesEvent _handleT2SupportFunction(const HeadphonesMessage& msg);
    HeadphonesEvent _handleNcAsmParam(const HeadphonesMessage& msg, CommandType ct);
    HeadphonesEvent _handleBatteryLevelRet(const HeadphonesMessage& msg);
    HeadphonesEvent _handlePlaybackParam(const HeadphonesMessage& msg, CommandType ct);
    HeadphonesEvent _handleSafeListeningNotifyParam(const HeadphonesMessage& msg);
    HeadphonesEvent _handleSafeListeningExtendedParam(const HeadphonesMessage& msg);
    HeadphonesEvent _handlePowerParam(const HeadphonesMessage& msg, CommandType ct);
    HeadphonesEvent _handleVoiceGuidanceParam(const HeadphonesMessage& msg, CommandType ct);
    HeadphonesEvent _handlePeripheralNotifyExtendedParam(const HeadphonesMessage& msg);
    HeadphonesEvent _handlePeripheralParam(const HeadphonesMessage& msg, CommandType ct);
    HeadphonesEvent _handlePlaybackStatus(const HeadphonesMessage& msg, CommandType ct);
    HeadphonesEvent _handleGsCapability(const HeadphonesMessage& msg);
    HeadphonesEvent _handleGeneralSettingParam(const HeadphonesMessage& msg, CommandType ct);
    HeadphonesEvent _handleAudioParam(const HeadphonesMessage& msg, CommandType ct);
    HeadphonesEvent _handleSystemParam(const HeadphonesMessage& msg, CommandType ct);
    HeadphonesEvent _handleSystemExtParam(const HeadphonesMessage& msg, CommandType ct);
    HeadphonesEvent _handleEqualizerAvailable(const HeadphonesMessage& msg);
    HeadphonesEvent _handleEqualizer(const HeadphonesMessage& msg);
    HeadphonesEvent _handleMiscDataRet(const HeadphonesMessage& msg);

    HeadphonesEvent _handleMessage(const HeadphonesMessage& msg);

    bool hasInit = false;
};

template <typename, typename = void>
struct PayloadIsForMultipleCommandTypes : std::false_type {};

template <typename T>
struct PayloadIsForMultipleCommandTypes<T, std::void_t<decltype(T::COMMAND_IDS)>> : std::true_type {};

template <typename T>
inline constexpr bool PayloadIsForMultipleCommandTypes_v = PayloadIsForMultipleCommandTypes<T>::value;

template <typename TPayload, typename... TArgs>
void Headphones::sendGet(TArgs&&... args)
{
    if constexpr (PayloadIsForMultipleCommandTypes_v<TPayload>)
    {
        static_assert(static_cast<uint8_t>(TPayload::COMMAND_IDS[CT_Get]) != 0xFF, "This command does not support Get");
    }
    sendMdrCommandWithType<TPayload>(CT_Get, std::forward<TArgs>(args)...);
}

template <typename TPayload, typename... TArgs>
void Headphones::sendSet(TArgs&&... args)
{
    if constexpr (PayloadIsForMultipleCommandTypes_v<TPayload>)
    {
        static_assert(static_cast<uint8_t>(TPayload::COMMAND_IDS[CT_Set]) != 0xFF, "This command does not support Set");
    }
    sendMdrCommandWithType<TPayload>(CT_Set, std::forward<TArgs>(args)...);
}

template <typename TPayload, typename... TArgs>
void Headphones::sendMdrCommandWithType(CommandType ct, TArgs&&... args)
{
    if constexpr (TPayload::VARIABLE_SIZE_ONE_ARRAY_AT_END)
    {
        size_t size;
        std::unique_ptr<TPayload> payload = [&]
        {
            if constexpr (PayloadIsForMultipleCommandTypes_v<TPayload>)
                return createVariableSizePayloadOneArrayAtEnd_CommandType<TPayload>(&size, ct, std::forward<TArgs>(args)...);
            else
                return createVariableSizePayloadOneArrayAtEnd<TPayload>(&size, std::forward<TArgs>(args)...);
        }();

        const uint8_t* data = reinterpret_cast<const uint8_t*>(payload.get());
        sendMdrCommandWithTypeHelper<TPayload>(ct, BufferSpan(data, data + size));
    }
    else
    {
        TPayload payload = [&]
        {
            if constexpr (PayloadIsForMultipleCommandTypes_v<TPayload>)
                return TPayload(ct, std::forward<TArgs>(args)...);
            else
                return TPayload(std::forward<TArgs>(args)...);
        }();

        if constexpr (TPayload::VARIABLE_SIZE_NEEDS_SERIALIZATION)
        {
            std::vector<uint8_t> buf;
            buf.reserve(payload.countBytes());
            payload.serialize(buf);
            sendMdrCommandWithTypeHelper<TPayload>(ct, buf);
        }
        else
        {
            const uint8_t* data = reinterpret_cast<const uint8_t*>(&payload);
            sendMdrCommandWithTypeHelper<TPayload>(ct, BufferSpan(data, data + sizeof(TPayload)));
        }
    }
}

template <typename TPayload>
void Headphones::sendMdrCommandWithTypeHelper(CommandType ct, const BufferSpan& span)
{
    DATA_TYPE type;
    if constexpr (std::is_base_of_v<THMSGV2T1::Payload, TPayload>)
    {
        type = DATA_TYPE::DATA_MDR;
    }
    else if constexpr (std::is_base_of_v<THMSGV2T2::Payload, TPayload>)
    {
        type = DATA_TYPE::DATA_MDR_NO2;
    }
    else
    {
        static_assert(sizeof(TPayload) == 0, "Unsupported payload type");
        type = DATA_TYPE::UNKNOWN;
    }

    bool valid;
    if constexpr (PayloadIsForMultipleCommandTypes_v<TPayload>)
        valid = TPayload::isValid(span, ct);
    else
        valid = TPayload::isValid(span);
    if (!valid)
    {
        throw std::runtime_error("Invalid outgoing payload");
    }

    _conn.sendCommand(span.data(), span.size(), type);

    /*uint8_t responseCommandId;
    if constexpr (PayloadIsForMultipleCommandTypes_v<TPayload>)
        responseCommandId = static_cast<uint8_t>(TPayload::RESPONSE_COMMAND_IDS[ct]);
    else
        responseCommandId = static_cast<uint8_t>(TPayload::RESPONSE_COMMAND_ID);
    if (responseCommandId != 0xFF)
    {
        _waitForResponseDataType = type;
        _waitForResponseCommandId = responseCommandId;
        waitForResponse();
    }*/
    waitForAck();
}
