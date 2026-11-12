#include <mdr-c/Headphones.h>

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MDR_ASSERT_U32(type) static_assert(sizeof(type) == sizeof(uint32_t), #type " must be uint32_t")
MDR_ASSERT_U32(MDRResult);
MDR_ASSERT_U32(MDRPropertyType);
MDR_ASSERT_U32(MDRBoolean);
MDR_ASSERT_U32(MDRFeatureAvailability);
MDR_ASSERT_U32(MDRHeadphonesInitFlags);
MDR_ASSERT_U32(MDRHeadphonesSyncFlags);
MDR_ASSERT_U32(MDRFeature);
MDR_ASSERT_U32(MDREvent);
MDR_ASSERT_U32(MDRPacketDirection);
MDR_ASSERT_U32(MDRText);
MDR_ASSERT_U32(MDRAudioCodec);
MDR_ASSERT_U32(MDRBatteryPart);
MDR_ASSERT_U32(MDRChargingState);
MDR_ASSERT_U32(MDRPlaybackStatus);
MDR_ASSERT_U32(MDRPlaybackAction);
MDR_ASSERT_U32(MDRNoiseMode);
MDR_ASSERT_U32(MDRAdaptiveSensitivity);
MDR_ASSERT_U32(MDRNoiseButtonMode);
MDR_ASSERT_U32(MDRSpeechSensitivity);
MDR_ASSERT_U32(MDRSpeakTimeout);
MDR_ASSERT_U32(MDRListeningMode);
MDR_ASSERT_U32(MDRRoomSize);
MDR_ASSERT_U32(MDREqualizerPreset);
MDR_ASSERT_U32(MDRDSEEType);
MDR_ASSERT_U32(MDRPairedDeviceCommand);
MDR_ASSERT_U32(MDRGeneralSettingType);
MDR_ASSERT_U32(MDRAssignableAction);
MDR_ASSERT_U32(MDRWearingPowerMode);
MDR_ASSERT_U32(MDRWearingStatus);
MDR_ASSERT_U32(MDRAudioPriority);
MDR_ASSERT_U32(MDRListeningBlocker);
MDR_ASSERT_U32(MDRVoiceAssistant);
MDR_ASSERT_U32(MDRVoiceAssistantKey);
#undef MDR_ASSERT_U32

static_assert(sizeof(((MDRDeviceInfo*)0)->szDeviceName) == 256, "device name capacity must be 256 bytes");
static_assert(offsetof(MDRDeviceInfo, szDeviceMacAddress) == 256, "device address offset must remain fixed");
static_assert(sizeof(MDRDeviceInfo) == 274, "device info array stride must remain fixed");

#define MDR_ASSERT_FIELD(type, field, scalar, offset)                                                                  \
    static_assert(sizeof(((type*)0)->field) == 4, #type "." #field " must be 32-bit");                                 \
    static_assert(_Generic(((type*)0)->field, scalar: 1, default: 0), #type "." #field " signedness");                 \
    static_assert(offsetof(type, field) == offset, #type "." #field " offset")
MDR_ASSERT_FIELD(MDRModel, model_color, uint32_t, 8);
MDR_ASSERT_FIELD(MDRBattery, level_percent, uint32_t, 4);
MDR_ASSERT_FIELD(MDRBattery, update_threshold_percent, uint32_t, 8);
MDR_ASSERT_FIELD(MDRPlayback, volume, uint32_t, 4);
MDR_ASSERT_FIELD(MDRNoiseControl, ambient_level, uint32_t, 4);
MDR_ASSERT_FIELD(MDREqualizer, clear_bass, int32_t, 4);
MDR_ASSERT_FIELD(MDRVoiceGuidance, volume, int32_t, 4);
MDR_ASSERT_FIELD(MDRSafeListening, sound_pressure, uint32_t, 0);
#undef MDR_ASSERT_FIELD

#define MDR_ASSERT_LAYOUT(type, size) static_assert(sizeof(type) == size && _Alignof(type) == 4, #type " ABI layout")
MDR_ASSERT_LAYOUT(MDRModel, 12);
MDR_ASSERT_LAYOUT(MDRBattery, 16);
MDR_ASSERT_LAYOUT(MDRPlayback, 8);
MDR_ASSERT_LAYOUT(MDRNoiseControl, 28);
MDR_ASSERT_LAYOUT(MDREqualizer, 28);
MDR_ASSERT_LAYOUT(MDRVoiceGuidance, 8);
MDR_ASSERT_LAYOUT(MDRSafeListening, 8);
MDR_ASSERT_LAYOUT(MDRListeningStatus, 8);
MDR_ASSERT_LAYOUT(MDRVoiceAssistantSettings, 12);
MDR_ASSERT_LAYOUT(MDRVoiceAssistantOption, 8);
MDR_ASSERT_LAYOUT(MDRGeneralSettingInfo, 20);
MDR_ASSERT_LAYOUT(MDRGeneralSetting, 12);
#undef MDR_ASSERT_LAYOUT
static_assert(offsetof(MDRGeneralSettingInfo, option_count) == 12 && offsetof(MDRGeneralSettingInfo, text_is_key) == 16,
              "general setting info offsets");
static_assert(offsetof(MDRGeneralSetting, list_value) == 8, "general setting list value offset");
static_assert(MDR_GENERAL_SETTING_OPTION_INDEX(3, 2) == 0x203u, "general setting option text index");
static_assert(offsetof(MDRBattery, charging) == 12, "battery charging offset");
static_assert(offsetof(MDRNoiseControl, changing_asm_level) == 8, "noise changing level offset");
static_assert(offsetof(MDREqualizer, band_count) == 8 && offsetof(MDREqualizer, dsee_available) == 24,
              "equalizer offsets");
static_assert(offsetof(MDRSafeListening, preview) == 4, "safe listening preview offset");

static_assert(_Generic((MDRPacketCallback)0,
                  void (*)(void*, MDRPacketDirection, uint64_t, const unsigned char*, int): 1,
                  default: 0),
              "packet callback signature");
static_assert(_Generic(&mdrDebugRequestCommand,
                  MDRResult (*)(MDRHeadphones*, const unsigned char*, uint32_t, uint32_t, MDRBoolean): 1,
                  default: 0),
              "debug command signature");
static_assert(_Generic(&mdrHeadphonesRequestInit, MDRResult (*)(MDRHeadphones*, MDRHeadphonesInitFlags): 1, default: 0),
              "init signature");
static_assert(_Generic(&mdrHeadphonesRequestSync, MDRResult (*)(MDRHeadphones*, MDRHeadphonesSyncFlags): 1, default: 0),
              "sync signature");
static_assert(_Generic(&mdrHeadphonesGetSync, MDRResult (*)(MDRHeadphones*, MDRHeadphonesSyncFlags*): 1, default: 0),
              "sync getter signature");
static_assert(_Generic(&mdrHeadphonesGetEqualizer,
                  MDRResult (*)(MDRHeadphones*, MDRPropertyType, MDREqualizer*): 1,
                  default: 0),
              "EQ getter signature");
static_assert(_Generic(&mdrHeadphonesSetEqualizerPreset,
                  MDRResult (*)(MDRHeadphones*, MDREqualizerPreset): 1,
                  default: 0),
              "EQ preset setter signature");
static_assert(_Generic(&mdrHeadphonesSetEqualizerClearBass, MDRResult (*)(MDRHeadphones*, int32_t): 1, default: 0),
              "Clear Bass setter signature");
static_assert(_Generic(&mdrHeadphonesSetDSEE, MDRResult (*)(MDRHeadphones*, MDRBoolean): 1, default: 0),
              "DSEE setter signature");
static_assert(_Generic(&mdrHeadphonesGetEqualizerBands,
                  MDRResult (*)(MDRHeadphones*, MDRPropertyType, int32_t*, uint32_t*): 1,
                  default: 0),
              "EQ bands getter signature");
static_assert(_Generic(&mdrHeadphonesSetEqualizerBands,
                  MDRResult (*)(MDRHeadphones*, const int32_t*, uint32_t): 1,
                  default: 0),
              "EQ bands setter signature");
static_assert(_Generic(&mdrHeadphonesSetPlaybackVolume, MDRResult (*)(MDRHeadphones*, uint32_t): 1, default: 0),
              "volume setter signature");
static_assert(_Generic(&mdrHeadphonesGetListeningStatus,
                  MDRResult (*)(MDRHeadphones*, MDRListeningMode, MDRListeningStatus*): 1,
                  default: 0),
              "listening status signature");
static_assert(_Generic(&mdrHeadphonesGetVoiceAssistant,
                  MDRResult (*)(MDRHeadphones*, MDRPropertyType, MDRVoiceAssistantSettings*): 1,
                  default: 0),
              "voice assistant getter signature");
static_assert(_Generic(&mdrHeadphonesGetVoiceAssistantOptions,
                  MDRResult (*)(MDRHeadphones*, MDRVoiceAssistantOption*, uint32_t*): 1,
                  default: 0),
              "voice assistant options signature");
static_assert(_Generic(&mdrHeadphonesSetVoiceAssistant,
                  MDRResult (*)(MDRHeadphones*, MDRVoiceAssistant): 1,
                  default: 0),
              "voice assistant setter signature");

enum
{
    MOCK_BUFFER_CAPACITY = 4096,
    FRAME_BUFFER_CAPACITY = 64,
    MDR_DATA_TYPE_ACK = 1,
    MDR_DATA_TYPE_DATA_MDR = 12
};

typedef struct MockTransport
{
    unsigned char rx[MOCK_BUFFER_CAPACITY];
    size_t rx_size;
    size_t rx_offset;
    unsigned char tx[MOCK_BUFFER_CAPACITY];
    size_t tx_size;
    MDRResult poll_result;
    MDRResult send_result;
    MDRResult receive_result;
    unsigned int poll_count;
    unsigned int send_count;
    unsigned int receive_count;
    MDRConnection connection;
} MockTransport;

typedef struct Session
{
    MockTransport transport;
    MDRHeadphones* headphones;
} Session;

static int g_failures;

static void check(int condition, const char* message)
{
    if (condition)
        return;
    fprintf(stderr, "FAIL: %s\n", message);
    ++g_failures;
}

static void check_result(MDRResult actual, MDRResult expected, const char* message)
{
    if (actual == expected)
        return;
    fprintf(stderr, "FAIL: %s (expected %ld, got %ld)\n", message, (long)expected, (long)actual);
    ++g_failures;
}

static MDRResult mock_connect(void* user, const char* address, const char* service)
{
    (void)user;
    (void)address;
    (void)service;
    return MDR_RESULT_OK;
}

static void mock_disconnect(void* user)
{
    (void)user;
}

static MDRResult mock_receive(void* user, char* destination, int size, int* received)
{
    MockTransport* transport = (MockTransport*)user;
    size_t remaining;
    size_t count;

    ++transport->receive_count;
    *received = 0;
    if (transport->receive_result != MDR_RESULT_OK)
        return transport->receive_result;
    if (transport->rx_offset == transport->rx_size)
        return MDR_RESULT_INPROGRESS;

    remaining = transport->rx_size - transport->rx_offset;
    count = (size_t)size < remaining ? (size_t)size : remaining;
    memcpy(destination, transport->rx + transport->rx_offset, count);
    transport->rx_offset += count;
    *received = (int)count;
    return MDR_RESULT_OK;
}

static MDRResult mock_send(void* user, const char* source, int size, int* sent)
{
    MockTransport* transport = (MockTransport*)user;
    size_t count = (size_t)size;

    ++transport->send_count;
    *sent = 0;
    if (transport->send_result != MDR_RESULT_OK)
        return transport->send_result;
    if (count > MOCK_BUFFER_CAPACITY - transport->tx_size)
        return MDR_RESULT_ERROR_BUFFER_TOO_SMALL;
    memcpy(transport->tx + transport->tx_size, source, count);
    transport->tx_size += count;
    *sent = size;
    return MDR_RESULT_OK;
}

static MDRResult mock_poll(void* user, int timeout)
{
    MockTransport* transport = (MockTransport*)user;
    (void)timeout;
    ++transport->poll_count;
    return transport->poll_result;
}

static MDRResult mock_get_devices(void* user, MDRDeviceInfo** devices, int* count)
{
    (void)user;
    *devices = NULL;
    *count = 0;
    return MDR_RESULT_OK;
}

static MDRResult mock_free_devices(void* user, MDRDeviceInfo** devices)
{
    (void)user;
    *devices = NULL;
    return MDR_RESULT_OK;
}

static const char* mock_get_last_error(void* user)
{
    (void)user;
    return "mock transport";
}

static void mock_init(MockTransport* transport)
{
    memset(transport, 0, sizeof(*transport));
    transport->connection.user = transport;
    transport->connection.connect = mock_connect;
    transport->connection.disconnect = mock_disconnect;
    transport->connection.recv = mock_receive;
    transport->connection.send = mock_send;
    transport->connection.poll = mock_poll;
    transport->connection.getDevicesList = mock_get_devices;
    transport->connection.freeDevicesList = mock_free_devices;
    transport->connection.getLastError = mock_get_last_error;
}

static void mock_load(MockTransport* transport, const unsigned char* data, size_t size)
{
    check(size <= MOCK_BUFFER_CAPACITY, "mock input fits");
    if (size > MOCK_BUFFER_CAPACITY)
        return;
    memcpy(transport->rx, data, size);
    transport->rx_size = size;
    transport->rx_offset = 0;
}

static void mock_append(MockTransport* transport, const unsigned char* data, size_t size)
{
    check(size <= MOCK_BUFFER_CAPACITY - transport->rx_size, "appended mock input fits");
    if (size > MOCK_BUFFER_CAPACITY - transport->rx_size)
        return;
    memcpy(transport->rx + transport->rx_size, data, size);
    transport->rx_size += size;
}

static int session_open_family(Session* session, MDRProtocolVersion family)
{
    memset(session, 0, sizeof(*session));
    mock_init(&session->transport);
    check_result(mdrHeadphonesCreate(MDR_ABI_VERSION, &session->transport.connection, family, &session->headphones),
                 MDR_RESULT_OK, "opaque headphones session opens");
    return session->headphones != NULL;
}

static int session_open(Session* session)
{
    return session_open_family(session, MDR_PROTOCOL_V2);
}

static void session_close(Session* session)
{
    mdrHeadphonesDestroy(session->headphones);
    session->headphones = NULL;
}

static size_t append_escaped(unsigned char byte, unsigned char* output, size_t offset)
{
    if (byte == 0x3c || byte == 0x3d || byte == 0x3e)
    {
        output[offset++] = 0x3d;
        output[offset++] = (unsigned char)(byte - 0x10);
    }
    else
    {
        output[offset++] = byte;
    }
    return offset;
}

static size_t pack_frame(unsigned char type, unsigned char sequence, const unsigned char* payload, size_t payload_size,
                         unsigned char output[FRAME_BUFFER_CAPACITY])
{
    unsigned char unescaped[FRAME_BUFFER_CAPACITY];
    unsigned char checksum = 0;
    size_t unescaped_size = 0;
    size_t output_size = 0;
    size_t index;

    check(payload_size <= FRAME_BUFFER_CAPACITY - 7, "test payload fits frame buffer");
    if (payload_size > FRAME_BUFFER_CAPACITY - 7)
        return 0;

    unescaped[unescaped_size++] = type;
    unescaped[unescaped_size++] = sequence;
    unescaped[unescaped_size++] = (unsigned char)(payload_size >> 24);
    unescaped[unescaped_size++] = (unsigned char)(payload_size >> 16);
    unescaped[unescaped_size++] = (unsigned char)(payload_size >> 8);
    unescaped[unescaped_size++] = (unsigned char)payload_size;
    if (payload_size != 0)
    {
        memcpy(unescaped + unescaped_size, payload, payload_size);
        unescaped_size += payload_size;
    }
    for (index = 0; index < unescaped_size; ++index)
        checksum = (unsigned char)(checksum + unescaped[index]);
    unescaped[unescaped_size++] = checksum;

    output[output_size++] = 0x3e;
    for (index = 0; index < unescaped_size; ++index)
        output_size = append_escaped(unescaped[index], output, output_size);
    output[output_size++] = 0x3c;
    return output_size;
}

static size_t pack_data_frame(const unsigned char* payload, size_t payload_size, unsigned char sequence,
                              unsigned char output[FRAME_BUFFER_CAPACITY])
{
    return pack_frame(MDR_DATA_TYPE_DATA_MDR, sequence, payload, payload_size, output);
}

typedef struct TxFrame
{
    unsigned char type;
    unsigned char sequence;
    unsigned char payload[FRAME_BUFFER_CAPACITY];
    size_t payload_size;
} TxFrame;

/*
 * Decodes the frame beginning at *offset and advances *offset past it, so a caller can walk
 * everything the library has transmitted - to answer it the way a device would, or to assert
 * on what was asked for. Returns 0 at the end of the stream, or on a frame still in flight.
 */
static int next_tx_frame(MockTransport* transport, size_t* offset, TxFrame* frame)
{
    unsigned char unescaped[FRAME_BUFFER_CAPACITY];
    size_t unescaped_size = 0;
    size_t index = *offset;

    while (index < transport->tx_size && transport->tx[index] != 0x3e)
        ++index;
    if (index == transport->tx_size)
        return 0;
    ++index;
    while (index < transport->tx_size && transport->tx[index] != 0x3c)
    {
        unsigned char byte = transport->tx[index++];
        if (byte == 0x3d && index < transport->tx_size)
            byte = (unsigned char)(transport->tx[index++] + 0x10);
        if (unescaped_size < FRAME_BUFFER_CAPACITY)
            unescaped[unescaped_size++] = byte;
    }
    if (index == transport->tx_size)
        return 0;
    ++index;
    /* type, sequence, four size bytes, payload, checksum */
    if (unescaped_size < 7)
        return 0;
    *offset = index;
    frame->type = unescaped[0];
    frame->sequence = unescaped[1];
    frame->payload_size = unescaped_size - 7;
    memcpy(frame->payload, unescaped + 6, frame->payload_size);
    return 1;
}

/* Sequence number of the last non-ACK frame the library actually put on the wire. */
static int last_tx_sequence(MockTransport* transport, unsigned char* sequence)
{
    TxFrame frame;
    size_t offset = 0;
    int found = 0;

    while (next_tx_frame(transport, &offset, &frame))
    {
        /* Our own acknowledgements are not something the device would acknowledge back. */
        if (frame.type != MDR_DATA_TYPE_ACK)
        {
            *sequence = frame.sequence;
            found = 1;
        }
    }
    return found;
}

/*
 * Devices acknowledge a DATA frame by echoing the inverted sequence number of the frame
 * they received, and libmdr only advances its transmit sequence once that acknowledgement
 * arrives. Deriving the ACK from what was actually transmitted - rather than from a fixed
 * constant - is what a device does, and keeps repeated exchanges honest about the toggle.
 */
static size_t pack_ack(MockTransport* transport, unsigned char output[FRAME_BUFFER_CAPACITY])
{
    unsigned char sequence = 0;

    check(last_tx_sequence(transport, &sequence), "an outbound frame is available to acknowledge");
    return pack_frame(MDR_DATA_TYPE_ACK, (unsigned char)(1 - sequence), NULL, 0, output);
}

static int poll_event(MDRHeadphones* headphones, MDREvent* event, const char* message)
{
    MDRResult result = mdrHeadphonesPoll(headphones, event);
    check_result(result, MDR_RESULT_OK, message);
    return result == MDR_RESULT_OK;
}

static char* get_text(MDRHeadphones* headphones, MDRText text)
{
    uint32_t size = 0;
    char* buffer;

    if (mdrHeadphonesGetText(headphones, text, 0, NULL, &size) != MDR_RESULT_OK)
        return NULL;
    buffer = (char*)malloc(size);
    if (buffer == NULL)
        return NULL;
    if (mdrHeadphonesGetText(headphones, text, 0, buffer, &size) != MDR_RESULT_OK)
    {
        free(buffer);
        return NULL;
    }
    return buffer;
}

static const unsigned char k_v2_protocol_info[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01};

/* As above, but with the table 2 support byte set to ENABLE rather than DISABLE. */
static const unsigned char k_v2_protocol_info_both_tables[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00};

/*
 * V1's reply is four bytes rather than eight: command, inquired type, and a big-endian
 * protocol version. It says nothing about tables - V1 works out whether it has a second one
 * from the advertised function list (see RefreshSupportV1).
 */
static const unsigned char k_v1_protocol_info[] = {0x01, 0x00, 0x00, 0x01};

typedef struct PacketLog
{
    uint64_t ids[128];
    MDRPacketDirection directions[128];
    unsigned char frames[128][FRAME_BUFFER_CAPACITY];
    int sizes[128];
    size_t count;
} PacketLog;

static void observe_packet(void* user, MDRPacketDirection direction, uint64_t packet_id, const unsigned char* frame,
                           int frame_size)
{
    PacketLog* log = (PacketLog*)user;
    check(packet_id != 0, "packet IDs reserve zero for no packet");
    check(frame != NULL && frame_size > 0 && frame_size <= FRAME_BUFFER_CAPACITY, "callback frame fits");
    check(log->count < 128, "packet log fits");
    if (!frame || frame_size <= 0 || frame_size > FRAME_BUFFER_CAPACITY || log->count >= 128)
        return;
    if (log->count)
        check(packet_id > log->ids[log->count - 1], "packet IDs increase across TX and RX");
    log->ids[log->count] = packet_id;
    log->directions[log->count] = direction;
    log->sizes[log->count] = frame_size;
    memcpy(log->frames[log->count], frame, (size_t)frame_size);
    ++log->count;
}

static void test_packet_counter(void)
{
    Session session;
    PacketLog log = {0};
    unsigned char frame[FRAME_BUFFER_CAPACITY];
    unsigned char ack[FRAME_BUFFER_CAPACITY];
    MDREvent event;
    const size_t size = pack_data_frame(k_v2_protocol_info, sizeof(k_v2_protocol_info), 0, frame);
    const size_t ack_size = pack_frame(MDR_DATA_TYPE_ACK, 1, NULL, 0, ack);

    if (!session_open(&session))
        return;
    mdrDebugSetPacketCallback(session.headphones, observe_packet, &log);
    mock_load(&session.transport, frame, 3);
    poll_event(session.headphones, &event, "partial RX frame polls");
    check(log.count == 0, "transport fragments do not receive packet IDs");
    mock_load(&session.transport, frame + 3, size - 3);
    poll_event(session.headphones, &event, "complete RX frame polls");
    check(log.count == 2 && log.ids[0] == 1 && log.ids[1] == 2, "RX and generated ACK share one counter");
    check(log.directions[0] == MDR_PACKET_DIRECTION_RX && log.directions[1] == MDR_PACKET_DIRECTION_TX,
          "the RX is observed before its generated TX ACK");
    check(log.sizes[0] == (int)size && memcmp(log.frames[0], frame, size) == 0, "callback preserves the RX frame");
    check(log.sizes[1] == (int)ack_size && memcmp(log.frames[1], ack, ack_size) == 0,
          "callback preserves the TX frame");
    poll_event(session.headphones, &event, "queued ACK flushes");
    check(log.count == 2, "flushing a queued TX frame does not allocate another ID");

    mdrDebugSetPacketCallback(session.headphones, NULL, NULL);
    mock_load(&session.transport, frame, size);
    poll_event(session.headphones, &event, "packets are processed without a callback");
    check(log.count == 2, "disabled observation does not call the old observer");
    log.count = 0;
    mdrDebugSetPacketCallback(session.headphones, observe_packet, &log);
    mock_load(&session.transport, frame, size);
    poll_event(session.headphones, &event, "observation resumes");
    check(log.count == 2 && log.ids[0] == 5 && log.ids[1] == 6,
          "neither disabling observation nor clearing client history resets the library counter");

    frame[size - 2] ^= 1;
    mock_load(&session.transport, frame, size);
    poll_event(session.headphones, &event, "bad checksum frame is discarded");
    check(log.count == 3 && log.ids[2] == 7 && log.directions[2] == MDR_PACKET_DIRECTION_RX,
          "invalid complete frames are observable but do not generate an ACK");
    frame[size - 2] ^= 1;
    session_close(&session);

    if (!session_open(&session))
        return;
    log.count = 0;
    mdrDebugSetPacketCallback(session.headphones, observe_packet, &log);
    mock_load(&session.transport, frame, size);
    poll_event(session.headphones, &event, "new session receives a frame");
    check(log.count == 2 && log.ids[0] == 1 && log.ids[1] == 2, "a new instance has its own packet counter");
    session_close(&session);
}

static void test_debug_command_arguments(void)
{
    Session session;
    PacketLog log = {0};
    unsigned char payload[2048];
    MDREvent event;

    if (!session_open(&session))
        return;
    mdrDebugSetPacketCallback(session.headphones, observe_packet, &log);
    check_result(mdrDebugRequestCommand(NULL, NULL, 0, MDR_DATA_TYPE_DATA_MDR, MDR_FALSE),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "debug command rejects a null session");
    check_result(mdrDebugRequestCommand(session.headphones, NULL, 1, MDR_DATA_TYPE_DATA_MDR, MDR_FALSE),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "debug command requires bytes for a nonempty payload");
    check_result(mdrDebugRequestCommand(session.headphones, payload, 2049, MDR_DATA_TYPE_DATA_MDR, MDR_FALSE),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "debug command rejects an oversized payload before reading it");
    check_result(mdrDebugRequestCommand(session.headphones, payload, UINT32_MAX, MDR_DATA_TYPE_DATA_MDR, MDR_FALSE),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "debug command rejects an overflowing payload length");
    check_result(mdrDebugRequestCommand(session.headphones, NULL, 0, 256, MDR_FALSE), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "debug command rejects envelope types above one byte");
    check_result(mdrDebugRequestCommand(session.headphones, NULL, 0, UINT32_MAX, MDR_FALSE),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "debug command does not truncate the envelope type");
    check_result(mdrDebugRequestCommand(session.headphones, NULL, 0, MDR_DATA_TYPE_DATA_MDR, 2),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "debug command rejects invalid ACK booleans");
    check(log.count == 0 && session.transport.tx_size == 0 && mdrHeadphonesIsReady(session.headphones),
          "invalid debug commands do not allocate packet IDs, send packets or occupy the task slot");

    mdrDebugSetPacketCallback(session.headphones, NULL, NULL);
    memset(payload, 0x55, sizeof(payload));
    check_result(mdrDebugRequestCommand(session.headphones, payload, (uint32_t)sizeof(payload), 0, MDR_FALSE),
                 MDR_RESULT_OK, "maximum-size debug payload and envelope type zero are accepted");
    memset(payload, 0, sizeof(payload));
    poll_event(session.headphones, &event, "debug command without ACK waiting completes");
    check(event == MDR_EVENT_UNHANDLED && mdrHeadphonesIsReady(session.headphones),
          "a debug command without ACK waiting completes through poll");
    poll_event(session.headphones, &event, "remaining bytes of the maximum-size debug frame flush");
    check(session.transport.tx_size == sizeof(payload) + 9 && session.transport.tx[7] == 0x55 &&
              session.transport.tx[7 + sizeof(payload) - 1] == 0x55,
          "debug send retains its copied payload after the caller overwrites the source");
    session_close(&session);
}

static void test_debug_command(MDRProtocolVersion family)
{
    Session session;
    PacketLog log = {0};
    unsigned char payload[] = {0xa6, 0x3c, 0x3d, 0x3e};
    unsigned char expected[FRAME_BUFFER_CAPACITY];
    unsigned char ack[FRAME_BUFFER_CAPACITY];
    size_t expected_size = pack_frame(MDR_DATA_TYPE_DATA_MDR, 0, payload, sizeof(payload), expected);
    size_t ack_size;
    size_t count;
    MDREvent event;

    if (!session_open_family(&session, family))
        return;
    mdrDebugSetPacketCallback(session.headphones, observe_packet, &log);
    check_result(mdrDebugRequestCommand(session.headphones, payload, (uint32_t)sizeof(payload), MDR_DATA_TYPE_DATA_MDR,
                                        MDR_TRUE),
                 MDR_RESULT_OK, "raw debug request can start before initialization without semantic validation");
    memset(payload, 0, sizeof(payload));
    check(log.count == 1 && log.ids[0] == 1 && log.directions[0] == MDR_PACKET_DIRECTION_TX &&
              log.sizes[0] == (int)expected_size && memcmp(log.frames[0], expected, expected_size) == 0,
          "callback exposes the library-assigned sequence, packet ID, escaping and checksum");
    check(session.transport.tx_size == 0 && !mdrHeadphonesIsReady(session.headphones),
          "debug request queues a frame without synchronous transport I/O");
    check_result(mdrDebugRequestCommand(session.headphones, NULL, 0, MDR_DATA_TYPE_DATA_MDR, MDR_FALSE),
                 MDR_RESULT_INPROGRESS, "a second debug request cannot interrupt an active request");
    check_result(mdrHeadphonesRequestInit(session.headphones, MDR_HEADPHONES_INIT_DEFAULT), MDR_RESULT_INPROGRESS,
                 "normal requests share the debug request task slot");
    check(log.count == 1, "busy requests do not emit packets or advance the counter");

    poll_event(session.headphones, &event, "debug request frame flushes");
    check(session.transport.tx_size == expected_size && memcmp(session.transport.tx, expected, expected_size) == 0,
          "transport sends the same copied frame exposed through the callback");
    check(!mdrHeadphonesIsReady(session.headphones), "debug request remains busy while its ACK is missing");
    ack_size = pack_frame(MDR_DATA_TYPE_ACK, 0, NULL, 0, ack);
    mock_load(&session.transport, ack, ack_size);
    poll_event(session.headphones, &event, "wrong-sequence ACK polls");
    check(!mdrHeadphonesIsReady(session.headphones), "wrong-sequence ACK cannot complete the debug request");
    ack_size = pack_ack(&session.transport, ack);
    mock_load(&session.transport, ack, ack_size);
    poll_event(session.headphones, &event, "matching debug ACK polls");
    poll_event(session.headphones, &event, "debug ACK completion polls");
    check(event == MDR_EVENT_UNHANDLED && mdrHeadphonesIsReady(session.headphones),
          "matching ACK releases the task slot through normal polling");

    count = log.count;
    expected_size = pack_frame(0xff, 1, NULL, 0, expected);
    check_result(mdrDebugRequestCommand(session.headphones, NULL, 0, 0xff, MDR_FALSE), MDR_RESULT_OK,
                 "empty payloads and unknown byte-sized envelope types are available for debugging");
    check(count > 0 && log.count == count + 1 && log.ids[count] == log.ids[count - 1] + 1 &&
              log.sizes[count] == (int)expected_size && memcmp(log.frames[count], expected, expected_size) == 0,
          "the next debug packet uses the sequence advanced by the library's ACK handler");
    poll_event(session.headphones, &event, "empty debug packet completes without an ACK");
    check(event == MDR_EVENT_UNHANDLED && mdrHeadphonesIsReady(session.headphones) &&
              !mdrHeadphonesIsInitialized(session.headphones),
          "debug sending does not imply device initialization");
    session_close(&session);
}

static void test_debug_command_timeout(void)
{
    Session session;
    PacketLog log = {0};
    const unsigned char payload[] = {0x00, 0x00};
    MDRResult result = MDR_RESULT_OK;
    MDREvent event;
    clock_t start;

    if (!session_open(&session))
        return;
    mdrDebugSetPacketCallback(session.headphones, observe_packet, &log);
    check_result(mdrHeadphonesSetTimeout(session.headphones, MDR_TIMEOUT_DEFAULT, 1), MDR_RESULT_OK,
                 "short debug ACK wait timeout sets");
    check_result(mdrDebugRequestCommand(session.headphones, payload, (uint32_t)sizeof(payload), MDR_DATA_TYPE_DATA_MDR,
                                        MDR_TRUE),
                 MDR_RESULT_OK, "debug request with a missing ACK starts");
    start = clock();
    while (result == MDR_RESULT_OK && clock() - start < CLOCKS_PER_SEC)
        result = mdrHeadphonesPoll(session.headphones, &event);
    check_result(result, MDR_RESULT_ERROR_TIMEOUT, "debug ACK wait uses the default timeout");
    check(log.count == 1 && !mdrHeadphonesIsReady(session.headphones),
          "debug command timeout is fatal without retries");
    check_result(mdrDebugRequestCommand(session.headphones, NULL, 0, MDR_DATA_TYPE_DATA_MDR, MDR_FALSE),
                 MDR_RESULT_ERROR_TIMEOUT, "a failed session rejects subsequent debug requests with the stored error");
    check(log.count == 1, "failed debug requests do not generate another packet");
    session_close(&session);
}

static void select_v2(Session* session, const char* message)
{
    unsigned char frame[FRAME_BUFFER_CAPACITY];
    MDREvent event;
    size_t frame_size = pack_data_frame(k_v2_protocol_info, sizeof(k_v2_protocol_info), 0, frame);
    mock_load(&session->transport, frame, frame_size);
    poll_event(session->headphones, &event, message);
}

static void test_abi_version_handshake(void)
{
    MockTransport transport;
    MDRHeadphones* headphones = (MDRHeadphones*)0x1;

    mock_init(&transport);
    check_result(mdrHeadphonesCreate(MDR_ABI_VERSION + 1u, &transport.connection, MDR_PROTOCOL_V2, &headphones),
                 MDR_RESULT_ERROR_ABI_MISMATCH, "a newer header is refused by an older library");
    check(headphones == NULL, "a refused handshake leaves no instance behind");

    headphones = (MDRHeadphones*)0x1;
    check_result(mdrHeadphonesCreate(MDR_ABI_VERSION - 1u, &transport.connection, MDR_PROTOCOL_V2, &headphones),
                 MDR_RESULT_ERROR_ABI_MISMATCH, "the previous ABI version is refused");
    check(headphones == NULL, "an older ABI handshake leaves no instance behind");

    check_result(mdrHeadphonesCreate(0u, &transport.connection, MDR_PROTOCOL_V2, &headphones),
                 MDR_RESULT_ERROR_ABI_MISMATCH, "an unversioned caller is refused");

    check_result(mdrHeadphonesCreate(MDR_ABI_VERSION, &transport.connection, MDR_PROTOCOL_V2, &headphones),
                 MDR_RESULT_OK, "the matching version is accepted");
    check(headphones != NULL, "an accepted handshake yields an instance");
    mdrHeadphonesDestroy(headphones);

    headphones = NULL;
    check_result(mdrHeadphonesCreate(MDR_ABI_VERSION, &transport.connection, MDR_PROTOCOL_V1, &headphones),
                 MDR_RESULT_OK, "the matching version accepts a V1 implementation");
    check(headphones != NULL, "a V1 handshake yields an opaque instance");
    mdrHeadphonesDestroy(headphones);
}

static void test_struct_and_buffer_contracts(void)
{
    Session session;
    uint32_t text_size;
    char short_text[1];
    uint32_t short_text_size;
    char* text;
    uint32_t copied_text_size;

    if (!session_open(&session))
        return;

    text_size = 1;
    check_result(mdrHeadphonesGetText(session.headphones, MDR_TEXT_LAST_ERROR, 0, NULL, &text_size),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "text size query requires a zero input size");
    text_size = 0;
    check_result(mdrHeadphonesGetText(session.headphones, MDR_TEXT_LAST_ERROR, 0, NULL, &text_size), MDR_RESULT_OK,
                 "text size query succeeds");
    check(text_size > 1, "text size includes a NUL terminator");

    short_text_size = (uint32_t)sizeof(short_text);
    check_result(mdrHeadphonesGetText(session.headphones, MDR_TEXT_LAST_ERROR, 0, short_text, &short_text_size),
                 MDR_RESULT_ERROR_BUFFER_TOO_SMALL, "text copy reports a short caller buffer");
    check(short_text_size == text_size, "text copy returns the required size");

    text = (char*)malloc(text_size);
    copied_text_size = text_size;
    check(text != NULL, "text test allocation succeeds");
    if (text != NULL)
    {
        check_result(mdrHeadphonesGetText(session.headphones, MDR_TEXT_LAST_ERROR, 0, text, &copied_text_size),
                     MDR_RESULT_OK, "text copy succeeds on the second call");
        check(copied_text_size == text_size && text[text_size - 1] == '\0', "text copy is NUL terminated");
        free(text);
    }

    session_close(&session);
}

static void test_one_operation_at_a_time(MDRProtocolVersion family)
{
    Session session;
    const MDRPlaybackCommand command = {MDR_PLAYBACK_PLAY};
    const MDRPower power = {0};

    if (!session_open_family(&session, family))
        return;
    check_result(mdrHeadphonesRequestInit(session.headphones, MDR_HEADPHONES_INIT_DEFAULT), MDR_RESULT_OK,
                 "initialization starts");
    check_result(mdrHeadphonesRequestSync(session.headphones, MDR_HEADPHONES_SYNC_DEFAULT), MDR_RESULT_INPROGRESS,
                 "a second operation is rejected while initialization is active");
    check_result(mdrHeadphonesRequestInit(session.headphones, MDR_HEADPHONES_INIT_DEFAULT), MDR_RESULT_INPROGRESS,
                 "initialization cannot overlap");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_INPROGRESS,
                 "commit cannot interrupt initialization");
    check_result(mdrDebugRequestCommand(session.headphones, NULL, 0, MDR_DATA_TYPE_DATA_MDR, MDR_FALSE),
                 MDR_RESULT_INPROGRESS, "debug command cannot interrupt initialization");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 10), MDR_RESULT_INPROGRESS,
                 "volume cannot stage during initial bootstrap");
    check_result(mdrHeadphonesSendPlaybackCommand(session.headphones, &command), MDR_RESULT_INPROGRESS,
                 "playback command cannot stage during initial bootstrap");
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_INPROGRESS,
                 "power cannot stage during initial bootstrap");
    check(!mdrHeadphonesIsDirty(session.headphones), "rejected bootstrap setters leave no edits");
    check(mdrHeadphonesIsReady(session.headphones) == MDR_FALSE, "status reports the active operation as busy");
    session_close(&session);
}

static void test_committed_state_staging(void)
{
    Session session;
    MDRPlayback current;

    if (!session_open(&session))
        return;
    check_result(mdrHeadphonesSetPlaybackVolume(NULL, 12), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "playback volume rejects a null session");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 31), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "playback volume rejects values above 30");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, UINT8_MAX), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "playback volume rejects the maximum byte value");
    check(mdrHeadphonesIsDirty(session.headphones) == MDR_FALSE, "invalid volume writes leave the session clean");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 0), MDR_RESULT_OK,
                 "minimum playback volume is accepted");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 30), MDR_RESULT_OK,
                 "maximum playback volume is accepted");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 12), MDR_RESULT_OK, "playback volume stages");

    memset(&current, 0, sizeof(current));
    check_result(mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_CURRENT, &current), MDR_RESULT_OK,
                 "committed playback is readable");
    check(current.volume == 0, "staging does not alter current playback");

    check(mdrHeadphonesIsDirty(session.headphones) == MDR_TRUE, "staging marks the session dirty");
    session_close(&session);
}

static void test_playback_actions(void)
{
    Session session;
    const MDRPlaybackAction actions[] = {MDR_PLAYBACK_PLAY, MDR_PLAYBACK_PAUSE, MDR_PLAYBACK_NEXT,
                                         MDR_PLAYBACK_PREVIOUS};
    size_t index;
    MDRPlaybackCommand command;
    unsigned char ack[FRAME_BUFFER_CAPACITY];
    size_t ack_size;
    MDREvent event;

    if (!session_open(&session))
        return;
    select_v2(&session, "V2 protocol is selected for playback actions");
    check_result(mdrHeadphonesSendPlaybackCommand(session.headphones, NULL), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "playback command rejects a null command");
    memset(&command, 0, sizeof(command));
    command.action = MDR_PLAYBACK_PLAY;
    check_result(mdrHeadphonesSendPlaybackCommand(NULL, &command), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "playback command rejects a null session");

    for (index = 0; index < sizeof(actions) / sizeof(actions[0]); ++index)
    {
        memset(&command, 0, sizeof(command));
        command.action = actions[index];
        check_result(mdrHeadphonesSendPlaybackCommand(session.headphones, &command), MDR_RESULT_OK,
                     "supported playback action stages");

        check(mdrHeadphonesIsDirty(session.headphones) == MDR_TRUE, "playback action is pending");

        check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "playback action apply starts");
        poll_event(session.headphones, &event, "playback action request flushes");
        ack_size = pack_ack(&session.transport, ack);
        mock_load(&session.transport, ack, ack_size);
        poll_event(session.headphones, &event, "playback action ACK polls");
        poll_event(session.headphones, &event, "playback action completion polls");
        check(event == MDR_EVENT_APPLY_COMPLETE, "playback action apply completes");

        check(mdrHeadphonesIsReady(session.headphones) == MDR_TRUE &&
                  mdrHeadphonesIsDirty(session.headphones) == MDR_FALSE,
              "playback action is consumed as a one-shot");
    }

    memset(&command, 0, sizeof(command));
    command.action = (MDRPlaybackAction)0xff;
    check_result(mdrHeadphonesSendPlaybackCommand(session.headphones, &command), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "unknown playback action is rejected");

    check(mdrHeadphonesIsDirty(session.headphones) == MDR_FALSE, "invalid playback commands leave the session clean");
    session_close(&session);
}

/*
 * Devices interleave unsolicited notifications and late responses into the exchange.
 * None of that may influence the sequence number we transmit with: a frame repeating
 * the sequence number of an already acknowledged one is dropped by the device as a
 * duplicate, and the request is then answered by silence.
 */
static void test_transmit_sequence_ignores_inbound_frames(void)
{
    Session session;
    const unsigned char notification[] = {0xfe};
    unsigned char frame[FRAME_BUFFER_CAPACITY];
    unsigned char ack[FRAME_BUFFER_CAPACITY];
    size_t frame_size;
    size_t ack_size;
    unsigned char first_sequence = 0;
    unsigned char second_sequence = 0;
    MDRPlaybackCommand command;
    MDREvent event;

    if (!session_open(&session))
        return;
    select_v2(&session, "V2 protocol is selected for sequence tracking");

    memset(&command, 0, sizeof(command));
    command.action = MDR_PLAYBACK_PLAY;
    check_result(mdrHeadphonesSendPlaybackCommand(session.headphones, &command), MDR_RESULT_OK, "first action stages");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "first apply starts");
    poll_event(session.headphones, &event, "first request flushes");
    check(last_tx_sequence(&session.transport, &first_sequence), "first request was transmitted");

    ack_size = pack_ack(&session.transport, ack);
    mock_load(&session.transport, ack, ack_size);
    poll_event(session.headphones, &event, "first apply ACK polls");
    poll_event(session.headphones, &event, "first apply completion polls");
    check(event == MDR_EVENT_APPLY_COMPLETE, "first apply completes");

    /*
     * A frame arriving after the acknowledgement - a lagging response, or a notification
     * the device pushed on its own - carrying the sequence number the acknowledged frame
     * used. Adopting it would make the next request look like a retransmission.
     */
    frame_size = pack_data_frame(notification, sizeof(notification), first_sequence, frame);
    mock_load(&session.transport, frame, frame_size);
    poll_event(session.headphones, &event, "late inbound frame polls");
    check(event == MDR_EVENT_UNHANDLED, "late inbound frame is reported, not fatal");

    memset(&command, 0, sizeof(command));
    command.action = MDR_PLAYBACK_NEXT;
    check_result(mdrHeadphonesSendPlaybackCommand(session.headphones, &command), MDR_RESULT_OK, "second action stages");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "second apply starts");
    poll_event(session.headphones, &event, "second request flushes");
    check(last_tx_sequence(&session.transport, &second_sequence), "second request was transmitted");
    check(second_sequence != first_sequence,
          "the next request does not repeat the sequence number of an acknowledged frame");
    session_close(&session);
}

/*
 * A device that understands the framing and acknowledges everything, but answers only the
 * two requests initialization cannot proceed without. Every other request is accepted and
 * dropped, which is what a real device does with a command it does not implement - so what
 * matters is that libmdr never asks for anything the advertised function list rules out.
 */
enum
{
    MDR_DATA_TYPE_DATA_MDR_NO2 = 14,
    REQUEST_LOG_CAPACITY = 128
};

typedef struct RequestLog
{
    unsigned char table;
    unsigned char command;
    unsigned char inquired;
    int has_inquired;
    /* The byte after the inquired type. On a playback request it says which detail was
     * asked for - the track name, the album, the artist - which is the whole of what a
     * sync has to get right. */
    unsigned char detail;
    int has_detail;
    size_t payload_size;
} RequestLog;

typedef struct Device
{
    MockTransport* transport;
    size_t tx_cursor;
    unsigned char sequence;
    const unsigned char* table1;
    size_t table1_size;
    const unsigned char* table2;
    size_t table2_size;
    /* When set, every EQEBB_SET_PARAM is answered the way a real device answers a preset
     * change: with the band steps it computed for that preset. */
    const unsigned char* eq_notification;
    size_t eq_notification_size;
    /* When set, the device answers a status request the way one that has just switched a
     * listening mode on does: with both the equalizer and the upscaling turned off. */
    int answer_status_disabled;
    /* When set, the handshake is answered the way a V1 headset answers it, and the device
     * replies to voice-guidance requests. Pair it with session_open_family(MDR_PROTOCOL_V1). */
    int protocol_v1;
    const unsigned char* assignable_capability;
    size_t assignable_capability_size;
    const unsigned char* assignable_params;
    size_t assignable_params_size;
    /* Listening modes the device refuses, as bit (1 << inquired type). Every listening status
     * request is answered, the way a real device answers it. */
    unsigned int listening_refused;
    /* Error codes a refused BGM_MODE_AND_ERRORCODE status carries. */
    const unsigned char* bgm_errors;
    size_t bgm_errors_size;
    /* Which listening modes are on, as bit (1 << inquired type), and the background music room.
     * AUDIO_SET_PARAM updates them and AUDIO_GET_PARAM reads them back. */
    unsigned int listening_on;
    unsigned char bgm_room;
    /* When set, AUDIO_SET_PARAM is followed by the AUDIO_NTFY_PARAM a device sends. */
    int notify_listening;
    /* When set, the device answers voice assistant requests with these. */
    const unsigned char* va_capability;
    size_t va_capability_size;
    unsigned char va_status;
    unsigned char va_current;
    /* When set, a reply goes out ahead of the ACK for the request it answers, the way a
     * WH-1000XM3 orders them. */
    int reply_before_ack;
    int block_ack;
    unsigned char held_seq;
    RequestLog log[REQUEST_LOG_CAPACITY];
    size_t log_size;
} Device;

static void device_send(Device* device, unsigned char type, const unsigned char* payload, size_t payload_size)
{
    unsigned char frame[FRAME_BUFFER_CAPACITY];
    const size_t size = pack_frame(type, device->sequence, payload, payload_size, frame);

    device->sequence = (unsigned char)(1 - device->sequence);
    mock_append(device->transport, frame, size);
}

static void device_ack(Device* device, unsigned char sequence)
{
    unsigned char frame[FRAME_BUFFER_CAPACITY];
    const size_t size = pack_frame(MDR_DATA_TYPE_ACK, (unsigned char)(1 - sequence), NULL, 0, frame);
    mock_append(device->transport, frame, size);
}

/* BGM_MODE, UPMIX_CINEMA, VOICE_CONTENTS, SOUND_LEAKAGE_REDUCTION, BGM_MODE_AND_ERRORCODE */
static int is_listening_type(unsigned char type)
{
    return type == 0x03 || type == 0x04 || type == 0x06 || type == 0x07 || type == 0x09;
}

/* Acknowledge and, where the handshake requires it, answer everything transmitted so far. */
static void device_pump(Device* device)
{
    TxFrame frame;

    while (next_tx_frame(device->transport, &device->tx_cursor, &frame))
    {
        unsigned char ack[FRAME_BUFFER_CAPACITY];
        size_t ack_size;
        unsigned char table;

        if (frame.type == MDR_DATA_TYPE_ACK || frame.payload_size == 0)
            continue;
        table = (unsigned char)(frame.type == MDR_DATA_TYPE_DATA_MDR_NO2 ? 2 : 1);

        if (device->log_size < REQUEST_LOG_CAPACITY)
        {
            RequestLog* entry = &device->log[device->log_size++];
            entry->table = table;
            entry->command = frame.payload[0];
            entry->has_inquired = frame.payload_size > 1;
            entry->inquired = entry->has_inquired ? frame.payload[1] : 0;
            entry->has_detail = frame.payload_size > 2;
            entry->detail = entry->has_detail ? frame.payload[2] : 0;
            entry->payload_size = frame.payload_size;
        }

        if (device->block_ack)
        {
            device->held_seq = frame.sequence;
            continue;
        }
        ack_size = pack_frame(MDR_DATA_TYPE_ACK, (unsigned char)(1 - frame.sequence), NULL, 0, ack);
        if (!device->reply_before_ack)
            mock_append(device->transport, ack, ack_size);

        if (table == 1 && frame.payload[0] == 0x00) /* CONNECT_GET_PROTOCOL_INFO */
        {
            if (device->protocol_v1)
                device_send(device, MDR_DATA_TYPE_DATA_MDR, k_v1_protocol_info, sizeof(k_v1_protocol_info));
            else
                device_send(device, MDR_DATA_TYPE_DATA_MDR, k_v2_protocol_info_both_tables,
                            sizeof(k_v2_protocol_info_both_tables));
        }
        /*
         * Voice guidance, answered the way a headset answers it: about the detail that was
         * asked for. Initialization asks about the switch, the language, the required time
         * and the download server in turn, so most of these replies are not the switch -
         * which is exactly the case that used to be read as one.
         */
        else if (device->protocol_v1 && table == 2 && frame.payload_size > 2 &&
                 (frame.payload[0] == 0x46 || frame.payload[0] == 0x42))
        {
            const unsigned char reply[4] = {
                (unsigned char)(frame.payload[0] + 1u), /* GET_PARAM -> RET_PARAM, likewise status */
                frame.payload[1], frame.payload[2], 0x00};
            device_send(device, MDR_DATA_TYPE_DATA_MDR_NO2, reply, sizeof(reply));
        }
        else if (frame.payload[0] == 0x06) /* CONNECT_GET_SUPPORT_FUNCTION */
        {
            if (table == 1)
                device_send(device, MDR_DATA_TYPE_DATA_MDR, device->table1, device->table1_size);
            else
                device_send(device, MDR_DATA_TYPE_DATA_MDR_NO2, device->table2, device->table2_size);
        }
        else if (table == 1 && frame.payload[0] == 0x52 && device->answer_status_disabled)
        {
            /* EQEBB_GET_STATUS -> EQEBB_RET_STATUS PRESET_EQ DISABLE */
            static const unsigned char answer[] = {0x53, 0x00, 0x01};
            device_send(device, MDR_DATA_TYPE_DATA_MDR, answer, sizeof(answer));
        }
        else if (table == 1 && frame.payload[0] == 0xe2 && frame.payload_size > 1 && frame.payload[1] == 0x01 &&
                 device->answer_status_disabled)
        {
            /* AUDIO_GET_STATUS UPSCALING -> AUDIO_RET_STATUS UPSCALING DISABLE */
            static const unsigned char answer[] = {0xe3, 0x01, 0x01};
            device_send(device, MDR_DATA_TYPE_DATA_MDR, answer, sizeof(answer));
        }
        else if (table == 1 && frame.payload[0] == 0xe2 && frame.payload_size > 1 &&
                 is_listening_type(frame.payload[1]))
        {
            /* AUDIO_GET_STATUS for a listening mode -> AUDIO_RET_STATUS. 0x00 enables. */
            unsigned char answer[32];
            const int refused = (device->listening_refused & (1u << frame.payload[1])) != 0;
            size_t size = 0;
            answer[size++] = 0xe3;
            answer[size++] = frame.payload[1];
            answer[size++] = (unsigned char)(refused ? 0x01 : 0x00);
            if (frame.payload[1] == 0x09)
            {
                const size_t count = refused ? device->bgm_errors_size : 0;
                answer[size++] = (unsigned char)count;
                if (count)
                    memcpy(answer + size, device->bgm_errors, count), size += count;
            }
            device_send(device, MDR_DATA_TYPE_DATA_MDR, answer, size);
        }
        else if (table == 1 && (frame.payload[0] == 0xe6 || frame.payload[0] == 0xe8) && frame.payload_size > 1 &&
                 is_listening_type(frame.payload[1]))
        {
            /* AUDIO_SET_PARAM updates the mode, AUDIO_GET_PARAM reads it: {command, type, on/off[, room]} */
            const unsigned char type = frame.payload[1];
            const int bgm = type == 0x03 || type == 0x09;
            unsigned char answer[4];
            if (frame.payload[0] == 0xe8 && frame.payload_size > 2)
            {
                if (frame.payload[2] == 0x00)
                    device->listening_on |= 1u << type;
                else
                    device->listening_on &= ~(1u << type);
                if (bgm && frame.payload_size > 3)
                    device->bgm_room = frame.payload[3];
            }
            answer[0] = (unsigned char)(frame.payload[0] == 0xe6 ? 0xe7 : 0xe9);
            answer[1] = type;
            answer[2] = (unsigned char)((device->listening_on & (1u << type)) ? 0x00 : 0x01);
            answer[3] = device->bgm_room;
            if (frame.payload[0] == 0xe6 || device->notify_listening)
                device_send(device, MDR_DATA_TYPE_DATA_MDR, answer, bgm ? 4u : 3u);
        }
        else if (table == 1 && device->va_capability != NULL && frame.payload_size > 1 && frame.payload[1] == 0x04 &&
                 (frame.payload[0] == 0xf0 || frame.payload[0] == 0xf2 || frame.payload[0] == 0xf6 ||
                  frame.payload[0] == 0xf8))
        {
            /* VOICE_ASSISTANT_SETTINGS: capability, status, param, and the notification a set brings */
            unsigned char answer[3] = {(unsigned char)(frame.payload[0] + 1u), 0x04, device->va_status};
            if (frame.payload[0] == 0xf0)
                device_send(device, MDR_DATA_TYPE_DATA_MDR, device->va_capability, device->va_capability_size);
            else if (frame.payload[0] == 0xf2)
                device_send(device, MDR_DATA_TYPE_DATA_MDR, answer, sizeof(answer));
            else
            {
                if (frame.payload[0] == 0xf8 && frame.payload_size > 2)
                    device->va_current = frame.payload[2];
                answer[2] = device->va_current;
                device_send(device, MDR_DATA_TYPE_DATA_MDR, answer, sizeof(answer));
            }
        }
        else if (table == 1 && frame.payload[0] == 0x58 && device->eq_notification != NULL)
        {
            /* EQEBB_SET_PARAM */
            device_send(device, MDR_DATA_TYPE_DATA_MDR, device->eq_notification, device->eq_notification_size);
        }
        else if (table == 1 && frame.payload[0] == 0xf0 && device->assignable_capability != NULL)
        {
            device_send(device, MDR_DATA_TYPE_DATA_MDR, device->assignable_capability,
                        device->assignable_capability_size);
        }
        else if (table == 1 && frame.payload[0] == 0xf6 && device->assignable_params != NULL)
        {
            device_send(device, MDR_DATA_TYPE_DATA_MDR, device->assignable_params, device->assignable_params_size);
        }

        if (device->reply_before_ack)
            mock_append(device->transport, ack, ack_size);
    }
}


/* Drives the exchange until `expected` arrives, or reports that it never did. */
static void device_run(Session* session, Device* device, MDREvent expected, const char* message)
{
    int iteration;

    for (iteration = 0; iteration < 4096; ++iteration)
    {
        MDREvent event = MDR_EVENT_NONE;
        if (mdrHeadphonesPoll(session->headphones, &event) != MDR_RESULT_OK)
        {
            check(0, message);
            return;
        }
        device_pump(device);
        if (event == expected)
            return;
    }
    check(0, message);
}

/* Runs selected initialization flags to completion against `device`, or reports why they could not. */
static void device_run_init_flags(Session* session, Device* device, MDRHeadphonesInitFlags flags)
{
    check_result(mdrHeadphonesRequestInit(session->headphones, flags), MDR_RESULT_OK, "initialization starts");
    device_run(session, device, MDR_EVENT_INITIALIZE_COMPLETE, "initialization completes");
}

static void device_run_init(Session* session, Device* device)
{
    device_run_init_flags(session, device, MDR_HEADPHONES_INIT_DEFAULT);
}

static int device_requested(const Device* device, unsigned char table, unsigned char command, int inquired);

/* Position of a request in the order it was transmitted, or -1 if it was never sent. */
static int device_request_order(const Device* device, unsigned char table, unsigned char command, int inquired)
{
    size_t index;

    for (index = 0; index < device->log_size; ++index)
    {
        const RequestLog* entry = &device->log[index];
        if (entry->table != table || entry->command != command)
            continue;
        if (inquired < 0 || (entry->has_inquired && entry->inquired == (unsigned char)inquired))
            return (int)index;
    }
    return -1;
}

static int device_requested(const Device* device, unsigned char table, unsigned char command, int inquired)
{
    return device_request_order(device, table, command, inquired) >= 0;
}

static int selective_init_command_allowed(unsigned char table, unsigned char command)
{
    if (table == 1)
    {
        switch (command)
        {
        case 0x00: /* CONNECT_GET_PROTOCOL_INFO */
        case 0x02: /* CONNECT_GET_CAPABILITY_INFO */
        case 0x04: /* CONNECT_GET_DEVICE_INFO */
        case 0x06: /* CONNECT_GET_SUPPORT_FUNCTION */
        case 0x50: /* EQEBB_GET_CAPABILITY */
        case 0x52: /* EQEBB_GET_STATUS */
        case 0x56: /* EQEBB_GET_PARAM */
        case 0x5a: /* EQEBB_GET_EXTENDED_INFO */
        case 0xd0: /* GENERAL_SETTING_GET_CAPABILITY */
        case 0xd6: /* GENERAL_SETTING_GET_PARAM */
        case 0xe0: /* AUDIO_GET_CAPABILITY */
        case 0xe2: /* AUDIO_GET_STATUS */
        case 0xe6: /* AUDIO_GET_PARAM */
            return 1;
        default:
            return 0;
        }
    }
    return command == 0x06 || command == 0x32 || command == 0x36;
}

static void test_v2_selective_init(void)
{
    static const unsigned char table1[] = {
        0x07, 0x00, 0x03, 0x50, 0xff, /* PRESET_EQ */
        0xe2, 0xff, /* UPSCALING_AUTO_OFF */
        0xd1, 0xff /* GENERAL_SETTING_1 */
    };
    static const unsigned char table2[] = {
        0x07, 0x00, 0x02, 0x30, 0xff, /* PAIRING_DEVICE_MANAGEMENT_CLASSIC_BT */
        0x31, 0xff /* SOURCE_SWITCH_CONTROL */
    };
    const MDRHeadphonesInitFlags selected_reads = MDR_HEADPHONES_INIT_EQUALIZER | MDR_HEADPHONES_INIT_DSEE |
        MDR_HEADPHONES_INIT_GENERAL_SETTINGS | MDR_HEADPHONES_INIT_PERIPHERALS;
    Session session;
    Device device;
    size_t index;
    size_t repeated_log_size;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);

    check_result(mdrHeadphonesRequestInit(session.headphones, MDR_HEADPHONES_INIT_NONE),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "empty initialization mask is rejected");
    check_result(mdrHeadphonesRequestInit(session.headphones, MDR_HEADPHONES_INIT_SUPPORT_FUNCTIONS),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "support discovery requires identity");
    check_result(
        mdrHeadphonesRequestInit(session.headphones, MDR_HEADPHONES_INIT_IDENTITY | MDR_HEADPHONES_INIT_EQUALIZER),
        MDR_RESULT_ERROR_INVALID_ARGUMENT, "feature reads require support discovery");
    check_result(mdrHeadphonesRequestInit(session.headphones, (MDRHeadphonesInitFlags)(1u << 31u)),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "unknown initialization flags are rejected");

    device_run_init_flags(&session, &device, MDR_HEADPHONES_INIT_IDENTITY);
    check(mdrHeadphonesIsInitialized(session.headphones) == MDR_FALSE,
          "identity alone does not complete baseline initialization");

    device_run_init_flags(&session, &device, MDR_HEADPHONES_INIT_SUPPORT_FUNCTIONS);
    check(mdrHeadphonesIsInitialized(session.headphones) == MDR_TRUE,
          "identity and support discovery complete baseline initialization");

    device_run_init_flags(&session, &device, selected_reads);
    for (index = 0; index < device.log_size; ++index)
    {
        const RequestLog* request = &device.log[index];
        check(selective_init_command_allowed(request->table, request->command),
              "selective initialization transmits only explicitly selected GET commands");
        check(request->command != 0x94 && request->command != 0xc4,
              "selective initialization does not enroll in notifications");
        check(request->command != 0x12 && request->command != 0x16,
              "selective initialization does not query noise control");
    }
    check(device_requested(&device, 1, 0x50, 0x00), "selective initialization reads EQ capability");
    check(device_requested(&device, 1, 0xe0, 0x01), "selective initialization reads DSEE capability");
    check(device_requested(&device, 1, 0xd0, 0xd1), "selective initialization reads general capability");
    check(device_requested(&device, 2, 0x32, 0x00), "selective initialization reads peripheral status");
    check(device_requested(&device, 2, 0x36, 0x01), "selective initialization reads source-switch state");

    repeated_log_size = device.log_size;
    device_run_init_flags(&session, &device, selected_reads);
    check(device.log_size == repeated_log_size, "completed initialization flags are idempotent");
    session_close(&session);
}

/*
 * Requests for functions the device does not advertise are worse than useless: the device
 * acknowledges and ignores them, so the state never arrives, and a device that ignores
 * unknown commands outright would stall initialization until the retry budget runs out.
 */
static void test_init_skips_unadvertised_functions(void)
{
    /* POWER_OFF and LR_BATTERY_LEVEL_WITH_THRESHOLD only. */
    static const unsigned char table1[] = {0x07, 0x00, 0x02, 0x23, 0xff, 0x29, 0xff};
    /* SAFE_LISTENING_TWS_1 only - table 2 is present, but carries no voice guidance. */
    static const unsigned char table2[] = {0x07, 0x00, 0x01, 0x51, 0xff};

    Session session;
    Device device;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);

    device_run_init(&session, &device);

    check(!device_requested(&device, 1, 0x50, -1), "no EQEBB_GET_CAPABILITY without an equalizer");
    check(!device_requested(&device, 1, 0x52, -1), "no EQEBB_GET_STATUS without an equalizer");
    check(!device_requested(&device, 1, 0x56, -1), "no EQEBB_GET_PARAM without an equalizer");
    check(!device_requested(&device, 1, 0x5a, -1), "no EQEBB_GET_EXTENDED_INFO without an equalizer");
    check(!device_requested(&device, 1, 0xa6, -1), "no PLAY_GET_PARAM without a playback controller");
    check(!device_requested(&device, 1, 0xa2, -1), "no PLAY_GET_STATUS without a playback controller");
    check(!device_requested(&device, 1, 0xf6, 0x01), "no PLAYBACK_CONTROL_BY_WEARING without wearing control");
    check(!device_requested(&device, 1, 0xe6, 0x09), "no BGM_MODE without a listening option");
    check(!device_requested(&device, 1, 0xe6, 0x04), "no UPMIX_CINEMA without a listening option");
    check(!device_requested(&device, 2, 0x46, -1), "no VOICE_GUIDANCE_GET_PARAM on a table 2 device without it");
    session_close(&session);
}

/*
 * The mirror image, plus the case that motivated splitting the listening-mode gate:
 * LISTENING_OPTION with only the background-music half implemented.
 */
static void test_init_requests_advertised_functions(void)
{
    static const unsigned char table1[] = {
        0x07, 0x00, 0x06, 0x50, 0xff, /* PRESET_EQ */
        0xa1, 0xff, /* PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT */
        0xf1, 0xff, /* PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF */
        0xe6, 0xff, /* LISTENING_OPTION */
        0xeb, 0xff, /* BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE - but no UPMIX_CINEMA */
        0x23, 0xff /* POWER_OFF */
    };
    static const unsigned char table2[] = {
        0x07, 0x00, 0x01, 0x42, 0xff /* VOICE_GUIDANCE_..._SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT */
    };

    Session session;
    Device device;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);

    device_run_init(&session, &device);

    check(device_requested(&device, 1, 0x50, 0x00), "EQEBB_GET_CAPABILITY for an advertised equalizer");
    check(device_requested(&device, 1, 0x52, -1), "EQEBB_GET_STATUS for an advertised equalizer");
    check(device_requested(&device, 1, 0x5a, 0x00), "EQEBB_GET_EXTENDED_INFO PRESET_EQ for an advertised equalizer");
    check(device_requested(&device, 1, 0xa6, -1), "PLAY_GET_PARAM for an advertised playback controller");
    check(device_requested(&device, 1, 0xa2, -1), "PLAY_GET_STATUS for an advertised playback controller");
    check(device_requested(&device, 1, 0xf6, 0x01), "PLAYBACK_CONTROL_BY_WEARING for advertised wearing control");
    check(device_requested(&device, 1, 0xe6, 0x09), "BGM_MODE for an advertised background-music mode");
    check(device_requested(&device, 2, 0x46, -1), "VOICE_GUIDANCE_GET_PARAM for advertised voice guidance");
    check(!device_requested(&device, 1, 0xe6, 0x04),
          "no UPMIX_CINEMA when only the background-music half of LISTENING_OPTION is advertised");
    session_close(&session);
}

/*
 * Listening modes are one exclusive setting made of independently advertised parts, so a
 * device offers whichever subset it implements - the WF-LC900 has background music, voice
 * boost and sound leakage reduction, but no cinema upmix.
 */
static void test_listening_modes(void)
{
    static const unsigned char table1[] = {
        0x07, 0x00, 0x04, 0xe6, 0xff, /* LISTENING_OPTION */
        0xeb, 0xff, /* BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE */
        0xe8, 0xff, /* VOICE_CONTENTS */
        0xe9, 0xff /* SOUND_LEAKAGE_REDUCTION - but no UPMIX_CINEMA */
    };
    static const unsigned char table2[] = {0x07, 0x00, 0x00};

    Session session;
    Device device;
    MDRListening listening;
    MDRFeatureAvailability availability;
    int off_order;
    int on_order;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);

    device_run_init(&session, &device);

    /* AUDIO_GET_PARAM for each advertised mode, and none for the absent one. */
    check(device_requested(&device, 1, 0xe6, 0x09), "background music is queried");
    check(device_requested(&device, 1, 0xe6, 0x06), "voice boost is queried");
    check(device_requested(&device, 1, 0xe6, 0x07), "sound leakage reduction is queried");
    check(!device_requested(&device, 1, 0xe6, 0x04), "cinema upmix is not queried");

    availability = MDR_AVAILABILITY_UNKNOWN;
    check_result(mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_LISTENING_VOICE_BOOST, &availability),
                 MDR_RESULT_OK, "voice boost availability is readable");
    check(availability == MDR_AVAILABILITY_AVAILABLE, "voice boost is available");
    availability = MDR_AVAILABILITY_UNKNOWN;
    check_result(mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_LISTENING_CINEMA, &availability),
                 MDR_RESULT_OK, "cinema availability is readable");
    check(availability == MDR_AVAILABILITY_UNAVAILABLE, "cinema is unavailable");

    /* A mode the device does not offer cannot be selected. */
    memset(&listening, 0, sizeof(listening));
    listening.mode = MDR_LISTENING_CINEMA;
    check_result(mdrHeadphonesSetListening(session.headphones, &listening), MDR_RESULT_ERROR_NOT_SUPPORTED,
                 "an unoffered mode is refused");

    memset(&listening, 0, sizeof(listening));
    listening.mode = MDR_LISTENING_BACKGROUND_MUSIC;
    listening.background_room = MDR_ROOM_MEDIUM;
    check_result(mdrHeadphonesSetListening(session.headphones, &listening), MDR_RESULT_OK, "background music stages");
    device.log_size = 0;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "background music apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "background music applies");

    /* Only the target changes (#76): nothing is switched off, as the device reports nothing on. */
    check(!device_requested(&device, 1, 0xe8, 0x06), "voice boost, already off, is not switched off");
    check(!device_requested(&device, 1, 0xe8, 0x07), "leakage reduction, already off, is not switched off");
    on_order = device_request_order(&device, 1, 0xe8, 0x09);
    check(on_order >= 0, "background music is switched on");
    check(device_request_order(&device, 1, 0xe2, 0x09) >= 0 && device_request_order(&device, 1, 0xe2, 0x09) < on_order,
          "its availability is re-read right before it is switched on");

    memset(&listening, 0, sizeof(listening));
    check_result(mdrHeadphonesGetListening(session.headphones, MDR_PROPERTY_CURRENT, &listening), MDR_RESULT_OK,
                 "device-reported listening mode is readable");
    check(listening.mode == MDR_LISTENING_STANDARD, "ACK does not synthesize a listening-mode report");
    check_result(mdrHeadphonesGetListening(session.headphones, MDR_PROPERTY_DESIRED, &listening), MDR_RESULT_OK,
                 "desired listening mode is readable");
    check(listening.mode == MDR_LISTENING_BACKGROUND_MUSIC, "background music remains the accepted target");

    /* The device reports it on, and from here reports every change the way it does. */
    {
        static const unsigned char bgm_on[] = {0xe9, 0x09, 0x00, 0x01};
        device_send(&device, MDR_DATA_TYPE_DATA_MDR, bgm_on, sizeof(bgm_on));
        device_run(&session, &device, MDR_EVENT_LISTENING_MODE_CHANGED, "background music being on polls");
        device.notify_listening = 1;
    }

    /*
     * Switching modes must deactivate the old one before activating the new one - a device
     * that refuses to hold two at once would reject the second command otherwise.
     */
    device.log_size = 0;
    memset(&listening, 0, sizeof(listening));
    listening.mode = MDR_LISTENING_VOICE_BOOST;
    check_result(mdrHeadphonesSetListening(session.headphones, &listening), MDR_RESULT_OK, "voice boost stages");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "voice boost apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "voice boost applies");

    off_order = device_request_order(&device, 1, 0xe8, 0x09); /* AUDIO_SET_PARAM, BGM */
    on_order = device_request_order(&device, 1, 0xe8, 0x06); /* AUDIO_SET_PARAM, voice contents */
    check(off_order >= 0, "the outgoing mode is switched off");
    check(on_order >= 0, "the incoming mode is switched on");
    check(off_order < on_order, "the outgoing mode is switched off before the incoming one is switched on");
    check(!device_requested(&device, 1, 0xe8, 0x07), "a mode that was never on is left alone");

    memset(&listening, 0, sizeof(listening));
    check_result(mdrHeadphonesGetListening(session.headphones, MDR_PROPERTY_DESIRED, &listening), MDR_RESULT_OK,
                 "desired listening mode is readable after the switch");
    check(listening.mode == MDR_LISTENING_VOICE_BOOST, "voice boost is the accepted target");
    check(mdrHeadphonesIsDirty(session.headphones) == MDR_FALSE, "the applied mode leaves nothing pending");

    /* Drain the reports, then reselect the active mode: nothing to send. */
    {
        int iteration;
        for (iteration = 0; iteration < 64; ++iteration)
        {
            MDREvent event = MDR_EVENT_NONE;
            mdrHeadphonesPoll(session.headphones, &event);
            device_pump(&device);
        }
    }
    mdrHeadphonesGetListening(session.headphones, MDR_PROPERTY_CURRENT, &listening);
    check(listening.mode == MDR_LISTENING_VOICE_BOOST, "the device reported voice boost on");
    device.log_size = 0;
    check_result(mdrHeadphonesSetListening(session.headphones, &listening), MDR_RESULT_OK, "voice boost restages");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "unchanged apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "unchanged apply completes");
    check(!device_requested(&device, 1, 0xe8, -1), "an unchanged mode sends nothing");

    /* Standard switches off only the active mode. */
    device.log_size = 0;
    memset(&listening, 0, sizeof(listening));
    listening.mode = MDR_LISTENING_STANDARD;
    check_result(mdrHeadphonesSetListening(session.headphones, &listening), MDR_RESULT_OK, "standard stages");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "standard apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "standard applies");
    check(device_requested(&device, 1, 0xe8, 0x06), "standard switches voice boost off");
    check(!device_requested(&device, 1, 0xe8, 0x09) && !device_requested(&device, 1, 0xe8, 0x07),
          "standard leaves the inactive modes alone");
    check(!device_requested(&device, 1, 0xe2, -1), "switching off needs no availability check");
    session_close(&session);
}

/*
 * #76: the WH-1000XM6 reboots when background music is switched on while voice control is selected.
 * The device reports the mode unavailable, and that report is the only guard. It has to hold when
 * staging and again when sending, since the report can change in between.
 */
static void test_listening_mode_refused_by_the_device(void)
{
    static const unsigned char table1[] = {
        0x07, 0x00, 0x03, 0xe6, 0xff, /* LISTENING_OPTION */
        0xeb, 0xff, /* BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE */
        0xe5, 0xff /* UPMIX_CINEMA */
    };
    static const unsigned char table2[] = {0x07, 0x00, 0x00};
    static const unsigned char voice_and_call[] = {0x06, 0x00}; /* VOICE_UI_SONY, CALLING */
    static const unsigned char bgm_enabled[] = {0xe5, 0x09, 0x00, 0x00};

    Session session;
    Device device;
    MDRListening listening;
    MDRListeningStatus status;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device.listening_refused = 1u << 0x09;
    device.bgm_errors = voice_and_call;
    device.bgm_errors_size = sizeof(voice_and_call);

    device_run_init(&session, &device);
    check(device_requested(&device, 1, 0xe0, 0x09), "background music capability is read with its own type");
    check(device_requested(&device, 1, 0xe2, 0x09), "background music availability is read");
    check(device_requested(&device, 1, 0xe2, 0x04), "cinema availability is read");
    {
        int iteration;
        for (iteration = 0; iteration < 64; ++iteration)
        {
            MDREvent event = MDR_EVENT_NONE;
            mdrHeadphonesPoll(session.headphones, &event);
            device_pump(&device);
        }
    }

    memset(&status, 0, sizeof(status));
    check_result(mdrHeadphonesGetListeningStatus(session.headphones, MDR_LISTENING_BACKGROUND_MUSIC, &status),
                 MDR_RESULT_OK, "background music status reads");
    check(status.available == MDR_FALSE, "background music reads unavailable");
    check(status.blockers == (MDR_LISTENING_BLOCKER_VOICE_ASSISTANT | MDR_LISTENING_BLOCKER_CALL),
          "the reported codes become blockers");
    check_result(mdrHeadphonesGetListeningStatus(session.headphones, MDR_LISTENING_STANDARD, &status), MDR_RESULT_OK,
                 "standard status reads");
    check(status.available == MDR_FALSE && status.blockers == MDR_LISTENING_BLOCKER_CALL,
          "standard is unavailable during a call, as in Sound Connect");
    check_result(mdrHeadphonesGetListeningStatus(session.headphones, MDR_LISTENING_CINEMA, &status), MDR_RESULT_OK,
                 "cinema status reads");
    check(status.available == MDR_TRUE && status.blockers == MDR_LISTENING_BLOCKER_NONE, "cinema stays available");
    check_result(mdrHeadphonesGetListeningStatus(session.headphones, MDR_LISTENING_VOICE_BOOST, &status),
                 MDR_RESULT_ERROR_NOT_SUPPORTED, "an unoffered mode has no status");

    memset(&listening, 0, sizeof(listening));
    listening.mode = MDR_LISTENING_BACKGROUND_MUSIC;
    listening.background_room = MDR_ROOM_SMALL;
    check_result(mdrHeadphonesSetListening(session.headphones, &listening), MDR_RESULT_ERROR_NOT_SUPPORTED,
                 "a refused mode does not stage");
    check(mdrHeadphonesIsDirty(session.headphones) == MDR_FALSE, "nothing is pending after a refusal");

    /* Available when staged, refused by the time it is sent. */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, bgm_enabled, sizeof(bgm_enabled));
    device_run(&session, &device, MDR_EVENT_LISTENING_MODE_CHANGED, "background music becoming available polls");
    check_result(mdrHeadphonesSetListening(session.headphones, &listening), MDR_RESULT_OK, "an available mode stages");
    device.log_size = 0;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "the refused apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "the refused apply completes");
    check(device_requested(&device, 1, 0xe2, 0x09), "availability is re-read before switching on");
    check(!device_requested(&device, 1, 0xe8, -1), "a mode refused at send time is not sent");
    memset(&listening, 0, sizeof(listening));
    mdrHeadphonesGetListening(session.headphones, MDR_PROPERTY_DESIRED, &listening);
    check(listening.mode == MDR_LISTENING_STANDARD, "the desired view falls back to what the device has");
    check(mdrHeadphonesIsDirty(session.headphones) == MDR_FALSE, "a refused switch leaves nothing pending");
    session_close(&session);
}

/* Without the error-code variant, background music is inquired as BGM_MODE (0x03), as in Sound Connect. */
static void test_background_music_without_error_codes(void)
{
    static const unsigned char table1[] = {
        0x07, 0x00, 0x02, 0xe6, 0xff, /* LISTENING_OPTION */
        0xe4, 0xff /* BGM_MODE_SMALL_MIDDLE_LARGE */
    };
    static const unsigned char table2[] = {0x07, 0x00, 0x00};
    static const unsigned char expected_on[] = {0xe8, 0x03, 0x00, 0x02};
    static const unsigned char disabled[] = {0xe5, 0x03, 0x01};

    Session session;
    Device device;
    MDRListening listening;
    MDRListeningStatus status;
    size_t tx_before;
    size_t offset;
    TxFrame frame;
    int found = 0;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);

    check(device_requested(&device, 1, 0xe6, 0x03), "the setting is read as BGM_MODE");
    check(device_requested(&device, 1, 0xe2, 0x03), "the availability is read as BGM_MODE");
    check(!device_requested(&device, 1, 0xe6, 0x09) && !device_requested(&device, 1, 0xe2, 0x09),
          "the error-code variant is never asked of a device without it");

    memset(&listening, 0, sizeof(listening));
    listening.mode = MDR_LISTENING_BACKGROUND_MUSIC;
    listening.background_room = MDR_ROOM_LARGE;
    check_result(mdrHeadphonesSetListening(session.headphones, &listening), MDR_RESULT_OK, "background music stages");
    tx_before = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "background music apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "background music applies");
    offset = tx_before;
    while (next_tx_frame(&session.transport, &offset, &frame))
        if (frame.payload_size == sizeof(expected_on) && memcmp(frame.payload, expected_on, sizeof(expected_on)) == 0)
            found = 1;
    check(found, "background music is switched on as BGM_MODE, Large");

    /* No error codes: unavailable, with no reason given. */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, disabled, sizeof(disabled));
    device_run(&session, &device, MDR_EVENT_LISTENING_MODE_CHANGED, "background music being disabled polls");
    check_result(mdrHeadphonesGetListeningStatus(session.headphones, MDR_LISTENING_BACKGROUND_MUSIC, &status),
                 MDR_RESULT_OK, "background music status reads");
    check(status.available == MDR_FALSE && status.blockers == MDR_LISTENING_BLOCKER_NONE,
          "a plain status disables without a reason");
    session_close(&session);
}

/*
 * Voice assistant settings, the way the WH-1000XM6 (1.0.5) answers them: a touch panel offering
 * voice control, the phone's assistant, Google Assistant and none, set to none.
 */
static void test_voice_assistant(void)
{
    static const unsigned char table1[] = {
        0x07, 0x00, 0x04, 0xf4, 0x20, /* VOICE_ASSISTANT_SETTINGS */
        0xe6, 0x0e, /* LISTENING_OPTION */
        0xe4, 0x0f, /* BGM_MODE_SMALL_MIDDLE_LARGE */
        0xe5, 0x10 /* UPMIX_CINEMA */
    };
    static const unsigned char table2[] = {0x07, 0x00, 0x00};
    static const unsigned char capability[] = {0xf1, 0x04, 0x01, 0x04, 0x34, 0x30, 0x31, 0xff};
    static const unsigned char expected_set[] = {0xf8, 0x04, 0x34};
    static const unsigned char bgm_disabled[] = {0xe5, 0x03, 0x01};
    static const unsigned char wake_word[] = {0xf5, 0x05, 0x02}; /* Sent, but never advertised */
    static const unsigned char va_locked[] = {0xf5, 0x04, 0x01};
    static const unsigned char va_unlocked[] = {0xf5, 0x04, 0x00};
    /* Cinema excludes Google Assistant, as the XM6 reports */
    static const unsigned char cinema_capability[] = {0xe1, 0x04, 0x03, 0x01, 0x00, 0x06};
    static const unsigned char cinema_on[] = {0xe9, 0x04, 0x00};
    static const unsigned char bgm_on[] = {0xe9, 0x03, 0x00, 0x02};
    static const unsigned char bgm_off[] = {0xe9, 0x03, 0x01, 0x02};

    Session session;
    Device device;
    MDRVoiceAssistantSettings settings;
    MDRVoiceAssistantOption options[8];
    MDRFeatureAvailability availability = MDR_AVAILABILITY_UNKNOWN;
    MDRListening listening;
    uint32_t count;
    size_t tx_before;
    size_t offset;
    TxFrame frame;
    int found = 0;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device.va_capability = capability;
    device.va_capability_size = sizeof(capability);
    device.va_status = 0x00;
    device.va_current = 0xff;

    check_result(mdrHeadphonesGetVoiceAssistantOptions(session.headphones, NULL, &(uint32_t){0}),
                 MDR_RESULT_ERROR_NOT_SUPPORTED, "options need the support list");
    device_run_init(&session, &device);
    {
        int iteration;
        for (iteration = 0; iteration < 64; ++iteration)
        {
            MDREvent event = MDR_EVENT_NONE;
            mdrHeadphonesPoll(session.headphones, &event);
            device_pump(&device);
        }
    }
    check(device_requested(&device, 1, 0xf0, 0x04), "the capability is read");
    check(device_requested(&device, 1, 0xf2, 0x04), "the status is read");
    check(device_requested(&device, 1, 0xf6, 0x04), "the setting is read");
    check(mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_VOICE_ASSISTANT, &availability) == MDR_RESULT_OK &&
              availability == MDR_AVAILABILITY_AVAILABLE,
          "voice assistant is a feature");

    memset(&settings, 0, sizeof(settings));
    check_result(mdrHeadphonesGetVoiceAssistant(session.headphones, MDR_PROPERTY_CURRENT, &settings), MDR_RESULT_OK,
                 "settings read");
    check(settings.assistant == MDR_VOICE_ASSISTANT_NONE && settings.available == MDR_TRUE &&
              settings.key == MDR_VOICE_ASSISTANT_KEY_TOUCH_PANEL,
          "settings are what the device reported");

    count = 0;
    check_result(mdrHeadphonesGetVoiceAssistantOptions(session.headphones, NULL, &count), MDR_RESULT_OK,
                 "option count reads");
    check_result((MDRResult)count, 4, "every advertised option is counted, none last");
    count = 2;
    check_result(mdrHeadphonesGetVoiceAssistantOptions(session.headphones, options, &count),
                 MDR_RESULT_ERROR_BUFFER_TOO_SMALL, "a short option buffer is refused");
    count = 8;
    check_result(mdrHeadphonesGetVoiceAssistantOptions(session.headphones, options, &count), MDR_RESULT_OK,
                 "options read");
    check(count == 4 && options[0].assistant == MDR_VOICE_ASSISTANT_SONY_VOICE_CONTROL &&
              options[1].assistant == MDR_VOICE_ASSISTANT_VOICE_RECOGNITION &&
              options[2].assistant == MDR_VOICE_ASSISTANT_GOOGLE_ASSISTANT &&
              options[3].assistant == MDR_VOICE_ASSISTANT_NONE,
          "options keep the device order");
    check(options[0].available && options[1].available && options[2].available && options[3].available,
          "every option is available");

    check_result(mdrHeadphonesSetVoiceAssistant(session.headphones, MDR_VOICE_ASSISTANT_AMAZON_ALEXA),
                 MDR_RESULT_ERROR_NOT_SUPPORTED, "an unadvertised assistant is refused");
    check_result(mdrHeadphonesSetVoiceAssistant(session.headphones, MDR_VOICE_ASSISTANT_OTHER_DEVICE),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "another device's assistant is read-only");
    check_result(mdrHeadphonesSetVoiceAssistant(session.headphones, MDR_VOICE_ASSISTANT_UNKNOWN),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "an unknown assistant is invalid");

    /* Background music on: Sound Connect switches to Standard before selecting a voice UI. */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, bgm_on, sizeof(bgm_on));
    device_run(&session, &device, MDR_EVENT_LISTENING_MODE_CHANGED, "background music being on polls");
    count = 8;
    mdrHeadphonesGetVoiceAssistantOptions(session.headphones, options, &count);
    check(options[0].available == MDR_FALSE && options[2].available == MDR_FALSE,
          "voice control and Google Assistant are unavailable over background music");
    check(options[1].available == MDR_TRUE && options[3].available == MDR_TRUE,
          "the phone's assistant and none stay available");
    check_result(mdrHeadphonesSetVoiceAssistant(session.headphones, MDR_VOICE_ASSISTANT_SONY_VOICE_CONTROL),
                 MDR_RESULT_ERROR_NOT_SUPPORTED, "voice control is refused over background music");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, bgm_off, sizeof(bgm_off));
    device_run(&session, &device, MDR_EVENT_LISTENING_MODE_CHANGED, "background music being off polls");

    /* Stale: the device has background music on but has not said so. The send re-reads it. */
    device.listening_on = 1u << 0x03;
    check_result(mdrHeadphonesSetVoiceAssistant(session.headphones, MDR_VOICE_ASSISTANT_SONY_VOICE_CONTROL),
                 MDR_RESULT_OK, "voice control stages on a stale report");
    device.log_size = 0;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "the stale apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "the stale apply completes");
    check(device_requested(&device, 1, 0xe6, 0x03), "background music is re-read before a voice UI is sent");
    check(!device_requested(&device, 1, 0xf8, 0x04), "a voice UI over background music is not sent");
    mdrHeadphonesGetVoiceAssistant(session.headphones, MDR_PROPERTY_DESIRED, &settings);
    check(settings.assistant == MDR_VOICE_ASSISTANT_NONE, "the desired view falls back to what the device has");
    device.listening_on = 0;
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, bgm_off, sizeof(bgm_off));
    device_run(&session, &device, MDR_EVENT_LISTENING_MODE_CHANGED, "background music being off again polls");

    check_result(mdrHeadphonesSetVoiceAssistant(session.headphones, MDR_VOICE_ASSISTANT_SONY_VOICE_CONTROL),
                 MDR_RESULT_OK, "voice control stages");
    mdrHeadphonesGetVoiceAssistant(session.headphones, MDR_PROPERTY_DESIRED, &settings);
    check(settings.assistant == MDR_VOICE_ASSISTANT_SONY_VOICE_CONTROL, "the desired view holds the edit");
    mdrHeadphonesGetVoiceAssistant(session.headphones, MDR_PROPERTY_CURRENT, &settings);
    check(settings.assistant == MDR_VOICE_ASSISTANT_NONE, "the current view waits for the device");
    tx_before = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "voice control apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "voice control applies");
    offset = tx_before;
    while (next_tx_frame(&session.transport, &offset, &frame))
        if (frame.payload_size == sizeof(expected_set) &&
            memcmp(frame.payload, expected_set, sizeof(expected_set)) == 0)
            found = 1;
    check(found, "voice control is sent as SYSTEM_SET_PARAM VOICE_ASSISTANT_SETTINGS SONY_VOICE_ASSISTANT");
    device_run(&session, &device, MDR_EVENT_VOICE_ASSISTANT_CHANGED, "the device reports the change");
    mdrHeadphonesGetVoiceAssistant(session.headphones, MDR_PROPERTY_CURRENT, &settings);
    check(settings.assistant == MDR_VOICE_ASSISTANT_SONY_VOICE_CONTROL, "the current view follows the report");

    /* What the XM6 pushes next: background music disabled, and a status it never advertised. */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, bgm_disabled, sizeof(bgm_disabled));
    device_run(&session, &device, MDR_EVENT_LISTENING_MODE_CHANGED, "background music being disabled polls");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, wake_word, sizeof(wake_word));
    device_run(&session, &device, MDR_EVENT_UNHANDLED, "an unadvertised wake word status is left alone");
    memset(&listening, 0, sizeof(listening));
    listening.mode = MDR_LISTENING_BACKGROUND_MUSIC;
    listening.background_room = MDR_ROOM_LARGE;
    check_result(mdrHeadphonesSetListening(session.headphones, &listening), MDR_RESULT_ERROR_NOT_SUPPORTED,
                 "background music is refused while voice control disables it");

    /* A locked setting refuses changes. */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, va_locked, sizeof(va_locked));
    device_run(&session, &device, MDR_EVENT_VOICE_ASSISTANT_CHANGED, "the setting being locked polls");
    mdrHeadphonesGetVoiceAssistant(session.headphones, MDR_PROPERTY_CURRENT, &settings);
    check(settings.available == MDR_FALSE, "a locked setting reads unavailable");
    check_result(mdrHeadphonesSetVoiceAssistant(session.headphones, MDR_VOICE_ASSISTANT_NONE),
                 MDR_RESULT_ERROR_NOT_SUPPORTED, "a locked setting refuses changes");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, va_unlocked, sizeof(va_unlocked));
    device_run(&session, &device, MDR_EVENT_VOICE_ASSISTANT_CHANGED, "the setting being unlocked polls");

    /* An active listening mode excludes the assistants it lists. */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, cinema_capability, sizeof(cinema_capability));
    device_run(&session, &device, MDR_EVENT_LISTENING_MODE_CHANGED, "the cinema capability polls");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, cinema_on, sizeof(cinema_on));
    device_run(&session, &device, MDR_EVENT_LISTENING_MODE_CHANGED, "cinema being on polls");
    count = 8;
    mdrHeadphonesGetVoiceAssistantOptions(session.headphones, options, &count);
    check(count == 4 && options[2].assistant == MDR_VOICE_ASSISTANT_GOOGLE_ASSISTANT &&
              options[2].available == MDR_FALSE && options[0].available == MDR_TRUE,
          "cinema makes Google Assistant unavailable, and only it");
    check_result(mdrHeadphonesSetVoiceAssistant(session.headphones, MDR_VOICE_ASSISTANT_GOOGLE_ASSISTANT),
                 MDR_RESULT_ERROR_NOT_SUPPORTED, "an excluded assistant is refused");
    check_result(mdrHeadphonesSetVoiceAssistant(session.headphones, MDR_VOICE_ASSISTANT_NONE), MDR_RESULT_OK,
                 "none stays selectable");
    session_close(&session);

    /* V1 has no such setting. */
    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    check_result(mdrHeadphonesGetVoiceAssistant(session.headphones, MDR_PROPERTY_CURRENT, &settings),
                 MDR_RESULT_ERROR_NOT_SUPPORTED, "V1 has no voice assistant settings");
    session_close(&session);
}

/*
 * A device switches the equalizer and DSEE off while a listening mode is active and reports
 * it. Without carrying that out to the caller, the controls stay live while every change is
 * ignored, so the two availability flags have to follow the device rather than the support
 * list - which keeps saying the device has an equalizer the whole time.
 */
static void test_equalizer_availability_follows_the_device(void)
{
    static const unsigned char table1[] = {
        0x07, 0x00, 0x04, 0xe6, 0xff, /* LISTENING_OPTION */
        0xeb, 0xff, /* BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE */
        0x50, 0xff, /* PRESET_EQ */
        0xe2, 0xff /* UPSCALING_AUTO_OFF */
    };
    static const unsigned char table2[] = {0x07, 0x00, 0x00};
    /* EQEBB_NTFY_STATUS PRESET_EQ, and AUDIO_NTFY_STATUS UPSCALING. 0x00 enables. */
    static const unsigned char eq_off[] = {0x55, 0x00, 0x01};
    static const unsigned char eq_on[] = {0x55, 0x00, 0x00};
    static const unsigned char dsee_off[] = {0xe5, 0x01, 0x01};
    static const unsigned char dsee_on[] = {0xe5, 0x01, 0x00};

    Session session;
    Device device;
    MDREqualizer equalizer;
    MDRFeatureAvailability availability_scratch = MDR_AVAILABILITY_UNKNOWN;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);

    device_run_init(&session, &device);

    /* Nothing has been said either way yet, so both stay usable. */
    memset(&equalizer, 0, sizeof(equalizer));
    check_result(mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_CURRENT, &equalizer), MDR_RESULT_OK,
                 "equalizer is readable");
    check(equalizer.available != MDR_FALSE, "the equalizer starts available");
    check(equalizer.dsee_available != MDR_FALSE, "DSEE starts available");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, eq_off, sizeof(eq_off));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "the equalizer being disabled polls");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, dsee_off, sizeof(dsee_off));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "DSEE being disabled polls");

    memset(&equalizer, 0, sizeof(equalizer));
    check_result(mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_CURRENT, &equalizer), MDR_RESULT_OK,
                 "equalizer is readable while disabled");
    check(equalizer.available == MDR_FALSE, "a disabled equalizer reads unavailable");
    check(equalizer.dsee_available == MDR_FALSE, "disabled DSEE reads unavailable");

    /* Support is unchanged throughout - the device still has an equalizer, just not now. */
    check(mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_EQUALIZER, &availability_scratch) == MDR_RESULT_OK &&
              availability_scratch == MDR_AVAILABILITY_AVAILABLE,
          "the equalizer feature stays supported while unavailable");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, eq_on, sizeof(eq_on));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "the equalizer being re-enabled polls");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, dsee_on, sizeof(dsee_on));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "DSEE being re-enabled polls");

    memset(&equalizer, 0, sizeof(equalizer));
    check_result(mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_CURRENT, &equalizer), MDR_RESULT_OK,
                 "equalizer is readable once re-enabled");
    check(equalizer.available != MDR_FALSE, "a re-enabled equalizer reads available");
    check(equalizer.dsee_available != MDR_FALSE, "re-enabled DSEE reads available");
    session_close(&session);
}

/*
 * Which presets a device has is a capability, and until it answers, nothing is known: an empty
 * list has to mean "not said" rather than "none", or a device whose capability carries no list
 * would look like one with no equalizer at all.
 */
static void test_equalizer_presets_follow_the_capability(void)
{
    static const unsigned char table1[] = {
        0x07, 0x00, 0x01, 0x50, 0xff /* PRESET_EQ */
    };
    static const unsigned char table2[] = {0x07, 0x00, 0x00};
    /* EQEBB_RET_CAPABILITY PRESET_EQ: ten bands of thirteen steps, and three named presets. */
    static const unsigned char capability[] = {0x51, 0x00, 0x0a, 0x0d, 0x03, 0x00, 0x03, 'O', 'f',
                                               'f',  0x10, 0x06, 'B',  'r',  'i',  'g',  'h', 't',
                                               0xa0, 0x06, 'C',  'u',  's',  't',  'o',  'm'};

    Session session;
    Device device;
    MDREqualizer equalizer;
    MDREqualizerPreset presets[8];
    uint32_t count;
    char name[16];
    uint32_t name_size;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);

    device_run_init(&session, &device);

    check(device_requested(&device, 1, 0x50, 0x00), "EQEBB_GET_CAPABILITY PRESET_EQ for an advertised equalizer");

    count = 0;
    check_result(mdrHeadphonesGetEqualizerPresets(session.headphones, NULL, &count), MDR_RESULT_OK,
                 "the preset count is readable before the device answers");
    check(count == 0, "no presets are known before the device answers");

    /* Nothing has been said, so nothing is refused - the caller is no worse off than before. */
    memset(&equalizer, 0, sizeof(equalizer));
    check_result(mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_CURRENT, &equalizer), MDR_RESULT_OK,
                 "equalizer is readable");
    equalizer.preset = MDR_EQ_ROCK;
    check_result(mdrHeadphonesSetEqualizerPreset(session.headphones, equalizer.preset), MDR_RESULT_OK,
                 "any encodable preset is accepted while the list is unknown");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, capability, sizeof(capability));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "the preset list polls");

    count = 0;
    check_result(mdrHeadphonesGetEqualizerPresets(session.headphones, NULL, &count), MDR_RESULT_OK,
                 "the preset count is readable");
    check_result((MDRResult)count, 3, "every advertised preset is counted");

    count = 1;
    check_result(mdrHeadphonesGetEqualizerPresets(session.headphones, presets, &count),
                 MDR_RESULT_ERROR_BUFFER_TOO_SMALL, "a short preset buffer is refused");
    check_result((MDRResult)count, 3, "a refused read reports the size it needs");

    count = 8;
    check_result(mdrHeadphonesGetEqualizerPresets(session.headphones, presets, &count), MDR_RESULT_OK,
                 "the preset list is readable");
    check_result((MDRResult)count, 3, "the list holds what the device advertised");
    check(presets[0] == MDR_EQ_OFF, "the first preset is the one listed first");
    check(presets[1] == MDR_EQ_BRIGHT, "the second preset is the one listed second");
    check(presets[2] == MDR_EQ_CUSTOM, "the third preset is the one listed third");

    /* The index into the list is also the index into its names. */
    name_size = sizeof(name);
    check_result(mdrHeadphonesGetText(session.headphones, MDR_TEXT_EQUALIZER_PRESET_NAME, 1, name, &name_size),
                 MDR_RESULT_OK, "a preset name is readable");
    check(strcmp(name, "Bright") == 0, "the name is the one the device gave that preset");
    name_size = sizeof(name);
    check_result(mdrHeadphonesGetText(session.headphones, MDR_TEXT_EQUALIZER_PRESET_NAME, 3, name, &name_size),
                 MDR_RESULT_ERROR_NOT_FOUND, "a name past the end of the list is not found");

    memset(&equalizer, 0, sizeof(equalizer));
    check_result(mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_CURRENT, &equalizer), MDR_RESULT_OK,
                 "equalizer is readable once the list is known");
    equalizer.preset = MDR_EQ_BRIGHT;
    check_result(mdrHeadphonesSetEqualizerPreset(session.headphones, equalizer.preset), MDR_RESULT_OK,
                 "an advertised preset is accepted");
    equalizer.preset = MDR_EQ_ROCK;
    check_result(mdrHeadphonesSetEqualizerPreset(session.headphones, equalizer.preset), MDR_RESULT_ERROR_NOT_SUPPORTED,
                 "a preset the device never listed is refused");
    session_close(&session);
}

/*
 * A listening mode takes the equalizer and the upscaling with it, and the device says so in
 * its own time - or not at all, if the notification is missed. Nothing re-reads either
 * status afterwards, RequestSyncV2 included, so a client that only ever waits to be told
 * goes on offering controls the device is ignoring for the rest of the session. Switching a
 * mode therefore asks.
 */
static void test_listening_mode_rereads_what_it_takes_away(void)
{
    static const unsigned char table1[] = {
        0x07, 0x00, 0x04, 0xe6, 0xff, /* LISTENING_OPTION */
        0xeb, 0xff, /* BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE */
        0x50, 0xff, /* PRESET_EQ */
        0xe2, 0xff /* UPSCALING_AUTO_OFF */
    };
    static const unsigned char table2[] = {0x07, 0x00, 0x00};

    Session session;
    Device device;
    MDRListening listening;
    MDREqualizer equalizer;
    size_t requests_before;
    int asked_equalizer = 0;
    int asked_upscaling = 0;
    int iteration;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);

    device_run_init(&session, &device);

    /* Nothing has been said, so both stand available. */
    memset(&equalizer, 0, sizeof(equalizer));
    check_result(mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_CURRENT, &equalizer), MDR_RESULT_OK,
                 "equalizer is readable");
    check(equalizer.available != MDR_FALSE, "the equalizer starts available");
    check(equalizer.dsee_available != MDR_FALSE, "DSEE starts available");

    /* From here the device answers a status request, and notifies nothing of its own. */
    device.answer_status_disabled = 1;

    /* Initialization asks for both statuses too, so only what follows the switch counts. */
    requests_before = device.log_size;

    memset(&listening, 0, sizeof(listening));
    mdrHeadphonesGetListening(session.headphones, MDR_PROPERTY_CURRENT, &listening);
    listening.mode = MDR_LISTENING_BACKGROUND_MUSIC;
    listening.background_room = MDR_ROOM_SMALL;
    check_result(mdrHeadphonesSetListening(session.headphones, &listening), MDR_RESULT_OK, "the listening mode stages");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "the listening change starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "the listening change completes");

    for (iteration = (int)requests_before; iteration < (int)device.log_size; ++iteration)
    {
        const RequestLog* entry = &device.log[iteration];
        if (entry->table != 1 || !entry->has_inquired)
            continue;
        if (entry->command == 0x52 && entry->inquired == 0x00)
            asked_equalizer = 1;
        if (entry->command == 0xe2 && entry->inquired == 0x01)
            asked_upscaling = 1;
    }
    check(asked_equalizer, "switching a listening mode asks for the equalizer status");
    check(asked_upscaling, "switching a listening mode asks for the upscaling status");

    /* Drain the answers the device gave to those. */
    for (iteration = 0; iteration < 64; ++iteration)
    {
        MDREvent event = MDR_EVENT_NONE;
        mdrHeadphonesPoll(session.headphones, &event);
        device_pump(&device);
    }

    memset(&equalizer, 0, sizeof(equalizer));
    check_result(mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_CURRENT, &equalizer), MDR_RESULT_OK,
                 "equalizer is readable after the switch");
    check(equalizer.available == MDR_FALSE, "the equalizer reads unavailable without having been notified");
    check(equalizer.dsee_available == MDR_FALSE, "DSEE reads unavailable without having been notified");
    session_close(&session);
}

static void test_poll_events(void)
{
    Session session;
    const unsigned char unknown_payload[] = {0xfe};
    unsigned char protocol_frame[FRAME_BUFFER_CAPACITY];
    unsigned char unknown_frame[FRAME_BUFFER_CAPACITY];
    size_t protocol_size;
    size_t unknown_size;
    MDREvent first;
    MDREvent second;

    if (!session_open(&session))
        return;
    protocol_size = pack_data_frame(k_v2_protocol_info, sizeof(k_v2_protocol_info), 0, protocol_frame);
    unknown_size = pack_data_frame(unknown_payload, sizeof(unknown_payload), 1, unknown_frame);
    mock_load(&session.transport, protocol_frame, protocol_size);
    mock_append(&session.transport, unknown_frame, unknown_size);

    poll_event(session.headphones, &first, "first frame polls");
    poll_event(session.headphones, &second, "second frame polls");
    check(first == MDR_EVENT_IDENTITY_CHANGED, "protocol state change is reported by its poll");
    check(second == MDR_EVENT_UNHANDLED, "unhandled frame is reported by its poll");
    session_close(&session);
}

static void test_v2_bootstrap(void)
{
    Session session;
    unsigned char frame[FRAME_BUFFER_CAPACITY];
    size_t frame_size;
    MDRModel identity;
    MDREvent event;

    if (!session_open(&session))
        return;
    check_result(mdrHeadphonesRequestInit(session.headphones, MDR_HEADPHONES_INIT_DEFAULT), MDR_RESULT_OK,
                 "automatic initialization starts");
    poll_event(session.headphones, &event, "protocol-info request flushes");
    frame_size = pack_ack(&session.transport, frame);
    mock_load(&session.transport, frame, frame_size);
    poll_event(session.headphones, &event, "protocol-info request ACK polls");

    frame_size = pack_data_frame(k_v2_protocol_info, sizeof(k_v2_protocol_info), 0, frame);
    mock_load(&session.transport, frame, frame_size);
    poll_event(session.headphones, &event, "eight-byte V2 protocol-info polls");

    memset(&identity, 0, sizeof(identity));
    check_result(mdrHeadphonesGetModel(session.headphones, &identity), MDR_RESULT_OK, "V2 identity is readable");
    check(identity.protocol_version == 2, "eight-byte payload selects MDR V2");
    check(event == MDR_EVENT_IDENTITY_CHANGED, "V2 protocol selection reports identity change");

    check(mdrHeadphonesIsInitialized(session.headphones) == MDR_FALSE &&
              mdrHeadphonesIsReady(session.headphones) == MDR_FALSE,
          "V2 bootstrap automatically continues into backend initialization");
    session_close(&session);
}

static void check_command_payload(Session* session, size_t offset, const unsigned char* payload, size_t size)
{
    TxFrame frame;
    int writes = 0;
    while (next_tx_frame(&session->transport, &offset, &frame))
    {
        if (frame.type != MDR_DATA_TYPE_DATA_MDR || frame.payload_size == 0 || frame.payload[0] != payload[0])
            continue;
        ++writes;
        check(frame.payload_size == size && memcmp(frame.payload, payload, size) == 0,
              "command carries its submitted snapshot, not a newer staged value");
    }
    check(writes == 1, "each submitted command is transmitted exactly once");
}

static void test_alert_responses(MDRProtocolVersion family, int arrival)
{
    static const unsigned char table1_v1[] = {0x07, 0, 1, 0xa1};
    static const unsigned char table1_v2[] = {0x07, 0, 2, 0x90, 0xff, 0xa1, 0xff};
    static const unsigned char table2[] = {0x07, 0, 0};
    int v1 = family == MDR_PROTOCOL_V1;
    unsigned char alert[] = {0x99, (unsigned char)(v1 ? 1 : 0), (unsigned char)(v1 ? 1 : 0), 1};
    unsigned char response[] = {0x98, (unsigned char)(v1 ? 1 : 0), (unsigned char)(v1 ? 1 : 0), 0};
    Session session;
    Device device;
    MDREvent event;
    size_t offset;

    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = v1;
    device.table1 = v1 ? table1_v1 : table1_v2;
    device.table1_size = v1 ? sizeof(table1_v1) : sizeof(table1_v2);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);
    check_result(mdrHeadphonesSetAlertResponse(session.headphones, MDR_ALERT_ACTION_POSITIVE),
                 MDR_RESULT_ERROR_NOT_FOUND, "no alert accepts a response");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, alert, sizeof(alert));
    device_run(&session, &device, MDR_EVENT_ALERT, "alert A arrives");
    check_result(mdrHeadphonesSetAlertResponse(session.headphones, 2), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "invalid alert action is rejected");
    check(!mdrHeadphonesIsDirty(session.headphones), "invalid response stages nothing");

    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 8), MDR_RESULT_OK, "blocker stages");
    device.block_ack = 1;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "blocker apply starts");
    poll_event(session.headphones, &event, "blocker flushes before its ACK");
    device_pump(&device);
    check_result(mdrHeadphonesSetAlertResponse(session.headphones, MDR_ALERT_ACTION_POSITIVE), MDR_RESULT_OK,
                 "alert A response stages while busy");
    check_result(mdrHeadphonesSetAlertResponse(session.headphones, MDR_ALERT_ACTION_NEGATIVE), MDR_RESULT_OK,
                 "latest response replaces the staged action");
    alert[2] = (unsigned char)((v1 ? 1 : 0) + (arrival != 1));
    if (arrival != 2)
    {
        device_send(&device, MDR_DATA_TYPE_DATA_MDR, alert, sizeof(alert));
        device_run(&session, &device, MDR_EVENT_ALERT, "alert B arrives before response submission");
    }
    device.block_ack = 0;
    device_ack(&device, device.held_seq);
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "blocker apply completes");
    check(mdrHeadphonesIsDirty(session.headphones), "response staged after snapshot remains dirty");

    offset = session.transport.tx_size;
    device.block_ack = 1;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "alert A response apply starts");
    poll_event(session.headphones, &event, "response A flushes before its ACK");
    device_pump(&device);
    check_command_payload(&session, offset, response, sizeof(response));
    if (arrival == 2)
    {
        check_result(mdrHeadphonesSetAlertResponse(session.headphones, MDR_ALERT_ACTION_POSITIVE),
                     MDR_RESULT_ERROR_NOT_FOUND, "submitted alert A is no longer awaiting a response");
        device_send(&device, MDR_DATA_TYPE_DATA_MDR, alert, sizeof(alert));
        device_run(&session, &device, MDR_EVENT_ALERT, "alert B arrives while response A awaits ACK");
    }
    check_result(mdrHeadphonesSetAlertResponse(session.headphones, MDR_ALERT_ACTION_POSITIVE), MDR_RESULT_OK,
                 "old response submission leaves alert B awaiting a response");
    device.block_ack = 0;
    device_ack(&device, device.held_seq);
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "alert A response completes");
    check(mdrHeadphonesIsDirty(session.headphones), "old response ACK preserves response B");

    response[2] = alert[2];
    response[3] = 1;
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "alert B response apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "alert B response completes");
    check_command_payload(&session, offset, response, sizeof(response));
    check(!mdrHeadphonesIsDirty(session.headphones), "response B ACK consumes its write");
    check_result(mdrHeadphonesSetAlertResponse(session.headphones, MDR_ALERT_ACTION_NEGATIVE),
                 MDR_RESULT_ERROR_NOT_FOUND, "answered alert cannot stage another response");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "empty response apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "empty response apply completes");
    check(session.transport.tx_size == offset, "completed response is not replayed");
    alert[3] = 0;
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, alert, sizeof(alert));
    device_run(&session, &device, MDR_EVENT_UNHANDLED, "confirmation-only alert arrives");
    check_result(mdrHeadphonesSetAlertResponse(session.headphones, MDR_ALERT_ACTION_POSITIVE),
                 MDR_RESULT_ERROR_NOT_FOUND, "confirmation-only alert accepts no response");
    check(!mdrHeadphonesIsDirty(session.headphones), "notification stages no response");
    session_close(&session);
}

static void check_eq_bands(MDRHeadphones* headphones, MDRPropertyType type, const int32_t* expected,
                           uint32_t expected_count);

static void test_equalizer_independent_writes(MDRProtocolVersion family)
{
    static const unsigned char table1_v1[] = {0x07, 0, 2, 0x51, 0xe2};
    static const unsigned char table1_v2[] = {0x07, 0, 2, 0x50, 0xff, 0xe2, 0xff};
    static const unsigned char table2[] = {0x07, 0, 0};
    static const unsigned char flat_v1[] = {0x57, 1, 0, 6, 10, 10, 10, 10, 10, 10};
    static const unsigned char flat_v2[] = {0x57, 0, 0, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
    static const unsigned char bright_v1[] = {0x59, 1, 0x10, 6, 10, 11, 11, 11, 11, 11};
    static const unsigned char bright_v2[] = {0x59, 0, 0x10, 10, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};
    static const int32_t ones[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    Session session;
    Device device;
    MDREqualizer current, desired;
    unsigned char preset[] = {0x58, 0, 0x10, 0};
    TxFrame frame;
    size_t offset;
    int v1 = family == MDR_PROTOCOL_V1;
    int writes;

    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = v1;
    device.table1 = v1 ? table1_v1 : table1_v2;
    device.table1_size = v1 ? sizeof(table1_v1) : sizeof(table1_v2);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? flat_v1 : flat_v2, v1 ? sizeof(flat_v1) : sizeof(flat_v2));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "initial EQ report polls");
    check_result(mdrHeadphonesSetEqualizerPreset(session.headphones, MDR_EQ_UNKNOWN), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "unknown preset is not a write");
    check(!mdrHeadphonesIsDirty(session.headphones), "invalid preset leaves no edit");
    device.eq_notification = v1 ? bright_v1 : bright_v2;
    device.eq_notification_size = v1 ? sizeof(bright_v1) : sizeof(bright_v2);
    device.reply_before_ack = 1;
    preset[1] = v1 ? 1 : 0;
    offset = session.transport.tx_size;
    device.log_size = 0;
    check_result(mdrHeadphonesSetEqualizerPreset(session.headphones, MDR_EQ_BRIGHT), MDR_RESULT_OK,
                 "preset stages alone");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "preset apply starts");
    check_result(mdrHeadphonesSetEqualizerPreset(session.headphones, MDR_EQ_BRIGHT), MDR_RESULT_OK,
                 "same preset stages while its old write is pending");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "preset apply completes");
    check_command_payload(&session, offset, preset, sizeof(preset));
    check(!device_requested(&device, 1, 0xe8, -1), "preset write does not write DSEE");
    mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_CURRENT, &current);
    mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_DESIRED, &desired);
    check(current.preset == MDR_EQ_BRIGHT && desired.preset == MDR_EQ_BRIGHT,
          "reported preset stands without an old curve write");
    check_eq_bands(session.headphones, MDR_PROPERTY_CURRENT, ones, v1 ? 5u : 10u);
    check_eq_bands(session.headphones, MDR_PROPERTY_DESIRED, ones, v1 ? 5u : 10u);
    check(mdrHeadphonesIsDirty(session.headphones), "old preset ACK preserves the repeated write");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "repeated preset apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "repeated preset apply completes");
    check_command_payload(&session, offset, preset, sizeof(preset));
    check(!mdrHeadphonesIsDirty(session.headphones), "repeated preset ACK consumes its write");

    device.eq_notification = NULL;
    device.log_size = 0;
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesSetDSEE(session.headphones, MDR_TRUE), MDR_RESULT_OK, "DSEE stages alone");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "DSEE apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "DSEE apply completes");
    check(device_requested(&device, 1, 0xe8, -1) && !device_requested(&device, 1, 0x58, -1),
          "DSEE write sends no preset or curve");
    mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_DESIRED, &desired);
    check(desired.preset == MDR_EQ_BRIGHT && desired.clear_bass == current.clear_bass,
          "DSEE write preserves EQ fields");

    device.log_size = 0;
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesSetEqualizerBands(session.headphones, ones, v1 ? 5u : 10u), MDR_RESULT_OK,
                 "same-value curve stages alone");
    if (v1)
        check_result(mdrHeadphonesSetEqualizerClearBass(session.headphones, 2), MDR_RESULT_OK,
                     "Clear Bass joins curve");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "curve apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "curve apply completes");
    writes = 0;
    while (next_tx_frame(&session.transport, &offset, &frame))
    {
        if (frame.type != MDR_DATA_TYPE_DATA_MDR || frame.payload_size == 0 || frame.payload[0] != 0x58)
            continue;
        ++writes;
        check(frame.payload_size == (v1 ? 10u : 14u) && frame.payload[2] == (v1 ? 0xff : 0x10),
              "curve write uses the protocol-specific preset tag");
        if (v1)
            check(frame.payload[4] == 12, "Clear Bass is included in the curve frame");
    }
    check(writes == 1 && !device_requested(&device, 1, 0xe8, -1) && !mdrHeadphonesIsDirty(session.headphones),
          "curve write adds no preset-only or DSEE command");
    if (v1)
    {
        offset = session.transport.tx_size;
        check_result(mdrHeadphonesSetEqualizerPreset(session.headphones, MDR_EQ_BRIGHT), MDR_RESULT_OK,
                     "explicit preset joins the next curve batch");
        check_result(mdrHeadphonesSetEqualizerBands(session.headphones, ones, 5), MDR_RESULT_OK,
                     "explicit curve joins preset");
        check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "preset and curve apply starts");
        device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "preset and curve apply completes");
        writes = 0;
        while (next_tx_frame(&session.transport, &offset, &frame))
        {
            if (frame.type != MDR_DATA_TYPE_DATA_MDR || frame.payload_size == 0 || frame.payload[0] != 0x58)
                continue;
            check(writes == 0 ? frame.payload_size == 4 && frame.payload[2] == 0x10
                              : frame.payload_size == 10 && frame.payload[2] == 0xff,
                  "V1 sends preset then curve in separate frames");
            ++writes;
        }
        check(writes == 2 && !mdrHeadphonesIsDirty(session.headphones), "V1 consumes both explicit writes");
    }
    session_close(&session);
}

static void test_staging_during_apply(MDRProtocolVersion family)
{
    static const unsigned char table1_v1[] = {0x07, 0, 1, 0xa1};
    static const unsigned char table1_v2[] = {0x07, 0, 1, 0xa1, 0xff};
    static const unsigned char table2[] = {0x07, 0, 0};
    Session session;
    Device device;
    MDRPlayback current, desired;
    const MDRPlaybackCommand command = {MDR_PLAYBACK_PLAY};
    int v1 = family == MDR_PROTOCOL_V1;
    unsigned char volume_v1[] = {0xa8, 1, 0x20, 10};
    unsigned char volume_v2[] = {0xa8, 0x20, 10};
    const unsigned char play[] = {0xa4, 1, 0, 7};
    size_t offset, tx_size;
    MDREvent event;
    int iteration;

    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = v1;
    device.table1 = v1 ? table1_v1 : table1_v2;
    device.table1_size = v1 ? sizeof(table1_v1) : sizeof(table1_v2);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);

    offset = session.transport.tx_size;
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 10), MDR_RESULT_OK, "first volume stages");
    check_result(mdrHeadphonesSendPlaybackCommand(session.headphones, &command), MDR_RESULT_OK, "first action stages");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "first batch starts");
    tx_size = session.transport.tx_size;
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 15), MDR_RESULT_OK,
                 "volume stages before first send");
    check_result(mdrHeadphonesSendPlaybackCommand(session.headphones, &command), MDR_RESULT_OK,
                 "same action stages before its old submitted command runs");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 20), MDR_RESULT_OK,
                 "latest volume replaces staged volume");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 31), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "invalid busy edit is still rejected");
    check(session.transport.tx_size == tx_size, "busy setters do not perform I/O");
    check(!mdrHeadphonesIsReady(session.headphones) && mdrHeadphonesIsDirty(session.headphones),
          "staging does not complete the active request");
    check_result(mdrHeadphonesRequestInit(session.headphones, MDR_HEADPHONES_INIT_DEFAULT), MDR_RESULT_INPROGRESS,
                 "busy Init remains serialized");
    check_result(mdrHeadphonesRequestSync(session.headphones, MDR_HEADPHONES_SYNC_DEFAULT), MDR_RESULT_INPROGRESS,
                 "busy Sync remains serialized");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_INPROGRESS,
                 "busy Commit cannot replace snapshot");
    poll_event(session.headphones, &event, "old volume flushes before its ACK");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 21), MDR_RESULT_OK,
                 "volume stages while waiting for ACK");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "first batch completes");
    check_command_payload(&session, offset, v1 ? volume_v1 : volume_v2, v1 ? sizeof(volume_v1) : sizeof(volume_v2));
    check_command_payload(&session, offset, play, sizeof(play));
    check_result(mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_CURRENT, &current), MDR_RESULT_OK,
                 "current reads");
    check_result(mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_DESIRED, &desired), MDR_RESULT_OK,
                 "desired reads");
    check(current.volume == 0 && desired.volume == 21 && mdrHeadphonesIsReady(session.headphones) &&
              mdrHeadphonesIsDirty(session.headphones),
          "old ACK leaves newer edits pending without synthesizing current");

    offset = session.transport.tx_size;
    device.log_size = 0;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "second batch starts");
    for (iteration = 0; iteration < 4096; ++iteration)
    {
        if (!poll_event(session.headphones, &event, "second batch advances to its playback action"))
            break;
        device_pump(&device);
        if (device_requested(&device, 1, 0xa4, 1))
            break;
    }
    check(device_requested(&device, 1, 0xa4, 1) && !mdrHeadphonesIsReady(session.headphones),
          "volume ACK is consumed while playback action remains in flight");

    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 22), MDR_RESULT_OK,
                 "already acknowledged property stages again while another property is in flight");
    check_result(mdrHeadphonesSendPlaybackCommand(session.headphones, &command), MDR_RESULT_OK,
                 "same action stages while its old action waits for ACK");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "second batch completes");
    volume_v1[3] = volume_v2[2] = 21;
    check_command_payload(&session, offset, v1 ? volume_v1 : volume_v2, v1 ? sizeof(volume_v1) : sizeof(volume_v2));
    check_command_payload(&session, offset, play, sizeof(play));
    check(mdrHeadphonesIsReady(session.headphones) && mdrHeadphonesIsDirty(session.headphones),
          "both edits survive completion of the second batch");

    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "third batch starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "third batch completes");
    volume_v1[3] = volume_v2[2] = 22;
    check_command_payload(&session, offset, v1 ? volume_v1 : volume_v2, v1 ? sizeof(volume_v1) : sizeof(volume_v2));
    check_command_payload(&session, offset, play, sizeof(play));
    check(mdrHeadphonesIsReady(session.headphones) && !mdrHeadphonesIsDirty(session.headphones),
          "last batch consumes all submitted edits");

    offset = session.transport.tx_size;
    device.log_size = 0;
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 22), MDR_RESULT_OK,
                 "same-value volume stages again");
    check(mdrHeadphonesIsDirty(session.headphones), "same-value Set retains explicit write intent");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "same-value batch starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "same-value batch completes");
    check_command_payload(&session, offset, v1 ? volume_v1 : volume_v2, v1 ? sizeof(volume_v1) : sizeof(volume_v2));
    check(!device_requested(&device, 1, 0xa4, -1) && !mdrHeadphonesIsDirty(session.headphones),
          "consumed one-shot action is not replayed by an unrelated batch");

    device.log_size = 0;
    check_result(mdrHeadphonesRequestSync(session.headphones, MDR_HEADPHONES_SYNC_DEFAULT), MDR_RESULT_OK,
                 "sync starts before new edits");
    tx_size = session.transport.tx_size;
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 23), MDR_RESULT_OK, "volume stages during sync");
    check_result(mdrHeadphonesSendPlaybackCommand(session.headphones, &command), MDR_RESULT_OK,
                 "action stages during sync");
    check(session.transport.tx_size == tx_size, "sync-time setters do not send commands");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_INPROGRESS, "commit waits for sync");
    device_run(&session, &device, MDR_EVENT_SYNC_COMPLETE, "sync completes with new edits staged");
    check(!device_requested(&device, 1, 0xa8, -1) && !device_requested(&device, 1, 0xa4, -1),
          "sync does not consume or transmit new write intent");
    check(mdrHeadphonesIsReady(session.headphones) && mdrHeadphonesIsDirty(session.headphones),
          "sync completion leaves its concurrent edits staged");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "post-sync edits start");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "post-sync edits complete");
    volume_v1[3] = volume_v2[2] = 23;
    check_command_payload(&session, offset, v1 ? volume_v1 : volume_v2, v1 ? sizeof(volume_v1) : sizeof(volume_v2));
    check_command_payload(&session, offset, play, sizeof(play));
    check(!mdrHeadphonesIsDirty(session.headphones), "post-sync ACK consumes its edits");
    session_close(&session);
}

/*
 * A connection-quality write has to name the inquired type it is setting. AUDIO_SET_PARAM
 * carries one, and AudioSetParamConnection's default is CONNECTION_MODE_CLASSIC_AUDIO_LE_AUDIO
 * - a different variant of the same command, with a field this one does not carry - so a
 * write that leaves the field alone asks the device for something it never advertised. The
 * framing layer acknowledges the frame either way, the payload is dropped, and the setting
 * reads back unchanged with nothing to say why.
 */
static void test_connection_mode_names_its_inquired_type(void)
{
    static const unsigned char table1[] = {
        0x07, 0x00, 0x01, 0xe1, 0xff /* CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY */
    };
    static const unsigned char table2[] = {0x07, 0x00, 0x00};

    Session session;
    Device device;
    MDRConnectionMode mode;
    size_t index;
    int writes = 0;
    int misdirected = 0;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);

    device_run_init(&session, &device);

    memset(&mode, 0, sizeof(mode));
    mode.audio_priority = MDR_AUDIO_PRIORITY_STABILITY;
    check_result(mdrHeadphonesSetConnectionMode(session.headphones, &mode), MDR_RESULT_OK,
                 "the connection mode stages");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "the connection mode change starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "the connection mode change completes");

    for (index = 0; index < device.log_size; ++index)
    {
        const RequestLog* entry = &device.log[index];
        if (entry->table != 1 || entry->command != 0xe8) /* AUDIO_SET_PARAM */
            continue;
        ++writes;
        /* CONNECTION_MODE, the type the advertised function owns and GET_PARAM reads on. */
        if (!entry->has_inquired || entry->inquired != 0x00)
            ++misdirected;
    }
    check(writes == 1, "the connection quality is written once");
    check(misdirected == 0, "it is written on the inquired type the device advertised");
    session_close(&session);
}

/*
 * A voice-guidance reply says which detail it carries, and only one of them is the on/off
 * switch. Initialization asks about the language, the required time and the download server
 * as well, so most of the replies it provokes are not the switch - and reading one as the
 * switch fails validation inside RequestInit, which takes the whole connection down: no
 * battery, no listening modes, nothing. Reported against a WH-1000XM4.
 */
static void test_v1_voice_guidance_detail_is_not_the_switch(void)
{
    /* VOICE_GUIDANCE only - which is also what gives this device a second table. */
    static const unsigned char table1[] = {0x07, 0x00, 0x01, 0x39};

    Session session;
    Device device;

    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = 1;
    device.table1 = table1;
    device.table1_size = sizeof(table1);

    /* The device answers every voice-guidance question, including the ones about a detail
     * this library keeps nothing for. Initialization has to survive all of them. */
    device_run_init(&session, &device);
    check(mdrHeadphonesIsInitialized(session.headphones) != MDR_FALSE,
          "a reply about a detail other than the switch does not end the session");
    check(device_requested(&device, 2, 0x46, 0x01), /* VOICE_GUIDANCE_GET_PARAM, VOICE_GUIDANCE_SETTING */
          "the voice-guidance parameters were asked for at all");
    session_close(&session);
}

/*
 * A WH-1000XM3 answers CONNECT_GET_PROTOCOL_INFO before it acknowledges the request, so the
 * reply lands while initialization is still waiting on the ACK. It must not be lost there.
 */
static void test_v1_reply_before_ack(void)
{
    /* VOICE_GUIDANCE only - which is also what gives this device a second table. */
    static const unsigned char table1[] = {0x07, 0x00, 0x01, 0x39};

    Session session;
    Device device;

    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = 1;
    device.reply_before_ack = 1;
    device.table1 = table1;
    device.table1_size = sizeof(table1);

    device_run_init(&session, &device);
    check(mdrHeadphonesIsInitialized(session.headphones) != MDR_FALSE,
          "a reply that arrives ahead of its ACK still completes initialization");
    check(device_requested(&device, 1, 0x06, -1), /* CONNECT_GET_SUPPORT_FUNCTION */
          "initialization moves past the protocol handshake");
    session_close(&session);
}

static void check_device_waiting(Session* session, Device* device, size_t request_count)
{
    int iteration;
    for (iteration = 0; iteration < 3; ++iteration)
    {
        MDREvent event;
        if (!poll_event(session->headphones, &event, "pending response wait polls"))
            return;
        device_pump(device);
    }
    check(mdrHeadphonesIsReady(session->headphones) == MDR_FALSE, "a fresh response is still required");
    check(device->log_size == request_count, "stale or unrelated signals do not advance to another GET");
}

static void test_protocol_reply_generation(MDRProtocolVersion family)
{
    static const unsigned char support[] = {0x07, 0x00, 0x00};
    const int v1 = family == MDR_PROTOCOL_V1;
    const unsigned char* protocol = v1 ? k_v1_protocol_info : k_v2_protocol_info_both_tables;
    const size_t protocol_size = v1 ? sizeof(k_v1_protocol_info) : sizeof(k_v2_protocol_info_both_tables);
    Session session;
    Device device = {0};
    MDREvent event;

    if (!session_open_family(&session, family))
        return;
    device.transport = &session.transport;
    device.protocol_v1 = v1;
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, protocol, protocol_size);
    device_run(&session, &device, MDR_EVENT_IDENTITY_CHANGED, "unsolicited protocol reply is observed");

    device.block_ack = 1;
    check_result(mdrHeadphonesRequestInit(session.headphones, MDR_HEADPHONES_INIT_IDENTITY), MDR_RESULT_OK,
                 "identity request starts after the old reply");
    poll_event(session.headphones, &event, "protocol GET flushes");
    device_pump(&device);
    device_ack(&device, device.held_seq);
    check_device_waiting(&session, &device, 1);

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, support, sizeof(support));
    device_run(&session, &device, MDR_EVENT_IDENTITY_CHANGED, "unrelated support reply is observed");
    check_device_waiting(&session, &device, 1);

    device.block_ack = 0;
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, protocol, protocol_size);
    device_run(&session, &device, MDR_EVENT_INITIALIZE_COMPLETE, "fresh protocol reply completes identity");
    session_close(&session);
}

static void test_support_reply_generation(MDRProtocolVersion family, int observe_packets)
{
    static const unsigned char support[] = {0x07, 0x00, 0x00};
    const int v1 = family == MDR_PROTOCOL_V1;
    const unsigned char* protocol = v1 ? k_v1_protocol_info : k_v2_protocol_info_both_tables;
    const size_t protocol_size = v1 ? sizeof(k_v1_protocol_info) : sizeof(k_v2_protocol_info_both_tables);
    Session session;
    Device device = {0};
    PacketLog packets = {0};
    MDREvent event;
    size_t request_count;

    if (!session_open_family(&session, family))
        return;
    if (observe_packets)
        mdrDebugSetPacketCallback(session.headphones, observe_packet, &packets);
    device.transport = &session.transport;
    device.protocol_v1 = v1;
    device.table1 = device.table2 = support;
    device.table1_size = device.table2_size = sizeof(support);
    device_run_init_flags(&session, &device, MDR_HEADPHONES_INIT_IDENTITY);

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, support, sizeof(support));
    device_run(&session, &device, MDR_EVENT_IDENTITY_CHANGED, "unsolicited table 1 support reply is observed");
    if (!v1)
    {
        device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, support, sizeof(support));
        device_run(&session, &device, MDR_EVENT_IDENTITY_CHANGED, "unsolicited table 2 support reply is observed");
    }
    device.block_ack = 1;
    check_result(mdrHeadphonesRequestInit(session.headphones, MDR_HEADPHONES_INIT_SUPPORT_FUNCTIONS), MDR_RESULT_OK,
                 "support discovery starts after the old replies");
    poll_event(session.headphones, &event, "table 1 support GET flushes");
    device_pump(&device);
    request_count = device.log_size;
    device_ack(&device, device.held_seq);
    check_device_waiting(&session, &device, request_count);

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, protocol, protocol_size);
    device_run(&session, &device, MDR_EVENT_IDENTITY_CHANGED, "unrelated protocol reply increments the packet counter");
    check_device_waiting(&session, &device, request_count);

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, support, sizeof(support));
    device_run(&session, &device, MDR_EVENT_IDENTITY_CHANGED, "fresh table 1 support reply is observed");
    if (!v1)
    {
        poll_event(session.headphones, &event, "table 2 support GET flushes");
        device_pump(&device);
        check(device.log_size == request_count + 1, "only the fresh table 1 reply advances to table 2");
        request_count = device.log_size;
        device_ack(&device, device.held_seq);
        check_device_waiting(&session, &device, request_count);
        device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, support, sizeof(support));
    }
    device_run(&session, &device, MDR_EVENT_INITIALIZE_COMPLETE, "fresh replies complete support discovery");
    session_close(&session);
}

static void test_reply_before_ack_retry(MDRProtocolVersion family)
{
    const int v1 = family == MDR_PROTOCOL_V1;
    const unsigned char* protocol = v1 ? k_v1_protocol_info : k_v2_protocol_info_both_tables;
    const size_t protocol_size = v1 ? sizeof(k_v1_protocol_info) : sizeof(k_v2_protocol_info_both_tables);
    Session session;
    Device device = {0};
    PacketLog packets = {0};
    MDREvent event;
    unsigned char sequence;
    unsigned char retry_sequence = 0;
    clock_t start;

    if (!session_open_family(&session, family))
        return;
    mdrDebugSetPacketCallback(session.headphones, observe_packet, &packets);
    device.transport = &session.transport;
    device.protocol_v1 = v1;
    device.block_ack = 1;
    check_result(mdrHeadphonesSetTimeout(session.headphones, MDR_TIMEOUT_ACK, 20), MDR_RESULT_OK,
                 "short ACK timeout sets");
    check_result(mdrHeadphonesRequestInit(session.headphones, MDR_HEADPHONES_INIT_IDENTITY), MDR_RESULT_OK,
                 "identity initialization starts");
    poll_event(session.headphones, &event, "first protocol GET flushes");
    device_pump(&device);
    sequence = device.held_seq;
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, protocol, protocol_size);
    device_run(&session, &device, MDR_EVENT_IDENTITY_CHANGED, "RET arrives before the missing ACK");

    start = clock();
    while (device.log_size < 2 && clock() - start < CLOCKS_PER_SEC)
    {
        if (!poll_event(session.headphones, &event, "waiting for an ACK retry"))
            break;
        device_pump(&device);
    }
    check(device.log_size >= 2 && device.log[1].command == 0x00, "missing ACK retransmits the protocol GET");
    check(last_tx_sequence(&session.transport, &retry_sequence) && retry_sequence == sequence,
          "retransmission keeps the same wire sequence");
    check_result(mdrHeadphonesSetTimeout(session.headphones, MDR_TIMEOUT_ACK, 1000), MDR_RESULT_OK,
                 "ACK timeout restores");
    device.block_ack = 0;
    device_ack(&device, sequence);
    device_run(&session, &device, MDR_EVENT_INITIALIZE_COMPLETE, "retry preserves the earlier RET generation");
    check(device_requested(&device, 1, 0x02, -1), "the cached RET advances past protocol discovery after the ACK");
    session_close(&session);
}

static void test_support_reply_timeout(MDRProtocolVersion family)
{
    Session session;
    Device device = {0};
    MDREvent event;
    MDRResult result = MDR_RESULT_OK;
    size_t request_count;
    clock_t start;

    if (!session_open_family(&session, family))
        return;
    device.transport = &session.transport;
    device.protocol_v1 = family == MDR_PROTOCOL_V1;
    device_run_init_flags(&session, &device, MDR_HEADPHONES_INIT_IDENTITY);
    check_result(mdrHeadphonesSetTimeout(session.headphones, MDR_TIMEOUT_DEFAULT, 1), MDR_RESULT_OK,
                 "short reply timeout sets");
    device.block_ack = 1;
    check_result(mdrHeadphonesRequestInit(session.headphones, MDR_HEADPHONES_INIT_SUPPORT_FUNCTIONS), MDR_RESULT_OK,
                 "support request starts");
    poll_event(session.headphones, &event, "support GET flushes");
    device_pump(&device);
    request_count = device.log_size;
    device_ack(&device, device.held_seq);
    start = clock();
    while (result == MDR_RESULT_OK && clock() - start < CLOCKS_PER_SEC)
    {
        result = mdrHeadphonesPoll(session.headphones, &event);
        device_pump(&device);
    }
    check_result(result, family == MDR_PROTOCOL_V1 ? MDR_RESULT_ERROR_NOT_SUPPORTED : MDR_RESULT_ERROR_TIMEOUT,
                 "a missing support RET preserves the protocol-specific initialization error");
    check(device.log_size == request_count, "a timed out table 1 request never starts table 2");
    check(mdrHeadphonesIsInitialized(session.headphones) == MDR_FALSE,
          "reply timeout does not mark discovery complete");
    session_close(&session);
}

/* Whether a request of that shape was transmitted at or after `from`. */
static int device_requested_after(const Device* device, unsigned char table, unsigned char command, size_t from)
{
    size_t index;

    for (index = from; index < device->log_size; ++index)
        if (device->log[index].table == table && device->log[index].command == command)
            return 1;
    return 0;
}

/* Track, album and artist names asked for by a V1 sync, which names each one separately. */
static int sync_asked_for_names(const Device* device, size_t from)
{
    size_t index;
    int track = 0;
    int album = 0;
    int artist = 0;

    for (index = from; index < device->log_size; ++index)
    {
        const RequestLog* entry = &device->log[index];
        if (entry->table != 1 || entry->command != 0xa6 || !entry->has_detail) /* PLAY_GET_PARAM */
            continue;
        track = track || entry->detail == 0x00; /* TRACK_NAME */
        album = album || entry->detail == 0x01; /* ALBUM_NAME */
        artist = artist || entry->detail == 0x02; /* ARTIST_NAME */
    }
    return track && album && artist;
}

/*
 * A sync is the caller's way of saying "ask about everything again", and RequestSyncV1 asked
 * for nothing at all - it returned completion without sending a command. Most of the state
 * survives that, because a headset announces its own changes, but the playback metadata does
 * not: the phone hands the headset a new track name over its own channel and says nothing on
 * the control link, so a name read back after the first track is the one initialization got.
 */
static void test_v1_sync_asks_for_the_track_names(void)
{
    /* PLAYBACK_CONTROLLER only. */
    static const unsigned char table1[] = {0x07, 0x00, 0x01, 0xa1};

    Session session;
    Device device;
    size_t after_init;

    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = 1;
    device.table1 = table1;
    device.table1_size = sizeof(table1);

    device_run_init(&session, &device);

    /* Initialization asks for the names once; only what a sync asks for counts here. */
    after_init = device.log_size;
    check_result(mdrHeadphonesRequestSync(session.headphones, MDR_HEADPHONES_SYNC_BATTERY), MDR_RESULT_OK,
                 "a sync without playback starts");
    device_run(&session, &device, MDR_EVENT_SYNC_COMPLETE, "the sync without playback completes");
    check(!device_requested_after(&device, 1, 0xa6, after_init), /* PLAY_GET_PARAM */
          "a V1 sync without playback skips the track names");
    after_init = device.log_size;
    check_result(mdrHeadphonesRequestSync(session.headphones, MDR_HEADPHONES_SYNC_PLAYBACK), MDR_RESULT_OK,
                 "a sync starts");
    device_run(&session, &device, MDR_EVENT_SYNC_COMPLETE, "the sync completes");
    check(sync_asked_for_names(&device, after_init), "a V1 sync asks for the track names again");
    session_close(&session);
}

/* The same gap on the V2 side, where the sync asked for battery and safe listening only. */
static void test_v2_sync_asks_for_the_track_names(void)
{
    /* PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT only. */
    static const unsigned char table1[] = {0x07, 0x00, 0x01, 0xa1, 0xff};
    static const unsigned char table2[] = {0x07, 0x00, 0x00};

    Session session;
    Device device;
    size_t after_init;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);

    device_run_init(&session, &device);

    after_init = device.log_size;
    check_result(mdrHeadphonesRequestSync(session.headphones, MDR_HEADPHONES_SYNC_PLAYBACK), MDR_RESULT_OK,
                 "a sync starts");
    device_run(&session, &device, MDR_EVENT_SYNC_COMPLETE, "the sync completes");
    /* V2 names no detail: one PLAY_GET_PARAM for the playback control brings the metadata
     * back with it, which is why this asks only that the request was made again. */
    check(device_requested_after(&device, 1, 0xa6, after_init), /* PLAY_GET_PARAM */
          "a V2 sync asks for the playback state again");
    session_close(&session);
}

/*
 * The proximity sensor lives on table 2 as WEARING_STATUS_CHECKER, and a WH-1000XM6 answers
 * it without ever advertising it: table 1 lists the pause-when-removed function, table 2
 * lists nothing at all. It also never pushes the checker on its own - the transitions
 * arrive as unitRemove / unitWear operation-log entries, and those are what have to send
 * the reader back for the status.
 */
static void test_wearing_status(void)
{
    /* PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF only, and an empty table 2. */
    static const unsigned char table1[] = {0x07, 0x00, 0x01, 0xf1, 0x00};
    static const unsigned char table2[] = {0x07, 0x00, 0x00};
    /* SYSTEM_RET_STATUS, WEARING_STATUS_CHECKER, BOTH_NOT_WEAR */
    static const unsigned char ret_removed[] = {0xf3, 0x00, 0x04};
    /* SYSTEM_NTFY_STATUS, WEARING_STATUS_CHECKER, NORMAL */
    static const unsigned char ntfy_worn[] = {0xf5, 0x00, 0x00};
    /* LOG_NTFY_PARAM, TIME_SERIES_OPERATIONLOG_NOTIFIER, "unitRemove" and no value. */
    static const unsigned char log_remove[] = {0xc9, 0x01, 0x0a, 'u', 'n', 'i',  't', 'R',
                                               'e',  'm',  'o',  'v', 'e', 0x00, 0x00};

    Session session;
    Device device;
    MDRWearingStatus status;
    MDRFeatureAvailability available = MDR_AVAILABILITY_UNKNOWN;
    MDRHeadphonesSyncFlags sync;
    size_t after_log;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);

    device_run_init(&session, &device);
    check(device_requested(&device, 2, 0xf2, 0x00), /* SYSTEM_GET_STATUS, WEARING_STATUS_CHECKER */
          "the checker is asked for where only pause-when-removed is advertised");
    check_result(mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_WEARING_STATUS, &available), MDR_RESULT_OK,
                 "the wearing status feature id is within the accepted range");
    check(available == MDR_AVAILABILITY_UNAVAILABLE, "an unadvertised checker counts as unavailable until it answers");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, ret_removed, sizeof(ret_removed));
    device_run(&session, &device, MDR_EVENT_WEARING_STATUS_CHANGED, "the checker reply polls");
    status = MDR_WEARING_STATUS_UNKNOWN;
    check_result(mdrHeadphonesGetWearingStatus(session.headphones, &status), MDR_RESULT_OK,
                 "the wearing status is readable");
    check(status == MDR_WEARING_STATUS_REMOVED, "BOTH_NOT_WEAR reads as removed");
    mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_WEARING_STATUS, &available);
    check(available == MDR_AVAILABILITY_AVAILABLE,
          "a reply makes the wearing status available on a device that never advertised it");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, ntfy_worn, sizeof(ntfy_worn));
    device_run(&session, &device, MDR_EVENT_WEARING_STATUS_CHANGED, "the checker notification polls");
    mdrHeadphonesGetWearingStatus(session.headphones, &status);
    check(status == MDR_WEARING_STATUS_WORN, "NORMAL reads as worn");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, log_remove, sizeof(log_remove));
    device_run(&session, &device, MDR_EVENT_NEED_SYNC, "a unitRemove log entry polls");
    sync = MDR_HEADPHONES_SYNC_NONE;
    check_result(mdrHeadphonesGetSync(session.headphones, &sync), MDR_RESULT_OK, "the sync flags are readable");
    check(sync == MDR_HEADPHONES_SYNC_WEARING_STATUS, "a unitRemove log entry marks only the wearing status stale");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, ntfy_worn, sizeof(ntfy_worn));
    device_run(&session, &device, MDR_EVENT_WEARING_STATUS_CHANGED, "the checker notification polls again");
    mdrHeadphonesGetSync(session.headphones, &sync);
    check(sync == MDR_HEADPHONES_SYNC_NONE, "a wearing status report clears its sync flag");
    check_result(mdrHeadphonesRequestSync(session.headphones, MDR_HEADPHONES_SYNC_NONE),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "empty sync mask is rejected");
    check_result(mdrHeadphonesRequestSync(session.headphones, (MDRHeadphonesSyncFlags)(1u << 31u)),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "unknown sync flags are rejected");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, log_remove, sizeof(log_remove));
    device_run(&session, &device, MDR_EVENT_NEED_SYNC, "a second unitRemove log entry polls");
    mdrHeadphonesGetSync(session.headphones, &sync);

    after_log = device.log_size;
    check_result(mdrHeadphonesRequestSync(session.headphones, sync), MDR_RESULT_OK,
                 "the sync a removal asks for starts");
    device_run(&session, &device, MDR_EVENT_SYNC_COMPLETE, "the sync completes");
    check(device_requested_after(&device, 2, 0xf2, after_log), /* SYSTEM_GET_STATUS */
          "a sync re-reads the wearing status");
    session_close(&session);
}

static void check_assignable_view(MDRHeadphones* headphones, MDRPropertyType type, const MDRAssignableControl* expected,
                                  uint32_t expected_count)
{
    MDRAssignableControl controls[3];
    uint32_t count = 1, i, j;

    check_result(mdrHeadphonesGetAssignableControls(headphones, type, NULL, &count), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "assignable size query requires zero count");
    count = 0;
    check_result(mdrHeadphonesGetAssignableControls(headphones, type, NULL, &count), MDR_RESULT_OK,
                 "selected assignable size queries");
    check(count == expected_count, "selected assignment count follows its preset array");
    memset(controls, 0, sizeof(controls));
    controls[0].action = MDR_ASSIGNABLE_UNKNOWN;
    count = expected_count - 1;
    check_result(mdrHeadphonesGetAssignableControls(headphones, type, controls, &count),
                 MDR_RESULT_ERROR_BUFFER_TOO_SMALL, "selected assignment detects short buffer");
    check(count == expected_count && controls[0].action == MDR_ASSIGNABLE_UNKNOWN,
          "short assignment buffer reports count without copying");
    count = (uint32_t)(sizeof(controls) / sizeof(controls[0]));
    check_result(mdrHeadphonesGetAssignableControls(headphones, type, controls, &count), MDR_RESULT_OK,
                 "selected assignments copy");
    check(count == expected_count, "selected assignments return element count");
    for (i = 0; i < count && i < 3; ++i)
    {
        for (j = 0; j < expected_count; ++j)
            if (expected[j].location == controls[i].location)
                break;
        check(j != expected_count, "key identity is always from capability");
        if (j != expected_count)
            check(controls[i].type == expected[j].type && controls[i].action == expected[j].action,
                  "selected action and capability key type retain their distinct sources");
    }
}

static void test_v2_assignable_controls(int limited)
{
    /* Non-left/right capability order, three keys, and distinct V2 preset variants. */
    static const unsigned char normal_capability[] = {
        0xf1, 0x03, 3,    1,    0, 0xff, 2, 0x22, 1, 0, 0,    0x20, 0xff, 1, 0, 0, 0,    2,    1, 0xff, 2, 0x30, 1,
        0,    0,    0x30, 0xff, 1, 0,    0, 0,    0, 0, 0xff, 2,    0x10, 1, 0, 0, 0x23, 0xff, 1, 0,    0, 0};
    /* One C button; BT Classic caution preset must not collapse to its plain variant. */
    static const unsigned char limited_capability[] = {0xf1, 0x0e, 0, 1, 3,    1, 0xff, 2, 0x43,
                                                       1,    0,    0, 2, 0xff, 1, 0,    0, 0};
    static const unsigned char normal_params[] = {0xf7, 0x03, 3, 0xff, 0xff, 0xff};
    static const unsigned char limited_params[] = {0xf7, 0x0e, 1, 0xff};
    const unsigned char support[] = {0x07, 0x00, 1, (unsigned char)(limited ? 0xfe : 0xf3), 0};
    static const unsigned char table2[] = {0x07, 0x00, 0};
    const unsigned char notification[] = {0xf9,
                                          (unsigned char)(limited ? 0x0e : 0x03),
                                          (unsigned char)(limited ? 1 : 3),
                                          (unsigned char)(limited ? 0x43 : 0x22),
                                          0x30,
                                          0x10};
    const MDRAssignableControl desired[] = {
        {MDR_ASSIGNABLE_ACTION_KEY_LEFT, MDR_ASSIGNABLE_ACTION_KEY_TYPE_TOUCH_SENSOR, MDR_ASSIGNABLE_VOLUME},
        {MDR_ASSIGNABLE_ACTION_KEY_CUSTOM, MDR_ASSIGNABLE_ACTION_KEY_TYPE_BUTTON, MDR_ASSIGNABLE_VOICE_RECOGNITION},
        {MDR_ASSIGNABLE_ACTION_KEY_RIGHT, MDR_ASSIGNABLE_ACTION_KEY_TYPE_TOUCH_SENSOR,
         MDR_ASSIGNABLE_PLAYBACK_VOICE_ASSISTANT_LIMITATION}};
    Session session;
    Device device;
    MDRAssignableControl controls[3], input[3];
    MDRAssignableAction options[2];
    MDRFeatureAvailability availability;
    uint32_t count, option_count;
    size_t offset;
    TxFrame frame;
    int found = 0;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = support;
    device.table1_size = sizeof(support);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device.assignable_capability = limited ? limited_capability : normal_capability;
    device.assignable_capability_size = limited ? sizeof(limited_capability) : sizeof(normal_capability);
    device.assignable_params = limited ? limited_params : normal_params;
    device.assignable_params_size = limited ? sizeof(limited_params) : sizeof(normal_params);
    device_run_init(&session, &device);

    check_result(mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_ASSIGNABLE_CONTROLS, &availability),
                 MDR_RESULT_OK, "assignable feature queries");
    check(availability == MDR_AVAILABILITY_AVAILABLE, "normal and limited controls are available");
    count = 0;
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, MDR_PROPERTY_CURRENT, NULL, &count),
                 MDR_RESULT_OK, "capability-driven control count queries");
    check(count == (limited ? 1u : 3u), "control count follows advertised keys");
    count = 0;
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, MDR_PROPERTY_CURRENT, controls, &count),
                 MDR_RESULT_ERROR_BUFFER_TOO_SMALL, "control copy detects short buffer");
    check(count == (limited ? 1u : 3u), "short control buffer reports required count");
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, MDR_PROPERTY_CURRENT, controls, &count),
                 MDR_RESULT_OK, "controls read after initialization");
    check(controls[0].location == (limited ? MDR_ASSIGNABLE_ACTION_KEY_C : MDR_ASSIGNABLE_ACTION_KEY_RIGHT),
          "controls retain capability order and key identity");
    check(controls[0].type ==
              (limited ? MDR_ASSIGNABLE_ACTION_KEY_TYPE_BUTTON : MDR_ASSIGNABLE_ACTION_KEY_TYPE_TOUCH_SENSOR),
          "controls expose advertised key type");
    check(controls[0].action == MDR_ASSIGNABLE_NONE, "parameter response supplies current assignment");
    option_count = 2;
    check_result(
        mdrHeadphonesGetAssignableControlActions(session.headphones, controls[0].location, options, &option_count),
        MDR_RESULT_OK, "key-specific presets read");
    check(option_count == 2 &&
              options[0] ==
                  (limited ? MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS_BT_CLASSIC_CAUTION
                           : MDR_ASSIGNABLE_PLAYBACK_VOICE_ASSISTANT_LIMITATION) &&
              options[1] == MDR_ASSIGNABLE_NONE,
          "distinct V2 variants and NONE retain their identities");

    if (limited)
    {
        input[0] = controls[0];
        input[0].action = MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS_BT_CLASSIC_CAUTION;
    }
    else
        memcpy(input, desired, sizeof(input));
    /* A valid earlier entry must not be staged when a later entry is invalid. */
    input[count - 1].action = MDR_ASSIGNABLE_GOOGLE_ASSISTANT;
    check_result(mdrHeadphonesSetAssignableControls(session.headphones, input, count),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "unadvertised action is rejected transactionally");
    check(!mdrHeadphonesIsDirty(session.headphones), "invalid assignment leaves no staged state");
    if (limited)
        input[0].action = MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS_BT_CLASSIC_CAUTION;
    else
        memcpy(input, desired, sizeof(input));
    input[0].type = MDR_ASSIGNABLE_ACTION_KEY_TYPE_UNKNOWN;
    check_result(mdrHeadphonesSetAssignableControls(session.headphones, input, count),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "wrong key type is rejected");
    check(!mdrHeadphonesIsDirty(session.headphones), "wrong type leaves no staged state");
    input[0].type = limited ? MDR_ASSIGNABLE_ACTION_KEY_TYPE_BUTTON : MDR_ASSIGNABLE_ACTION_KEY_TYPE_TOUCH_SENSOR;
    if (!limited)
    {
        input[1] = input[0];
        check_result(mdrHeadphonesSetAssignableControls(session.headphones, input, count),
                     MDR_RESULT_ERROR_INVALID_ARGUMENT, "duplicate key cannot replace a missing key");
        check(!mdrHeadphonesIsDirty(session.headphones), "duplicate keys leave no staged state");
        memcpy(input, desired, sizeof(input));
    }
    offset = session.transport.tx_size;
    check_assignable_view(session.headphones, MDR_PROPERTY_CURRENT, controls, count);
    check_assignable_view(session.headphones, MDR_PROPERTY_DESIRED, controls, count);
    check_result(mdrHeadphonesSetAssignableControls(session.headphones, input, count), MDR_RESULT_OK,
                 "advertised actions stage in arbitrary caller order");
    check_assignable_view(session.headphones, MDR_PROPERTY_CURRENT, controls, count);
    check_assignable_view(session.headphones, MDR_PROPERTY_DESIRED, input, count);
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, device.assignable_params, device.assignable_params_size);
    device_run(&session, &device, MDR_EVENT_ASSIGNABLE_CONTROLS_CHANGED,
               "assignment notification with local edits polls");
    check_assignable_view(session.headphones, MDR_PROPERTY_DESIRED, input, count);
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "assignment apply starts");
    check_result(mdrHeadphonesSetAssignableControls(session.headphones, controls, count), MDR_RESULT_OK,
                 "busy assignment Set stages a new array");
    check_assignable_view(session.headphones, MDR_PROPERTY_DESIRED, controls, count);
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "assignment apply completes");
    check_assignable_view(session.headphones, MDR_PROPERTY_CURRENT, controls, count);
    check_assignable_view(session.headphones, MDR_PROPERTY_DESIRED, controls, count);
    check(mdrHeadphonesIsDirty(session.headphones), "old assignment ACK preserves the newly staged array");
    while (next_tx_frame(&session.transport, &offset, &frame))
    {
        if (frame.type == MDR_DATA_TYPE_DATA_MDR && frame.payload[0] == 0xf8)
        {
            found = 1;
            check(frame.payload_size == (limited ? 4u : 6u) && frame.payload[1] == (limited ? 0x0e : 0x03) &&
                      frame.payload[2] == count && frame.payload[3] == (limited ? 0x43 : 0x22) &&
                      (limited || (frame.payload[4] == 0x30 && frame.payload[5] == 0x10)),
                  "assignment serialization preserves capability order and exact wire presets");
        }
    }
    check(found, "assignment commit transmits a settings update");
    {
        const unsigned char reset[] = {
            0xf8, (unsigned char)(limited ? 0x0e : 0x03), (unsigned char)(limited ? 1 : 3), 0xff, 0xff, 0xff};
        offset = session.transport.tx_size;
        check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "new assignment array starts");
        device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "new assignment array completes");
        check_command_payload(&session, offset, reset, limited ? 4u : sizeof(reset));
        check(!mdrHeadphonesIsDirty(session.headphones), "second assignment ACK consumes the new array");
    }
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, notification, limited ? 4u : sizeof(notification));
    device_run(&session, &device, MDR_EVENT_ASSIGNABLE_CONTROLS_CHANGED, "assignment notification emits its own event");
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, MDR_PROPERTY_CURRENT, controls, &count),
                 MDR_RESULT_OK, "notified assignments read");
    check(controls[0].action == options[0], "notification preserves distinct preset identity");
    session_close(&session);
}

static void test_v1_assignable_controls(void)
{
    /* Two buttons in CUSTOM, LEFT order, rather than implicit left/right indices. */
    static const unsigned char support[] = {0x07, 0, 1, 0xf6};
    static const unsigned char table2[] = {0x07, 0, 0};
    static const unsigned char capability[] = {0xf1, 6, 2, 2,    1, 0,    2, 0, 1,    0,    2, 0x30, 1, 0,
                                               0x30, 0, 1, 0x20, 2, 0x20, 1, 0, 0x20, 0xff, 1, 0,    0};
    static const unsigned char params[] = {0xf7, 6, 2, 0, 0x20};
    static const unsigned char notification[] = {0xf9, 6, 2, 0x30, 0xff};
    Session session;
    Device device;
    MDRAssignableControl controls[2], desired[2];
    MDRAssignableAction options[2];
    uint32_t count = 2, option_count = 2;
    TxFrame frame;
    size_t offset;
    int found = 0;

    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = 1;
    device.table1 = support;
    device.table1_size = sizeof(support);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device.assignable_capability = capability;
    device.assignable_capability_size = sizeof(capability);
    device.assignable_params = params;
    device.assignable_params_size = sizeof(params);
    device_run_init(&session, &device);
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, MDR_PROPERTY_CURRENT, controls, &count),
                 MDR_RESULT_OK, "V1 capability controls read");
    check(count == 2 && controls[0].location == MDR_ASSIGNABLE_ACTION_KEY_CUSTOM &&
              controls[0].type == MDR_ASSIGNABLE_ACTION_KEY_TYPE_BUTTON &&
              controls[0].action == MDR_ASSIGNABLE_NOISE_CONTROL &&
              controls[1].location == MDR_ASSIGNABLE_ACTION_KEY_LEFT && controls[1].action == MDR_ASSIGNABLE_PLAYBACK,
          "V1 key order, type, and current preset survive");
    check_result(
        mdrHeadphonesGetAssignableControlActions(session.headphones, controls[0].location, options, &option_count),
        MDR_RESULT_OK, "V1 per-key actions read");
    check(option_count == 2 && options[0] == MDR_ASSIGNABLE_NOISE_CONTROL &&
              options[1] == MDR_ASSIGNABLE_VOICE_RECOGNITION,
          "V1 actions follow each key capability");
    desired[0] = controls[1];
    desired[1] = controls[0];
    desired[0].action = MDR_ASSIGNABLE_NONE;
    desired[1].action = MDR_ASSIGNABLE_GOOGLE_ASSISTANT;
    check_result(mdrHeadphonesSetAssignableControls(session.headphones, desired, 2), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "V1 rejects an unadvertised action after a valid entry");
    check(!mdrHeadphonesIsDirty(session.headphones), "V1 rejected batch leaves no staged assignments");
    desired[1].action = MDR_ASSIGNABLE_VOICE_RECOGNITION;
    offset = session.transport.tx_size;
    check_assignable_view(session.headphones, MDR_PROPERTY_CURRENT, controls, 2);
    check_assignable_view(session.headphones, MDR_PROPERTY_DESIRED, controls, 2);
    check_result(mdrHeadphonesSetAssignableControls(session.headphones, desired, 2), MDR_RESULT_OK,
                 "V1 accepts reversed caller order");
    check_assignable_view(session.headphones, MDR_PROPERTY_CURRENT, controls, 2);
    check_assignable_view(session.headphones, MDR_PROPERTY_DESIRED, desired, 2);
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, params, sizeof(params));
    device_run(&session, &device, MDR_EVENT_ASSIGNABLE_CONTROLS_CHANGED,
               "V1 assignment notification with local edits polls");
    check_assignable_view(session.headphones, MDR_PROPERTY_DESIRED, desired, 2);
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "V1 assignment apply starts");
    check_result(mdrHeadphonesSetAssignableControls(session.headphones, controls, 2), MDR_RESULT_OK,
                 "busy V1 assignment Set stages a new array");
    check_assignable_view(session.headphones, MDR_PROPERTY_DESIRED, controls, 2);
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "V1 assignment apply completes");
    check_assignable_view(session.headphones, MDR_PROPERTY_CURRENT, controls, 2);
    check_assignable_view(session.headphones, MDR_PROPERTY_DESIRED, controls, 2);
    check(mdrHeadphonesIsDirty(session.headphones), "old V1 assignment ACK preserves the newly staged array");
    while (next_tx_frame(&session.transport, &offset, &frame))
    {
        if (frame.type == MDR_DATA_TYPE_DATA_MDR && frame.payload[0] == 0xf8)
        {
            found = 1;
            check(frame.payload_size == 5 && frame.payload[1] == 6 && frame.payload[2] == 2 &&
                      frame.payload[3] == 0x30 && frame.payload[4] == 0xff,
                  "V1 assignment writes in capability order, not caller order");
        }
    }
    check(found, "V1 assignment reaches the transport");
    {
        const unsigned char reset[] = {0xf8, 6, 2, 0, 0x20};
        offset = session.transport.tx_size;
        check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "new V1 assignment array starts");
        device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "new V1 assignment array completes");
        check_command_payload(&session, offset, reset, sizeof(reset));
        check(!mdrHeadphonesIsDirty(session.headphones), "second V1 assignment ACK consumes the new array");
    }
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, notification, sizeof(notification));
    device_run(&session, &device, MDR_EVENT_ASSIGNABLE_CONTROLS_CHANGED, "V1 assignment notification polls");
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, MDR_PROPERTY_CURRENT, controls, &count),
                 MDR_RESULT_OK, "V1 notified assignments read");
    check(controls[0].action == MDR_ASSIGNABLE_VOICE_RECOGNITION && controls[1].action == MDR_ASSIGNABLE_NONE,
          "V1 notification updates both assignments");
    session_close(&session);
}

static void check_value(MDRHeadphones* headphones, MDRValue key, uint32_t index, int32_t expected, const char* message)
{
    int32_t value = INT32_MIN;
    check_result(mdrHeadphonesGetValue(headphones, key, index, &value), MDR_RESULT_OK, message);
    check(value == expected, message);
}

static void check_range(MDRHeadphones* headphones, MDRValue min_key, uint32_t index, int32_t min, int32_t max,
                        const char* message)
{
    check_value(headphones, min_key, index, min, message);
    check_value(headphones, min_key + 1, index, max, message);
    check_value(headphones, min_key + 2, index, 1, message);
}

/*
 * Device limits come from the capability replies where the device sends them, and from the protocol defaults
 * until then. The Set* calls validate against the same values. The capability frames marked XM3 are verbatim from
 * the WH-1000XM3-4.5.2 capture; the others change one field to show that the value is read, not assumed.
 */
static void test_v1_device_values(void)
{
    /* BATTERY_LEVEL, NC/ASM, PRESET_EQ, PLAYBACK_CONTROLLER, AUTO_POWER_OFF, from the XM3's own table */
    static const unsigned char support[] = {0x07, 0, 5, 0x11, 0x62, 0x51, 0xa1, 0xf4};
    static const unsigned char table2[] = {0x07, 0, 0};
    /* XM3: PRESET_EQ, 6 bands (Clear Bass + 5) of 21 steps, 12 unnamed presets */
    static const unsigned char eq_xm3[] = {0x51, 0x01, 0x06, 0x15, 0x0c, 0x00, 0x00, 0x10, 0x00, 0x11,
                                           0x00, 0x12, 0x00, 0x13, 0x00, 0x14, 0x00, 0x15, 0x00, 0x16,
                                           0x00, 0x17, 0x00, 0xa0, 0x00, 0xa1, 0x00, 0xa2, 0x00};
    /* 13 steps (-6..6), no presets */
    static const unsigned char eq_13[] = {0x51, 0x01, 0x06, 0x0d, 0x00};
    /* EQEBB_RET_PARAM: Clear Bass 0, bands +2 0 0 0 0 at offset 6 */
    static const unsigned char eq_param[] = {0x57, 0x01, 0xa0, 0x06, 0x06, 0x08, 0x06, 0x06, 0x06, 0x06};
    /* SetEqEbbParam for Clear Bass 6, bands 6 -6 0 0 0 at offset 6 */
    static const unsigned char eq_write[] = {0x58, 0x01, 0xff, 0x06, 12, 12, 0, 6, 6, 6};
    /* XM3: volumeStep 31 (0..30) */
    static const unsigned char play_xm3[] = {0xa1, 0x01, 0x1f, 0x01, 0x01};
    static const unsigned char play_16[] = {0xa1, 0x01, 0x10, 0x01, 0x01};
    /* XM3: 5, 30, 60, 180 min and off; no 15 */
    static const unsigned char power_xm3[] = {0xf1, 0x04, 0x05, 0x00, 0x01, 0x02, 0x03, 0x11};
    /* 30 min, WHEN_REMOVED_FROM_EARS (no duration), and off */
    static const unsigned char power_short[] = {0xf1, 0x04, 0x03, 0x01, 0x10, 0x11};
    /* XM3: 100%, not charging */
    static const unsigned char battery_xm3[] = {0x11, 0x00, 0x64, 0x00};
    static const int32_t bands_ok[5] = {6, -6, 0, 0, 0};
    static const int32_t bands_wide[5] = {7, 0, 0, 0, 0};
    Session session;
    Device device;
    MDRBattery battery;
    MDRPower power;
    int32_t value = 0;
    int32_t bands[5];
    uint32_t count;
    size_t offset;

    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_PLAYBACK_VOLUME_MAX, 0, &value),
                 MDR_RESULT_INPROGRESS, "limits are not known before initialization");
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = 1;
    device.table1 = support;
    device.table1_size = sizeof(support);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);

    check_result(mdrHeadphonesGetValue(NULL, MDR_VALUE_PLAYBACK_VOLUME_MAX, 0, &value),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "a null handle is refused");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_PLAYBACK_VOLUME_MAX, 0, NULL),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "a null output is refused");
    check_result(mdrHeadphonesGetValue(session.headphones, 0, 0, &value), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "key 0 is not a key");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_GENERAL_SETTING_OPTION_TEXT_IS_KEY + 1, 0, &value),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "a key past the last one is refused");

    /* Defaults, before any capability reply */
    check_range(session.headphones, MDR_VALUE_PLAYBACK_VOLUME_MIN, 0, 0, 30, "default volume range");
    check_range(session.headphones, MDR_VALUE_AMBIENT_LEVEL_MIN, 0, 1, 20, "default ambient range");
    check_range(session.headphones, MDR_VALUE_EQUALIZER_BAND_MIN, 5, -10, 10, "default 5-band range");
    check_range(session.headphones, MDR_VALUE_EQUALIZER_BAND_MIN, 10, -6, 6, "default 10-band range");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_EQUALIZER_BAND_MIN, 7, &value),
                 MDR_RESULT_ERROR_NOT_FOUND, "an unknown band count has no range");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_EQUALIZER_CLEAR_BASS_MIN, 0, &value),
                 MDR_RESULT_ERROR_NOT_SUPPORTED, "no Clear Bass before the 5-band layout is known");
    check_result(mdrHeadphonesSetEqualizerClearBass(session.headphones, 0), MDR_RESULT_ERROR_NOT_SUPPORTED,
                 "Clear Bass setter also requires a known supported layout");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_VOICE_GUIDANCE_VOLUME_MIN, 0, &value),
                 MDR_RESULT_ERROR_NOT_SUPPORTED, "V1 has no voice guidance volume");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION_COUNT, 0, 5, "default V1 auto power off count");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION, 1, 30, "default V1 list has no 15 min");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION, 5, &value),
                 MDR_RESULT_ERROR_NOT_FOUND, "an option past the end is not found");

    /* Battery */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, battery_xm3, sizeof(battery_xm3));
    device_run(&session, &device, MDR_EVENT_BATTERY_CHANGED, "XM3 battery polls");
    count = 1;
    check_result(mdrHeadphonesGetBatteries(session.headphones, &battery, &count), MDR_RESULT_OK, "battery reads");
    check(count == 1 && battery.part == MDR_BATTERY_MAIN && battery.level_percent == 100,
          "XM3 battery decodes as one main part at 100%");

    /* Playback volume */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, play_xm3, sizeof(play_xm3));
    device_run(&session, &device, MDR_EVENT_PLAYBACK_CHANGED, "XM3 play capability polls");
    check_range(session.headphones, MDR_VALUE_PLAYBACK_VOLUME_MIN, 0, 0, 30, "XM3 volumeStep 31 is 0..30");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, play_16, sizeof(play_16));
    device_run(&session, &device, MDR_EVENT_PLAYBACK_CHANGED, "short play capability polls");
    check_range(session.headphones, MDR_VALUE_PLAYBACK_VOLUME_MIN, 0, 0, 15, "volumeStep 16 is 0..15");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 16), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "volume past the advertised maximum is refused");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 15), MDR_RESULT_OK,
                 "the advertised maximum is accepted");

    /* Auto power off */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, power_xm3, sizeof(power_xm3));
    device_run(&session, &device, MDR_EVENT_POWER_CHANGED, "XM3 auto power off capability polls");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION_COUNT, 0, 5, "XM3 lists five choices");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION, 0, 5, "XM3 lists 5 min first");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION, 4, 0, "XM3 lists off last");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, power_short, sizeof(power_short));
    device_run(&session, &device, MDR_EVENT_POWER_CHANGED, "short auto power off capability polls");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION_COUNT, 0, 2,
                "elements without a duration are skipped");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION, 0, 30, "30 min survives");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION, 1, 0, "off survives");
    check_result(mdrHeadphonesGetPower(session.headphones, MDR_PROPERTY_DESIRED, &power), MDR_RESULT_OK, "power reads");
    power.auto_power_off_minutes = 60;
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "a duration the device didn't list is refused");
    power.auto_power_off_minutes = 30;
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_OK, "a listed duration is accepted");

    /* Equalizer and Clear Bass */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, eq_xm3, sizeof(eq_xm3));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "XM3 EQ capability polls");
    check_range(session.headphones, MDR_VALUE_EQUALIZER_BAND_MIN, 5, -10, 10, "XM3 step 21 is -10..10");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, eq_13, sizeof(eq_13));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "13-step EQ capability polls");
    check_range(session.headphones, MDR_VALUE_EQUALIZER_BAND_MIN, 5, -6, 6, "step 13 is -6..6");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, eq_param, sizeof(eq_param));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "EQ param polls");
    count = 5;
    check_result(mdrHeadphonesGetEqualizerBands(session.headphones, MDR_PROPERTY_CURRENT, bands, &count), MDR_RESULT_OK,
                 "bands read");
    check(count == 5 && bands[0] == 2 && bands[1] == 0, "bands decode at the advertised offset");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_EQUALIZER_BAND_MIN, 10, &value),
                 MDR_RESULT_ERROR_NOT_FOUND, "a default layout the device doesn't use has no range");
    check_range(session.headphones, MDR_VALUE_EQUALIZER_CLEAR_BASS_MIN, 0, -6, 6, "Clear Bass shares the EQ step");
    check_result(mdrHeadphonesSetEqualizerClearBass(session.headphones, 7), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "Clear Bass past the advertised range is refused");
    check_result(mdrHeadphonesSetEqualizerBands(session.headphones, bands_wide, 5), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "a band past the advertised range is refused");
    check_result(mdrHeadphonesSetEqualizerClearBass(session.headphones, 6), MDR_RESULT_OK, "Clear Bass 6 stages");
    check_result(mdrHeadphonesSetEqualizerBands(session.headphones, bands_ok, 5), MDR_RESULT_OK, "bands stage");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "EQ apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "EQ apply completes");
    check_command_payload(&session, offset, eq_write, sizeof(eq_write));
    session_close(&session);
}

static void test_power_wearing_capability(MDRProtocolVersion family)
{
    static const unsigned char support_v1[] = {0x07, 0, 2, 0xf4, 0xf3};
    static const unsigned char support_v2[] = {0x07, 0, 2, 0x25, 0xff, 0xf1, 0xff};
    static const unsigned char table2[] = {0x07, 0, 0};
    /* XM4 (V1) and XM6 (V2): when removed and disabled, no timed options. */
    static const unsigned char cap_v1[] = {0xf1, 0x04, 2, 0x10, 0x11};
    static const unsigned char cap_v2[] = {0x21, 0x05, 2, 0x10, 0x11};
    static const unsigned char param_v1[] = {0xf7, 0x04, 0x01, 0x10, 0x00};
    static const unsigned char param_v2[] = {0x27, 0x05, 0x10, 0x00};
    static const unsigned char write_v1[] = {0xf8, 0x04, 0x01, 0x10, 0x00};
    static const unsigned char write_v2[] = {0x28, 0x05, 0x10, 0x00};
    static const unsigned char disable_v1[] = {0xf8, 0x04, 0x01, 0x11, 0x00};
    static const unsigned char disable_v2[] = {0x28, 0x05, 0x11, 0x00};
    static const unsigned char disabled_only_v1[] = {0xf1, 0x04, 1, 0x11};
    static const unsigned char disabled_only_v2[] = {0x21, 0x05, 1, 0x11};
    static const unsigned char empty_v2[] = {0x21, 0x05, 0};
    Session session;
    Device device;
    MDRPower power;
    int32_t value;
    size_t offset;
    int v1 = family == MDR_PROTOCOL_V1;

    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = v1;
    device.table1 = v1 ? support_v1 : support_v2;
    device.table1_size = v1 ? sizeof(support_v1) : sizeof(support_v2);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);
    check(device_requested(&device, 1, v1 ? 0xf0 : 0x20, v1 ? 0x04 : 0x05),
          "power initialization asks for the supported capability");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION_COUNT, 0, v1 ? 5 : 6,
                "missing capability retains the protocol fallback");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? cap_v1 : cap_v2, v1 ? sizeof(cap_v1) : sizeof(cap_v2));
    device_run(&session, &device, MDR_EVENT_POWER_CHANGED, "wearing power capability polls");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION_COUNT, 0, 1, "wearing-only list has no timers");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION, 0, 0, "disabled is the only numeric option");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION, 1, &value),
                 MDR_RESULT_ERROR_NOT_FOUND, "wearing-only list has no second numeric option");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? param_v1 : param_v2, v1 ? sizeof(param_v1) : sizeof(param_v2));
    device_run(&session, &device, MDR_EVENT_POWER_CHANGED, "when-removed power setting polls");
    check_result(mdrHeadphonesGetPower(session.headphones, MDR_PROPERTY_CURRENT, &power), MDR_RESULT_OK,
                 "when-removed power reads");
    check(power.auto_power_off_minutes == 0 && power.wearing_power == MDR_WEARING_POWER_WHEN_REMOVED,
          "when-removed state is not confused with disabled");
    power.auto_power_off_minutes = 5;
    power.auto_pause = MDR_TRUE;
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "unadvertised timer is rejected even while when-removed is selected");
    check(!mdrHeadphonesIsDirty(session.headphones), "invalid power request does not stage auto pause");
    power.auto_power_off_minutes = 0;
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_OK, "auto pause edit stages");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "power write-back starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "power write-back completes");
    check_command_payload(&session, offset, v1 ? write_v1 : write_v2, v1 ? sizeof(write_v1) : sizeof(write_v2));
    check_result(mdrHeadphonesGetPower(session.headphones, MDR_PROPERTY_DESIRED, &power), MDR_RESULT_OK,
                 "power after write-back reads");
    check(power.wearing_power == MDR_WEARING_POWER_WHEN_REMOVED, "auto pause edit retains when-removed power");

    power.wearing_power = MDR_WEARING_POWER_DISABLED;
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_OK, "wearing power disable stages");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "power disable starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "power disable completes");
    check_command_payload(&session, offset, v1 ? disable_v1 : disable_v2, v1 ? sizeof(disable_v1) : sizeof(disable_v2));
    power.wearing_power = MDR_WEARING_POWER_WHEN_REMOVED;
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_OK, "wearing power enable stages");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "power enable starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "power enable completes");
    check_command_payload(&session, offset, v1 ? write_v1 : write_v2, v1 ? sizeof(write_v1) : sizeof(write_v2));

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? disabled_only_v1 : disabled_only_v2,
                v1 ? sizeof(disabled_only_v1) : sizeof(disabled_only_v2));
    device_run(&session, &device, MDR_EVENT_POWER_CHANGED, "capability without when-removed polls");
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "when-removed is refused if absent from the capability");
    check_result(mdrHeadphonesGetPower(session.headphones, MDR_PROPERTY_DESIRED, &power), MDR_RESULT_OK,
                 "power with unadvertised current state reads");
    check(power.wearing_power == MDR_WEARING_POWER_UNAVAILABLE, "unadvertised wearing option is hidden");
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_OK,
                 "unchanged unadvertised raw state can be written back");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "unadvertised write-back starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "unadvertised write-back completes");
    check_command_payload(&session, offset, v1 ? write_v1 : write_v2, v1 ? sizeof(write_v1) : sizeof(write_v2));
    if (!v1)
    {
        device_send(&device, MDR_DATA_TYPE_DATA_MDR, empty_v2, sizeof(empty_v2));
        device_run(&session, &device, MDR_EVENT_POWER_CHANGED, "empty V2 power capability polls");
        check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION_COUNT, 0, 0,
                    "empty advertised list does not restore hardcoded timers");
    }
    session_close(&session);
}

static void test_v2_timed_power_capability(void)
{
    static const unsigned char table1[] = {0x07, 0, 1, 0x24, 0xff};
    static const unsigned char table2[] = {0x07, 0, 0};
    static const unsigned char capability[] = {0x21, 0x04, 4, 0x04, 0x01, 0x04, 0x11};
    static const unsigned char write[] = {0x28, 0x04, 0x04, 0x00};
    Session session;
    Device device;
    MDRPower power;
    size_t offset;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);
    check(device_requested(&device, 1, 0x20, 0x04), "timed power capability is requested");
    check(!device_requested(&device, 1, 0x20, 0x05), "unadvertised wearing variant is not requested");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, capability, sizeof(capability));
    device_run(&session, &device, MDR_EVENT_POWER_CHANGED, "timed power capability polls");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION_COUNT, 0, 3, "duplicate durations are omitted");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION, 0, 15,
                "V2 15-minute option follows capability order");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION, 1, 30,
                "V2 30-minute option follows capability order");
    check_value(session.headphones, MDR_VALUE_AUTO_POWER_OFF_OPTION, 2, 0,
                "V2 disabled option follows capability order");
    check_result(mdrHeadphonesGetPower(session.headphones, MDR_PROPERTY_DESIRED, &power), MDR_RESULT_OK,
                 "timed power reads");
    power.auto_power_off_minutes = 60;
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "unadvertised timed power option is refused");
    power.auto_power_off_minutes = 15;
    power.wearing_power = MDR_WEARING_POWER_WHEN_REMOVED;
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "timed-only device refuses when-removed power");
    power.wearing_power = MDR_WEARING_POWER_UNAVAILABLE;
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_OK, "advertised 15-minute timer stages");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "timed power apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "timed power apply completes");
    check_command_payload(&session, offset, write, sizeof(write));
    session_close(&session);
}

/*
 * V2 EQ layout comes from the device: band count and step from EQEBB_RET_CAPABILITY, frequencies from
 * EQEBB_RET_EXTENDED_INFO. Frames marked XM6 are verbatim from a WH-1000XM6 3.1.5 debugger session (unescaped).
 */
static void test_v2_equalizer_layout(void)
{
    static const unsigned char table1[] = {0x07, 0x00, 0x01, 0x50, 0xff /* PRESET_EQ */};
    static const unsigned char table2[] = {0x07, 0x00, 0x00};
    /* XM6: answered as PRESET_EQ_AND_ERRORCODE, 10 bands, 13 steps, 9 unnamed presets */
    static const unsigned char cap_xm6[] = {0x51, 0x04, 0x0a, 0x0d, 0x09, 0x00, 0x00, 0x30, 0x00, 0x31, 0x00, 0x32,
                                            0x00, 0x33, 0x00, 0x20, 0x00, 0xa0, 0x00, 0xa1, 0x00, 0xa2, 0x00};
    /* As XM6 with 15 steps */
    static const unsigned char cap_15[] = {0x51, 0x04, 0x0a, 0x0f, 0x00};
    /* XM6: preset OFF, all ten bands at 0 */
    static const unsigned char param_xm6[] = {0x57, 0x00, 0x00, 0x0a, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
    /* XM6: 10 x HZ, 31 .. 16000 */
    static const unsigned char info_xm6[] = {0x5b, 0x00, 0x0a, 0x01, 0x00, 0x1f, 0x01, 0x00, 0x3f, 0x01, 0x00,
                                             0x7d, 0x01, 0x00, 0xfa, 0x01, 0x01, 0xf4, 0x01, 0x03, 0xe8, 0x01,
                                             0x07, 0xd0, 0x01, 0x0f, 0xa0, 0x01, 0x1f, 0x40, 0x01, 0x3e, 0x80};
    /* As XM6 with the first band at 32 Hz */
    static const unsigned char info_32[] = {0x5b, 0x00, 0x0a, 0x01, 0x00, 0x20, 0x01, 0x00, 0x3f, 0x01, 0x00,
                                            0x7d, 0x01, 0x00, 0xfa, 0x01, 0x01, 0xf4, 0x01, 0x03, 0xe8, 0x01,
                                            0x07, 0xd0, 0x01, 0x0f, 0xa0, 0x01, 0x1f, 0x40, 0x01, 0x3e, 0x80};
    /* -7, 0 x 8, 7 at the 15-step offset of 7 */
    static const unsigned char eq_write[] = {0x58, 0x00, 0x00, 0x0a, 0, 7, 7, 7, 7, 7, 7, 7, 7, 14};
    static const int32_t bands_ok[10] = {-7, 0, 0, 0, 0, 0, 0, 0, 0, 7};
    static const int32_t bands_wide[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 8};
    Session session;
    Device device;
    MDREqualizer equalizer;
    int32_t value = 0;
    size_t offset;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);
    check(device_requested(&device, 1, 0x5a, 0x00), "band info is requested as PRESET_EQ");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, cap_xm6, sizeof(cap_xm6));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "XM6 EQ capability polls");
    check_range(session.headphones, MDR_VALUE_EQUALIZER_BAND_MIN, 10, -6, 6, "XM6 13 steps is -6..6");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, param_xm6, sizeof(param_xm6));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "XM6 EQ param polls");
    check_result(mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_CURRENT, &equalizer), MDR_RESULT_OK,
                 "equalizer reads");
    check(equalizer.band_count == 10 && equalizer.clear_bass == 0, "XM6 has ten bands");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_EQUALIZER_CLEAR_BASS_MIN, 0, &value),
                 MDR_RESULT_ERROR_NOT_SUPPORTED, "a ten-value param has no Clear Bass");
    check_result(mdrHeadphonesSetEqualizerClearBass(session.headphones, 1), MDR_RESULT_ERROR_NOT_SUPPORTED,
                 "a ten-value layout refuses Clear Bass writes");
    check(!mdrHeadphonesIsDirty(session.headphones), "unsupported Clear Bass does not stage an EQ resend");
    check_value(session.headphones, MDR_VALUE_EQUALIZER_BAND_FREQUENCY, 0, 31,
                "default 10-band layout before band info");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, info_xm6, sizeof(info_xm6));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "XM6 band info polls");
    check_value(session.headphones, MDR_VALUE_EQUALIZER_BAND_FREQUENCY, 9, 16000, "XM6 last band is 16 kHz");
    check_value(session.headphones, MDR_VALUE_EQUALIZER_BAND_FREQUENCY, 4, 500, "XM6 middle band is 500 Hz");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, info_32, sizeof(info_32));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "modified band info polls");
    check_value(session.headphones, MDR_VALUE_EQUALIZER_BAND_FREQUENCY, 0, 32, "band frequencies are read");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_EQUALIZER_BAND_FREQUENCY, 10, &value),
                 MDR_RESULT_ERROR_NOT_FOUND, "a band past the layout has no frequency");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, param_xm6, sizeof(param_xm6));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "XM6 EQ param polls with band info");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_EQUALIZER_CLEAR_BASS_MIN, 0, &value),
                 MDR_RESULT_ERROR_NOT_SUPPORTED, "ten values over ten frequencies still have no Clear Bass");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, cap_15, sizeof(cap_15));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "15-step EQ capability polls");
    check_range(session.headphones, MDR_VALUE_EQUALIZER_BAND_MIN, 10, -7, 7, "15 steps is -7..7");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_EQUALIZER_BAND_MIN, 5, &value),
                 MDR_RESULT_ERROR_NOT_FOUND, "a default layout the device doesn't use has no range");
    check_result(mdrHeadphonesSetEqualizerBands(session.headphones, bands_ok, 5), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "a band count off the device's layout is refused");
    check_result(mdrHeadphonesSetEqualizerBands(session.headphones, bands_wide, 10), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "a band past the advertised range is refused");
    check_result(mdrHeadphonesSetEqualizerBands(session.headphones, bands_ok, 10), MDR_RESULT_OK,
                 "the advertised extremes stage");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "EQ apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "EQ apply completes");
    check_command_payload(&session, offset, eq_write, sizeof(eq_write));
    session_close(&session);
}

static void check_noise(MDRHeadphones* headphones, MDRNoiseMode mode, uint32_t level, MDRBoolean voice,
                        const char* message)
{
    MDRNoiseControl noise;
    check_result(mdrHeadphonesGetNoiseControl(headphones, MDR_PROPERTY_CURRENT, &noise), MDR_RESULT_OK, message);
    check(noise.mode == mode && noise.ambient_level == level && noise.focus_on_voice == voice, message);
}

static void check_text(MDRHeadphones* headphones, MDRText text, uint32_t index, const char* expected,
                       const char* message)
{
    char buffer[64] = {0};
    uint32_t size = sizeof(buffer);
    check_result(mdrHeadphonesGetText(headphones, text, index, buffer, &size), MDR_RESULT_OK, message);
    check(strcmp(buffer, expected) == 0, message);
}

/*
 * V1 noise control is reported in the V2 model: the V1 NC selector becomes the mode, wind noise reduction gets a
 * mode of its own. EQ band frequencies and list-type general settings come from the device. Frames marked XM3 are
 * verbatim from the WH-1000XM3-4.5.2 capture.
 */
static void test_v1_noise_eq_general_settings(void)
{
    /* NC/ASM, PRESET_EQ, GENERAL_SETTING1 */
    static const unsigned char support[] = {0x07, 0, 3, 0x62, 0x51, 0xd1};
    static const unsigned char table2[] = {0x07, 0, 0};
    /* XM3: DUAL_SINGLE_OFF, ambient NORMAL/VOICE with 20 steps */
    static const unsigned char nc_cap_xm3[] = {0x61, 0x02, 0x02, 0x00, 0x01, 0x02, 0x00, 0x14, 0x01, 0x14};
    /* ON_OFF NC, 10 ambient steps */
    static const unsigned char nc_cap_on_off[] = {0x61, 0x02, 0x00, 0x00, 0x01, 0x02, 0x00, 0x0a, 0x01, 0x0a};
    /* XM3: on, DUAL (NC), ambient 0 */
    static const unsigned char nc_xm3[] = {0x67, 0x02, 0x01, 0x02, 0x02, 0x01, 0x00, 0x00};
    static const unsigned char nc_wnr[] = {0x69, 0x02, 0x01, 0x02, 0x01, 0x01, 0x00, 0x00};
    static const unsigned char nc_ambient[] = {0x69, 0x02, 0x01, 0x02, 0x00, 0x01, 0x01, 0x0f};
    static const unsigned char nc_off[] = {0x69, 0x02, 0x00, 0x02, 0x02, 0x01, 0x00, 0x00};
    static const unsigned char nc_write_wnr[] = {0x68, 0x02, 0x11, 0x02, 0x01, 0x01, 0x00, 0x00};
    static const unsigned char nc_write_ambient[] = {0x68, 0x02, 0x11, 0x02, 0x00, 0x01, 0x01, 20};
    static const unsigned char nc_write_on_off[] = {0x68, 0x02, 0x11, 0x00, 0x01, 0x01, 0x00, 0x00};
    /* EQEBB_RET_PARAM: Clear Bass + 5 bands, all 0 at the default offset */
    static const unsigned char eq_param[] = {0x57, 0x01, 0xa0, 0x06, 10, 10, 10, 10, 10, 10};
    /* XM3: Clear Bass, then 400, 1000, 2500, 6300, 16000 Hz */
    static const unsigned char eq_info_xm3[] = {0x5b, 0x01, 0x06, 0x10, 0x00, 0x01, 0x01, 0x01, 0x90, 0x01, 0x03,
                                                0xe8, 0x01, 0x09, 0xc4, 0x01, 0x18, 0x9c, 0x01, 0x3e, 0x80};
    /* First band at 100 Hz */
    static const unsigned char eq_info_100[] = {0x5b, 0x01, 0x06, 0x10, 0x00, 0x01, 0x01, 0x00, 0x64, 0x01, 0x03,
                                                0xe8, 0x01, 0x09, 0xc4, 0x01, 0x18, 0x9c, 0x01, 0x3e, 0x80};
    /* GENERAL_SETTING1, ENUM_NAME "KEY", LIST_TYPE: {ENUM_NAME "A"}, {RAW_NAME "b" / "c"}; shaped like the XM3's
     * ASSIGNABLE_KEY_SETTING */
    static const unsigned char gs_list[] = {0xd1, 0xd1, 0x02, 3, 'K',  'E', 'Y', 0, 0x02, 2,
                                            0x02, 1,    'A',  0, 0x01, 1,   'b', 1, 'c'};
    static const unsigned char gs_param[] = {0xd7, 0xd1, 0x02, 0x01};
    static const unsigned char gs_write[] = {0xd8, 0xd1, 0x02, 0x00};
    Session session;
    Device device;
    MDRNoiseControl noise;
    MDRFeatureAvailability availability;
    MDRGeneralSettingInfo info;
    MDRGeneralSetting setting;
    int32_t value = 0;
    uint32_t count;
    size_t offset;

    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = 1;
    device.table1 = support;
    device.table1_size = sizeof(support);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);

    /* Noise control */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, nc_cap_xm3, sizeof(nc_cap_xm3));
    device_run(&session, &device, MDR_EVENT_NOISE_CONTROL_CHANGED, "XM3 NC/ASM capability polls");
    check(mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_WIND_NOISE_REDUCTION, &availability) ==
                  MDR_RESULT_OK &&
              availability == MDR_AVAILABILITY_AVAILABLE,
          "DUAL_SINGLE_OFF offers wind noise reduction");
    check_range(session.headphones, MDR_VALUE_AMBIENT_LEVEL_MIN, 0, 1, 20, "XM3 ambient steps 20 is 1..20");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, nc_xm3, sizeof(nc_xm3));
    device_run(&session, &device, MDR_EVENT_NOISE_CONTROL_CHANGED, "XM3 NC param polls");
    check_noise(session.headphones, MDR_NOISE_MODE_CANCELLING, 0, MDR_FALSE, "XM3 DUAL reads as noise cancelling");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, nc_wnr, sizeof(nc_wnr));
    device_run(&session, &device, MDR_EVENT_NOISE_CONTROL_CHANGED, "SINGLE notification polls");
    check_noise(session.headphones, MDR_NOISE_MODE_WIND_NOISE_REDUCTION, 0, MDR_FALSE,
                "SINGLE reads as wind noise reduction");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, nc_ambient, sizeof(nc_ambient));
    device_run(&session, &device, MDR_EVENT_NOISE_CONTROL_CHANGED, "ambient notification polls");
    check_noise(session.headphones, MDR_NOISE_MODE_AMBIENT, 15, MDR_TRUE, "OFF selector reads as ambient");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, nc_off, sizeof(nc_off));
    device_run(&session, &device, MDR_EVENT_NOISE_CONTROL_CHANGED, "off notification polls");
    check_noise(session.headphones, MDR_NOISE_MODE_OFF, 0, MDR_FALSE, "effect OFF reads as off");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, nc_xm3, sizeof(nc_xm3));
    device_run(&session, &device, MDR_EVENT_NOISE_CONTROL_CHANGED, "XM3 NC param polls again");

    mdrHeadphonesGetNoiseControl(session.headphones, MDR_PROPERTY_DESIRED, &noise);
    noise.mode = MDR_NOISE_MODE_WIND_NOISE_REDUCTION;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_OK,
                 "wind noise reduction stages");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "WNR apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "WNR apply completes");
    check_command_payload(&session, offset, nc_write_wnr, sizeof(nc_write_wnr));

    mdrHeadphonesGetNoiseControl(session.headphones, MDR_PROPERTY_DESIRED, &noise);
    noise.mode = MDR_NOISE_MODE_AMBIENT;
    noise.ambient_level = 0;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "ambient mode refuses level zero");
    check(!mdrHeadphonesIsDirty(session.headphones), "invalid ambient minimum leaves noise control unstaged");
    noise.ambient_level = 21;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "ambient past the advertised steps is refused");
    noise.ambient_level = 20;
    noise.focus_on_voice = MDR_TRUE;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_OK, "ambient stages");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "ambient apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "ambient apply completes");
    check_command_payload(&session, offset, nc_write_ambient, sizeof(nc_write_ambient));

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, nc_cap_on_off, sizeof(nc_cap_on_off));
    device_run(&session, &device, MDR_EVENT_NOISE_CONTROL_CHANGED, "ON_OFF NC/ASM capability polls");
    check(mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_WIND_NOISE_REDUCTION, &availability) ==
                  MDR_RESULT_OK &&
              availability == MDR_AVAILABILITY_UNAVAILABLE,
          "ON_OFF NC has no wind noise reduction");
    check_range(session.headphones, MDR_VALUE_AMBIENT_LEVEL_MIN, 0, 1, 10, "ambient steps 10 is 1..10");
    mdrHeadphonesGetNoiseControl(session.headphones, MDR_PROPERTY_DESIRED, &noise);
    noise.mode = MDR_NOISE_MODE_WIND_NOISE_REDUCTION;
    noise.ambient_level = 0;
    noise.focus_on_voice = MDR_FALSE;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_ERROR_NOT_SUPPORTED,
                 "wind noise reduction is refused without the feature");
    noise.mode = MDR_NOISE_MODE_CANCELLING;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_OK, "ON_OFF NC stages");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "ON_OFF NC apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "ON_OFF NC apply completes");
    check_command_payload(&session, offset, nc_write_on_off, sizeof(nc_write_on_off));

    /* EQ band frequencies */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, eq_param, sizeof(eq_param));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "EQ param polls");
    check_value(session.headphones, MDR_VALUE_EQUALIZER_BAND_FREQUENCY, 0, 400, "default 5-band layout starts at 400");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, eq_info_xm3, sizeof(eq_info_xm3));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "XM3 EQ extended info polls");
    check_value(session.headphones, MDR_VALUE_EQUALIZER_BAND_FREQUENCY, 4, 16000, "XM3 last band is 16 kHz");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, eq_info_100, sizeof(eq_info_100));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "modified EQ extended info polls");
    check_value(session.headphones, MDR_VALUE_EQUALIZER_BAND_FREQUENCY, 0, 100, "band frequencies are read");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_EQUALIZER_BAND_FREQUENCY, 5, &value),
                 MDR_RESULT_ERROR_NOT_FOUND, "a band past the layout has no frequency");

    /* List-type general setting */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, gs_list, sizeof(gs_list));
    device_run(&session, &device, MDR_EVENT_GENERAL_SETTINGS_CHANGED, "list general setting capability polls");
    count = 1;
    check_result(mdrHeadphonesGetGeneralSettingInfo(session.headphones, &info, &count), MDR_RESULT_OK,
                 "general setting info reads");
    check(count == 1 && info.index == 0 && info.type == MDR_GENERAL_SETTING_LIST && info.option_count == 2 &&
              info.writable == MDR_TRUE && info.text_is_key == MDR_TRUE,
          "list setting reports its options and text kind");
    check_text(session.headphones, MDR_TEXT_GENERAL_SETTING_SUBJECT, 0, "KEY", "setting subject reads");
    check_text(session.headphones, MDR_TEXT_GENERAL_SETTING_OPTION_SUBJECT, MDR_GENERAL_SETTING_OPTION_INDEX(0, 0), "A",
               "first option subject reads");
    check_text(session.headphones, MDR_TEXT_GENERAL_SETTING_OPTION_SUBJECT, MDR_GENERAL_SETTING_OPTION_INDEX(0, 1), "b",
               "second option subject reads");
    check_text(session.headphones, MDR_TEXT_GENERAL_SETTING_OPTION_SUMMARY, MDR_GENERAL_SETTING_OPTION_INDEX(0, 1), "c",
               "second option summary reads");
    check_value(session.headphones, MDR_VALUE_GENERAL_SETTING_OPTION_TEXT_IS_KEY,
                MDR_GENERAL_SETTING_OPTION_INDEX(0, 0), 1, "ENUM_NAME option text is a key");
    check_value(session.headphones, MDR_VALUE_GENERAL_SETTING_OPTION_TEXT_IS_KEY,
                MDR_GENERAL_SETTING_OPTION_INDEX(0, 1), 0,
                "RAW_NAME option text is display text under an ENUM_NAME setting");
    check_result(mdrHeadphonesGetValue(session.headphones, MDR_VALUE_GENERAL_SETTING_OPTION_TEXT_IS_KEY,
                                       MDR_GENERAL_SETTING_OPTION_INDEX(0, 2), &value),
                 MDR_RESULT_ERROR_NOT_FOUND, "an option past the list has no text format");
    count = 0;
    check_result(mdrHeadphonesGetText(session.headphones, MDR_TEXT_GENERAL_SETTING_OPTION_SUBJECT,
                                      MDR_GENERAL_SETTING_OPTION_INDEX(0, 2), NULL, &count),
                 MDR_RESULT_ERROR_NOT_FOUND, "an option past the list is not found");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, gs_param, sizeof(gs_param));
    device_run(&session, &device, MDR_EVENT_GENERAL_SETTINGS_CHANGED, "list general setting param polls");
    check_result(mdrHeadphonesGetGeneralSetting(session.headphones, MDR_PROPERTY_CURRENT, 0, &setting), MDR_RESULT_OK,
                 "list setting reads");
    check(setting.list_value == 1, "list setting decodes the current option");
    setting.list_value = 2;
    check_result(mdrHeadphonesSetGeneralSetting(session.headphones, &setting), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "an option past the list is refused");
    setting.list_value = 0;
    check_result(mdrHeadphonesSetGeneralSetting(session.headphones, &setting), MDR_RESULT_OK, "list option stages");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "list setting apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "list setting apply completes");
    check_command_payload(&session, offset, gs_write, sizeof(gs_write));
    session_close(&session);
}

static void test_property_getter_contracts(MDRProtocolVersion family)
{
    Session session;
    MDRGeneralSetting setting = {UINT32_MAX, MDR_TRUE};
    MDRGeneralSetting unchanged = setting;
    int32_t bands[1] = {INT32_MIN};
    MDRAssignableControl control = {0};
    uint32_t count;
    size_t tx_size;

    if (!session_open_family(&session, family))
        return;
    tx_size = session.transport.tx_size;

#define CHECK_GETTER(getter, type)                                                                                     \
    do                                                                                                                 \
    {                                                                                                                  \
        type current = {0}, desired = {0}, sentinel, before;                                                           \
        MDRResult current_result = getter(session.headphones, MDR_PROPERTY_CURRENT, &current);                         \
        MDRResult desired_result = getter(session.headphones, MDR_PROPERTY_DESIRED, &desired);                         \
        check_result(desired_result, current_result, #getter " initial views have the same result");                   \
        if (current_result == MDR_RESULT_OK)                                                                           \
            check(memcmp(&current, &desired, sizeof(current)) == 0, #getter " initial views agree");                   \
        memset(&sentinel, 0xa5, sizeof(sentinel));                                                                     \
        before = sentinel;                                                                                             \
        check_result(getter(session.headphones, UINT32_MAX, &sentinel), MDR_RESULT_ERROR_INVALID_ARGUMENT,             \
                     #getter " rejects unknown selector");                                                             \
        check_result(getter(session.headphones, 2u, &sentinel), MDR_RESULT_ERROR_INVALID_ARGUMENT,                     \
                     #getter " rejects selector 2");                                                                   \
        check(memcmp(&sentinel, &before, sizeof(sentinel)) == 0, #getter " invalid selector leaves output untouched"); \
        check_result(getter(NULL, MDR_PROPERTY_DESIRED, &desired), MDR_RESULT_ERROR_INVALID_ARGUMENT,                  \
                     #getter " rejects null headphones");                                                              \
        check_result(getter(session.headphones, MDR_PROPERTY_DESIRED, NULL), MDR_RESULT_ERROR_INVALID_ARGUMENT,        \
                     #getter " rejects null output");                                                                  \
    }                                                                                                                  \
    while (0)
    CHECK_GETTER(mdrHeadphonesGetPlayback, MDRPlayback);
    CHECK_GETTER(mdrHeadphonesGetNoiseControl, MDRNoiseControl);
    CHECK_GETTER(mdrHeadphonesGetSpeakToChat, MDRSpeakToChat);
    CHECK_GETTER(mdrHeadphonesGetListening, MDRListening);
    CHECK_GETTER(mdrHeadphonesGetEqualizer, MDREqualizer);
    CHECK_GETTER(mdrHeadphonesGetPairing, MDRPairing);
    CHECK_GETTER(mdrHeadphonesGetSourceSwitchControl, MDRBoolean);
    CHECK_GETTER(mdrHeadphonesGetPower, MDRPower);
    CHECK_GETTER(mdrHeadphonesGetVoiceGuidance, MDRVoiceGuidance);
    CHECK_GETTER(mdrHeadphonesGetConnectionMode, MDRConnectionMode);
    CHECK_GETTER(mdrHeadphonesGetSafeListening, MDRSafeListening);
#undef CHECK_GETTER

    check_result(mdrHeadphonesGetGeneralSetting(session.headphones, 2u, 0, &setting), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "general setting rejects unknown selector before lookup");
    check(memcmp(&setting, &unchanged, sizeof(setting)) == 0, "invalid general selector preserves output");
    check_result(mdrHeadphonesGetGeneralSetting(NULL, MDR_PROPERTY_DESIRED, 0, &setting),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "general setting rejects null headphones");
    check_result(mdrHeadphonesGetGeneralSetting(session.headphones, MDR_PROPERTY_DESIRED, 0, NULL),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "general setting rejects null output");
    check_result(mdrHeadphonesGetGeneralSetting(session.headphones, MDR_PROPERTY_DESIRED, UINT32_MAX, &setting),
                 MDR_RESULT_ERROR_NOT_FOUND, "general setting rejects unknown index");
    count = 7;
    check_result(mdrHeadphonesGetEqualizerBands(session.headphones, 2u, bands, &count),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "EQ array rejects unknown selector");
    check(count == 7 && bands[0] == INT32_MIN, "invalid EQ selector preserves count and buffer");
    check_result(mdrHeadphonesGetEqualizerBands(session.headphones, MDR_PROPERTY_DESIRED, bands, NULL),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "EQ array rejects null count");
    check_result(mdrHeadphonesGetEqualizerBands(NULL, MDR_PROPERTY_DESIRED, bands, &count),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "EQ array rejects null headphones");
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, 2u, &control, &count),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "assignable array rejects unknown selector before support check");
    check(count == 7 && control.action == 0, "invalid assignable selector preserves outputs");
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, MDR_PROPERTY_DESIRED, &control, NULL),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "assignable array rejects null count");
    check_result(mdrHeadphonesGetAssignableControls(NULL, MDR_PROPERTY_DESIRED, &control, &count),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "assignable array rejects null headphones");
    check(session.transport.tx_size == tx_size && !mdrHeadphonesIsDirty(session.headphones) &&
              mdrHeadphonesIsReady(session.headphones),
          "all getters leave transport and state unchanged");
    session_close(&session);
}

static void test_widened_inputs(MDRProtocolVersion family)
{
    static const uint32_t unsigned_values[] = {256u, UINT32_MAX};
    static const int32_t signed_values[] = {INT32_MIN, INT32_MAX, -256, 256};
    Session session;
    MDRNoiseControl noise, noise_before;
    MDREqualizer equalizer, equalizer_before;
    MDRVoiceGuidance voice, voice_before;
    MDRPlayback playback;
    int32_t bands[10] = {0};
    uint32_t i, band_count;

    if (!session_open_family(&session, family))
        return;
    check_result(mdrHeadphonesGetNoiseControl(session.headphones, MDR_PROPERTY_DESIRED, &noise_before), MDR_RESULT_OK,
                 "initial desired noise reads");
    check_result(mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_DESIRED, &equalizer_before), MDR_RESULT_OK,
                 "initial desired EQ reads");
    check_result(mdrHeadphonesGetVoiceGuidance(session.headphones, MDR_PROPERTY_DESIRED, &voice_before), MDR_RESULT_OK,
                 "initial desired voice reads");
    for (i = 0; i < sizeof(unsigned_values) / sizeof(unsigned_values[0]); ++i)
    {
        check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, unsigned_values[i]),
                     MDR_RESULT_ERROR_INVALID_ARGUMENT, "wide unsigned volume cannot truncate into range");
        check_result(mdrHeadphonesSetEqualizerPreset(session.headphones, unsigned_values[i]),
                     MDR_RESULT_ERROR_INVALID_ARGUMENT, "wide preset cannot truncate into range");
        check_result(mdrHeadphonesSetDSEE(session.headphones, unsigned_values[i]), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                     "wide DSEE boolean is rejected");
        noise = noise_before;
        noise.mode = MDR_NOISE_MODE_CANCELLING;
        noise.focus_on_voice = MDR_TRUE;
        noise.ambient_level = unsigned_values[i];
        check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                     "wide ambient level cannot truncate into range");
    }
    noise = noise_before;
    noise.ambient_level = 21;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "ambient range is still capped at 20");
    noise.ambient_level = 255;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "the old V1 sentinel is just an out-of-range level");
    noise = noise_before;
    noise.mode = MDR_NOISE_MODE_AMBIENT;
    noise.ambient_level = 0;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "ambient minimum applies to both protocol families");
    noise.mode = MDR_NOISE_MODE_WIND_NOISE_REDUCTION + 1;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "unknown noise mode is rejected");
    for (i = 0; i < sizeof(signed_values) / sizeof(signed_values[0]); ++i)
    {
        check_result(mdrHeadphonesSetEqualizerClearBass(session.headphones, signed_values[i]),
                     MDR_RESULT_ERROR_INVALID_ARGUMENT, "wide clear bass cannot truncate into range");
        voice = voice_before;
        voice.enabled = MDR_TRUE;
        voice.volume = signed_values[i];
        check_result(mdrHeadphonesSetVoiceGuidance(session.headphones, &voice), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                     "wide signed voice volume cannot truncate into range");
        for (band_count = 5; band_count <= 10; band_count += 5)
        {
            memset(bands, 0, sizeof(bands));
            bands[band_count - 1] = signed_values[i];
            check_result(mdrHeadphonesSetEqualizerBands(session.headphones, bands, band_count),
                         MDR_RESULT_ERROR_INVALID_ARGUMENT, "wide last band rejects the entire array");
        }
    }
    for (i = 0; i < 2; ++i)
    {
        check_result(mdrHeadphonesSetEqualizerClearBass(session.headphones, i ? 11 : -11),
                     MDR_RESULT_ERROR_INVALID_ARGUMENT, "clear bass remains within -10..10");
        voice = voice_before;
        voice.volume = i ? 3 : -3;
        check_result(mdrHeadphonesSetVoiceGuidance(session.headphones, &voice), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                     "voice volume remains within -2..2");
        memset(bands, 0, sizeof(bands));
        bands[4] = i ? 11 : -11;
        check_result(mdrHeadphonesSetEqualizerBands(session.headphones, bands, 5), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                     "five bands remain within -10..10");
        bands[4] = i ? 7 : -7;
        check_result(mdrHeadphonesSetEqualizerBands(session.headphones, bands, 10), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                     "ten bands remain within -6..6");
    }
    check(!mdrHeadphonesIsDirty(session.headphones), "invalid widened inputs leave no staged changes");
    mdrHeadphonesGetNoiseControl(session.headphones, MDR_PROPERTY_DESIRED, &noise);
    mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_DESIRED, &equalizer);
    mdrHeadphonesGetVoiceGuidance(session.headphones, MDR_PROPERTY_DESIRED, &voice);
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_DESIRED, &playback);
    check(memcmp(&noise, &noise_before, sizeof(noise)) == 0 &&
              memcmp(&equalizer, &equalizer_before, sizeof(equalizer)) == 0 &&
              memcmp(&voice, &voice_before, sizeof(voice)) == 0 && playback.volume == 0,
          "invalid widened inputs preserve desired values transactionally");
    session_close(&session);
}

static void check_eq_bands(MDRHeadphones* headphones, MDRPropertyType type, const int32_t* expected,
                           uint32_t expected_count)
{
    int32_t bands[10];
    uint32_t count = 1, i;

    check_result(mdrHeadphonesGetEqualizerBands(headphones, type, NULL, &count), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "EQ size query requires zero count");
    count = 0;
    check_result(mdrHeadphonesGetEqualizerBands(headphones, type, NULL, &count), MDR_RESULT_OK,
                 "selected EQ array size queries");
    check(count == expected_count, "EQ size follows the selected array");
    for (i = 0; i < 10; ++i)
        bands[i] = INT32_MIN;
    count = expected_count - 1;
    check_result(mdrHeadphonesGetEqualizerBands(headphones, type, bands, &count), MDR_RESULT_ERROR_BUFFER_TOO_SMALL,
                 "selected EQ array rejects short buffer");
    check(count == expected_count && bands[0] == INT32_MIN, "short EQ copy reports count without copying");
    count = (uint32_t)(sizeof(bands) / sizeof(bands[0]));
    check_result(mdrHeadphonesGetEqualizerBands(headphones, type, bands, &count), MDR_RESULT_OK,
                 "selected EQ array copies");
    check(count == expected_count && memcmp(bands, expected, expected_count * sizeof(expected[0])) == 0,
          "EQ copy and count come from the same view");
}

static void test_eq_array_views(MDRProtocolVersion family)
{
    static const unsigned char table1_v1[] = {0x07, 0, 1, 0x51};
    static const unsigned char table1_v2[] = {0x07, 0, 1, 0x50, 0xff};
    static const unsigned char table2[] = {0x07, 0, 0};
    static const unsigned char flat_v1[] = {0x57, 1, 0, 6, 10, 10, 10, 10, 10, 10};
    static const unsigned char flat_v2[] = {0x57, 0, 0, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
    static const unsigned char other_v1[] = {0x57, 1, 0, 11, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
    static const unsigned char other_v2[] = {0x57, 0, 0, 6, 10, 10, 10, 10, 10, 10};
    static const int32_t flat[10] = {0};
    static const int32_t five[5] = {-10, -1, 0, 1, 10};
    static const int32_t ten[10] = {-6, -1, 0, 1, 6, -6, -1, 0, 1, 6};
    Session session;
    Device device;
    MDREqualizer current, desired;
    int v1 = family == MDR_PROTOCOL_V1;
    uint32_t current_count = v1 ? 5u : 10u;
    uint32_t desired_count = v1 ? 10u : 5u;
    const int32_t* edited = v1 ? ten : five;

    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = v1;
    device.table1 = v1 ? table1_v1 : table1_v2;
    device.table1_size = v1 ? sizeof(table1_v1) : sizeof(table1_v2);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);
    /* The views can only differ in size when the device changes layout under a local edit. */
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? other_v1 : other_v2, v1 ? sizeof(other_v1) : sizeof(other_v2));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "initial EQ array polls");
    check_eq_bands(session.headphones, MDR_PROPERTY_CURRENT, flat, desired_count);
    check_eq_bands(session.headphones, MDR_PROPERTY_DESIRED, flat, desired_count);
    check_result(mdrHeadphonesSetEqualizerBands(session.headphones, edited, desired_count), MDR_RESULT_OK,
                 "desired EQ array stages");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? flat_v1 : flat_v2, v1 ? sizeof(flat_v1) : sizeof(flat_v2));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "EQ layout change with a local edit polls");
    check_eq_bands(session.headphones, MDR_PROPERTY_CURRENT, flat, current_count);
    check_eq_bands(session.headphones, MDR_PROPERTY_DESIRED, edited, desired_count);
    mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_CURRENT, &current);
    mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_DESIRED, &desired);
    check(current.band_count == current_count && desired.band_count == desired_count,
          "equalizer band_count corresponds to the selected config");
    check_result(mdrHeadphonesSetEqualizerBands(session.headphones, edited, desired_count),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "a band count off the device's layout is refused");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? flat_v1 : flat_v2, v1 ? sizeof(flat_v1) : sizeof(flat_v2));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "EQ notification with a local edit polls");
    check_eq_bands(session.headphones, MDR_PROPERTY_DESIRED, edited, desired_count);
    session_close(&session);
}

static void test_grouped_staging_during_apply(MDRProtocolVersion family)
{
    static const unsigned char table1_v1[] = {0x07, 0, 2, 0x51, 0xa1};
    static const unsigned char table1_v2[] = {0x07, 0, 2, 0x50, 0xff, 0xa1, 0xff};
    static const unsigned char table2[] = {0x07, 0, 0};
    static const unsigned char flat_v1[] = {0x57, 1, 0, 6, 10, 10, 10, 10, 10, 10};
    static const unsigned char flat_v2[] = {0x57, 0, 0, 10, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};
    static const unsigned char first_v1[] = {0x58, 1, 0xff, 6, 10, 11, 11, 11, 11, 11};
    static const unsigned char first_v2[] = {0x58, 0, 0, 10, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7};
    static const unsigned char next_v1[] = {0x58, 1, 0xff, 6, 12, 12, 12, 12, 12, 12};
    static const unsigned char next_v2[] = {0x58, 0, 0, 10, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8};
    static const int32_t flat[10] = {0};
    static const int32_t first[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    static const int32_t next[10] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    Session session;
    Device device;
    MDREqualizer equalizer;
    TxFrame frame;
    MDREvent event;
    size_t offset;
    int curves = 0;
    int v1 = family == MDR_PROTOCOL_V1;
    uint32_t count = v1 ? 5u : 10u;

    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = v1;
    device.table1 = v1 ? table1_v1 : table1_v2;
    device.table1_size = v1 ? sizeof(table1_v1) : sizeof(table1_v2);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? flat_v1 : flat_v2, v1 ? sizeof(flat_v1) : sizeof(flat_v2));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "initial grouped EQ report polls");
    check_result(mdrHeadphonesSetEqualizerBands(session.headphones, first, count), MDR_RESULT_OK, "first curve stages");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 10), MDR_RESULT_OK, "volume stages ahead of EQ");
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK,
                 "grouped batch starts with volume in flight");
    check_result(mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_DESIRED, &equalizer), MDR_RESULT_OK,
                 "group reads");
    check_result(mdrHeadphonesSetEqualizerClearBass(session.headphones, 2),
                 v1 ? MDR_RESULT_OK : MDR_RESULT_ERROR_NOT_SUPPORTED,
                 "only the Clear Bass layout can stage bass after the batch snapshot");
    check_result(mdrHeadphonesSetEqualizerBands(session.headphones, next, count), MDR_RESULT_OK,
                 "new array stages before the old EQ command has run");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? flat_v1 : flat_v2, v1 ? sizeof(flat_v1) : sizeof(flat_v2));
    poll_event(session.headphones, &event, "group notification ahead of volume ACK polls");
    check(event == MDR_EVENT_EQUALIZER_CHANGED, "EQ report is processed before old group completion");
    check_eq_bands(session.headphones, MDR_PROPERTY_CURRENT, flat, count);
    check_eq_bands(session.headphones, MDR_PROPERTY_DESIRED, next, count);
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "first grouped batch completes");
    check_command_payload(&session, offset, v1 ? first_v1 : first_v2, v1 ? sizeof(first_v1) : sizeof(first_v2));
    check_eq_bands(session.headphones, MDR_PROPERTY_DESIRED, next, count);
    check_result(mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_DESIRED, &equalizer), MDR_RESULT_OK,
                 "group reads after old ACK");
    check(equalizer.clear_bass == (v1 ? 2 : 0), "group commit retains staged bass only for the Clear Bass layout");
    check(mdrHeadphonesIsDirty(session.headphones), "group commit preserves the submitted-staged array");

    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "next grouped batch starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "next grouped batch completes");
    while (next_tx_frame(&session.transport, &offset, &frame))
    {
        if (frame.type == MDR_DATA_TYPE_DATA_MDR && frame.payload_size > 4 && frame.payload[0] == 0x58)
        {
            const unsigned char* payload = v1 ? next_v1 : next_v2;
            const size_t size = v1 ? sizeof(next_v1) : sizeof(next_v2);
            ++curves;
            check(frame.payload_size == size && memcmp(frame.payload, payload, size) == 0,
                  "next curve uses new array and newly staged group snapshot");
        }
    }
    check(curves == 1 && !mdrHeadphonesIsDirty(session.headphones), "next grouped ACK consumes all staged members");
    check_eq_bands(session.headphones, MDR_PROPERTY_CURRENT, flat, count);
    check_eq_bands(session.headphones, MDR_PROPERTY_DESIRED, next, count);
    session_close(&session);
}

static void test_playback_property_views(MDRProtocolVersion family)
{
    static const unsigned char table1_v1[] = {0x07, 0, 1, 0xa1};
    static const unsigned char table1_v2[] = {0x07, 0, 1, 0xa1, 0xff};
    static const unsigned char table2[] = {0x07, 0, 0};
    static const unsigned char volume_v1[] = {0xa9, 1, 0x20, 7};
    static const unsigned char volume_v2[] = {0xa9, 0x20, 7};
    static const unsigned char changed_volume_v1[] = {0xa9, 1, 0x20, 9};
    static const unsigned char changed_volume_v2[] = {0xa9, 0x20, 9};
    static const unsigned char status_v1[] = {0xa5, 1, 0, 1};
    static const unsigned char status_v2[] = {0xa5, 1, 0, 1, 0};
    Session session;
    Device device;
    MDRPlayback current, desired;
    int v1 = family == MDR_PROTOCOL_V1;
    size_t tx_size;

    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = v1;
    device.table1 = v1 ? table1_v1 : table1_v2;
    device.table1_size = v1 ? sizeof(table1_v1) : sizeof(table1_v2);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? volume_v1 : volume_v2,
                v1 ? sizeof(volume_v1) : sizeof(volume_v2));
    device_run(&session, &device, MDR_EVENT_PLAYBACK_CHANGED, "volume notification without local edits polls");
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_CURRENT, &current);
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_DESIRED, &desired);
    check(current.volume == 7 && desired.volume == 7, "unmodified property notification updates both views");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 12), MDR_RESULT_OK, "new volume stages");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? volume_v1 : volume_v2,
                v1 ? sizeof(volume_v1) : sizeof(volume_v2));
    device_run(&session, &device, MDR_EVENT_PLAYBACK_CHANGED, "volume notification with local edits polls");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? status_v1 : status_v2,
                v1 ? sizeof(status_v1) : sizeof(status_v2));
    device_run(&session, &device, MDR_EVENT_PLAYBACK_CHANGED, "playback status notification polls");
    tx_size = session.transport.tx_size;
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_CURRENT, &current);
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_DESIRED, &desired);
    check(current.volume == 7 && desired.volume == 12, "notification preserves the outstanding local desired edit");
    check(current.status == MDR_PLAYBACK_PLAYING && desired.status == current.status,
          "playback status does not switch views");
    check(session.transport.tx_size == tx_size, "reading both views sends no packets");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "volume apply starts");
    check(!mdrHeadphonesIsReady(session.headphones), "apply remains busy");
    check_result(mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_DESIRED, &desired), MDR_RESULT_OK,
                 "desired remains readable during apply");
    check(desired.volume == 12, "apply reads the latest accepted target");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 20), MDR_RESULT_OK,
                 "busy Set stages the next target");
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_DESIRED, &desired);
    check(desired.volume == 20, "busy Set updates desired without changing the in-flight target");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? changed_volume_v1 : changed_volume_v2,
                v1 ? sizeof(changed_volume_v1) : sizeof(changed_volume_v2));
    {
        MDREvent event;
        poll_event(session.headphones, &event, "notification ahead of apply ACK polls");
        check(event == MDR_EVENT_PLAYBACK_CHANGED, "device value changes before apply ACK");
    }
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_CURRENT, &current);
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_DESIRED, &desired);
    check(current.volume == 9 && desired.volume == 20, "new staged target survives a device notification");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "volume apply completes through ACK");
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_CURRENT, &current);
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_DESIRED, &desired);
    check(current.volume == 9 && desired.volume == 20 && mdrHeadphonesIsDirty(session.headphones),
          "ACK preserves the next edit and the earlier device notification");
    tx_size = session.transport.tx_size;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "next volume apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "next volume apply completes");
    {
        const unsigned char next_v1[] = {0xa8, 1, 0x20, 20};
        const unsigned char next_v2[] = {0xa8, 0x20, 20};
        check_command_payload(&session, tx_size, v1 ? next_v1 : next_v2, v1 ? sizeof(next_v1) : sizeof(next_v2));
    }
    check(!mdrHeadphonesIsDirty(session.headphones), "next ACK consumes the staged volume");
    device.log_size = 0;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "clean apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "clean apply completes");
    check(!device_requested(&device, 1, 0xa8, v1 ? 1 : 0x20), "reported and desired differences do not resend volume");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? volume_v1 : volume_v2,
                v1 ? sizeof(volume_v1) : sizeof(volume_v2));
    device_run(&session, &device, MDR_EVENT_PLAYBACK_CHANGED, "post-apply device volume polls");
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_CURRENT, &current);
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_DESIRED, &desired);
    check(current.volume == 7 && desired.volume == 7, "notification after apply updates both unedited views");
    session_close(&session);
}

static void test_unedited_property_is_not_submitted(MDRProtocolVersion family)
{
    static const unsigned char table1_v1[] = {0x07, 0, 2, 0x62, 0xa1};
    static const unsigned char table1_v2[] = {0x07, 0, 2, 0x6d, 0xff, 0xa1, 0xff};
    static const unsigned char table2[] = {0x07, 0, 0};
    static const unsigned char volume_v1[] = {0xa9, 1, 0x20, 7};
    static const unsigned char volume_v2[] = {0xa9, 0x20, 7};
    static const unsigned char changed_volume_v1[] = {0xa9, 1, 0x20, 9};
    static const unsigned char changed_volume_v2[] = {0xa9, 0x20, 9};
    Session session;
    Device device;
    MDRNoiseControl noise;
    MDRPlayback current, desired;
    MDREvent event;
    int v1 = family == MDR_PROTOCOL_V1;

    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = v1;
    device.table1 = v1 ? table1_v1 : table1_v2;
    device.table1_size = v1 ? sizeof(table1_v1) : sizeof(table1_v2);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? volume_v1 : volume_v2,
                v1 ? sizeof(volume_v1) : sizeof(volume_v2));
    device_run(&session, &device, MDR_EVENT_PLAYBACK_CHANGED, "initial unedited volume report polls");
    check_result(mdrHeadphonesGetNoiseControl(session.headphones, MDR_PROPERTY_DESIRED, &noise), MDR_RESULT_OK,
                 "noise desired view reads");
    noise.mode = MDR_NOISE_MODE_AMBIENT;
    noise.ambient_level = 10;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_OK, "only noise stages");
    device.log_size = 0;
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "noise apply starts");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? changed_volume_v1 : changed_volume_v2,
                v1 ? sizeof(changed_volume_v1) : sizeof(changed_volume_v2));
    poll_event(session.headphones, &event, "unedited volume changes while noise waits for ACK");
    check(event == MDR_EVENT_PLAYBACK_CHANGED, "volume notification precedes noise ACK");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "noise apply completes");
    check(!device_requested(&device, 1, 0xa8, -1), "noise apply never sends an unedited volume snapshot");
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_CURRENT, &current);
    mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_DESIRED, &desired);
    check(current.volume == 9 && desired.volume == 9 && !mdrHeadphonesIsDirty(session.headphones),
          "unedited device notification updates both views without becoming a pending write");
    session_close(&session);
}

static void test_editable_property_views(MDRProtocolVersion family, int wearing)
{
    static const unsigned char table1_v1[] = {0x07, 0, 8, 0x51, 0xf5, 0xd1, 0xd3, 0xe1, 0xf4, 0x39, 0x38};
    const unsigned char table1_v2[] = {0x07, 0,    9,    0x50, 0xff, 0xfc, 0xff,
                                       0xd1, 0xff, 0xd4, 0xff, 0xe1, 0xff, (unsigned char)(wearing ? 0x25 : 0x24),
                                       0xff, 0xe6, 0xff, 0xeb, 0xff, 0xe8, 0xff};
    static const unsigned char table2[] = {0x07, 0, 4, 0x30, 0xff, 0x31, 0xff, 0x42, 0xff, 0x50, 0xff};
    static const unsigned char gs_v1[] = {0xd1, 0xd3, 1, 0, 0, 1};
    static const unsigned char gs_v2[] = {0xd1, 0xd4, 0, 0, 0, 0};
    static const unsigned char measurement[] = {0x5b, 0, 85, 0};
    static const unsigned char eq_off[] = {0x55, 0, 1};
    static const unsigned char eq_v1[] = {0x57, 1, 0, 6, 10, 10, 10, 10, 10, 10};
    static const unsigned char eq_v2[] = {0x57, 0, 0, 6, 10, 10, 10, 10, 10, 10};
    Session session;
    Device device;
    MDRNoiseControl noise, current_noise;
    MDRSpeakToChat speak, current_speak;
    MDREqualizer equalizer, current_equalizer;
    MDRPower power, current_power;
    MDRVoiceGuidance voice, current_voice, initial_voice;
    MDRPairing pairing, current_pairing;
    MDRConnectionMode mode, current_mode;
    MDRGeneralSetting setting, current_setting;
    MDRListening listening, current_listening;
    MDRSafeListening safe, current_safe;
    MDRBoolean source, current_source;
    uint32_t setting_index = family == MDR_PROTOCOL_V1 ? 2u : 3u;
    int v1 = family == MDR_PROTOCOL_V1;

    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = v1;
    device.table1 = v1 ? table1_v1 : table1_v2;
    device.table1_size = v1 ? sizeof(table1_v1) : sizeof(table1_v2);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);

    check_result(mdrHeadphonesGetNoiseControl(session.headphones, MDR_PROPERTY_DESIRED, &noise), MDR_RESULT_OK,
                 "desired noise reads");
    noise.mode = MDR_NOISE_MODE_AMBIENT;
    noise.ambient_level = 20;
    noise.changing_asm_level = MDR_TRUE;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_OK,
                 "noise level and changing flag stage");
    mdrHeadphonesGetNoiseControl(session.headphones, MDR_PROPERTY_DESIRED, &noise);
    noise.focus_on_voice = MDR_TRUE;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_OK,
                 "second desired noise edit stages");
    mdrHeadphonesGetNoiseControl(session.headphones, MDR_PROPERTY_CURRENT, &current_noise);
    mdrHeadphonesGetNoiseControl(session.headphones, MDR_PROPERTY_DESIRED, &noise);
    check(current_noise.mode == MDR_NOISE_MODE_OFF && current_noise.changing_asm_level == MDR_FALSE &&
              noise.mode == MDR_NOISE_MODE_AMBIENT && noise.ambient_level == 20 &&
              noise.changing_asm_level == MDR_TRUE && noise.focus_on_voice == MDR_TRUE,
          "desired noise edits preserve mode, level and changing flag while current stays unchanged");
    noise.mode = MDR_NOISE_MODE_CANCELLING;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_OK, "NC mode stages");
    mdrHeadphonesGetNoiseControl(session.headphones, MDR_PROPERTY_DESIRED, &noise);
    check(noise.mode == MDR_NOISE_MODE_CANCELLING && noise.changing_asm_level == MDR_TRUE,
          "NC mode round-trips without losing changing flag");
    noise.mode = MDR_NOISE_MODE_WIND_NOISE_REDUCTION;
    check_result(mdrHeadphonesSetNoiseControl(session.headphones, &noise), MDR_RESULT_ERROR_NOT_SUPPORTED,
                 "wind noise reduction needs the feature");

    mdrHeadphonesGetSpeakToChat(session.headphones, MDR_PROPERTY_DESIRED, &speak);
    speak.enabled = MDR_TRUE;
    speak.sensitivity = MDR_SPEECH_SENSITIVITY_LOW;
    speak.timeout = MDR_SPEAK_TIMEOUT_SHORT;
    check_result(mdrHeadphonesSetSpeakToChat(session.headphones, &speak), MDR_RESULT_OK, "speech settings stage");
    mdrHeadphonesGetSpeakToChat(session.headphones, MDR_PROPERTY_DESIRED, &speak);
    speak.timeout = MDR_SPEAK_TIMEOUT_LONG;
    check_result(mdrHeadphonesSetSpeakToChat(session.headphones, &speak), MDR_RESULT_OK, "second speech field stages");
    mdrHeadphonesGetSpeakToChat(session.headphones, MDR_PROPERTY_CURRENT, &current_speak);
    mdrHeadphonesGetSpeakToChat(session.headphones, MDR_PROPERTY_DESIRED, &speak);
    check(current_speak.enabled == MDR_FALSE && speak.enabled == MDR_TRUE &&
              speak.sensitivity == MDR_SPEECH_SENSITIVITY_LOW && speak.timeout == MDR_SPEAK_TIMEOUT_LONG,
          "speech desired timeout edit retains pending enable and sensitivity");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? eq_v1 : eq_v2, v1 ? sizeof(eq_v1) : sizeof(eq_v2));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "Clear Bass layout for property edits polls");
    check_result(mdrHeadphonesSetEqualizerPreset(session.headphones, MDR_EQ_BRIGHT), MDR_RESULT_OK, "preset stages");
    check_result(mdrHeadphonesSetEqualizerClearBass(session.headphones, -10), MDR_RESULT_OK,
                 "negative clear bass stages");
    mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_DESIRED, &equalizer);
    check(equalizer.clear_bass == -10, "negative clear bass survives the public type bridge");
    check_result(mdrHeadphonesSetEqualizerClearBass(session.headphones, 10), MDR_RESULT_OK,
                 "second clear bass edit stages");
    check_result(mdrHeadphonesSetDSEE(session.headphones, MDR_TRUE), MDR_RESULT_OK, "DSEE stages independently");
    if (!v1)
    {
        device_send(&device, MDR_DATA_TYPE_DATA_MDR, eq_off, sizeof(eq_off));
        device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "EQ availability change with local edits polls");
    }
    mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_CURRENT, &current_equalizer);
    mdrHeadphonesGetEqualizer(session.headphones, MDR_PROPERTY_DESIRED, &equalizer);
    check(current_equalizer.preset == MDR_EQ_OFF && current_equalizer.clear_bass == 0 &&
              equalizer.preset == MDR_EQ_BRIGHT && equalizer.clear_bass == 10 && equalizer.dsee_enabled == MDR_TRUE,
          "desired EQ edits retain pending preset and signed bass");
    check(equalizer.available == current_equalizer.available &&
              equalizer.dsee_available == current_equalizer.dsee_available &&
              equalizer.dsee_type == current_equalizer.dsee_type && (v1 || equalizer.available == MDR_FALSE),
          "EQ availability and DSEE capabilities never select desired");

    mdrHeadphonesGetPairing(session.headphones, MDR_PROPERTY_DESIRED, &pairing);
    pairing.enabled = MDR_TRUE;
    check_result(mdrHeadphonesSetPairing(session.headphones, &pairing), MDR_RESULT_OK, "pairing stages");
    mdrHeadphonesGetPairing(session.headphones, MDR_PROPERTY_CURRENT, &current_pairing);
    mdrHeadphonesGetPairing(session.headphones, MDR_PROPERTY_DESIRED, &pairing);
    check(current_pairing.enabled == MDR_FALSE && pairing.enabled == MDR_TRUE, "pairing has two views");

    mdrHeadphonesGetPower(session.headphones, MDR_PROPERTY_DESIRED, &power);
    power.auto_power_off_minutes = 30;
    power.auto_pause = MDR_TRUE;
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_OK, "power time and pause stage");
    mdrHeadphonesGetPower(session.headphones, MDR_PROPERTY_DESIRED, &power);
    power.head_gesture = MDR_TRUE;
    power.shutdown_requested = MDR_TRUE;
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_OK, "other desired power fields stage");
    mdrHeadphonesGetPower(session.headphones, MDR_PROPERTY_CURRENT, &current_power);
    mdrHeadphonesGetPower(session.headphones, MDR_PROPERTY_DESIRED, &power);
    check(current_power.auto_pause == MDR_FALSE && current_power.shutdown_requested == MDR_FALSE &&
              power.auto_power_off_minutes == 30 && power.auto_pause == MDR_TRUE && power.head_gesture == MDR_TRUE &&
              power.shutdown_requested == MDR_TRUE,
          "power branches read all fields from the same desired view");
    if (wearing)
    {
        power.wearing_power = MDR_WEARING_POWER_WHEN_REMOVED;
        check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_OK, "wearing power stages");
        mdrHeadphonesGetPower(session.headphones, MDR_PROPERTY_DESIRED, &power);
        check(power.wearing_power == MDR_WEARING_POWER_WHEN_REMOVED &&
                  current_power.wearing_power != MDR_WEARING_POWER_WHEN_REMOVED,
              "wearing power follows the chosen view");
    }

    mdrHeadphonesGetVoiceGuidance(session.headphones, MDR_PROPERTY_DESIRED, &initial_voice);
    voice = initial_voice;
    voice.enabled = initial_voice.enabled == MDR_FALSE ? MDR_TRUE : MDR_FALSE;
    voice.volume = -2;
    check_result(mdrHeadphonesSetVoiceGuidance(session.headphones, &voice), MDR_RESULT_OK,
                 "negative voice volume stages");
    mdrHeadphonesGetVoiceGuidance(session.headphones, MDR_PROPERTY_DESIRED, &voice);
    check(voice.volume == (v1 ? initial_voice.volume : -2), "voice volume follows existing protocol support");
    voice.volume = 2;
    check_result(mdrHeadphonesSetVoiceGuidance(session.headphones, &voice), MDR_RESULT_OK,
                 "positive voice volume stages");
    mdrHeadphonesGetVoiceGuidance(session.headphones, MDR_PROPERTY_CURRENT, &current_voice);
    mdrHeadphonesGetVoiceGuidance(session.headphones, MDR_PROPERTY_DESIRED, &voice);
    check(current_voice.enabled == initial_voice.enabled && voice.enabled != initial_voice.enabled &&
              voice.volume == (v1 ? initial_voice.volume : 2),
          "desired voice volume edit retains pending enable");

    mdrHeadphonesGetConnectionMode(session.headphones, MDR_PROPERTY_CURRENT, &current_mode);
    mdrHeadphonesGetConnectionMode(session.headphones, MDR_PROPERTY_DESIRED, &mode);
    mode.audio_priority = current_mode.audio_priority == MDR_AUDIO_PRIORITY_QUALITY ? MDR_AUDIO_PRIORITY_STABILITY
                                                                                    : MDR_AUDIO_PRIORITY_QUALITY;
    check_result(mdrHeadphonesSetConnectionMode(session.headphones, &mode), MDR_RESULT_OK, "connection mode stages");
    mdrHeadphonesGetConnectionMode(session.headphones, MDR_PROPERTY_DESIRED, &mode);
    check(mode.audio_priority != current_mode.audio_priority, "connection mode reads desired in both protocols");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, v1 ? gs_v1 : gs_v2, v1 ? sizeof(gs_v1) : sizeof(gs_v2));
    device_run(&session, &device, MDR_EVENT_GENERAL_SETTINGS_CHANGED, "last general setting capability polls");
    check_result(mdrHeadphonesGetGeneralSetting(session.headphones, MDR_PROPERTY_DESIRED, setting_index, &setting),
                 MDR_RESULT_OK, "last general setting desired reads within each protocol array");
    setting.boolean_value = MDR_TRUE;
    check_result(mdrHeadphonesSetGeneralSetting(session.headphones, &setting), MDR_RESULT_OK, "general setting stages");
    mdrHeadphonesGetGeneralSetting(session.headphones, MDR_PROPERTY_CURRENT, setting_index, &current_setting);
    mdrHeadphonesGetGeneralSetting(session.headphones, MDR_PROPERTY_DESIRED, setting_index, &setting);
    check(current_setting.boolean_value == MDR_FALSE && setting.boolean_value == MDR_TRUE &&
              setting.index == setting_index,
          "general setting value selects view without changing index");

    if (!v1)
    {
        mdrHeadphonesGetListening(session.headphones, MDR_PROPERTY_DESIRED, &listening);
        listening.mode = MDR_LISTENING_BACKGROUND_MUSIC;
        listening.background_room = MDR_ROOM_MEDIUM;
        check_result(mdrHeadphonesSetListening(session.headphones, &listening), MDR_RESULT_OK,
                     "listening room and mode stage");
        mdrHeadphonesGetListening(session.headphones, MDR_PROPERTY_DESIRED, &listening);
        listening.background_room = MDR_ROOM_LARGE;
        check_result(mdrHeadphonesSetListening(session.headphones, &listening), MDR_RESULT_OK,
                     "second listening field stages");
        mdrHeadphonesGetListening(session.headphones, MDR_PROPERTY_CURRENT, &current_listening);
        mdrHeadphonesGetListening(session.headphones, MDR_PROPERTY_DESIRED, &listening);
        check(current_listening.mode == MDR_LISTENING_STANDARD && listening.mode == MDR_LISTENING_BACKGROUND_MUSIC &&
                  listening.background_room == MDR_ROOM_LARGE,
              "desired room edit preserves the enabled mode");
        listening.mode = MDR_LISTENING_VOICE_BOOST;
        check_result(mdrHeadphonesSetListening(session.headphones, &listening), MDR_RESULT_OK,
                     "desired mode switches mutually exclusively");
        mdrHeadphonesGetListening(session.headphones, MDR_PROPERTY_DESIRED, &listening);
        check(listening.mode == MDR_LISTENING_VOICE_BOOST && listening.background_room == MDR_ROOM_LARGE,
              "all listening enabled properties use the selected view");
        mdrHeadphonesGetSourceSwitchControl(session.headphones, MDR_PROPERTY_CURRENT, &current_source);
        check_result(
            mdrHeadphonesSetSourceSwitchControl(session.headphones, current_source == MDR_FALSE ? MDR_TRUE : MDR_FALSE),
            MDR_RESULT_OK, "source switch target stages");
        mdrHeadphonesGetSourceSwitchControl(session.headphones, MDR_PROPERTY_DESIRED, &source);
        check(source != current_source, "source switch selects desired");
        device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, measurement, sizeof(measurement));
        device_run(&session, &device, MDR_EVENT_SAFE_LISTENING_CHANGED, "safe listening measurement polls");
        mdrHeadphonesGetSafeListening(session.headphones, MDR_PROPERTY_DESIRED, &safe);
        safe.preview = MDR_TRUE;
        safe.sound_pressure = 256u;
        check_result(mdrHeadphonesSetSafeListening(session.headphones, &safe), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                     "wide measurement cannot truncate into the current value");
        safe.sound_pressure = UINT32_MAX;
        check_result(mdrHeadphonesSetSafeListening(session.headphones, &safe), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                     "maximum measurement cannot truncate into the current value");
        mdrHeadphonesGetSafeListening(session.headphones, MDR_PROPERTY_DESIRED, &safe);
        check(safe.preview == MDR_FALSE && safe.sound_pressure == 85,
              "invalid read-only measurement never stages preview");
        safe.preview = MDR_TRUE;
        check_result(mdrHeadphonesSetSafeListening(session.headphones, &safe), MDR_RESULT_OK,
                     "safe listening preview stages");
        mdrHeadphonesGetSafeListening(session.headphones, MDR_PROPERTY_CURRENT, &current_safe);
        mdrHeadphonesGetSafeListening(session.headphones, MDR_PROPERTY_DESIRED, &safe);
        check(current_safe.preview == MDR_FALSE && safe.preview == MDR_TRUE && current_safe.sound_pressure == 85 &&
                  safe.sound_pressure == current_safe.sound_pressure,
              "safe preview selects view but measurement never does");
    }

    /*
     * Every staged property has to leave the commit CLEAN, whether it was written, dropped
     * as unsupported, or consumed as a one-shot action. One that stays SUBMITTED keeps
     * IsDirty() true forever, so a client that commits whenever it is dirty and syncs only
     * when it is not never gets to sync again.
     */
    check(mdrHeadphonesIsDirty(session.headphones) != MDR_FALSE, "all edits above are still staged");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK,
                 "commit of every staged property starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "commit of every staged property completes");
    check(mdrHeadphonesIsReady(session.headphones) != MDR_FALSE &&
              mdrHeadphonesIsDirty(session.headphones) == MDR_FALSE,
          "no staged property survives a completed commit");
    if (v1)
    {
        mdrHeadphonesGetPower(session.headphones, MDR_PROPERTY_CURRENT, &current_power);
        mdrHeadphonesGetPower(session.headphones, MDR_PROPERTY_DESIRED, &power);
        check(power.head_gesture == current_power.head_gesture,
              "V1 head gesture, which has no write, is dropped rather than left desired");
    }
    check_result(mdrHeadphonesRequestSync(session.headphones, MDR_HEADPHONES_SYNC_DEFAULT), MDR_RESULT_OK,
                 "sync is reachable after the commit");
    device_run(&session, &device, MDR_EVENT_SYNC_COMPLETE, "sync after the commit completes");
    session_close(&session);
}

static void check_failed_session(Session* session, MDRResult failure)
{
    union
    {
        MDRModel model;
        MDRBattery battery;
        MDRPlayback playback;
        MDRNoiseControl noise;
        MDRSpeakToChat speak;
        MDRListening listening;
        MDREqualizer equalizer;
        MDREqualizerPreset preset;
        MDRPairedDevice device;
        MDRPairing pairing;
        MDRBoolean enabled;
        MDRSourceSwitchControlResult source_result;
        MDRGeneralSettingInfo setting_info;
        MDRGeneralSetting setting;
        MDRAssignableControl control;
        MDRAssignableAction action;
        MDRPower power;
        MDRVoiceGuidance voice;
        MDRConnectionMode connection;
        MDRSafeListening safe;
        MDRWearingStatus wearing;
        MDRHeadphonesSyncFlags sync;
        MDRFeatureAvailability feature;
        int32_t bands[5];
        char text[64];
    } output, before;
    union
    {
        MDRNoiseControl noise;
        MDRSpeakToChat speak;
        MDRListening listening;
        MDREqualizer equalizer;
        MDRPairing pairing;
        MDRGeneralSetting setting;
        MDRAssignableControl control;
        MDRPower power;
        MDRVoiceGuidance voice;
        MDRConnectionMode connection;
        MDRSafeListening safe;
        int32_t bands[5];
    } input;
    const MDRPlaybackCommand command = {MDR_PLAYBACK_PLAY};
    const MDRPairedDeviceAction action = {MDR_PAIRED_DEVICE_CONNECT, "00:11:22:33:44:55", 17};
    const MDRPropertyType views[] = {MDR_PROPERTY_CURRENT, MDR_PROPERTY_DESIRED};
    uint32_t count = 1;
    uint32_t text_size = sizeof(output.text);
    unsigned int poll_count = session->transport.poll_count;
    unsigned int send_count = session->transport.send_count;
    unsigned int receive_count = session->transport.receive_count;
    size_t tx_size = session->transport.tx_size;
    char* error = get_text(session->headphones, MDR_TEXT_LAST_ERROR);
    char* later_error;
    size_t i;

    memset(&output, 0xa5, sizeof(output));
    memcpy(&before, &output, sizeof(output));
    memset(&input, 0, sizeof(input));
    check(error != NULL, "failed instance retains readable error text");
    check(!mdrHeadphonesIsInitialized(session->headphones) && !mdrHeadphonesIsReady(session->headphones) &&
              !mdrHeadphonesIsDirty(session->headphones),
          "failed instance reports no usable state");

#define CHECK_FAILED(call) check_result((call), failure, #call " rejects failed instance")
    for (i = 0; i < sizeof(views) / sizeof(views[0]); ++i)
    {
        CHECK_FAILED(mdrHeadphonesGetPlayback(session->headphones, views[i], &output.playback));
        CHECK_FAILED(mdrHeadphonesGetNoiseControl(session->headphones, views[i], &output.noise));
        CHECK_FAILED(mdrHeadphonesGetSpeakToChat(session->headphones, views[i], &output.speak));
        CHECK_FAILED(mdrHeadphonesGetListening(session->headphones, views[i], &output.listening));
        CHECK_FAILED(mdrHeadphonesGetEqualizer(session->headphones, views[i], &output.equalizer));
        CHECK_FAILED(mdrHeadphonesGetEqualizerBands(session->headphones, views[i], output.bands, &count));
        CHECK_FAILED(mdrHeadphonesGetPairing(session->headphones, views[i], &output.pairing));
        CHECK_FAILED(mdrHeadphonesGetSourceSwitchControl(session->headphones, views[i], &output.enabled));
        CHECK_FAILED(mdrHeadphonesGetGeneralSetting(session->headphones, views[i], 0, &output.setting));
        CHECK_FAILED(mdrHeadphonesGetAssignableControls(session->headphones, views[i], &output.control, &count));
        CHECK_FAILED(mdrHeadphonesGetPower(session->headphones, views[i], &output.power));
        CHECK_FAILED(mdrHeadphonesGetVoiceGuidance(session->headphones, views[i], &output.voice));
        CHECK_FAILED(mdrHeadphonesGetConnectionMode(session->headphones, views[i], &output.connection));
        CHECK_FAILED(mdrHeadphonesGetSafeListening(session->headphones, views[i], &output.safe));
    }
    CHECK_FAILED(mdrHeadphonesGetFeature(session->headphones, MDR_FEATURE_IDENTITY, &output.feature));
    CHECK_FAILED(mdrHeadphonesGetModel(session->headphones, &output.model));
    CHECK_FAILED(mdrHeadphonesGetBatteries(session->headphones, &output.battery, &count));
    CHECK_FAILED(mdrHeadphonesGetEqualizerPresets(session->headphones, &output.preset, &count));
    CHECK_FAILED(mdrHeadphonesGetPairedDevices(session->headphones, &output.device, &count));
    CHECK_FAILED(mdrHeadphonesGetSourceSwitchControlResult(session->headphones, &output.source_result));
    CHECK_FAILED(mdrHeadphonesGetGeneralSettingInfo(session->headphones, &output.setting_info, &count));
    CHECK_FAILED(mdrHeadphonesGetAssignableControlActions(session->headphones, MDR_ASSIGNABLE_ACTION_KEY_LEFT,
                                                          &output.action, &count));
    CHECK_FAILED(mdrHeadphonesGetWearingStatus(session->headphones, &output.wearing));
    CHECK_FAILED(mdrHeadphonesGetSync(session->headphones, &output.sync));
    CHECK_FAILED(mdrHeadphonesGetText(session->headphones, MDR_TEXT_MODEL_NAME, 0, output.text, &text_size));
    check(memcmp(&output, &before, sizeof(output)) == 0 && count == 1 && text_size == sizeof(output.text),
          "failed getters preserve output buffers and sizes");

    CHECK_FAILED(mdrHeadphonesSetPlaybackVolume(session->headphones, 20));
    CHECK_FAILED(mdrHeadphonesSendPlaybackCommand(session->headphones, &command));
    CHECK_FAILED(mdrHeadphonesSetNoiseControl(session->headphones, &input.noise));
    CHECK_FAILED(mdrHeadphonesSetSpeakToChat(session->headphones, &input.speak));
    CHECK_FAILED(mdrHeadphonesSetListening(session->headphones, &input.listening));
    CHECK_FAILED(mdrHeadphonesSetEqualizerPreset(session->headphones, MDR_EQ_OFF));
    CHECK_FAILED(mdrHeadphonesSetEqualizerClearBass(session->headphones, 0));
    CHECK_FAILED(mdrHeadphonesSetDSEE(session->headphones, MDR_TRUE));
    CHECK_FAILED(mdrHeadphonesSetEqualizerBands(session->headphones, input.bands, 5));
    CHECK_FAILED(mdrHeadphonesSetPairedDevice(session->headphones, &action));
    CHECK_FAILED(mdrHeadphonesSetPairing(session->headphones, &input.pairing));
    CHECK_FAILED(mdrHeadphonesSetSourceSwitchControl(session->headphones, MDR_TRUE));
    CHECK_FAILED(mdrHeadphonesSetGeneralSetting(session->headphones, &input.setting));
    CHECK_FAILED(mdrHeadphonesSetAssignableControls(session->headphones, &input.control, 1));
    CHECK_FAILED(mdrHeadphonesSetPower(session->headphones, &input.power));
    CHECK_FAILED(mdrHeadphonesSetVoiceGuidance(session->headphones, &input.voice));
    CHECK_FAILED(mdrHeadphonesSetConnectionMode(session->headphones, &input.connection));
    CHECK_FAILED(mdrHeadphonesSetSafeListening(session->headphones, &input.safe));
    CHECK_FAILED(mdrHeadphonesSetAlertResponse(session->headphones, MDR_ALERT_ACTION_POSITIVE));
    CHECK_FAILED(mdrHeadphonesSetTimeout(session->headphones, MDR_TIMEOUT_DEFAULT, 1));
    CHECK_FAILED(mdrHeadphonesRequestInit(session->headphones, MDR_HEADPHONES_INIT_DEFAULT));
    CHECK_FAILED(mdrHeadphonesRequestSync(session->headphones, MDR_HEADPHONES_SYNC_DEFAULT));
    CHECK_FAILED(mdrHeadphonesRequestCommit(session->headphones));
    CHECK_FAILED(mdrDebugRequestCommand(session->headphones, NULL, 0, MDR_DATA_TYPE_DATA_MDR, MDR_FALSE));
    mdrDebugSetPacketCallback(session->headphones, NULL, NULL);

    session->transport.poll_result = MDR_RESULT_OK;
    session->transport.send_result = MDR_RESULT_OK;
    session->transport.receive_result = MDR_RESULT_OK;
    for (i = 0; i < 2; ++i)
    {
        MDREvent event = MDR_EVENT_APPLY_COMPLETE;
        CHECK_FAILED(mdrHeadphonesPoll(session->headphones, &event));
        check(event == MDR_EVENT_NONE, "failed poll never reports a completion event");
        session->transport.poll_result = MDR_RESULT_ERROR_NO_CONNECTION;
    }
#undef CHECK_FAILED
    check(session->transport.poll_count == poll_count && session->transport.send_count == send_count &&
              session->transport.receive_count == receive_count && session->transport.tx_size == tx_size,
          "failed instance performs no transport operations even when transport recovers");
    later_error = get_text(session->headphones, MDR_TEXT_LAST_ERROR);
    check(error != NULL && later_error != NULL && strcmp(error, later_error) == 0,
          "later operations preserve the first fatal error text");
    free(later_error);
    free(error);
}

static void test_fatal_failure(MDRProtocolVersion family, int source)
{
    static const unsigned char table1_v1[] = {0x07, 0, 1, 0xa1};
    static const unsigned char table1_v2[] = {0x07, 0, 1, 0xa1, 0xff};
    static const unsigned char table2[] = {0x07, 0, 0};
    static const unsigned char malformed_protocol[] = {0x01};
    Session session;
    Device device;
    MDRPlayback playback;
    MDREvent event = MDR_EVENT_NONE;
    MDRResult result;
    MDRResult failure = MDR_RESULT_ERROR_NET;
    int v1 = family == MDR_PROTOCOL_V1;

    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = v1;
    device.table1 = v1 ? table1_v1 : table1_v2;
    device.table1_size = v1 ? sizeof(table1_v1) : sizeof(table1_v2);
    device.table2 = table2;
    device.table2_size = sizeof(table2);
    device_run_init(&session, &device);
    check(mdrHeadphonesIsInitialized(session.headphones), "session is initialized before fatal failure");
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 31), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "caller validation error is not fatal");
    session.transport.poll_result = MDR_RESULT_ERROR_TIMEOUT;
    check_result(mdrHeadphonesPoll(session.headphones, &event), MDR_RESULT_OK, "idle poll timeout is not fatal");
    session.transport.poll_result = MDR_RESULT_INPROGRESS;
    check_result(mdrHeadphonesPoll(session.headphones, &event), MDR_RESULT_OK,
                 "temporary poll unavailability is not fatal");
    session.transport.poll_result = MDR_RESULT_OK;
    check(mdrHeadphonesIsReady(session.headphones), "recoverable conditions preserve readiness");
    if (source == 4)
    {
        check_result(mdrHeadphonesSetTimeout(session.headphones, MDR_TIMEOUT_ACK, 1), MDR_RESULT_OK,
                     "short ACK timeout sets");
        check_result(mdrHeadphonesSetTimeout(session.headphones, MDR_TIMEOUT_ACK_RETRIES, 1), MDR_RESULT_OK,
                     "single ACK attempt sets");
    }
    check_result(mdrHeadphonesSetPlaybackVolume(session.headphones, 10), MDR_RESULT_OK, "volume stages before failure");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "apply starts before failure");
    switch (source)
    {
    case 0:
        session.transport.poll_result = failure;
        break;
    case 1:
        session.transport.send_result = failure;
        break;
    case 2:
        session.transport.receive_result = failure;
        break;
    case 3:
        failure = MDR_RESULT_ERROR_MALFORMED_PAYLOAD;
        device_send(&device, MDR_DATA_TYPE_DATA_MDR, malformed_protocol, sizeof(malformed_protocol));
        break;
    case 4:
        failure = MDR_RESULT_ERROR_TIMEOUT;
        break;
    }
    result = mdrHeadphonesPoll(session.headphones, &event);
    if (source == 4)
    {
        const clock_t start = clock();
        while (result == MDR_RESULT_OK && clock() - start < CLOCKS_PER_SEC)
            result = mdrHeadphonesPoll(session.headphones, &event);
    }
    check_result(result, failure, "runtime failure invalidates the session");
    check(event == MDR_EVENT_NONE, "fatal failure does not report a semantic success");
    check_failed_session(&session, failure);
    session_close(&session);
    if (!session_open_family(&session, family))
        return;
    check(mdrHeadphonesIsReady(session.headphones), "new instance is not poisoned by old failure");
    check_result(mdrHeadphonesGetPlayback(session.headphones, MDR_PROPERTY_CURRENT, &playback), MDR_RESULT_OK,
                 "new instance accepts reads");
    session_close(&session);
}

int main(void)
{
    int failure_source;
    int arrival;
    for (arrival = 0; arrival < 3; ++arrival)
    {
        test_alert_responses(MDR_PROTOCOL_V1, arrival);
        test_alert_responses(MDR_PROTOCOL_V2, arrival);
    }
    for (failure_source = 0; failure_source < 5; ++failure_source)
    {
        test_fatal_failure(MDR_PROTOCOL_V1, failure_source);
        test_fatal_failure(MDR_PROTOCOL_V2, failure_source);
    }
    test_property_getter_contracts(MDR_PROTOCOL_V1);
    test_property_getter_contracts(MDR_PROTOCOL_V2);
    test_widened_inputs(MDR_PROTOCOL_V1);
    test_widened_inputs(MDR_PROTOCOL_V2);
    test_equalizer_independent_writes(MDR_PROTOCOL_V1);
    test_equalizer_independent_writes(MDR_PROTOCOL_V2);
    test_eq_array_views(MDR_PROTOCOL_V1);
    test_eq_array_views(MDR_PROTOCOL_V2);
    test_grouped_staging_during_apply(MDR_PROTOCOL_V1);
    test_grouped_staging_during_apply(MDR_PROTOCOL_V2);
    test_playback_property_views(MDR_PROTOCOL_V1);
    test_playback_property_views(MDR_PROTOCOL_V2);
    test_unedited_property_is_not_submitted(MDR_PROTOCOL_V1);
    test_unedited_property_is_not_submitted(MDR_PROTOCOL_V2);
    test_editable_property_views(MDR_PROTOCOL_V1, 0);
    test_editable_property_views(MDR_PROTOCOL_V2, 0);
    test_editable_property_views(MDR_PROTOCOL_V2, 1);
    test_abi_version_handshake();
    test_struct_and_buffer_contracts();
    test_one_operation_at_a_time(MDR_PROTOCOL_V1);
    test_one_operation_at_a_time(MDR_PROTOCOL_V2);
    test_committed_state_staging();
    test_playback_actions();
    test_poll_events();
    test_v2_selective_init();
    test_init_skips_unadvertised_functions();
    test_init_requests_advertised_functions();
    test_listening_modes();
    test_listening_mode_refused_by_the_device();
    test_background_music_without_error_codes();
    test_voice_assistant();
    test_equalizer_availability_follows_the_device();
    test_equalizer_presets_follow_the_capability();
    test_listening_mode_rereads_what_it_takes_away();
    test_transmit_sequence_ignores_inbound_frames();
    test_v2_bootstrap();
    test_staging_during_apply(MDR_PROTOCOL_V1);
    test_staging_during_apply(MDR_PROTOCOL_V2);
    test_connection_mode_names_its_inquired_type();
    test_v1_voice_guidance_detail_is_not_the_switch();
    test_v1_reply_before_ack();
    test_packet_counter();
    test_debug_command_arguments();
    test_debug_command(MDR_PROTOCOL_V1);
    test_debug_command(MDR_PROTOCOL_V2);
    test_debug_command_timeout();
    test_protocol_reply_generation(MDR_PROTOCOL_V1);
    test_protocol_reply_generation(MDR_PROTOCOL_V2);
    test_support_reply_generation(MDR_PROTOCOL_V1, 0);
    test_support_reply_generation(MDR_PROTOCOL_V1, 1);
    test_support_reply_generation(MDR_PROTOCOL_V2, 0);
    test_support_reply_generation(MDR_PROTOCOL_V2, 1);
    test_reply_before_ack_retry(MDR_PROTOCOL_V1);
    test_reply_before_ack_retry(MDR_PROTOCOL_V2);
    test_support_reply_timeout(MDR_PROTOCOL_V1);
    test_support_reply_timeout(MDR_PROTOCOL_V2);
    test_v1_sync_asks_for_the_track_names();
    test_v2_sync_asks_for_the_track_names();
    test_wearing_status();
    test_v2_assignable_controls(0);
    test_v2_assignable_controls(1);
    test_v1_assignable_controls();
    test_v1_device_values();
    test_v1_noise_eq_general_settings();
    test_power_wearing_capability(MDR_PROTOCOL_V1);
    test_power_wearing_capability(MDR_PROTOCOL_V2);
    test_v2_timed_power_capability();
    test_v2_equalizer_layout();

    if (g_failures != 0)
        fprintf(stderr, "%d test assertion(s) failed\n", g_failures);
    return g_failures != 0 ? 1 : 0;
}
