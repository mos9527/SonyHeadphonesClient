#include <mdr-c/Headphones.h>

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MDR_ASSERT_U32(type) \
    static_assert(sizeof(type) == sizeof(uint32_t), #type " must be uint32_t")
MDR_ASSERT_U32(MDRResult);
MDR_ASSERT_U32(MDRBoolean);
MDR_ASSERT_U32(MDRFeatureAvailability);
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
MDR_ASSERT_U32(MDRAudioPriority);
#undef MDR_ASSERT_U32

#undef MDR_ASSERT_C_STRUCT

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
    fprintf(
        stderr,
        "FAIL: %s (expected %ld, got %ld)\n",
        message,
        (long)expected,
        (long)actual
    );
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

    *received = 0;
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

    if (count > MOCK_BUFFER_CAPACITY - transport->tx_size)
        return MDR_RESULT_ERROR_BUFFER_TOO_SMALL;
    memcpy(transport->tx + transport->tx_size, source, count);
    transport->tx_size += count;
    *sent = size;
    return MDR_RESULT_OK;
}

static MDRResult mock_poll(void* user, int timeout)
{
    (void)user;
    (void)timeout;
    return MDR_RESULT_OK;
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
    check_result(
        mdrHeadphonesCreate(MDR_ABI_VERSION, &session->transport.connection, family, &session->headphones),
        MDR_RESULT_OK,
        "opaque headphones session opens"
    );
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

static size_t pack_frame(
    unsigned char type,
    unsigned char sequence,
    const unsigned char* payload,
    size_t payload_size,
    unsigned char output[FRAME_BUFFER_CAPACITY]
)
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

static size_t pack_data_frame(
    const unsigned char* payload,
    size_t payload_size,
    unsigned char sequence,
    unsigned char output[FRAME_BUFFER_CAPACITY]
)
{
    return pack_frame(
        MDR_DATA_TYPE_DATA_MDR,
        sequence,
        payload,
        payload_size,
        output
    );
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

    check(
        last_tx_sequence(transport, &sequence),
        "an outbound frame is available to acknowledge"
    );
    return pack_frame(
        MDR_DATA_TYPE_ACK,
        (unsigned char)(1 - sequence),
        NULL,
        0,
        output
    );
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

static const unsigned char k_v2_protocol_info[] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01
};

/* As above, but with the table 2 support byte set to ENABLE rather than DISABLE. */
static const unsigned char k_v2_protocol_info_both_tables[] = {
    0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00
};

/*
 * V1's reply is four bytes rather than eight: command, inquired type, and a big-endian
 * protocol version. It says nothing about tables - V1 works out whether it has a second one
 * from the advertised function list (see RefreshSupportV1).
 */
static const unsigned char k_v1_protocol_info[] = {
    0x01, 0x00, 0x00, 0x01
};

static void select_v2(Session* session, const char* message)
{
    unsigned char frame[FRAME_BUFFER_CAPACITY];
    MDREvent event;
    size_t frame_size = pack_data_frame(
        k_v2_protocol_info,
        sizeof(k_v2_protocol_info),
        0,
        frame
    );
    mock_load(&session->transport, frame, frame_size);
    poll_event(session->headphones, &event, message);
}

static void test_abi_version_handshake(void)
{
    MockTransport transport;
    MDRHeadphones* headphones = (MDRHeadphones*)0x1;

    mock_init(&transport);
    check_result(
        mdrHeadphonesCreate(MDR_ABI_VERSION + 1u, &transport.connection, MDR_PROTOCOL_V2, &headphones),
        MDR_RESULT_ERROR_ABI_MISMATCH,
        "a newer header is refused by an older library"
    );
    check(headphones == NULL, "a refused handshake leaves no instance behind");

    check_result(
        mdrHeadphonesCreate(0u, &transport.connection, MDR_PROTOCOL_V2, &headphones),
        MDR_RESULT_ERROR_ABI_MISMATCH,
        "an unversioned caller is refused"
    );

    check_result(
        mdrHeadphonesCreate(MDR_ABI_VERSION, &transport.connection, MDR_PROTOCOL_V2, &headphones),
        MDR_RESULT_OK,
        "the matching version is accepted"
    );
    check(headphones != NULL, "an accepted handshake yields an instance");
    mdrHeadphonesDestroy(headphones);

    headphones = NULL;
    check_result(
        mdrHeadphonesCreate(MDR_ABI_VERSION, &transport.connection, MDR_PROTOCOL_V1, &headphones),
        MDR_RESULT_OK,
        "the matching version accepts a V1 implementation"
    );
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
    check_result(
        mdrHeadphonesGetText(
            session.headphones, MDR_TEXT_LAST_ERROR, 0, NULL, &text_size
        ),
        MDR_RESULT_ERROR_INVALID_ARGUMENT,
        "text size query requires a zero input size"
    );
    text_size = 0;
    check_result(
        mdrHeadphonesGetText(
            session.headphones, MDR_TEXT_LAST_ERROR, 0, NULL, &text_size
        ),
        MDR_RESULT_OK,
        "text size query succeeds"
    );
    check(text_size > 1, "text size includes a NUL terminator");

    short_text_size = (uint32_t)sizeof(short_text);
    check_result(
        mdrHeadphonesGetText(
            session.headphones,
            MDR_TEXT_LAST_ERROR,
            0,
            short_text,
            &short_text_size
        ),
        MDR_RESULT_ERROR_BUFFER_TOO_SMALL,
        "text copy reports a short caller buffer"
    );
    check(short_text_size == text_size, "text copy returns the required size");

    text = (char*)malloc(text_size);
    copied_text_size = text_size;
    check(text != NULL, "text test allocation succeeds");
    if (text != NULL)
    {
        check_result(
            mdrHeadphonesGetText(
                session.headphones,
                MDR_TEXT_LAST_ERROR,
                0,
                text,
                &copied_text_size
            ),
            MDR_RESULT_OK,
            "text copy succeeds on the second call"
        );
        check(
            copied_text_size == text_size && text[text_size - 1] == '\0',
            "text copy is NUL terminated"
        );
        free(text);
    }

    session_close(&session);
}

static void test_one_operation_at_a_time(void)
{
    Session session;

    if (!session_open(&session))
        return;
    check_result(
        mdrHeadphonesRequestInit(session.headphones),
        MDR_RESULT_OK,
        "initialization starts"
    );
    check_result(
        mdrHeadphonesRequestSync(session.headphones),
        MDR_RESULT_INPROGRESS,
        "a second operation is rejected while initialization is active"
    );
    check(
        mdrHeadphonesIsReady(session.headphones) == MDR_FALSE,
        "status reports the active operation as busy"
    );
    session_close(&session);
}

static void test_committed_state_staging(void)
{
    Session session;
    MDRPlayback staged;
    MDRPlayback current;

    if (!session_open(&session))
        return;
    memset(&staged, 0, sizeof(staged));
    staged.status = MDR_PLAYBACK_UNKNOWN;
    staged.volume = 12;
    check_result(
        mdrHeadphonesSetPlayback(session.headphones, &staged),
        MDR_RESULT_OK,
        "playback volume stages"
    );

    memset(&current, 0, sizeof(current));
    check_result(
        mdrHeadphonesGetPlayback(session.headphones, &current),
        MDR_RESULT_OK,
        "committed playback is readable"
    );
    check(current.volume == 0, "staging does not alter current playback");

    check(
        mdrHeadphonesIsDirty(session.headphones) == MDR_TRUE,
        "staging marks the session dirty"
    );
    session_close(&session);
}

static void test_playback_actions(void)
{
    Session session;
    const MDRPlaybackAction actions[] = {
        MDR_PLAYBACK_PLAY,
        MDR_PLAYBACK_PAUSE,
        MDR_PLAYBACK_NEXT,
        MDR_PLAYBACK_PREVIOUS
    };
    size_t index;
    MDRPlaybackCommand command;
    unsigned char ack[FRAME_BUFFER_CAPACITY];
    size_t ack_size;
    MDRPlayback unsupported_status;
    MDREvent event;

    if (!session_open(&session))
        return;
    select_v2(&session, "V2 protocol is selected for playback actions");

    for (index = 0; index < sizeof(actions) / sizeof(actions[0]); ++index)
    {
        memset(&command, 0, sizeof(command));
        command.action = actions[index];
        check_result(
            mdrHeadphonesPlayback(session.headphones, &command),
            MDR_RESULT_OK,
            "supported playback action stages"
        );

        check(
            mdrHeadphonesIsDirty(session.headphones) == MDR_TRUE,
            "playback action is pending"
        );

        check_result(
            mdrHeadphonesRequestCommit(session.headphones),
            MDR_RESULT_OK,
            "playback action apply starts"
        );
        poll_event(session.headphones, &event, "playback action request flushes");
        ack_size = pack_ack(&session.transport, ack);
        mock_load(&session.transport, ack, ack_size);
        poll_event(session.headphones, &event, "playback action ACK polls");
        poll_event(session.headphones, &event, "playback action completion polls");
        check(event == MDR_EVENT_APPLY_COMPLETE, "playback action apply completes");

        check(
            mdrHeadphonesIsReady(session.headphones) == MDR_TRUE
                && mdrHeadphonesIsDirty(session.headphones) == MDR_FALSE,
            "playback action is consumed as a one-shot"
        );
    }

    memset(&command, 0, sizeof(command));
    command.action = (MDRPlaybackAction)0xff;
    check_result(
        mdrHeadphonesPlayback(session.headphones, &command),
        MDR_RESULT_ERROR_INVALID_ARGUMENT,
        "unknown playback action is rejected"
    );

    memset(&unsupported_status, 0, sizeof(unsupported_status));
    unsupported_status.status = MDR_PLAYBACK_PLAYING;
    unsupported_status.volume = 10;
    check_result(
        mdrHeadphonesSetPlayback(session.headphones, &unsupported_status),
        MDR_RESULT_ERROR_NOT_SUPPORTED,
        "playback status is not misrepresented as a staged volume change"
    );
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
    check_result(
        mdrHeadphonesPlayback(session.headphones, &command),
        MDR_RESULT_OK,
        "first action stages"
    );
    check_result(
        mdrHeadphonesRequestCommit(session.headphones),
        MDR_RESULT_OK,
        "first apply starts"
    );
    poll_event(session.headphones, &event, "first request flushes");
    check(
        last_tx_sequence(&session.transport, &first_sequence),
        "first request was transmitted"
    );

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
    frame_size = pack_data_frame(
        notification,
        sizeof(notification),
        first_sequence,
        frame
    );
    mock_load(&session.transport, frame, frame_size);
    poll_event(session.headphones, &event, "late inbound frame polls");
    check(event == MDR_EVENT_UNHANDLED, "late inbound frame is reported, not fatal");

    memset(&command, 0, sizeof(command));
    command.action = MDR_PLAYBACK_NEXT;
    check_result(
        mdrHeadphonesPlayback(session.headphones, &command),
        MDR_RESULT_OK,
        "second action stages"
    );
    check_result(
        mdrHeadphonesRequestCommit(session.headphones),
        MDR_RESULT_OK,
        "second apply starts"
    );
    poll_event(session.headphones, &event, "second request flushes");
    check(
        last_tx_sequence(&session.transport, &second_sequence),
        "second request was transmitted"
    );
    check(
        second_sequence != first_sequence,
        "the next request does not repeat the sequence number of an acknowledged frame"
    );
    session_close(&session);
}

/*
 * A device that understands the framing and acknowledges everything, but answers only the
 * two requests initialization cannot proceed without. Every other request is accepted and
 * dropped, which is what a real device does with a command it does not implement - so what
 * matters is that libmdr never asks for anything the advertised function list rules out.
 */
enum { MDR_DATA_TYPE_DATA_MDR_NO2 = 14, REQUEST_LOG_CAPACITY = 128 };

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

        ack_size = pack_frame(MDR_DATA_TYPE_ACK, (unsigned char)(1 - frame.sequence), NULL, 0, ack);
        mock_append(device->transport, ack, ack_size);

        if (table == 1 && frame.payload[0] == 0x00) /* CONNECT_GET_PROTOCOL_INFO */
        {
            if (device->protocol_v1)
                device_send(device, MDR_DATA_TYPE_DATA_MDR, k_v1_protocol_info, sizeof(k_v1_protocol_info));
            else
                device_send(
                    device,
                    MDR_DATA_TYPE_DATA_MDR,
                    k_v2_protocol_info_both_tables,
                    sizeof(k_v2_protocol_info_both_tables)
                );
        }
        /*
         * Voice guidance, answered the way a headset answers it: about the detail that was
         * asked for. Initialization asks about the switch, the language, the required time
         * and the download server in turn, so most of these replies are not the switch -
         * which is exactly the case that used to be read as one.
         */
        else if (device->protocol_v1 && table == 2 && frame.payload_size > 2
                 && (frame.payload[0] == 0x46 || frame.payload[0] == 0x42))
        {
            const unsigned char reply[4] = {
                (unsigned char)(frame.payload[0] + 1u), /* GET_PARAM -> RET_PARAM, likewise status */
                frame.payload[1],
                frame.payload[2],
                0x00
            };
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
        else if (table == 1 && frame.payload[0] == 0xe2 && frame.payload_size > 1
                 && frame.payload[1] == 0x01 && device->answer_status_disabled)
        {
            /* AUDIO_GET_STATUS UPSCALING -> AUDIO_RET_STATUS UPSCALING DISABLE */
            static const unsigned char answer[] = {0xe3, 0x01, 0x01};
            device_send(device, MDR_DATA_TYPE_DATA_MDR, answer, sizeof(answer));
        }
        else if (table == 1 && frame.payload[0] == 0x58 && device->eq_notification != NULL)
        {
            /* EQEBB_SET_PARAM */
            device_send(
                device,
                MDR_DATA_TYPE_DATA_MDR,
                device->eq_notification,
                device->eq_notification_size
            );
        }
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

/* Runs initialization to completion against `device`, or reports why it could not. */
static void device_run_init(Session* session, Device* device)
{
    check_result(
        mdrHeadphonesRequestInit(session->headphones),
        MDR_RESULT_OK,
        "initialization starts"
    );
    device_run(session, device, MDR_EVENT_INITIALIZE_COMPLETE, "initialization completes");
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
        0x07, 0x00, 0x06,
        0x50, 0xff, /* PRESET_EQ */
        0xa1, 0xff, /* PLAYBACK_CONTROLLER_WITH_CALL_VOLUME_ADJUSTMENT */
        0xf1, 0xff, /* PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF */
        0xe6, 0xff, /* LISTENING_OPTION */
        0xeb, 0xff, /* BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE - but no UPMIX_CINEMA */
        0x23, 0xff  /* POWER_OFF */
    };
    static const unsigned char table2[] = {
        0x07, 0x00, 0x01,
        0x42, 0xff /* VOICE_GUIDANCE_..._SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT */
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
    check(device_requested(&device, 1, 0xa6, -1), "PLAY_GET_PARAM for an advertised playback controller");
    check(device_requested(&device, 1, 0xa2, -1), "PLAY_GET_STATUS for an advertised playback controller");
    check(device_requested(&device, 1, 0xf6, 0x01), "PLAYBACK_CONTROL_BY_WEARING for advertised wearing control");
    check(device_requested(&device, 1, 0xe6, 0x09), "BGM_MODE for an advertised background-music mode");
    check(device_requested(&device, 2, 0x46, -1), "VOICE_GUIDANCE_GET_PARAM for advertised voice guidance");
    check(
        !device_requested(&device, 1, 0xe6, 0x04),
        "no UPMIX_CINEMA when only the background-music half of LISTENING_OPTION is advertised"
    );
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
        0x07, 0x00, 0x04,
        0xe6, 0xff, /* LISTENING_OPTION */
        0xeb, 0xff, /* BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE */
        0xe8, 0xff, /* VOICE_CONTENTS */
        0xe9, 0xff  /* SOUND_LEAKAGE_REDUCTION - but no UPMIX_CINEMA */
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
    check_result(
        mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_LISTENING_VOICE_BOOST, &availability),
        MDR_RESULT_OK,
        "voice boost availability is readable"
    );
    check(availability == MDR_AVAILABILITY_AVAILABLE, "voice boost is available");
    availability = MDR_AVAILABILITY_UNKNOWN;
    check_result(
        mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_LISTENING_CINEMA, &availability),
        MDR_RESULT_OK,
        "cinema availability is readable"
    );
    check(availability == MDR_AVAILABILITY_UNAVAILABLE, "cinema is unavailable");

    /* A mode the device does not offer cannot be selected. */
    memset(&listening, 0, sizeof(listening));
    listening.mode = MDR_LISTENING_CINEMA;
    check_result(
        mdrHeadphonesSetListening(session.headphones, &listening),
        MDR_RESULT_ERROR_NOT_SUPPORTED,
        "an unoffered mode is refused"
    );

    memset(&listening, 0, sizeof(listening));
    listening.mode = MDR_LISTENING_BACKGROUND_MUSIC;
    listening.background_room = MDR_ROOM_MEDIUM;
    check_result(
        mdrHeadphonesSetListening(session.headphones, &listening),
        MDR_RESULT_OK,
        "background music stages"
    );
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "background music apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "background music applies");

    memset(&listening, 0, sizeof(listening));
    check_result(
        mdrHeadphonesGetListening(session.headphones, &listening),
        MDR_RESULT_OK,
        "listening mode is readable"
    );
    check(listening.mode == MDR_LISTENING_BACKGROUND_MUSIC, "background music is the active mode");

    /*
     * Switching modes must deactivate the old one before activating the new one - a device
     * that refuses to hold two at once would reject the second command otherwise.
     */
    device.log_size = 0;
    memset(&listening, 0, sizeof(listening));
    listening.mode = MDR_LISTENING_VOICE_BOOST;
    check_result(
        mdrHeadphonesSetListening(session.headphones, &listening),
        MDR_RESULT_OK,
        "voice boost stages"
    );
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "voice boost apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "voice boost applies");

    off_order = device_request_order(&device, 1, 0xe8, 0x09); /* AUDIO_SET_PARAM, BGM */
    on_order = device_request_order(&device, 1, 0xe8, 0x06);  /* AUDIO_SET_PARAM, voice contents */
    check(off_order >= 0, "the outgoing mode is switched off");
    check(on_order >= 0, "the incoming mode is switched on");
    check(off_order < on_order, "the outgoing mode is switched off before the incoming one is switched on");

    memset(&listening, 0, sizeof(listening));
    check_result(
        mdrHeadphonesGetListening(session.headphones, &listening),
        MDR_RESULT_OK,
        "listening mode is readable after the switch"
    );
    check(listening.mode == MDR_LISTENING_VOICE_BOOST, "voice boost is the active mode");
    check(
        mdrHeadphonesIsDirty(session.headphones) == MDR_FALSE,
        "the applied mode leaves nothing pending"
    );
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
        0x07, 0x00, 0x04,
        0xe6, 0xff, /* LISTENING_OPTION */
        0xeb, 0xff, /* BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE */
        0x50, 0xff, /* PRESET_EQ */
        0xe2, 0xff  /* UPSCALING_AUTO_OFF */
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
    check_result(
        mdrHeadphonesGetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "equalizer is readable"
    );
    check(equalizer.available != MDR_FALSE, "the equalizer starts available");
    check(equalizer.dsee_available != MDR_FALSE, "DSEE starts available");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, eq_off, sizeof(eq_off));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "the equalizer being disabled polls");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, dsee_off, sizeof(dsee_off));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "DSEE being disabled polls");

    memset(&equalizer, 0, sizeof(equalizer));
    check_result(
        mdrHeadphonesGetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "equalizer is readable while disabled"
    );
    check(equalizer.available == MDR_FALSE, "a disabled equalizer reads unavailable");
    check(equalizer.dsee_available == MDR_FALSE, "disabled DSEE reads unavailable");

    /* Support is unchanged throughout - the device still has an equalizer, just not now. */
    check(
        mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_EQUALIZER, &availability_scratch) == MDR_RESULT_OK &&
        availability_scratch == MDR_AVAILABILITY_AVAILABLE,
        "the equalizer feature stays supported while unavailable"
    );

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, eq_on, sizeof(eq_on));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "the equalizer being re-enabled polls");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, dsee_on, sizeof(dsee_on));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "DSEE being re-enabled polls");

    memset(&equalizer, 0, sizeof(equalizer));
    check_result(
        mdrHeadphonesGetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "equalizer is readable once re-enabled"
    );
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
        0x07, 0x00, 0x01,
        0x50, 0xff /* PRESET_EQ */
    };
    static const unsigned char table2[] = {0x07, 0x00, 0x00};
    /* EQEBB_RET_CAPABILITY PRESET_EQ: ten bands of thirteen steps, and three named presets. */
    static const unsigned char capability[] = {
        0x51, 0x00,
        0x0a, 0x0d,
        0x03,
        0x00, 0x03, 'O', 'f', 'f',
        0x10, 0x06, 'B', 'r', 'i', 'g', 'h', 't',
        0xa0, 0x06, 'C', 'u', 's', 't', 'o', 'm'
    };

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

    check(
        device_requested(&device, 1, 0x50, 0x00),
        "EQEBB_GET_CAPABILITY PRESET_EQ for an advertised equalizer"
    );

    count = 0;
    check_result(
        mdrHeadphonesGetEqualizerPresets(session.headphones, NULL, &count),
        MDR_RESULT_OK,
        "the preset count is readable before the device answers"
    );
    check(count == 0, "no presets are known before the device answers");

    /* Nothing has been said, so nothing is refused - the caller is no worse off than before. */
    memset(&equalizer, 0, sizeof(equalizer));
    check_result(
        mdrHeadphonesGetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "equalizer is readable"
    );
    equalizer.preset = MDR_EQ_ROCK;
    check_result(
        mdrHeadphonesSetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "any encodable preset is accepted while the list is unknown"
    );

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, capability, sizeof(capability));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "the preset list polls");

    count = 0;
    check_result(
        mdrHeadphonesGetEqualizerPresets(session.headphones, NULL, &count),
        MDR_RESULT_OK,
        "the preset count is readable"
    );
    check_result((MDRResult)count, 3, "every advertised preset is counted");

    count = 1;
    check_result(
        mdrHeadphonesGetEqualizerPresets(session.headphones, presets, &count),
        MDR_RESULT_ERROR_BUFFER_TOO_SMALL,
        "a short preset buffer is refused"
    );
    check_result((MDRResult)count, 3, "a refused read reports the size it needs");

    count = 8;
    check_result(
        mdrHeadphonesGetEqualizerPresets(session.headphones, presets, &count),
        MDR_RESULT_OK,
        "the preset list is readable"
    );
    check_result((MDRResult)count, 3, "the list holds what the device advertised");
    check(presets[0] == MDR_EQ_OFF, "the first preset is the one listed first");
    check(presets[1] == MDR_EQ_BRIGHT, "the second preset is the one listed second");
    check(presets[2] == MDR_EQ_CUSTOM, "the third preset is the one listed third");

    /* The index into the list is also the index into its names. */
    name_size = sizeof(name);
    check_result(
        mdrHeadphonesGetText(session.headphones, MDR_TEXT_EQUALIZER_PRESET_NAME, 1, name, &name_size),
        MDR_RESULT_OK,
        "a preset name is readable"
    );
    check(strcmp(name, "Bright") == 0, "the name is the one the device gave that preset");
    name_size = sizeof(name);
    check_result(
        mdrHeadphonesGetText(session.headphones, MDR_TEXT_EQUALIZER_PRESET_NAME, 3, name, &name_size),
        MDR_RESULT_ERROR_NOT_FOUND,
        "a name past the end of the list is not found"
    );

    memset(&equalizer, 0, sizeof(equalizer));
    check_result(
        mdrHeadphonesGetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "equalizer is readable once the list is known"
    );
    equalizer.preset = MDR_EQ_BRIGHT;
    check_result(
        mdrHeadphonesSetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "an advertised preset is accepted"
    );
    equalizer.preset = MDR_EQ_ROCK;
    check_result(
        mdrHeadphonesSetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_ERROR_NOT_SUPPORTED,
        "a preset the device never listed is refused"
    );
    session_close(&session);
}

/*
 * Picking a preset used to land on CUSTOM with a flat curve. The device recomputes the band
 * steps for the preset and reports them while the commit is still running, which moves the band
 * config's `current` away from the values submitted at the top of that pass - making a config
 * nobody touched look pending, and sending it. Band steps are what makes an EQ custom, so the
 * device dutifully switched to CUSTOM and threw the preset away. Observed on a WF-LC900 for
 * Heavy, Clear, Hard and Soft alike.
 */
static void test_preset_change_does_not_write_bands(void)
{
    static const unsigned char table1[] = {
        0x07, 0x00, 0x01,
        0x50, 0xff /* PRESET_EQ */
    };
    static const unsigned char table2[] = {0x07, 0x00, 0x00};
    /* EQEBB_RET_PARAM: preset OFF, ten bands, all flat - the state the device starts in. */
    static const unsigned char flat[] = {
        0x57, 0x00, 0x00, 0x0a,
        0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06
    };
    /* EQEBB_NTFY_PARAM: Heavy, and the curve the device computed for it. */
    static const unsigned char heavy[] = {
        0x59, 0x00, 0x30, 0x0a,
        0x0a, 0x0a, 0x05, 0x05, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06
    };
    static const int8_t heavy_bands[] = {4, 4, -1, -1, 0, 0, 0, 0, 0, 0};

    Session session;
    Device device;
    MDREqualizer equalizer;
    int8_t bands[16];
    uint32_t count;
    size_t index;
    int band_writes = 0;
    int i;

    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = table1;
    device.table1_size = sizeof(table1);
    device.table2 = table2;
    device.table2_size = sizeof(table2);

    device_run_init(&session, &device);

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, flat, sizeof(flat));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "the starting curve polls");

    /* From here the device answers any EQEBB_SET_PARAM the way the hardware does. */
    device.eq_notification = heavy;
    device.eq_notification_size = sizeof(heavy);

    memset(&equalizer, 0, sizeof(equalizer));
    check_result(
        mdrHeadphonesGetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "equalizer is readable"
    );
    equalizer.preset = MDR_EQ_HEAVY;
    check_result(
        mdrHeadphonesSetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "the preset stages"
    );
    check_result(
        mdrHeadphonesRequestCommit(session.headphones),
        MDR_RESULT_OK,
        "the preset change starts"
    );
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "the preset change completes");

    for (index = 0; index < device.log_size; ++index)
    {
        const RequestLog* entry = &device.log[index];
        /*
         * EQEBB_SET_PARAM carrying band steps. A preset-only write is four bytes - command,
         * inquired type, preset id, and the zero count of an empty band array - so anything
         * longer is a curve.
         */
        if (entry->table == 1 && entry->command == 0x58 && entry->payload_size > 4)
            ++band_writes;
    }
    check(band_writes == 0, "a preset change writes no band steps");

    memset(&equalizer, 0, sizeof(equalizer));
    check_result(
        mdrHeadphonesGetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "equalizer is readable after the change"
    );
    check(equalizer.preset == MDR_EQ_HEAVY, "the preset the device reported is the one that stands");

    count = sizeof(bands);
    check_result(
        mdrHeadphonesGetEqualizerBands(session.headphones, bands, &count),
        MDR_RESULT_OK,
        "the bands are readable after the change"
    );
    check_result((MDRResult)count, 10, "the device's band count stands");
    for (i = 0; i < 10; ++i)
        check(bands[i] == heavy_bands[i], "the preset's own curve stands");

    /* An actual band edit still goes out - the guard is about intent, not about the preset. */
    band_writes = 0;
    device.eq_notification = NULL;
    for (i = 0; i < 10; ++i)
        bands[i] = 1;
    check_result(
        mdrHeadphonesSetEqualizerBands(session.headphones, bands, 10),
        MDR_RESULT_OK,
        "a band edit stages"
    );
    check_result(
        mdrHeadphonesRequestCommit(session.headphones),
        MDR_RESULT_OK,
        "the band edit starts"
    );
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "the band edit completes");
    for (index = 0; index < device.log_size; ++index)
    {
        const RequestLog* entry = &device.log[index];
        if (entry->table == 1 && entry->command == 0x58 && entry->payload_size > 4)
            ++band_writes;
    }
    check(band_writes == 1, "an edited band config is transmitted");
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
        0x07, 0x00, 0x04,
        0xe6, 0xff, /* LISTENING_OPTION */
        0xeb, 0xff, /* BGM_MODE_SMALL_MIDDLE_LARGE_AND_ERRORCODE */
        0x50, 0xff, /* PRESET_EQ */
        0xe2, 0xff  /* UPSCALING_AUTO_OFF */
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
    check_result(
        mdrHeadphonesGetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "equalizer is readable"
    );
    check(equalizer.available != MDR_FALSE, "the equalizer starts available");
    check(equalizer.dsee_available != MDR_FALSE, "DSEE starts available");

    /* From here the device answers a status request, and notifies nothing of its own. */
    device.answer_status_disabled = 1;

    /* Initialization asks for both statuses too, so only what follows the switch counts. */
    requests_before = device.log_size;

    memset(&listening, 0, sizeof(listening));
    mdrHeadphonesGetListening(session.headphones, &listening);
    listening.mode = MDR_LISTENING_BACKGROUND_MUSIC;
    listening.background_room = MDR_ROOM_SMALL;
    check_result(
        mdrHeadphonesSetListening(session.headphones, &listening),
        MDR_RESULT_OK,
        "the listening mode stages"
    );
    check_result(
        mdrHeadphonesRequestCommit(session.headphones),
        MDR_RESULT_OK,
        "the listening change starts"
    );
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
    check_result(
        mdrHeadphonesGetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "equalizer is readable after the switch"
    );
    check(
        equalizer.available == MDR_FALSE,
        "the equalizer reads unavailable without having been notified"
    );
    check(
        equalizer.dsee_available == MDR_FALSE,
        "DSEE reads unavailable without having been notified"
    );
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
    protocol_size = pack_data_frame(
        k_v2_protocol_info,
        sizeof(k_v2_protocol_info),
        0,
        protocol_frame
    );
    unknown_size = pack_data_frame(
        unknown_payload,
        sizeof(unknown_payload),
        1,
        unknown_frame
    );
    mock_load(&session.transport, protocol_frame, protocol_size);
    mock_append(&session.transport, unknown_frame, unknown_size);

    poll_event(session.headphones, &first, "first frame polls");
    poll_event(session.headphones, &second, "second frame polls");
    check(
        first == MDR_EVENT_IDENTITY_CHANGED,
        "protocol state change is reported by its poll"
    );
    check(
        second == MDR_EVENT_UNHANDLED,
        "unhandled frame is reported by its poll"
    );
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
    check_result(
        mdrHeadphonesRequestInit(session.headphones),
        MDR_RESULT_OK,
        "automatic initialization starts"
    );
    poll_event(session.headphones, &event, "protocol-info request flushes");
    frame_size = pack_ack(&session.transport, frame);
    mock_load(&session.transport, frame, frame_size);
    poll_event(session.headphones, &event, "protocol-info request ACK polls");

    frame_size = pack_data_frame(
        k_v2_protocol_info,
        sizeof(k_v2_protocol_info),
        0,
        frame
    );
    mock_load(&session.transport, frame, frame_size);
    poll_event(session.headphones, &event, "eight-byte V2 protocol-info polls");

    memset(&identity, 0, sizeof(identity));
    check_result(
        mdrHeadphonesGetModel(session.headphones, &identity),
        MDR_RESULT_OK,
        "V2 identity is readable"
    );
    check(identity.protocol_version == 2, "eight-byte payload selects MDR V2");
    check(
        event == MDR_EVENT_IDENTITY_CHANGED,
        "V2 protocol selection reports identity change"
    );

    check(
        mdrHeadphonesIsInitialized(session.headphones) == MDR_FALSE
            && mdrHeadphonesIsReady(session.headphones) == MDR_FALSE,
        "V2 bootstrap automatically continues into backend initialization"
    );
    session_close(&session);
}

static void test_newer_staging_survives_apply(void)
{
    Session session;
    MDRPlayback first;
    MDRPlayback newer;
    MDRPlayback current;
    unsigned char ack[FRAME_BUFFER_CAPACITY];
    size_t ack_size;
    MDREvent ack_event;
    MDREvent completion;

    if (!session_open(&session))
        return;
    select_v2(&session, "V2 protocol is selected for apply");

    memset(&first, 0, sizeof(first));
    first.status = MDR_PLAYBACK_UNKNOWN;
    first.volume = 10;
    check_result(
        mdrHeadphonesSetPlayback(session.headphones, &first),
        MDR_RESULT_OK,
        "first playback value stages"
    );
    check_result(
        mdrHeadphonesRequestCommit(session.headphones),
        MDR_RESULT_OK,
        "apply starts"
    );

    memset(&newer, 0, sizeof(newer));
    newer.status = MDR_PLAYBACK_UNKNOWN;
    newer.volume = 20;
    check_result(
        mdrHeadphonesSetPlayback(session.headphones, &newer),
        MDR_RESULT_OK,
        "newer playback value stages during apply"
    );

    poll_event(session.headphones, &ack_event, "apply request flushes");
    ack_size = pack_ack(&session.transport, ack);
    mock_load(&session.transport, ack, ack_size);
    poll_event(session.headphones, &ack_event, "apply ACK polls");
    poll_event(session.headphones, &completion, "apply completion polls");
    check(
        ack_event == MDR_EVENT_UNHANDLED
            && completion == MDR_EVENT_APPLY_COMPLETE,
        "ACK and completion are reported in poll order"
    );

    memset(&current, 0, sizeof(current));
    check_result(
        mdrHeadphonesGetPlayback(session.headphones, &current),
        MDR_RESULT_OK,
        "applied playback is readable"
    );
    check(current.volume == first.volume, "apply commits its original snapshot");

    check(
        mdrHeadphonesIsReady(session.headphones) == MDR_TRUE
            && mdrHeadphonesIsDirty(session.headphones) == MDR_TRUE,
        "newer value remains pending after apply completes"
    );
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
        0x07, 0x00, 0x01,
        0xe1, 0xff /* CONNECTION_MODE_SOUND_QUALITY_CONNECTION_QUALITY */
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
    check_result(
        mdrHeadphonesSetConnectionMode(session.headphones, &mode),
        MDR_RESULT_OK,
        "the connection mode stages"
    );
    check_result(
        mdrHeadphonesRequestCommit(session.headphones),
        MDR_RESULT_OK,
        "the connection mode change starts"
    );
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
    check(
        mdrHeadphonesIsInitialized(session.headphones) != MDR_FALSE,
        "a reply about a detail other than the switch does not end the session"
    );
    check(
        device_requested(&device, 2, 0x46, 0x01), /* VOICE_GUIDANCE_GET_PARAM, VOICE_GUIDANCE_SETTING */
        "the voice-guidance parameters were asked for at all"
    );
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
        track = track || entry->detail == 0x00;  /* TRACK_NAME */
        album = album || entry->detail == 0x01;  /* ALBUM_NAME */
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
    check_result(
        mdrHeadphonesRequestSync(session.headphones),
        MDR_RESULT_OK,
        "a sync starts"
    );
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
    check_result(
        mdrHeadphonesRequestSync(session.headphones),
        MDR_RESULT_OK,
        "a sync starts"
    );
    device_run(&session, &device, MDR_EVENT_SYNC_COMPLETE, "the sync completes");
    /* V2 names no detail: one PLAY_GET_PARAM for the playback control brings the metadata
     * back with it, which is why this asks only that the request was made again. */
    check(
        device_requested_after(&device, 1, 0xa6, after_init), /* PLAY_GET_PARAM */
        "a V2 sync asks for the playback state again"
    );
    session_close(&session);
}

/*
 * EQEBB_SET_PARAM carries a preset and band steps together, and RequestCommitV1 always sent
 * both: the preset that was staged, followed by every band step as it stood. A V1 device
 * takes one of them at a time. A WH-1000XM4 acknowledges a frame carrying both and drops it,
 * keeping the preset and the curve it already had, so neither a preset change nor a band
 * move ever took. Reported against that headset.
 */
static void test_v1_preset_and_curve_never_share_a_frame(void)
{
    /* PRESET_EQ only. */
    static const unsigned char table1[] = {0x07, 0x00, 0x01, 0x51};
    /* EQEBB_RET_PARAM PRESET_EQ: preset OFF, then six flat steps - a V1 frame counts clear
     * bass as the first of its band steps, so this is clear bass and five bands. */
    static const unsigned char flat[] = {
        0x57, 0x01, 0x00, 0x06,
        0x0a, 0x0a, 0x0a, 0x0a, 0x0a, 0x0a
    };

    Session session;
    Device device;
    MDREqualizer equalizer;
    int8_t bands[8];
    size_t index;
    size_t after;
    int frames = 0;
    int carried_steps = 0;
    int named_a_preset = 0;
    int i;

    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = 1;
    device.table1 = table1;
    device.table1_size = sizeof(table1);

    device_run_init(&session, &device);

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, flat, sizeof(flat));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "the starting curve polls");

    /* A preset change goes out as the preset alone. */
    after = device.log_size;
    memset(&equalizer, 0, sizeof(equalizer));
    check_result(
        mdrHeadphonesGetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "equalizer is readable"
    );
    equalizer.preset = MDR_EQ_BASS;
    check_result(
        mdrHeadphonesSetEqualizer(session.headphones, &equalizer),
        MDR_RESULT_OK,
        "the preset stages"
    );
    check_result(
        mdrHeadphonesRequestCommit(session.headphones),
        MDR_RESULT_OK,
        "the preset change starts"
    );
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "the preset change completes");

    for (index = after; index < device.log_size; ++index)
    {
        const RequestLog* entry = &device.log[index];
        if (entry->table != 1 || entry->command != 0x58) /* EQEBB_SET_PARAM */
            continue;
        ++frames;
        /* Command, inquired type, preset id, and the zero count of an empty band array -
         * anything longer carries a curve. The preset id is the byte the log keeps as the
         * detail. */
        if (entry->payload_size > 4)
            ++carried_steps;
        if (entry->has_detail && entry->detail == 0x16) /* BASS */
            ++named_a_preset;
    }
    check(frames == 1, "a V1 preset change is one frame");
    check(carried_steps == 0, "it carries no band steps");
    check(named_a_preset == 1, "it names the preset that was asked for");

    /* A curve goes out with the preset left UNSPECIFIED, and the device selects CUSTOM. */
    after = device.log_size;
    frames = 0;
    carried_steps = 0;
    named_a_preset = 0;
    for (i = 0; i < 5; ++i)
        bands[i] = 1;
    check_result(
        mdrHeadphonesSetEqualizerBands(session.headphones, bands, 5),
        MDR_RESULT_OK,
        "a band edit stages"
    );
    check_result(
        mdrHeadphonesRequestCommit(session.headphones),
        MDR_RESULT_OK,
        "the band edit starts"
    );
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "the band edit completes");

    for (index = after; index < device.log_size; ++index)
    {
        const RequestLog* entry = &device.log[index];
        if (entry->table != 1 || entry->command != 0x58)
            continue;
        ++frames;
        if (entry->payload_size > 4)
            ++carried_steps;
        if (entry->has_detail && entry->detail != 0xff) /* UNSPECIFIED */
            ++named_a_preset;
    }
    check(frames == 1, "a V1 band edit is one frame");
    check(carried_steps == 1, "it carries the band steps");
    check(named_a_preset == 0, "it names no preset alongside them");
    session_close(&session);
}

int main(void)
{
    test_abi_version_handshake();
    test_struct_and_buffer_contracts();
    test_one_operation_at_a_time();
    test_committed_state_staging();
    test_playback_actions();
    test_poll_events();
    test_init_skips_unadvertised_functions();
    test_init_requests_advertised_functions();
    test_listening_modes();
    test_equalizer_availability_follows_the_device();
    test_equalizer_presets_follow_the_capability();
    test_preset_change_does_not_write_bands();
    test_listening_mode_rereads_what_it_takes_away();
    test_transmit_sequence_ignores_inbound_frames();
    test_v2_bootstrap();
    test_newer_staging_survives_apply();
    test_connection_mode_names_its_inquired_type();
    test_v1_voice_guidance_detail_is_not_the_switch();
    test_v1_sync_asks_for_the_track_names();
    test_v2_sync_asks_for_the_track_names();
    test_v1_preset_and_curve_never_share_a_frame();

    if (g_failures != 0)
        fprintf(stderr, "%d test assertion(s) failed\n", g_failures);
    return g_failures != 0 ? 1 : 0;
}
