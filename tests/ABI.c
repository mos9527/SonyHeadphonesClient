#if !defined(_WIN32) && !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif

#include <mdr-c/Headphones.h>

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <errno.h>
#include <time.h>
#endif

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
MDR_ASSERT_U32(MDRNCOptimizerStatus);
MDR_ASSERT_U32(MDRNCOptimizerResult);
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
#undef MDR_ASSERT_U32
static_assert(MDR_ABI_VERSION == 2u, "optimizer extension keeps ABI version 2");
static_assert(MDR_FEATURE_NC_OPTIMIZER == 35u, "optimizer feature id is appended");
static_assert(MDR_FEATURE_MAX_VALUE == MDR_FEATURE_NC_OPTIMIZER, "optimizer feature is queryable");
static_assert(MDR_EVENT_NC_OPTIMIZER_CHANGED == 25u, "optimizer event id is appended");
static_assert(sizeof(MDRNCOptimizer) == 32u, "optimizer C struct has a stable layout");
static_assert(offsetof(MDRNCOptimizer, pressure_tenths_atm) == 24u, "optimizer pressure offset is stable");
static_assert(offsetof(MDRNCOptimizer, optimization_seconds) == 28u, "optimizer duration offset is stable");

#undef MDR_ASSERT_C_STRUCT

enum
{
    MOCK_BUFFER_CAPACITY = 4096,
    FRAME_BUFFER_CAPACITY = 256,
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
    0x01, 0x00, 0x03, 0x00, 0x10, 0x04, 0x00, 0x01
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
 * Protocol fixtures are keyed by the complete GET payload, not by arithmetic on its
 * opcode. Unknown GETs receive no RET and fail the test; SETs receive transport ACKs.
 * Bodies use the wire schema (length-prefixed strings/lists, variant-specific fields).
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
    const unsigned char* assignable_capability;
    size_t assignable_capability_size;
    const unsigned char* assignable_params;
    size_t assignable_params_size;
    /* When set, a reply goes out ahead of the ACK for the request it answers, the way a
     * WH-1000XM3 orders them. */
    int reply_before_ack;
    /* A duplicate RET is independently framed before the request's ACK. */
    int duplicate_reply;
    const unsigned char* protocol_info;
    size_t protocol_info_size;
    const unsigned char* eq_capability;
    size_t eq_capability_size;
    int voice_support_onoff;
    int voice_support_language;
    int bgm_on;
    /* Withhold a single response while still acknowledging the request. */
    unsigned char hold_table;
    unsigned char hold_command;
    unsigned char hold_inquired;
    int hold_has_detail;
    unsigned char hold_detail;
    int hold_active;
    unsigned char held_type;
    unsigned char held_payload[FRAME_BUFFER_CAPACITY];
    size_t held_size;
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

typedef struct ResponseFixture
{
    unsigned char family; /* 0 = both families, 1 = V1, 2 = V2 */
    unsigned char table;
    unsigned char request_size;
    unsigned char response_size;
    unsigned char request[4];
    unsigned char response[64];
} ResponseFixture;

static const ResponseFixture k_responses[] = {
    {2, 1, 3, 8, {0x02, 0x01, 0x01}, {0x03, 0x01, 1, 4, 'u', 'n', 'i', 't'}},
    {0, 1, 2, 8, {0x02, 0x00}, {0x03, 0x00, 1, 4, 'u', 'n', 'i', 't'}},
    {0, 1, 2, 8, {0x04, 0x01}, {0x05, 0x01, 5, 'M', 'o', 'd', 'e', 'l'}},
    {0, 1, 2, 8, {0x04, 0x02}, {0x05, 0x02, 5, '1', '.', '0', '.', '0'}},
    {0, 1, 2, 4, {0x04, 0x03}, {0x05, 0x03, 0, 0}},
    {0, 1, 2, 3, {0x04, 0x04}, {0x05, 0x04, 0}},
    /* EQ: English capability, enabled status, flat curve and matching band metadata. */
    {2, 1, 3, 5, {0x50, 0x00, 1}, {0x51, 0x00, 10, 13, 0}},
    {2, 1, 3, 5, {0x50, 0x02, 1}, {0x51, 0x02, 10, 13, 0}},
    {2, 1, 3, 6, {0x50, 0x03, 1}, {0x51, 0x03, 10, 13, 2, 0}},
    {2, 1, 3, 5, {0x50, 0x04, 1}, {0x51, 0x04, 10, 13, 0}},
    {1, 1, 3, 5, {0x50, 0x01, 1}, {0x51, 0x01, 6, 21, 0}},
    {1, 1, 3, 5, {0x50, 0x03, 1}, {0x51, 0x03, 6, 21, 0}},
    {2, 1, 2, 3, {0x52, 0x00}, {0x53, 0x00, 0}},
    {2, 1, 2, 3, {0x52, 0x02}, {0x53, 0x02, 0}},
    {2, 1, 2, 3, {0x52, 0x03}, {0x53, 0x03, 0}},
    {2, 1, 2, 4, {0x52, 0x04}, {0x53, 0x04, 0, 0}},
    {1, 1, 2, 3, {0x52, 0x01}, {0x53, 0x01, 0}},
    {1, 1, 2, 3, {0x52, 0x03}, {0x53, 0x03, 0}},
    {2, 1, 2, 14, {0x56, 0x00}, {0x57, 0x00, 0x10, 10, 6,6,6,6,6,6,6,6,6,6}},
    {2, 1, 2, 14, {0x56, 0x02}, {0x57, 0x02, 0x10, 10, 6,6,6,6,6,6,6,6,6,6}},
    {2, 1, 2, 15, {0x56, 0x03}, {0x57, 0x03, 0x10, 0, 10, 6,6,6,6,6,6,6,6,6,6}},
    {2, 1, 2, 14, {0x56, 0x04}, {0x57, 0x04, 0x10, 10, 6,6,6,6,6,6,6,6,6,6}},
    {1, 1, 2, 10, {0x56, 0x01}, {0x57, 0x01, 0x10, 6, 10,10,10,10,10,10}},
    {1, 1, 2, 10, {0x56, 0x03}, {0x57, 0x03, 0x10, 6, 10,10,10,10,10,10}},
    {2, 1, 2, 23, {0x5a, 0x00}, {0x5b, 0x00, 10, 1,31, 1,63, 1,125, 1,250, 2,0, 2,1, 2,2, 2,4, 2,8, 2,16}},
    {2, 1, 2, 23, {0x5a, 0x02}, {0x5b, 0x02, 10, 1,31, 1,63, 1,125, 1,250, 2,0, 2,1, 2,2, 2,4, 2,8, 2,16}},
    {1, 1, 2, 21, {0x5a, 0x03}, {0x5b, 0x03, 6, 0x10,0,1, 1,1,0x90, 2,0,1, 2,0,2, 2,0,6, 2,0,16}},
    {2, 1, 2, 23, {0x5a, 0x03}, {0x5b, 0x03, 10, 1,31, 1,63, 1,125, 1,250, 2,0, 2,1, 2,2, 2,4, 2,8, 2,16}},
    {2, 1, 2, 23, {0x5a, 0x04}, {0x5b, 0x04, 10, 1,31, 1,63, 1,125, 1,250, 2,0, 2,1, 2,2, 2,4, 2,8, 2,16}},
    {1, 1, 2, 21, {0x5a, 0x01}, {0x5b, 0x01, 6, 0x10,0,1, 1,1,0x90, 2,0,1, 2,0,2, 2,0,6, 2,0,16}},
    {1, 1, 3, 4, {0x50, 0x02, 1}, {0x51, 0x02, 0xf6, 10}},
    {1, 1, 2, 3, {0x52, 0x02}, {0x53, 0x02, 0}},
    {1, 1, 2, 3, {0x56, 0x02}, {0x57, 0x02, 4}},
    {1, 1, 2, 3, {0x60, 1}, {0x61, 1, 0}},
    {1, 1, 2, 6, {0x60, 2}, {0x61, 2, 0,1, 0,0}},
    {1, 1, 2, 4, {0x60, 3}, {0x61, 3, 0,0}},
    {1, 1, 2, 3, {0x62, 1}, {0x63, 1, 0}},
    {1, 1, 2, 3, {0x62, 2}, {0x63, 2, 0}},
    {1, 1, 2, 3, {0x62, 3}, {0x63, 3, 0}},
    {1, 1, 2, 4, {0x66, 1}, {0x67, 1, 0,0}},
    {1, 1, 2, 8, {0x66, 2}, {0x67, 2, 0,0,0,0,0,0}},
    {1, 1, 2, 6, {0x66, 3}, {0x67, 3, 0,0,0,0}},
    /* Firmware identity strings reload even when firmware update is not advertised. */
    {1, 1, 2, 4, {0x36, 2}, {0x37, 2, 1,'c'}},
    {1, 1, 2, 4, {0x36, 3}, {0x37, 3, 1,'s'}},
    {1, 1, 2, 4, {0x36, 4}, {0x37, 4, 1,'n'}},
    {1, 1, 2, 4, {0x36, 5}, {0x37, 5, 1,'l'}},
    {1, 1, 2, 4, {0x36, 6}, {0x37, 6, 1,'i'}},
    {1, 1, 2, 3, {0x36, 7}, {0x37, 7, 0}},
    {1, 1, 2, 3, {0x36, 8}, {0x37, 8, 20}},
    {1, 1, 2, 3, {0x36, 9}, {0x37, 9, 1}},
    {1, 1, 2, 3, {0x36, 10}, {0x37, 10, 10}},
    {1, 1, 2, 6, {0x36, 11}, {0x37, 11, 3,'u','i','d'}},
    {1, 1, 2, 3, {0xf2, 6}, {0xf3, 6, 0}},
    /* Playback capabilities and volumes deliberately differ across variants. */
    {1, 1, 2, 5, {0xa0, 0x01}, {0xa1, 0x01, 100, 1, 1}},
    {2, 1, 2, 4, {0xa0, 0x01}, {0xa1, 0x01, 100, 100}},
    {2, 1, 2, 4, {0xa0, 0x02}, {0xa1, 0x02, 100, 100}},
    {2, 1, 2, 3, {0xa0, 0x03}, {0xa1, 0x03, 100}},
    {1, 1, 2, 4, {0xa2, 0x01}, {0xa3, 0x01, 0, 1}},
    {2, 1, 2, 5, {0xa2, 0x01}, {0xa3, 0x01, 0, 1, 0}},
    {2, 1, 2, 6, {0xa2, 0x02}, {0xa3, 0x02, 0, 1, 0, 0}},
    {2, 1, 2, 5, {0xa2, 0x03}, {0xa3, 0x03, 0, 1, 0}},
    {2, 1, 2, 10, {0xa6, 0x01}, {0xa7, 0x01, 0,0, 0,0, 0,0, 0,0}},
    {2, 1, 2, 10, {0xa6, 0x02}, {0xa7, 0x02, 0,0, 0,0, 0,0, 0,0}},
    {2, 1, 2, 10, {0xa6, 0x03}, {0xa7, 0x03, 0,0, 0,0, 0,0, 0,0}},
    {2, 1, 2, 3, {0xa6, 0x20}, {0xa7, 0x20, 37}},
    {2, 1, 2, 3, {0xa6, 0x21}, {0xa7, 0x21, 22}},
    {2, 1, 2, 4, {0xa6, 0x30}, {0xa7, 0x30, 37, 1}},
    {2, 1, 2, 4, {0xa6, 0x31}, {0xa7, 0x31, 22, 1}},
    {1, 1, 3, 5, {0xa6, 0x01, 0}, {0xa7, 0x01, 0, 0, 0}},
    {1, 1, 3, 5, {0xa6, 0x01, 1}, {0xa7, 0x01, 1, 0, 0}},
    {1, 1, 3, 5, {0xa6, 0x01, 2}, {0xa7, 0x01, 2, 0, 0}},
    {1, 1, 3, 5, {0xa6, 0x01, 3}, {0xa7, 0x01, 3, 0, 0}},
    {1, 1, 3, 4, {0xa6, 0x01, 4}, {0xa7, 0x01, 4, 37}},
    {1, 1, 3, 4, {0xa6, 0x01, 0x20}, {0xa7, 0x01, 0x20, 37}},
    /* Listening capabilities carry an empty exclusive-function list, not a mode value. */
    {2, 1, 2, 2, {0xe0, 0x00}, {0xe1, 0x00}},
    {2, 1, 2, 3, {0xe0, 0x01}, {0xe1, 0x01, 1}},
    {2, 1, 2, 3, {0xe0, 0x03}, {0xe1, 0x03, 0}},
    {2, 1, 2, 3, {0xe0, 0x04}, {0xe1, 0x04, 0}},
    {2, 1, 2, 3, {0xe0, 0x06}, {0xe1, 0x06, 0}},
    {2, 1, 2, 3, {0xe0, 0x07}, {0xe1, 0x07, 0}},
    {2, 1, 2, 3, {0xe0, 0x09}, {0xe1, 0x09, 0}},
    {2, 1, 2, 2, {0xe2, 0x00}, {0xe3, 0x00}},
    {2, 1, 2, 3, {0xe2, 0x01}, {0xe3, 0x01, 0}},
    {2, 1, 2, 3, {0xe2, 0x03}, {0xe3, 0x03, 0}},
    {2, 1, 2, 3, {0xe2, 0x04}, {0xe3, 0x04, 0}},
    {2, 1, 2, 3, {0xe2, 0x06}, {0xe3, 0x06, 0}},
    {2, 1, 2, 3, {0xe2, 0x07}, {0xe3, 0x07, 0}},
    {2, 1, 2, 4, {0xe2, 0x09}, {0xe3, 0x09, 0, 0}},
    {2, 1, 2, 3, {0xe6, 0x00}, {0xe7, 0x00, 0}},
    {2, 1, 2, 3, {0xe6, 0x01}, {0xe7, 0x01, 0}},
    {2, 1, 2, 4, {0xe6, 0x03}, {0xe7, 0x03, 1, 0}},
    {2, 1, 2, 3, {0xe6, 0x04}, {0xe7, 0x04, 1}},
    {2, 1, 2, 3, {0xe6, 0x06}, {0xe7, 0x06, 1}},
    {2, 1, 2, 3, {0xe6, 0x07}, {0xe7, 0x07, 1}},
    {2, 1, 2, 4, {0xe6, 0x09}, {0xe7, 0x09, 1, 0}},
    /* Per-key assignable status is a counted list. */
    {2, 1, 2, 3, {0xf2, 0x03}, {0xf3, 0x03, 0}},
    {2, 1, 2, 3, {0xf2, 0x0e}, {0xf3, 0x0e, 0}},
    {2, 2, 2, 3, {0xf2, 0x00}, {0xf3, 0x00, 4}},
    {2, 1, 2, 8, {0x22, 0x09}, {0x23, 0x09, 61,0,20, 62,0,20}},
    {2, 2, 2, 9, {0x50, 0x01}, {0x51, 0x01, 60, 0,0,0,1, 1,10}},
    {2, 2, 2, 3, {0x56, 0x01}, {0x57, 0x01, 0}},
    {2, 1, 2, 7, {0xfa, 0x03}, {0xfb, 0x03, 1, 0x22, 1,0,0}},
    {2, 1, 2, 7, {0xfa, 0x0e}, {0xfb, 0x0e, 1, 0x22, 1,0,0}},
    /* BGM's separate LE Audio limitation holder returns an unavailable-reason byte. */
    {2, 1, 2, 3, {0x42, 0xf5}, {0x43, 0xf5, 0}},
    /* Optimizer firmware fixture: two supported optimizers; measured 0.9 atm. */
    {1, 1, 2, 7, {0x80, 0x01}, {0x81, 0x01, 2, 1,9, 1,2}},
    {1, 1, 2, 4, {0x82, 0x01}, {0x83, 0x01, 0,0}},
    {1, 1, 2, 6, {0x86, 0x01}, {0x87, 0x01, 1,1, 1,9}},
    /* Captured XM4 capabilities: fixed four timeout bytes and one-byte string lengths. */
    {1, 1, 2, 12, {0xf0, 5}, {0xf1,5,0,1,0,0,0,0,15,30,60,0}},
    {1, 1, 2, 4, {0xf2, 5}, {0xf3,5,0,0}},
    {1, 1, 2, 4, {0xf6, 5}, {0xf7,5,0,1}},
    {1, 1, 2, 6, {0xfa, 5}, {0xfb,5,0,0,1,1}},
    {1, 1, 2, 11, {0x1c,1}, {0x1d,1,8,'c','d','2','c','5','5','b','4'}},
    {1, 1, 2, 20, {0x1c,0}, {0x1d,0,17,'0','0',':','1','1',':','2','2',':','3','3',':','4','4',':','5','5'}},
    /* V1 voice details are independent responses, with a real method payload. */
    {1, 2, 3, 4, {0x42, 1,1}, {0x43, 1,1,0}},
    {1, 2, 3, 4, {0x42, 1,2}, {0x43, 1,2,0}},
    {1, 2, 3, 4, {0x46, 1,1}, {0x47, 1,1,0}},
    {1, 2, 3, 4, {0x46, 1,2}, {0x47, 1,2,1}},
    {1, 2, 3, 4, {0x46, 1,3}, {0x47, 1,3,10}},
    {1, 2, 3, 7, {0x46, 1,4}, {0x47, 1,4,1,0,0,0}},
    {1, 2, 3, 6, {0x46, 1,5}, {0x47, 1,5,0x10,0,20}},
    {2, 2, 3, 4, {0x42, 0,0}, {0x43, 0,0,0}},
    {2, 2, 3, 4, {0x42, 0,1}, {0x43, 0,1,0}},
    {2, 2, 3, 4, {0x42, 1,0}, {0x43, 1,0,0}},
    {2, 2, 3, 4, {0x42, 1,1}, {0x43, 1,1,0}},
    {2, 2, 3, 4, {0x42, 2,0}, {0x43, 2,0,0}},
    {2, 2, 3, 4, {0x42, 2,1}, {0x43, 2,1,0}},
    {2, 2, 2, 3, {0x42, 3}, {0x43, 3,0}},
    {2, 2, 2, 3, {0x42, 0x20}, {0x43, 0x20,0}},
    {2, 2, 2, 3, {0x42, 0x21}, {0x43, 0x21,0}},
    {2, 2, 2, 3, {0x46, 0}, {0x47, 0,0}},
    {2, 2, 2, 4, {0x46, 1}, {0x47, 1,0,1}},
    {2, 2, 2, 4, {0x46, 2}, {0x47, 2,0,1}},
    {2, 2, 2, 3, {0x46, 3}, {0x47, 3,0}},
    {2, 2, 2, 3, {0x46, 0x20}, {0x47, 0x20,2}},
    {2, 2, 2, 3, {0x46, 0x21}, {0x47, 0x21,2}},
    {2, 2, 2, 4, {0x4a, 1}, {0x4b, 1,0,0}}
};

static void device_reply(Device* device, unsigned char type,
                         const unsigned char* payload, size_t size, const TxFrame* request,
                         unsigned char table)
{
    if (device->hold_active && table == device->hold_table &&
        request->payload_size > 1 && request->payload[0] == device->hold_command &&
        request->payload[1] == device->hold_inquired &&
        (!device->hold_has_detail || (request->payload_size > 2 &&
                                     request->payload[2] == device->hold_detail)))
    {
        check(size <= sizeof(device->held_payload), "withheld response fits");
        if (size <= sizeof(device->held_payload))
        {
            memcpy(device->held_payload, payload, size);
            device->held_size = size;
            device->held_type = type;
        }
        return;
    }
    device_send(device, type, payload, size);
    if (device->duplicate_reply)
        device_send(device, type, payload, size);
}

static void device_fixture_reply(Device* device, const TxFrame* frame, unsigned char table)
{
    size_t index;
    const unsigned char family = device->protocol_v1 ? 1 : 2;
    for (index = 0; index < sizeof(k_responses) / sizeof(k_responses[0]); ++index)
    {
        const ResponseFixture* fixture = &k_responses[index];
        if ((fixture->family == 0 || fixture->family == family) && fixture->table == table &&
            fixture->request_size == frame->payload_size &&
            memcmp(fixture->request, frame->payload, frame->payload_size) == 0)
        {
            device_reply(device, frame->type, fixture->response, fixture->response_size, frame, table);
            return;
        }
    }
    /* Set commands have no required RET. No catch-all GET response is allowed. */
    if (family == 1 && table == 1 && frame->payload[0] == 0x22)
        return; /* COMMON_SET_POWER_OFF */
    switch (frame->payload[0])
    {
        case 0x58: case 0xa4: case 0xa8: case 0xe8: case 0xf8: case 0x84:
            return;
        default:
            fprintf(stderr, "No fixture: V%u table%u command%02x inquiry%02x size%zu\n",
                    family, table, frame->payload[0],
                    frame->payload_size > 1 ? frame->payload[1] : 0, frame->payload_size);
            check(0, "every initialization GET has an explicit protocol response fixture");
    }
}

/* Acknowledge requests and deliver independently framed matching RETs. */
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
        if (!device->reply_before_ack)
            mock_append(device->transport, ack, ack_size);

        if (table == 1 && frame.payload[0] == 0x00) /* CONNECT_GET_PROTOCOL_INFO */
        {
            const unsigned char v1[] = {0x01, 0x00, 0x70, 0x00};
            const unsigned char v2[] = {0x01, 0x00, 0x03, 0x00, 0x10, 0x04, 0x00, 0x00};
            const unsigned char* info = device->protocol_info;
            size_t size = device->protocol_info_size;
            check(frame.payload_size == 2 && frame.payload[1] == 0,
                  "protocol-info request uses the fixed-value inquiry");
            if (info == NULL)
            {
                info = device->protocol_v1 ? v1 : v2;
                size = device->protocol_v1 ? sizeof(v1) : sizeof(v2);
            }
            device_reply(device, MDR_DATA_TYPE_DATA_MDR, info, size, &frame, table);
        }
        else if (table == 2 && frame.payload[0] == 0x40 && frame.payload_size == 2)
        {
            unsigned char voice[10] = {0x41, 0, 0,0,0,0,0, 1,1};
            size_t size;
            voice[1] = frame.payload[1];
            if (device->protocol_v1)
            {
                voice[2] = (unsigned char)device->voice_support_onoff;
                voice[3] = (unsigned char)device->voice_support_language;
                voice[4] = 1;
                voice[5] = 1;
                size = device->voice_support_language ? 6 : 4;
            }
            else if (voice[1] == 0 || voice[1] == 1)
            {
                voice[6] = (unsigned char)device->voice_support_onoff;
                size = voice[1] == 0 ? 7 : 9;
            }
            else if (voice[1] == 2)
            {
                voice[2] = (unsigned char)device->voice_support_onoff;
                voice[3] = 1;
                voice[4] = 1;
                size = 5;
            }
            else if (voice[1] == 3)
                size = 2;
            else if (voice[1] == 0x20 || voice[1] == 0x21)
            {
                voice[2] = 5;
                size = 3;
            }
            else
            {
                check(0, "voice capability inquiry is a known setting or volume variant");
                size = 0;
            }
            if (size != 0)
                device_reply(device, frame.type, voice, size, &frame, table);
        }
        else if (table == 1 && frame.payload_size == 2 && frame.payload[0] == 0xe6 &&
                 (frame.payload[1] == 3 || frame.payload[1] == 9) && device->bgm_on)
        {
            const unsigned char bgm[] = {0xe7, frame.payload[1], 0, 0};
            device_reply(device, frame.type, bgm, sizeof(bgm), &frame, table);
        }
        else if (table == 1 && frame.payload_size == 2 && frame.payload[0] == 0xf2 &&
                 device->assignable_params != NULL)
        {
            unsigned char statuses[64] = {0xf3, 0, 0};
            const unsigned char count = device->assignable_params[2];
            check(frame.payload[1] == device->assignable_params[1],
                  "assignable status GET uses the discovered variant");
            check(count <= sizeof(statuses) - 3, "key statuses fit the fixture");
            statuses[1] = device->assignable_params[1];
            statuses[2] = count;
            if (count <= sizeof(statuses) - 3)
                device_reply(device, frame.type, statuses, (size_t)count + 3, &frame, table);
        }
        else if (table == 1 && frame.payload[0] == 0x50 && device->eq_capability != NULL)
        {
            check(frame.payload_size == 3 && frame.payload[1] == device->eq_capability[1] &&
                  frame.payload[2] == 1, "equalizer capability uses the advertised variant and English");
            device_reply(device, frame.type, device->eq_capability, device->eq_capability_size, &frame, table);
        }
        else if (frame.payload[0] == 0x06) /* CONNECT_GET_SUPPORT_FUNCTION */
        {
            const unsigned char* support = table == 1 ? device->table1 : device->table2;
            const size_t size = table == 1 ? device->table1_size : device->table2_size;
            check(frame.payload_size == 2 && frame.payload[1] == 0,
                  "support-function request uses the fixed-value inquiry");
            check(support != NULL && size >= 3, "the requested support table has an explicit fixture");
            if (support != NULL && size >= 3)
                device_reply(device, frame.type, support, size, &frame, table);
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
        else if (table == 1 && frame.payload[0] == 0xf0 && device->assignable_capability != NULL)
        {
            check(frame.payload_size == 2 && frame.payload[1] == device->assignable_capability[1],
                  "assignable capability GET uses its advertised inquiry");
            device_reply(device, frame.type, device->assignable_capability,
                         device->assignable_capability_size, &frame, table);
        }
        else if (table == 1 && frame.payload[0] == 0xf6 && device->assignable_params != NULL)
        {
            check(frame.payload_size == 2 && frame.payload[1] == device->assignable_params[1],
                  "assignable parameter GET uses its capability inquiry");
            device_reply(device, frame.type, device->assignable_params,
                         device->assignable_params_size, &frame, table);
        }
        else
            device_fixture_reply(device, &frame, table);
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

    /* The initial enabled RET statuses, not incidental defaults, make both available. */
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
    device.eq_capability = capability;
    device.eq_capability_size = sizeof(capability);

    device_run_init(&session, &device);

    check(
        device_requested(&device, 1, 0x50, 0x00),
        "EQEBB_GET_CAPABILITY PRESET_EQ for an advertised equalizer"
    );


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

    /* Initial enabled RET statuses make both available. */
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
    check(identity.protocol_version == 0x03001004u, "eight-byte payload exposes the V2 wire protocol version");
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

static void test_staging_is_rejected_during_apply(void)
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
        MDR_RESULT_INPROGRESS,
        "a playback value is rejected during apply"
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
            && mdrHeadphonesIsDirty(session.headphones) == MDR_FALSE,
        "rejected value does not remain pending after apply completes"
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
    device.voice_support_onoff = 1;
    device.voice_support_language = 1;

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
    check(
        mdrHeadphonesIsInitialized(session.headphones) != MDR_FALSE,
        "a reply that arrives ahead of its ACK still completes initialization"
    );
    check(
        device_requested(&device, 1, 0x06, -1), /* CONNECT_GET_SUPPORT_FUNCTION */
        "initialization moves past the protocol handshake"
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

/*
 * The advertised table-two proximity checker supplies current worn/removed state.
 * Notifications update it, and operation-log transitions trigger a fresh status read.
 */
static void test_wearing_status(void)
{
    static const unsigned char table1[] = {0x07, 0x00, 0x00};
    static const unsigned char table2[] = {0x07, 0x00, 0x01, 0xf0, 0xff};
    /* SYSTEM_RET_STATUS, WEARING_STATUS_CHECKER, BOTH_NOT_WEAR */
    static const unsigned char ret_removed[] = {0xf3, 0x00, 0x04};
    /* SYSTEM_NTFY_STATUS, WEARING_STATUS_CHECKER, NORMAL */
    static const unsigned char ntfy_worn[] = {0xf5, 0x00, 0x00};
    /* LOG_NTFY_PARAM, TIME_SERIES_OPERATIONLOG_NOTIFIER, "unitRemove" and no value. */
    static const unsigned char log_remove[] = {
        0xc9, 0x01, 0x0a, 'u', 'n', 'i', 't', 'R', 'e', 'm', 'o', 'v', 'e', 0x00, 0x00
    };

    Session session;
    Device device;
    MDRWearingStatus status;
    MDRFeatureAvailability available = MDR_AVAILABILITY_UNKNOWN;
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
    check(
        device_requested(&device, 2, 0xf2, 0x00), /* SYSTEM_GET_STATUS, WEARING_STATUS_CHECKER */
        "the advertised checker is queried during live-state reload"
    );
    check_result(
        mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_WEARING_STATUS, &available),
        MDR_RESULT_OK,
        "the wearing status feature id is within the accepted range"
    );
    check(
        available == MDR_AVAILABILITY_AVAILABLE,
        "the advertised checker is available after its initial RET"
    );

    device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, ret_removed, sizeof(ret_removed));
    device_run(&session, &device, MDR_EVENT_WEARING_STATUS_CHANGED, "the checker reply polls");
    status = MDR_WEARING_STATUS_UNKNOWN;
    check_result(
        mdrHeadphonesGetWearingStatus(session.headphones, &status),
        MDR_RESULT_OK,
        "the wearing status is readable"
    );
    check(status == MDR_WEARING_STATUS_REMOVED, "BOTH_NOT_WEAR reads as removed");
    mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_WEARING_STATUS, &available);
    check(
        available == MDR_AVAILABILITY_AVAILABLE,
        "the checker remains available after a repeated RET"
    );

    device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, ntfy_worn, sizeof(ntfy_worn));
    device_run(&session, &device, MDR_EVENT_WEARING_STATUS_CHANGED, "the checker notification polls");
    mdrHeadphonesGetWearingStatus(session.headphones, &status);
    check(status == MDR_WEARING_STATUS_WORN, "NORMAL reads as worn");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, log_remove, sizeof(log_remove));
    device_run(&session, &device, MDR_EVENT_NEED_SYNC, "a unitRemove log entry polls");

    after_log = device.log_size;
    check_result(
        mdrHeadphonesRequestSync(session.headphones),
        MDR_RESULT_OK,
        "the sync a removal asks for starts"
    );
    device_run(&session, &device, MDR_EVENT_SYNC_COMPLETE, "the sync completes");
    check(
        device_requested_after(&device, 2, 0xf2, after_log), /* SYSTEM_GET_STATUS */
        "a sync re-reads the wearing status"
    );
    session_close(&session);
}

static void test_v2_assignable_controls(int limited)
{
    /* Non-left/right capability order, three keys, and distinct V2 preset variants. */
    static const unsigned char normal_capability[] = {
        0xf1, 0x03, 3,
        1, 0, 0xff, 2, 0x22, 1, 0, 0, 0x20, 0xff, 1, 0, 0, 0,
        2, 1, 0xff, 2, 0x30, 1, 0, 0, 0x30, 0xff, 1, 0, 0, 0,
        0, 0, 0xff, 2, 0x10, 1, 0, 0, 0x23, 0xff, 1, 0, 0, 0
    };
    /* One C button; BT Classic caution preset must not collapse to its plain variant. */
    static const unsigned char limited_capability[] = {
        0xf1, 0x0e, 0, 1,
        3, 1, 0xff, 2, 0x43, 1, 0, 0, 2, 0xff, 1, 0, 0, 0
    };
    static const unsigned char normal_params[] = {0xf7, 0x03, 3, 0xff, 0xff, 0xff};
    static const unsigned char limited_params[] = {0xf7, 0x0e, 1, 0xff};
    const unsigned char support[] = {0x07, 0x00, 1, (unsigned char)(limited ? 0xfe : 0xf3), 0};
    static const unsigned char table2[] = {0x07, 0x00, 0};
    const unsigned char notification[] = {
        0xf9, (unsigned char)(limited ? 0x0e : 0x03),
        (unsigned char)(limited ? 1 : 3), (unsigned char)(limited ? 0x43 : 0x22), 0x30, 0x10
    };
    const MDRAssignableControl desired[] = {
        {MDR_ASSIGNABLE_ACTION_KEY_LEFT, MDR_ASSIGNABLE_ACTION_KEY_TYPE_TOUCH_SENSOR, MDR_ASSIGNABLE_VOLUME},
        {MDR_ASSIGNABLE_ACTION_KEY_CUSTOM, MDR_ASSIGNABLE_ACTION_KEY_TYPE_BUTTON, MDR_ASSIGNABLE_VOICE_RECOGNITION},
        {MDR_ASSIGNABLE_ACTION_KEY_RIGHT, MDR_ASSIGNABLE_ACTION_KEY_TYPE_TOUCH_SENSOR,
         MDR_ASSIGNABLE_PLAYBACK_VOICE_ASSISTANT_LIMITATION}
    };
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
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, NULL, &count),
                 MDR_RESULT_OK, "capability-driven control count queries");
    check(count == (limited ? 1u : 3u), "control count follows advertised keys");
    count = 0;
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, controls, &count),
                 MDR_RESULT_ERROR_BUFFER_TOO_SMALL, "control copy detects short buffer");
    check(count == (limited ? 1u : 3u), "short control buffer reports required count");
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, controls, &count),
                 MDR_RESULT_OK, "controls read after initialization");
    check(controls[0].location == (limited ? MDR_ASSIGNABLE_ACTION_KEY_C : MDR_ASSIGNABLE_ACTION_KEY_RIGHT),
          "controls retain capability order and key identity");
    check(controls[0].type == (limited ? MDR_ASSIGNABLE_ACTION_KEY_TYPE_BUTTON : MDR_ASSIGNABLE_ACTION_KEY_TYPE_TOUCH_SENSOR),
          "controls expose advertised key type");
    check(controls[0].action == MDR_ASSIGNABLE_NONE, "parameter response supplies current assignment");
    option_count = 2;
    check_result(mdrHeadphonesGetAssignableControlActions(session.headphones, controls[0].location, options, &option_count),
                 MDR_RESULT_OK, "key-specific presets read");
    check(option_count == 2 && options[0] == (limited ? MDR_ASSIGNABLE_NOISE_CONTROL_QUICK_ACCESS_BT_CLASSIC_CAUTION :
                                            MDR_ASSIGNABLE_PLAYBACK_VOICE_ASSISTANT_LIMITATION) &&
          options[1] == MDR_ASSIGNABLE_NONE, "distinct V2 variants and NONE retain their identities");

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
    check_result(mdrHeadphonesSetAssignableControls(session.headphones, input, count),
                 MDR_RESULT_OK, "advertised actions stage in arbitrary caller order");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "assignment apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "assignment apply completes");
    while (next_tx_frame(&session.transport, &offset, &frame))
    {
        if (frame.type == MDR_DATA_TYPE_DATA_MDR && frame.payload[0] == 0xf8)
        {
            found = 1;
            check(frame.payload_size == (limited ? 4u : 6u) &&
                  frame.payload[1] == (limited ? 0x0e : 0x03) &&
                  frame.payload[2] == count && frame.payload[3] == (limited ? 0x43 : 0x22) &&
                  (limited || (frame.payload[4] == 0x30 && frame.payload[5] == 0x10)),
                  "assignment serialization preserves capability order and exact wire presets");
        }
    }
    check(found, "assignment commit transmits a settings update");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, notification, limited ? 4u : sizeof(notification));
    device_run(&session, &device, MDR_EVENT_ASSIGNABLE_CONTROLS_CHANGED, "assignment notification emits its own event");
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, controls, &count),
                 MDR_RESULT_OK, "notified assignments read");
    check(controls[0].action == options[0], "notification preserves distinct preset identity");
    session_close(&session);
}

static void test_v1_assignable_controls(void)
{
    /* Two buttons in CUSTOM, LEFT order, rather than implicit left/right indices. */
    static const unsigned char support[] = {0x07, 0, 1, 0xf6};
    static const unsigned char table2[] = {0x07, 0, 0};
    static const unsigned char capability[] = {
        0xf1, 6, 2,
        2, 1, 0, 2, 0, 1, 0, 2, 0x30, 1, 0, 0x30,
        0, 1, 0x20, 2, 0x20, 1, 0, 0x20, 0xff, 1, 0, 0
    };
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
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, controls, &count),
                 MDR_RESULT_OK, "V1 capability controls read");
    check(count == 2 && controls[0].location == MDR_ASSIGNABLE_ACTION_KEY_CUSTOM &&
          controls[0].type == MDR_ASSIGNABLE_ACTION_KEY_TYPE_BUTTON &&
          controls[0].action == MDR_ASSIGNABLE_NOISE_CONTROL &&
          controls[1].location == MDR_ASSIGNABLE_ACTION_KEY_LEFT &&
          controls[1].action == MDR_ASSIGNABLE_PLAYBACK, "V1 key order, type, and current preset survive");
    check_result(mdrHeadphonesGetAssignableControlActions(session.headphones, controls[0].location, options, &option_count),
                 MDR_RESULT_OK, "V1 per-key actions read");
    check(option_count == 2 && options[0] == MDR_ASSIGNABLE_NOISE_CONTROL &&
          options[1] == MDR_ASSIGNABLE_VOICE_RECOGNITION, "V1 actions follow each key capability");
    desired[0] = controls[1];
    desired[1] = controls[0];
    desired[0].action = MDR_ASSIGNABLE_NONE;
    desired[1].action = MDR_ASSIGNABLE_GOOGLE_ASSISTANT;
    check_result(mdrHeadphonesSetAssignableControls(session.headphones, desired, 2),
                 MDR_RESULT_ERROR_INVALID_ARGUMENT, "V1 rejects an unadvertised action after a valid entry");
    check(!mdrHeadphonesIsDirty(session.headphones), "V1 rejected batch leaves no staged assignments");
    desired[1].action = MDR_ASSIGNABLE_VOICE_RECOGNITION;
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesSetAssignableControls(session.headphones, desired, 2),
                 MDR_RESULT_OK, "V1 accepts reversed caller order");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "V1 assignment apply starts");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "V1 assignment apply completes");
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
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, notification, sizeof(notification));
    device_run(&session, &device, MDR_EVENT_ASSIGNABLE_CONTROLS_CHANGED, "V1 assignment notification polls");
    check_result(mdrHeadphonesGetAssignableControls(session.headphones, controls, &count),
                 MDR_RESULT_OK, "V1 notified assignments read");
    check(controls[0].action == MDR_ASSIGNABLE_VOICE_RECOGNITION &&
          controls[1].action == MDR_ASSIGNABLE_NONE, "V1 notification updates both assignments");
    session_close(&session);
}

static void test_v1_nc_optimizer(void)
{
    static const unsigned char support[] = {0x07, 0x00, 0x01, 0x81};
    /* WH-1000XM4 firmware 2.5.0 replies, including the actual 0.9 atm measurement. */
    static const unsigned char capability[] = {0x81, 0x01, 0x02, 0x01, 0x09, 0x01, 0x02};
    static const unsigned char idle[] = {0x83, 0x01, 0x00, 0x00};
    static const unsigned char parameters[] = {0x87, 0x01, 0x01, 0x01, 0x01, 0x09};
    static const unsigned char disabled[] = {0x85, 0x01, 0x01, 0x00};
    static const unsigned char unknown_status[] = {0x85, 0x01, 0xff, 0x00};
    static const unsigned char unmeasured[] = {0x89, 0x01, 0x01, 0x00, 0x01, 0x00};
    static const unsigned char measured[] = {0x89, 0x01, 0x01, 0x01, 0x01, 0x09};
    static const unsigned char unknown_results[] = {0x89, 0x01, 0x01, 0x7f, 0x01, 0x7f};
    static const unsigned char unsupported_types[] = {0x89, 0x01, 0xff, 0x01, 0xff, 0x09};
    static const unsigned char unknown_capability[] = {0x81, 0x01, 0x02, 0xff, 0x09, 0xff, 0x02};
    static const unsigned char other_optimizer[] = {0x89, 0x02, 0x01, 0x00, 0x01, 0x00};
    static const unsigned char no_support[] = {0x07, 0x00, 0x00};
    static const unsigned char phases[] = {0x01, 0x02, 0x10, 0x11};
    static const MDRNCOptimizerStatus phase_statuses[] = {
        MDR_NC_OPTIMIZER_STATUS_PERSONAL, MDR_NC_OPTIMIZER_STATUS_BAROMETRIC,
        MDR_NC_OPTIMIZER_STATUS_OPTIMIZING, MDR_NC_OPTIMIZER_STATUS_COMPLETE
    };
    Session session;
    Device device;
    MDRNCOptimizer optimizer;
    MDRFeatureAvailability availability;
    TxFrame frame;
    size_t offset;
    size_t requests_before;
    size_t index;
    int found = 0;

    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = 1;
    device.table1 = support;
    device.table1_size = sizeof(support);
    check_result(mdrHeadphonesGetNCOptimizer(NULL, &optimizer), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "optimizer getter rejects a null device");
    check_result(mdrHeadphonesGetNCOptimizer(session.headphones, NULL), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "optimizer getter rejects a null output");
    check_result(mdrHeadphonesRequestStartNCOptimizer(NULL), MDR_RESULT_ERROR_INVALID_ARGUMENT,
                 "optimizer start rejects a null device");
    check_result(mdrHeadphonesRequestInit(session.headphones), MDR_RESULT_OK, "optimizer initialization starts");
    check_result(mdrHeadphonesRequestStartNCOptimizer(session.headphones), MDR_RESULT_INPROGRESS,
                 "optimizer start cannot overlap initialization");
    device_run(&session, &device, MDR_EVENT_INITIALIZE_COMPLETE, "optimizer initialization completes");
    check_result(mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_NC_OPTIMIZER, &availability),
                 MDR_RESULT_OK, "advertised optimizer feature is queryable");
    check(availability == MDR_AVAILABILITY_AVAILABLE, "V1 advertised optimizer is supported");
    check(device_requested(&device, 1, 0x80, 1) && device_requested(&device, 1, 0x82, 1) &&
          device_requested(&device, 1, 0x86, 1), "initialization requests optimizer capability, status and results");
    check_result(mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer), MDR_RESULT_OK,
                 "optimizer is readable after response-gated initialization");
    check(optimizer.available == MDR_TRUE && optimizer.status == MDR_NC_OPTIMIZER_STATUS_IDLE &&
          optimizer.personal_supported == MDR_TRUE && optimizer.barometric_supported == MDR_TRUE &&
          optimizer.personal_result == MDR_NC_OPTIMIZER_RESULT_OPTIMIZED &&
          optimizer.barometric_result == MDR_NC_OPTIMIZER_RESULT_OPTIMIZED &&
          optimizer.pressure_tenths_atm == 9 && optimizer.optimization_seconds == 2,
          "initialization consumes real optimizer capability, status and measurements");

    offset = session.transport.tx_size;
    check_result(mdrHeadphonesRequestStartNCOptimizer(session.headphones), MDR_RESULT_OK, "optimizer start schedules");
    check_result(mdrHeadphonesRequestStartNCOptimizer(session.headphones), MDR_RESULT_INPROGRESS,
                 "a second optimizer start cannot overlap its acknowledgment");
    check_result(mdrHeadphonesRequestSync(session.headphones), MDR_RESULT_INPROGRESS,
                 "optimizer start occupies the existing operation slot");
    check(mdrHeadphonesIsDirty(session.headphones) == MDR_FALSE, "optimizer start does not stage settings");
    mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer);
    check(optimizer.status == MDR_NC_OPTIMIZER_STATUS_IDLE, "start does not manufacture a running status");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "optimizer start acknowledgment completes apply");
    while (next_tx_frame(&session.transport, &offset, &frame))
    {
        if (frame.type == MDR_DATA_TYPE_DATA_MDR && frame.payload_size > 0 && frame.payload[0] == 0x84)
        {
            ++found;
            check(frame.payload_size == 4 && frame.payload[1] == 1 && frame.payload[2] == 0 &&
                  frame.payload[3] == 1, "optimizer start sends OPT_SET_STATUS NC_OPTIMIZER ENABLE START");
        }
    }
    check(found == 1, "optimizer start reaches the transport exactly once");
    mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer);
    check(optimizer.status == MDR_NC_OPTIMIZER_STATUS_IDLE,
          "command acknowledgment does not imply progress or physical completion");
    for (index = 0; index < sizeof(phases); ++index)
    {
        const unsigned char notification[] = {0x85, 0x01, 0x00, phases[index]};
        requests_before = device.log_size;
        device_send(&device, MDR_DATA_TYPE_DATA_MDR, notification, sizeof(notification));
        device_run(&session, &device, MDR_EVENT_NC_OPTIMIZER_CHANGED, "optimizer received phase polls");
        mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer);
        check(optimizer.status == phase_statuses[index], "optimizer phases and END map from actual notifications");
        check(optimizer.personal_result == MDR_NC_OPTIMIZER_RESULT_OPTIMIZED && optimizer.pressure_tenths_atm == 9,
              "phase notifications do not overwrite independent measurements");
        if (index + 1 < sizeof(phases))
        {
            check_result(mdrHeadphonesRequestStartNCOptimizer(session.headphones), MDR_RESULT_INPROGRESS,
                         "received running phase prevents a duplicate start after ACK");
            check(device.log_size == requests_before, "a busy optimizer start sends no command");
        }
    }
    check_result(mdrHeadphonesRequestStartNCOptimizer(session.headphones), MDR_RESULT_OK,
                 "received END permits another optimization");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "second optimizer command is acknowledged");
    mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer);
    check(optimizer.status == MDR_NC_OPTIMIZER_STATUS_COMPLETE, "a new start does not overwrite received END");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, unmeasured, sizeof(unmeasured));
    device_run(&session, &device, MDR_EVENT_NC_OPTIMIZER_CHANGED, "unmeasured notification polls");
    mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer);
    check(optimizer.personal_result == MDR_NC_OPTIMIZER_RESULT_UNMEASURED &&
          optimizer.barometric_result == MDR_NC_OPTIMIZER_RESULT_UNMEASURED && optimizer.pressure_tenths_atm == 0 &&
          optimizer.status == MDR_NC_OPTIMIZER_STATUS_COMPLETE, "END alone does not manufacture successful results");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, measured, sizeof(measured));
    device_run(&session, &device, MDR_EVENT_NC_OPTIMIZER_CHANGED, "optimized result notification polls");
    mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer);
    check(optimizer.personal_result == MDR_NC_OPTIMIZER_RESULT_OPTIMIZED &&
          optimizer.barometric_result == MDR_NC_OPTIMIZER_RESULT_OPTIMIZED &&
          optimizer.pressure_tenths_atm == 9 && optimizer.status == MDR_NC_OPTIMIZER_STATUS_COMPLETE,
          "received optimized results distinguish a successful completed measurement");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, unknown_results, sizeof(unknown_results));
    device_run(&session, &device, MDR_EVENT_NC_OPTIMIZER_CHANGED, "unknown measurement values poll");
    mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer);
    check(optimizer.personal_result == MDR_NC_OPTIMIZER_RESULT_UNKNOWN &&
          optimizer.barometric_result == MDR_NC_OPTIMIZER_RESULT_UNKNOWN && optimizer.pressure_tenths_atm == 0,
          "unknown measurement values are not success or unmeasured");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, unsupported_types, sizeof(unsupported_types));
    device_run(&session, &device, MDR_EVENT_NC_OPTIMIZER_CHANGED, "unknown measurement types poll");
    mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer);
    check(optimizer.personal_result == MDR_NC_OPTIMIZER_RESULT_UNKNOWN &&
          optimizer.barometric_result == MDR_NC_OPTIMIZER_RESULT_UNKNOWN,
          "unknown measurement types do not claim known optimized results");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, other_optimizer, sizeof(other_optimizer));
    device_run(&session, &device, MDR_EVENT_UNHANDLED, "different optimizer inquiry is not misinterpreted");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, disabled, sizeof(disabled));
    device_run(&session, &device, MDR_EVENT_NC_OPTIMIZER_CHANGED, "disabled optimizer notification polls");
    mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer);
    check(optimizer.available == MDR_FALSE, "disabled optimizer becomes unavailable");
    check_result(mdrHeadphonesRequestStartNCOptimizer(session.headphones), MDR_RESULT_ERROR_NOT_SUPPORTED,
                 "disabled optimizer refuses start");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, unknown_status, sizeof(unknown_status));
    device_run(&session, &device, MDR_EVENT_NC_OPTIMIZER_CHANGED, "unknown availability status polls");
    mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer);
    check(optimizer.available == MDR_FALSE, "unknown enable status is not availability");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, idle, sizeof(idle));
    device_run(&session, &device, MDR_EVENT_NC_OPTIMIZER_CHANGED, "optimizer is enabled again");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, unknown_capability, sizeof(unknown_capability));
    device_run(&session, &device, MDR_EVENT_NC_OPTIMIZER_CHANGED, "unknown capability types poll");
    mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer);
    check(optimizer.available == MDR_FALSE && optimizer.personal_supported == MDR_FALSE &&
          optimizer.barometric_supported == MDR_FALSE, "unknown capability does not claim support");
    check_result(mdrHeadphonesRequestStartNCOptimizer(session.headphones), MDR_RESULT_ERROR_NOT_SUPPORTED,
                 "optimizer with no supported measurement capability refuses start");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR, no_support, sizeof(no_support));
    device_run(&session, &device, MDR_EVENT_IDENTITY_CHANGED, "optimizer advertisement is removed");
    check_result(mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer), MDR_RESULT_ERROR_NOT_SUPPORTED,
                 "optimizer getter follows the current advertised functions");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, support, sizeof(support));
    device_run(&session, &device, MDR_EVENT_IDENTITY_CHANGED, "optimizer advertisement returns");
    mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer);
    check(optimizer.available == MDR_FALSE && optimizer.status == MDR_NC_OPTIMIZER_STATUS_UNKNOWN &&
          optimizer.personal_result == MDR_NC_OPTIMIZER_RESULT_UNKNOWN,
          "removed capability cannot leave stale optimizer state when re-advertised");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, capability, sizeof(capability));
    device_run(&session, &device, MDR_EVENT_NC_OPTIMIZER_CHANGED, "capability arrives before status");
    mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer);
    check(optimizer.available == MDR_FALSE && optimizer.status == MDR_NC_OPTIMIZER_STATUS_UNKNOWN &&
          optimizer.personal_supported == MDR_TRUE && optimizer.barometric_supported == MDR_TRUE,
          "capability alone cannot manufacture enabled availability or idle status");
    check_result(mdrHeadphonesRequestStartNCOptimizer(session.headphones), MDR_RESULT_ERROR_NOT_SUPPORTED,
                 "known capability without enabled status cannot start");
    session_close(&session);
}

static void test_nc_optimizer_rejects_unsupported_families(void)
{
    static const unsigned char no_support[] = {0x07, 0x00, 0x00};
    static const unsigned char capability[] = {0x81, 0x01, 0x02, 0x01, 0x09, 0x01, 0x02};
    MDRProtocolVersion family;
    for (family = MDR_PROTOCOL_V1; family <= MDR_PROTOCOL_V2; ++family)
    {
        Session session;
        Device device;
        MDRNCOptimizer optimizer;
        MDRFeatureAvailability availability;
        size_t requests_before;
        if (!session_open_family(&session, family))
            continue;
        memset(&device, 0, sizeof(device));
        device.transport = &session.transport;
        device.protocol_v1 = family == MDR_PROTOCOL_V1;
        device.table1 = no_support;
        device.table1_size = sizeof(no_support);
        device.table2 = no_support;
        device.table2_size = sizeof(no_support);
        device_run_init(&session, &device);
        check_result(mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_NC_OPTIMIZER, &availability),
                     MDR_RESULT_OK, "unsupported optimizer feature remains a valid feature id");
        check(availability == MDR_AVAILABILITY_UNAVAILABLE, "unadvertised V1 and all V2 optimizers are unsupported");
        check(!device_requested(&device, 1, 0x80, 1) && !device_requested(&device, 1, 0x82, 1) &&
              !device_requested(&device, 1, 0x86, 1), "unsupported initialization sends no optimizer commands");
        check_result(mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer), MDR_RESULT_ERROR_NOT_SUPPORTED,
                     "unsupported optimizer getter refuses");
        requests_before = device.log_size;
        check_result(mdrHeadphonesRequestStartNCOptimizer(session.headphones), MDR_RESULT_ERROR_NOT_SUPPORTED,
                     "unsupported optimizer start refuses");
        check(!device_requested_after(&device, 1, 0x84, requests_before),
              "unsupported start sends no optimizer command");
        if (family == MDR_PROTOCOL_V1)
        {
            device_send(&device, MDR_DATA_TYPE_DATA_MDR, capability, sizeof(capability));
            device_run(&session, &device, MDR_EVENT_UNHANDLED, "unsolicited unadvertised capability is ignored");
            check_result(mdrHeadphonesGetNCOptimizer(session.headphones, &optimizer), MDR_RESULT_ERROR_NOT_SUPPORTED,
                         "unsolicited capability cannot bypass advertised V1 support");
        }
        session_close(&session);
    }
}

/* Fixed polling windows exercise latches without sleeping or depending on a wall-clock timeout. */
static void device_poll_window(Session* session, Device* device)
{
    int iteration;
    for (iteration = 0; iteration < 128; ++iteration)
    {
        MDREvent event = MDR_EVENT_NONE;
        check_result(mdrHeadphonesPoll(session->headphones, &event), MDR_RESULT_OK,
                     "isolated response barrier polls");
        device_pump(device);
        check(event != MDR_EVENT_INITIALIZE_COMPLETE, "a missing required RET cannot complete initialization");
    }
}

/* The public ABI uses steady-clock deadlines; exercise its real two-second live-holder
 * deadline rather than introducing a private clock or source-wiring test. */
static void wait_for_live_response_timeout(void)
{
#ifdef _WIN32
    Sleep(2100);
#else
    struct timespec remaining = {2, 100000000};
    while (nanosleep(&remaining, &remaining) != 0)
    {
        if (errno != EINTR)
        {
            check(0, "live-response timeout sleep succeeds");
            break;
        }
    }
#endif
}

static void test_completed_response_generation(MDRProtocolVersion family)
{
    static const unsigned char v1_support[] = {0x07, 0, 1, 0x51};
    static const unsigned char v2_support[] = {0x07, 0, 1, 0x50, 0xff};
    static const unsigned char empty[] = {0x07, 0, 0};
    Session session;
    Device device;
    unsigned char capability[FRAME_BUFFER_CAPACITY];
    size_t capability_size;
    size_t stopped;
    size_t next_init;

    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.duplicate_reply = 1;
    device.transport = &session.transport;
    device.protocol_v1 = family == MDR_PROTOCOL_V1;
    device.reply_before_ack = 1;
    device.table1 = device.protocol_v1 ? v1_support : v2_support;
    device.table1_size = device.protocol_v1 ? sizeof(v1_support) : sizeof(v2_support);
    device.table2 = empty;
    device.table2_size = sizeof(empty);
    device.hold_active = 1;
    device.hold_table = 1;
    device.hold_command = 0x50;
    device.hold_inquired = device.protocol_v1 ? 1 : 0;
    check_result(mdrHeadphonesRequestInit(session.headphones), MDR_RESULT_OK,
                 "response-generation initialization starts");
    device_poll_window(&session, &device);
    check(device.held_size > 0, "capability response is withheld for generation test");
    capability_size = device.held_size;
    memcpy(capability, device.held_payload, capability_size);

    /* Complete discovery, but leave the next response category unfulfilled. */
    device.hold_command = 0x52;
    device.held_size = 0;
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, capability, capability_size);
    device_poll_window(&session, &device);
    check(device.held_size == 3, "the next generation waits for equalizer status");
    stopped = device.log_size;
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, capability, capability_size);
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, capability, capability_size);
    device_poll_window(&session, &device);
    check(device.log_size == stopped,
          "duplicate completed capability RETs cannot complete the next status request");
    check(!device_requested(&device, 1, 0x56, device.hold_inquired),
          "a completed response category cannot release another category");

    device.hold_active = 0;
    device_send(&device, device.held_type, device.held_payload, device.held_size);
    device_run(&session, &device, MDR_EVENT_INITIALIZE_COMPLETE,
               "the next generation completes only after its matching RET");

    /* A duplicate received with no pending operation must not be remembered as success
     * when the same status category is requested in a subsequent fresh initialization. */
    stopped = device.log_size;
    device_send(&device, device.held_type, device.held_payload, device.held_size);
    device_poll_window(&session, &device);
    check(device.log_size == stopped, "an idle duplicate RET schedules no request");
    check(mdrHeadphonesIsReady(session.headphones) == MDR_TRUE,
          "an idle duplicate RET leaves the completed session ready");
    next_init = device.log_size;
    device.hold_active = 1;
    device.held_size = 0;
    check_result(mdrHeadphonesRequestInit(session.headphones), MDR_RESULT_OK,
                 "a new initialization requests fresh response generations");
    device_poll_window(&session, &device);
    check(device.held_size == 3 && device_requested_after(&device, 1, 0x52, next_init),
          "the same status category is actually requested again");
    check(!device_requested_after(&device, 1, 0x56, next_init),
          "the next generation cannot inherit old completed status success");
    check(mdrHeadphonesIsInitialized(session.headphones) == MDR_FALSE &&
          mdrHeadphonesIsReady(session.headphones) == MDR_FALSE,
          "a new generation remains uninitialized and busy until its own RET");
    device.hold_active = 0;
    device_send(&device, device.held_type, device.held_payload, device.held_size);
    device_run(&session, &device, MDR_EVENT_INITIALIZE_COMPLETE,
               "fresh same-category RET completes the new initialization");
    session_close(&session);
}

static void test_live_response_timeout_generation(MDRProtocolVersion family)
{
    static const unsigned char v1_support[] = {0x07, 0, 2, 0x51, 0xa1};
    static const unsigned char v2_support[] = {0x07, 0, 2, 0x50,0xff, 0xa1,0xff};
    static const unsigned char empty[] = {0x07, 0, 0};
    Session session;
    Device device;
    MDREqualizer equalizer;
    MDRPlayback playback;
    unsigned char notification[3] = {0x55, 0, 1};
    size_t stopped;
    size_t next_init;

    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = family == MDR_PROTOCOL_V1;
    device.reply_before_ack = 1;
    device.table1 = device.protocol_v1 ? v1_support : v2_support;
    device.table1_size = device.protocol_v1 ? sizeof(v1_support) : sizeof(v2_support);
    device.table2 = empty;
    device.table2_size = sizeof(empty);
    device.hold_active = 1;
    device.hold_table = 1;
    device.hold_command = 0x52;
    device.hold_inquired = device.protocol_v1 ? 1 : 0;
    notification[1] = device.hold_inquired;
    check_result(mdrHeadphonesRequestInit(session.headphones), MDR_RESULT_OK,
                 "live-timeout initialization starts");
    device_poll_window(&session, &device);
    check(device.held_size == 3, "the live holder has an ACK but no status RET");
    stopped = device.log_size;
    device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, notification, sizeof(notification));
    device_poll_window(&session, &device);
    check(device.log_size == stopped, "status notification on the wrong table cannot complete the request");
    notification[1] = 0x7f;
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, notification, sizeof(notification));
    device_poll_window(&session, &device);
    check(device.log_size == stopped, "status notification with the wrong inquiry cannot complete the request");
    notification[1] = device.hold_inquired;
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, notification, sizeof(notification));
    device_poll_window(&session, &device);
    check(device.log_size == stopped, "matching status notification cannot stand in for status RET");
    memset(&equalizer, 0, sizeof(equalizer));
    check_result(mdrHeadphonesGetEqualizer(session.headphones, &equalizer), MDR_RESULT_OK,
                 "notifications still update the consumer-visible feature state");
    check(equalizer.available == MDR_FALSE, "disabled status notification is consumed without completing a request");

    wait_for_live_response_timeout();
    device_run(&session, &device, MDR_EVENT_INITIALIZE_COMPLETE,
               "live timeout stops one holder and permits later holders to complete");
    check(!device_requested(&device, 1, 0x56, device.hold_inquired) &&
          !device_requested(&device, 1, 0x5a, device.hold_inquired),
          "status timeout skips the remaining equalizer holder requests");
    check(device_requested(&device, 1, 0xa2, 1) && device_requested(&device, 1, 0xa6, 1),
          "status timeout does not skip the later playback holder");
    memset(&playback, 0, sizeof(playback));
    check_result(mdrHeadphonesGetPlayback(session.headphones, &playback), MDR_RESULT_OK,
                 "later holder state is readable after another holder times out");
    check(playback.volume == 37, "later playback RET reaches the public getter after equalizer timeout");
    check(mdrHeadphonesIsInitialized(session.headphones) == MDR_TRUE &&
          mdrHeadphonesIsReady(session.headphones) == MDR_TRUE,
          "a tolerated live-holder timeout finishes the public initialization operation");

    /* Close the timed-out generation before receiving a late RET. It can refresh state,
     * but it cannot resurrect the old holder or supply a future request's completion. */
    stopped = device.log_size;
    device_send(&device, device.held_type, device.held_payload, device.held_size);
    device_poll_window(&session, &device);
    check(device.log_size == stopped, "a late timed-out RET cannot restart the abandoned holder");
    next_init = device.log_size;
    device.held_size = 0;
    check_result(mdrHeadphonesRequestInit(session.headphones), MDR_RESULT_OK,
                 "initialization after timeout starts a new generation");
    device_poll_window(&session, &device);
    check(device.held_size == 3 && device_requested_after(&device, 1, 0x52, next_init),
          "the timed-out response category is requested again");
    check(!device_requested_after(&device, 1, 0x56, next_init) &&
          !device_requested_after(&device, 1, 0xa2, next_init),
          "late old RET success cannot release a new generation");
    device.hold_active = 0;
    device_send(&device, device.held_type, device.held_payload, device.held_size);
    device_run(&session, &device, MDR_EVENT_INITIALIZE_COMPLETE,
               "new RET releases the next generation after a prior timeout");
    session_close(&session);
}

static void test_discovery_response_barrier(MDRProtocolVersion family, int before_ack)
{
    static const unsigned char v1_support[] = {0x07, 0, 1, 0x51};
    static const unsigned char v2_support[] = {0x07, 0, 1, 0x50, 0xff};
    static const unsigned char empty[] = {0x07, 0, 0};
    Session session;
    Device device;
    unsigned char wrong[FRAME_BUFFER_CAPACITY];
    size_t stopped;
    uint32_t presets = 0;

    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = family == MDR_PROTOCOL_V1;
    device.reply_before_ack = before_ack;
    device.table1 = device.protocol_v1 ? v1_support : v2_support;
    device.table1_size = device.protocol_v1 ? sizeof(v1_support) : sizeof(v2_support);
    device.table2 = empty;
    device.table2_size = sizeof(empty);
    device.hold_active = 1;
    device.hold_table = 1;
    device.hold_command = 0x50;
    device.hold_inquired = device.protocol_v1 ? 1 : 0;
    check_result(mdrHeadphonesRequestInit(session.headphones), MDR_RESULT_OK,
                 "response-barrier initialization starts");
    device_poll_window(&session, &device);
    check(device.held_size > 0, "the advertised capability request reaches its RET barrier");
    stopped = device.log_size;
    check(!device_requested(&device, 1, 0x52, -1),
          "current-state phase cannot begin before capability data arrives");
    check_result(mdrHeadphonesGetEqualizerPresets(session.headphones, NULL, &presets), MDR_RESULT_OK,
                 "unknown presets remain readable while capability RET is absent");
    check(presets == 0, "an ACK cannot manufacture an equalizer capability");

    device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, device.held_payload, device.held_size);
    device_poll_window(&session, &device);
    check(device.log_size == stopped, "the right RET on the wrong table does not release the barrier");
    memcpy(wrong, device.held_payload, device.held_size);
    wrong[1] = 0x7f;
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, wrong, device.held_size);
    device_poll_window(&session, &device);
    check(device.log_size == stopped, "the right RET with the wrong inquiry does not release the barrier");
    {
        const unsigned char notification[] = {0x55, device.hold_inquired, 0};
        device_send(&device, MDR_DATA_TYPE_DATA_MDR, notification, sizeof(notification));
    }
    device_poll_window(&session, &device);
    check(device.log_size == stopped, "a feature notification does not replace its required capability RET");
    device.hold_active = 0;
    device_send(&device, device.held_type, device.held_payload, device.held_size);
    device_run(&session, &device, MDR_EVENT_INITIALIZE_COMPLETE,
               "the matching RET releases discovery and initialization completes");
    check(device_requested(&device, 1, 0x52, device.hold_inquired),
          "the live-state holder uses the discovered inquiry");
    session_close(&session);
}

static void test_cold_eq_variants(void)
{
    static const unsigned char functions[] = {0x50, 0x52, 0x53, 0x57};
    static const unsigned char inquiries[] = {0x00, 0x02, 0x03, 0x04};
    static const unsigned char empty[] = {0x07, 0, 0};
    size_t index;
    for (index = 0; index < sizeof(functions); ++index)
    {
        const unsigned char support[] = {0x07, 0, 1, functions[index], 0xff};
        Session session;
        Device device;
        MDREqualizer equalizer;
        int capability, status, parameter, extended;
        if (!session_open(&session))
            return;
        memset(&device, 0, sizeof(device));
        device.transport = &session.transport;
        device.table1 = support;
        device.table1_size = sizeof(support);
        device.table2 = empty;
        device.table2_size = sizeof(empty);
        device.reply_before_ack = 1;
        device_run_init(&session, &device);
        capability = device_request_order(&device, 1, 0x50, inquiries[index]);
        status = device_request_order(&device, 1, 0x52, inquiries[index]);
        parameter = device_request_order(&device, 1, 0x56, inquiries[index]);
        extended = device_request_order(&device, 1, 0x5a, inquiries[index]);
        check(capability >= 0 && status > capability && parameter > status && extended > parameter,
              "EQ variant completes capability before status, curve and extended bands in order");
        if (inquiries[index] != 0)
            check(!device_requested(&device, 1, 0x56, 0), "nonordinary EQ never falls back to ordinary inquiry");
        memset(&equalizer, 0, sizeof(equalizer));
        check_result(mdrHeadphonesGetEqualizer(session.headphones, &equalizer), MDR_RESULT_OK,
                     "advertised EQ variant state is readable");
        check(equalizer.available == MDR_TRUE && equalizer.preset == MDR_EQ_BRIGHT,
              "variant-specific initial RET updates consumer-visible equalizer state");
        session_close(&session);
    }
}

static void test_cold_playback_variants(void)
{
    static const unsigned char functions[] = {0xa1, 0xa3, 0xa4, 0xa2};
    static const unsigned char inquiries[] = {1, 2, 3, 1};
    static const unsigned char empty[] = {0x07, 0, 0};
    size_t index;
    for (index = 0; index < sizeof(functions); ++index)
    {
        const unsigned char support[] = {0x07, 0, 1, functions[index], 0xff};
        const int music = index == 3 ? 0x30 : 0x20;
        const int call = index == 3 ? 0x31 : 0x21;
        Session session;
        Device device;
        MDRPlayback playback;
        int status, names, volume;
        if (!session_open(&session))
            return;
        memset(&device, 0, sizeof(device));
        device.transport = &session.transport;
        device.table1 = support;
        device.table1_size = sizeof(support);
        device.table2 = empty;
        device.table2_size = sizeof(empty);
        device.reply_before_ack = 1;
        device_run_init(&session, &device);
        status = device_request_order(&device, 1, 0xa2, inquiries[index]);
        names = device_request_order(&device, 1, 0xa6, inquiries[index]);
        volume = device_request_order(&device, 1, 0xa6, music);
        check(status >= 0 && names > status && volume > names,
              "playback variant reload orders status, metadata and matching music volume");
        check(device_requested(&device, 1, 0xa6, call) == (index != 2),
              "call-volume queries follow the advertised playback variant");
        if (index == 3)
            check(!device_requested(&device, 1, 0xa6, 0x20),
                  "mute variant does not request nonmute music volume");
        memset(&playback, 0, sizeof(playback));
        check_result(mdrHeadphonesGetPlayback(session.headphones, &playback), MDR_RESULT_OK,
                     "initial playback RET is exposed through the C ABI");
        check(playback.volume == 37, "initial playback music volume comes from the matching variant RET");
        session_close(&session);
    }
}

static void test_cold_bgm_independent_gate(void)
{
    static const unsigned char functions[] = {0xe4, 0xeb, 0x45};
    static const unsigned char inquiries[] = {0x03, 0x09};
    static const unsigned char empty[] = {0x07, 0, 0};
    size_t index;
    for (index = 0; index < sizeof(functions); ++index)
    {
        const unsigned char support[] = {0x07, 0, 1, functions[index], 0xff};
        Session session;
        Device device;
        MDRFeatureAvailability availability;
        if (!session_open(&session))
            return;
        memset(&device, 0, sizeof(device));
        device.transport = &session.transport;
        device.table1 = support;
        device.table1_size = sizeof(support);
        device.table2 = empty;
        device.table2_size = sizeof(empty);
        device.bgm_on = 1;
        device_run_init(&session, &device);
        check_result(mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_LISTENING_BACKGROUND_MUSIC, &availability),
                     MDR_RESULT_OK, "BGM feature support can be queried");
        if (index < 2)
        {
            const int capability = device_request_order(&device, 1, 0xe0, inquiries[index]);
            const int parameter = device_request_order(&device, 1, 0xe6, inquiries[index]);
            const int status = device_request_order(&device, 1, 0xe2, inquiries[index]);
            check(capability >= 0 && parameter > capability && status > parameter,
                  "BGM works without LISTENING_OPTION and uses its parameter-before-status holder order");
            check(availability == MDR_AVAILABILITY_AVAILABLE, "positive BGM advertisement exposes the feature");
            {
                MDRListening listening;
                memset(&listening, 0, sizeof(listening));
                check_result(mdrHeadphonesGetListening(session.headphones, &listening), MDR_RESULT_OK,
                             "independent BGM current state is readable");
                check(listening.mode == MDR_LISTENING_BACKGROUND_MUSIC,
                      "ordinary and error-code BGM initial RETs update consumer-visible listening mode");
            }
        }
        else
        {
            check(!device_requested(&device, 1, 0xe0, 0x03) &&
                  !device_requested(&device, 1, 0xe0, 0x09) &&
                  !device_requested(&device, 1, 0xe6, 0x03) &&
                  !device_requested(&device, 1, 0xe6, 0x09),
                  "a LE-audio restriction flag is not a BGM capability");
            check(availability == MDR_AVAILABILITY_UNAVAILABLE,
                  "a limitation-only advertisement does not manufacture BGM support");
        }
        session_close(&session);
    }
}

static void test_cold_voice_nested_gate(void)
{
    static const unsigned char functions[] = {0x40, 0x41, 0x42, 0x44, 0x45, 0x43};
    static const unsigned char inquiries[] = {0, 1, 1, 2, 3, 0x21};
    static const unsigned char empty[] = {0x07, 0, 0};
    size_t index;
    int onoff;
    for (index = 0; index < sizeof(functions); ++index)
        for (onoff = 0; onoff < 2; ++onoff)
        {
            const unsigned char support[] = {0x07, 0, 1, functions[index], 0xff};
            Session session;
            Device device;
            size_t query;
            int switch_status = 0;
            if (!session_open(&session))
                return;
            memset(&device, 0, sizeof(device));
            device.transport = &session.transport;
            device.table1 = empty;
            device.table1_size = sizeof(empty);
            device.table2 = support;
            device.table2_size = sizeof(support);
            device.voice_support_onoff = onoff;
            device_run_init(&session, &device);
            check((index >= 4 || device_requested(&device, 2, 0x40, inquiries[index])) &&
                  device_requested(&device, 2, 0x46, inquiries[index]),
                  "voice guidance reloads its variant after any required capability discovery");
            for (query = 0; query < device.log_size; ++query)
            {
                const RequestLog* request = &device.log[query];
                if (request->table == 2 && request->command == 0x42 &&
                    request->inquired == inquiries[index] &&
                    request->has_detail && request->detail == 0)
                    switch_status = 1;
            }
            if (index < 4)
                check(switch_status == onoff, "nested voice on/off capability gates only the switch status query");
            check(device_requested(&device, 2, 0x46, 0x20) == (index == 2),
                  "adjustable voice volume is requested only by the volume-adjustment holder");
            if (index == 5)
                check(!device_requested(&device, 2, 0x46, 1),
                      "fixed-five-step volume does not imply an unrelated MTK language setting");
            {
                MDRVoiceGuidance voice;
                memset(&voice, 0, sizeof(voice));
                check_result(mdrHeadphonesGetVoiceGuidance(session.headphones, &voice), MDR_RESULT_OK,
                             "advertised voice variant state is readable");
                if (index < 5)
                    check(voice.enabled == MDR_TRUE, "setting-variant RET updates the voice on/off state");
                if (index == 2 || index == 5)
                    check(voice.volume == 2, "adjustable and fixed-step volume RETs expose the initial volume");
            }
            session_close(&session);
        }
}

static void test_v1_independent_coadvertisement(void)
{
    static const unsigned char support[] = {0x07, 0, 5, 0x51,0x53,0x61,0x62,0x63};
    Session session;
    Device device;
    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = 1;
    device.table1 = support;
    device.table1_size = sizeof(support);
    device_run_init(&session, &device);
    check(device_request_order(&device, 1, 0x50, 1) >= 0 &&
          device_request_order(&device, 1, 0x50, 3) > device_request_order(&device, 1, 0x50, 1),
          "coadvertised EQ capabilities are independent ordered branches");
    check(device_request_order(&device, 1, 0x60, 1) >= 0 &&
          device_request_order(&device, 1, 0x60, 2) > device_request_order(&device, 1, 0x60, 1) &&
          device_request_order(&device, 1, 0x60, 3) > device_request_order(&device, 1, 0x60, 2),
          "NC-only, combined NC/ASM and ASM discovery are independent ordered branches");
    check(device_request_order(&device, 1, 0x66, 1) >= 0 &&
          device_request_order(&device, 1, 0x66, 2) > device_request_order(&device, 1, 0x66, 1) &&
          device_request_order(&device, 1, 0x66, 3) > device_request_order(&device, 1, 0x66, 2),
          "all coadvertised NC holders reload independently rather than falling through else-if");
    check(device_requested(&device, 1, 0x56, 3) && !device_requested(&device, 1, 0x56, 1),
          "the singular EQ holder uses the last discovered noncustomizable capability");
    {
        MDREqualizer equalizer;
        memset(&equalizer, 0, sizeof(equalizer));
        check_result(mdrHeadphonesGetEqualizer(session.headphones, &equalizer), MDR_RESULT_OK,
                     "V1 noncustomizable initial EQ state is readable");
        check(equalizer.preset == MDR_EQ_BRIGHT && equalizer.band_count == 5,
              "V1 noncustomizable RET and extended clear-bass metadata reach the C consumer");
    }
    session_close(&session);
}

static void test_v1_protocol_boundaries(void)
{
    static const unsigned int versions[] = {0x1000,0x2000,0x3000,0x4000,0x4010,0x5000};
    static const unsigned char support[] = {0x07, 0, 1, 0x30};
    size_t index;
    for (index = 0; index < sizeof(versions) / sizeof(versions[0]); ++index)
    {
        const unsigned char protocol[] = {0x01, 0, (unsigned char)(versions[index] >> 8),
                                           (unsigned char)versions[index]};
        Session session;
        Device device;
        if (!session_open_family(&session, MDR_PROTOCOL_V1))
            return;
        memset(&device, 0, sizeof(device));
        device.transport = &session.transport;
        device.protocol_v1 = 1;
        device.protocol_info = protocol;
        device.protocol_info_size = sizeof(protocol);
        device.table1 = support;
        device.table1_size = sizeof(support);
        device_run_init(&session, &device);
        check(device_requested(&device, 1, 0x04, 4) == (versions[index] >= 0x5000),
              "V1 instruction-guide discovery starts exactly at protocol 5000");
        check(device_requested(&device, 1, 0x36, 7) == (versions[index] >= 0x2000) &&
              device_requested(&device, 1, 0x36, 8) == (versions[index] >= 0x2000),
              "V1 firmware BLE power and battery threshold begin at protocol 2000");
        check(device_requested(&device, 1, 0x36, 9) == (versions[index] >= 0x4000),
              "V1 firmware update method begins at protocol 4000");
        check(device_requested(&device, 1, 0x36, 10) == (versions[index] >= 0x5000) &&
              device_requested(&device, 1, 0x36, 11) == (versions[index] >= 0x5000),
              "V1 interruption threshold and binding identity begin at protocol 5000");
        session_close(&session);
    }
}

static void test_voice_detail_response_barrier(void)
{
    static const unsigned char empty[] = {0x07, 0, 0};
    static const unsigned char support[] = {0x07, 0, 1, 0x41, 0xff};
    static const unsigned char language_status[] = {0x43, 1, 1, 0};
    Session session;
    Device device;
    size_t stopped;
    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = empty;
    device.table1_size = sizeof(empty);
    device.table2 = support;
    device.table2_size = sizeof(support);
    device.voice_support_onoff = 1;
    device.hold_active = 1;
    device.hold_table = 2;
    device.hold_command = 0x42;
    device.hold_inquired = 1;
    device.hold_has_detail = 1;
    device.hold_detail = 0;
    check_result(mdrHeadphonesRequestInit(session.headphones), MDR_RESULT_OK,
                 "voice live-state barrier starts");
    device_poll_window(&session, &device);
    check(device.held_size == 4, "the voice holder waits for its on/off status RET");
    stopped = device.log_size;
    device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, language_status, sizeof(language_status));
    device_poll_window(&session, &device);
    check(device.log_size == stopped, "language status cannot satisfy an on/off status RET barrier");
    {
        const unsigned char notification[] = {0x45, 1, 0, 0};
        device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, notification, sizeof(notification));
    }
    device_poll_window(&session, &device);
    check(device.log_size == stopped, "matching voice notification cannot complete its required status RET");
    device.hold_active = 0;
    device_send(&device, device.held_type, device.held_payload, device.held_size);
    device_run(&session, &device, MDR_EVENT_INITIALIZE_COMPLETE,
               "matching voice detail releases live-state reload");
    session_close(&session);
}

static void test_assignable_nested_capability_gate(void)
{
    static const unsigned char support[] = {0x07, 0, 1, 0xf3, 0xff};
    static const unsigned char empty[] = {0x07, 0, 0};
    static const unsigned char ordinary[] = {
        0xf1, 3, 1, 0,0,0x22,1, 0x22,1,0,0,0
    };
    static const unsigned char customizable[] = {
        0xf1, 3, 1, 0,0,0x22,1, 0x22,1,1,0,0,0,0,1,0
    };
    static const unsigned char params[] = {0xf7, 3, 1, 0x22};
    int custom;
    for (custom = 0; custom < 2; ++custom)
    {
        Session session;
        Device device;
        if (!session_open(&session))
            return;
        memset(&device, 0, sizeof(device));
        device.transport = &session.transport;
        device.table1 = support;
        device.table1_size = sizeof(support);
        device.table2 = empty;
        device.table2_size = sizeof(empty);
        device.assignable_capability = custom ? customizable : ordinary;
        device.assignable_capability_size = custom ? sizeof(customizable) : sizeof(ordinary);
        device.assignable_params = params;
        device.assignable_params_size = sizeof(params);
        device_run_init(&session, &device);
        check(device_requested(&device, 1, 0xfa, 3) == custom,
              "customizable actions, not ordinary fixed actions, gate extended assignable state");
        check(device_request_order(&device, 1, 0xf0, 3) < device_request_order(&device, 1, 0xf2, 3) &&
              device_request_order(&device, 1, 0xf2, 3) < device_request_order(&device, 1, 0xf6, 3),
              "assignable capability completes before holder status and parameter");
        session_close(&session);
    }
}

static void test_v2_protocol_boundaries(void)
{
    static const uint32_t versions[] = {
        0x03001003,0x03001004,0x03002013,0x03002014,0x03002015
    };
    static const unsigned char empty[] = {0x07, 0, 0};
    size_t index;
    for (index = 0; index < sizeof(versions) / sizeof(versions[0]); ++index)
    {
        const uint32_t version = versions[index];
        const unsigned char protocol[] = {
            0x01,0,(unsigned char)(version >> 24),(unsigned char)(version >> 16),
            (unsigned char)(version >> 8),(unsigned char)version,0,0
        };
        const int early_guide = version < 50335748u || version == 50339859u || version == 50339860u;
        const int late_guide = version > 50339860u;
        const int app_type = version == 60397977u || (version & 0xff000000u) == 0x04000000u;
        Session session;
        Device device;
        int guide, support;
        if (!session_open(&session))
            return;
        memset(&device, 0, sizeof(device));
        device.transport = &session.transport;
        device.table1 = empty;
        device.table1_size = sizeof(empty);
        device.table2 = empty;
        device.table2_size = sizeof(empty);
        device.protocol_info = protocol;
        device.protocol_info_size = sizeof(protocol);
        device_run_init(&session, &device);
        guide = device_request_order(&device, 1, 0x04, 4);
        support = device_request_order(&device, 1, 0x06, 0);
        check((guide >= 0) == (early_guide || late_guide),
              "V2 instruction-guide boundaries preserve the source's skipped interval");
        if (early_guide)
            check(guide < support, "early V2 instruction guide precedes support discovery");
        else if (late_guide)
            check(guide > support, "late V2 instruction guide follows support discovery");
        check(device_requested(&device, 1, 0x02, 1) == app_type &&
              device_requested(&device, 1, 0x02, 0) != app_type,
              "V2 capability-info app-type inquiry obeys exact version gates");
        session_close(&session);
    }
}

static void test_v2_rejects_unaccepted_protocols(void)
{
    static const uint32_t versions[] = {2,0x03999999,0x04000000};
    size_t index;
    for (index = 0; index < sizeof(versions) / sizeof(versions[0]); ++index)
    {
        const uint32_t version = versions[index];
        const unsigned char protocol[] = {
            0x01,0,(unsigned char)(version >> 24),(unsigned char)(version >> 16),
            (unsigned char)(version >> 8),(unsigned char)version,0,0
        };
        Session session;
        Device device;
        MDRResult result = MDR_RESULT_OK;
        int iteration;
        if (!session_open(&session))
            return;
        memset(&device, 0, sizeof(device));
        device.transport = &session.transport;
        device.protocol_info = protocol;
        device.protocol_info_size = sizeof(protocol);
        check_result(mdrHeadphonesRequestInit(session.headphones), MDR_RESULT_OK,
                     "unsupported protocol initialization schedules");
        for (iteration = 0; iteration < 128 && result == MDR_RESULT_OK; ++iteration)
        {
            MDREvent event = MDR_EVENT_NONE;
            result = mdrHeadphonesPoll(session.headphones, &event);
            device_pump(&device);
            check(event != MDR_EVENT_INITIALIZE_COMPLETE,
                  "a protocol outside Sony's accepted list never reports initialization complete");
        }
        check_result(result, MDR_RESULT_ERROR_NOT_SUPPORTED, "unaccepted V2 protocol returns not-supported");
        check(!device_requested(&device, 1, 0x02, -1),
              "version acceptance precedes ordinary and app-type capability discovery");
        check(mdrHeadphonesIsInitialized(session.headphones) == MDR_FALSE,
              "unsupported protocol cannot leave a falsely initialized public session");
        session_close(&session);
    }
}

static int device_requested_detail(const Device* device, unsigned char table,
                                   unsigned char command, unsigned char inquiry,
                                   unsigned char detail)
{
    size_t index;
    for (index = 0; index < device->log_size; ++index)
    {
        const RequestLog* request = &device->log[index];
        if (request->table == table && request->command == command && request->has_inquired &&
            request->inquired == inquiry && request->has_detail && request->detail == detail)
            return 1;
    }
    return 0;
}

static void test_v1_voice_nested_switches(void)
{
    static const unsigned char support[] = {0x07, 0, 1, 0x39};
    int onoff, language;
    for (onoff = 0; onoff < 2; ++onoff)
        for (language = 0; language < 2; ++language)
        {
            Session session;
            Device device;
            if (!session_open_family(&session, MDR_PROTOCOL_V1))
                return;
            memset(&device, 0, sizeof(device));
            device.transport = &session.transport;
            device.protocol_v1 = 1;
            device.table1 = support;
            device.table1_size = sizeof(support);
            device.voice_support_onoff = onoff;
            device.voice_support_language = language;
            device_run_init(&session, &device);
            check(device_requested_detail(&device, 2, 0x46, 1, 5),
                  "V1 voice update-method discovery is independent of both nested switch flags");
            check(device_requested_detail(&device, 2, 0x42, 1, 1) == onoff &&
                  device_requested_detail(&device, 2, 0x46, 1, 1) == onoff,
                  "V1 nested on/off support gates switch status and parameter together");
            check(device_requested_detail(&device, 2, 0x42, 1, 2) == language &&
                  device_requested_detail(&device, 2, 0x46, 1, 2) == language &&
                  device_requested_detail(&device, 2, 0x46, 1, 3) == language &&
                  device_requested_detail(&device, 2, 0x46, 1, 4) == language,
                  "V1 nested language support gates language status, language, time and download server");
            session_close(&session);
        }
}

static void test_v2_independent_coadvertisement(void)
{
    static const unsigned char support[] = {0x07, 0, 4, 0xa1,0xff,0xa4,0xff,0xe4,0xff,0xeb,0xff};
    static const unsigned char empty[] = {0x07, 0, 0};
    Session session;
    Device device;
    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = support;
    device.table1_size = sizeof(support);
    device.table2 = empty;
    device.table2_size = sizeof(empty);
    device_run_init(&session, &device);
    check(device_request_order(&device, 1, 0xa0, 1) >= 0 &&
          device_request_order(&device, 1, 0xa0, 3) > device_request_order(&device, 1, 0xa0, 1),
          "coadvertised playback capability branches remain independent and ordered");
    check(device_request_order(&device, 1, 0xa2, 1) >= 0 &&
          device_request_order(&device, 1, 0xa2, 3) > device_request_order(&device, 1, 0xa2, 1),
          "coadvertised playback variants each reload their own status holder");
    check(device_request_order(&device, 1, 0xe0, 3) >= 0 &&
          device_request_order(&device, 1, 0xe0, 9) > device_request_order(&device, 1, 0xe0, 3),
          "ordinary and error-code BGM capabilities are independently discovered");
    check(!device_requested(&device, 1, 0xe6, 3) && device_requested(&device, 1, 0xe6, 9),
          "the single BGM holder selects the advertised error-code variant after discovery");
    check(device_request_order(&device, 1, 0xe0, 9) < device_request_order(&device, 1, 0xa2, 1),
          "the last capability completes before any playback current-state request");
    session_close(&session);
}

static void test_v1_ebb_current_state(void)
{
    static const unsigned char support[] = {0x07, 0, 1, 0x52};
    static const unsigned char negative_level[] = {0x57, 2, 0xfb};
    static const unsigned char positive_limit[] = {0x57, 2, 0x7f};
    static const unsigned char notify_signed_limit[] = {0x59, 2, 0x80};
    Session session;
    Device device;
    MDREqualizer equalizer;
    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = 1;
    device.table1 = support;
    device.table1_size = sizeof(support);
    device.hold_active = 1;
    device.hold_table = 1;
    device.hold_command = 0x56;
    device.hold_inquired = 2;
    check_result(mdrHeadphonesRequestInit(session.headphones), MDR_RESULT_OK,
                 "signed EBB initialization starts");
    device_poll_window(&session, &device);
    check(device.held_size == 3, "EBB live holder waits for its signed level RET");
    device.held_payload[2] = 0x80;
    device.hold_active = 0;
    device_send(&device, device.held_type, device.held_payload, device.held_size);
    device_run(&session, &device, MDR_EVENT_INITIALIZE_COMPLETE, "signed EBB initialization completes");
    memset(&equalizer, 0, sizeof(equalizer));
    check_result(mdrHeadphonesGetEqualizer(session.headphones, &equalizer), MDR_RESULT_OK,
                 "EBB-only equalizer state is readable");
    check(equalizer.available == MDR_TRUE && equalizer.clear_bass == -127 && equalizer.band_count == 0,
          "EBB RET roundLevel maps -128 to -127 without capability-range clamping or a preset EQ curve");
    check(device_requested(&device, 1, 0x50, 2) && device_requested(&device, 1, 0x52, 2) &&
          device_requested(&device, 1, 0x56, 2) && !device_requested(&device, 1, 0x56, 1),
          "EBB-only initialization never substitutes ordinary preset EQ");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, negative_level, sizeof(negative_level));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "negative EBB RET polls");
    check_result(mdrHeadphonesGetEqualizer(session.headphones, &equalizer), MDR_RESULT_OK,
                 "negative EBB level is readable");
    check(equalizer.clear_bass == -5, "EBB RET decodes the signed wire level");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, positive_limit, sizeof(positive_limit));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "positive EBB limit RET polls");
    check_result(mdrHeadphonesGetEqualizer(session.headphones, &equalizer), MDR_RESULT_OK,
                 "positive EBB limit is readable");
    check(equalizer.clear_bass == 127, "EBB RET keeps a signed positive limit outside capability range");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, notify_signed_limit, sizeof(notify_signed_limit));
    device_run(&session, &device, MDR_EVENT_EQUALIZER_CHANGED, "signed EBB notification polls");
    check_result(mdrHeadphonesGetEqualizer(session.headphones, &equalizer), MDR_RESULT_OK,
                 "signed EBB notification is readable");
    check(equalizer.clear_bass == -128, "EBB notification preserves -128 rather than applying RET rounding");
    session_close(&session);
}

static void test_wearing_control_is_not_checker_support(void)
{
    static const unsigned char support[] = {0x07, 0, 1, 0xf1, 0xff};
    static const unsigned char empty[] = {0x07, 0, 0};
    Session session;
    Device device;
    MDRFeatureAvailability availability;
    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = support;
    device.table1_size = sizeof(support);
    device.table2 = empty;
    device.table2_size = sizeof(empty);
    device_run_init(&session, &device);
    check(!device_requested(&device, 2, 0xf2, 0),
          "wearing playback control alone never causes an unadvertised checker query");
    check_result(mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_WEARING_STATUS, &availability),
                 MDR_RESULT_OK, "unadvertised wearing checker feature remains queryable");
    check(availability == MDR_AVAILABILITY_UNAVAILABLE,
          "wearing playback control cannot manufacture checker support");
    session_close(&session);
}

static void test_v1_fixed_timeout_and_byte_strings(void)
{
    static const unsigned char support[] = {0x07,0,2,0xf5,0x14};
    Session session;
    Device device;
    MDRSpeakToChat chat;
    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = 1;
    device.table1 = support;
    device.table1_size = sizeof(support);
    device_run_init(&session, &device);
    memset(&chat, 0, sizeof(chat));
    check_result(mdrHeadphonesGetSpeakToChat(session.headphones, &chat), MDR_RESULT_OK,
                 "current Speak-to-Chat state follows valid fixed-length capability discovery");
    check(chat.enabled == MDR_TRUE && mdrHeadphonesIsInitialized(session.headphones) == MDR_TRUE,
          "XM4 timeout bytes and byte-prefixed BLE strings permit later state acquisition");
    session_close(&session);
}

static void test_mtk_voice_toggle_notifications(void)
{
    static const unsigned char empty[] = {0x07,0,0};
    static const unsigned char support[] = {0x07,0,1,0x42,0xff};
    static const unsigned char off[] = {0x49,0x01,0x01};
    static const unsigned char on[] = {0x49,0x01,0x00};
    Session session;
    Device device;
    MDRVoiceGuidance original, actual;
    if (!session_open(&session))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.table1 = empty;
    device.table1_size = sizeof(empty);
    device.table2 = support;
    device.table2_size = sizeof(support);
    device.voice_support_onoff = 1;
    device_run_init(&session, &device);
    check_result(mdrHeadphonesGetVoiceGuidance(session.headphones, &original), MDR_RESULT_OK,
                 "MTK voice setting is readable after initialization");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, off, sizeof(off));
    device_run(&session, &device, MDR_EVENT_VOICE_GUIDANCE_CHANGED,
               "three-byte MTK OFF notification is accepted without disconnecting");
    check_result(mdrHeadphonesGetVoiceGuidance(session.headphones, &actual), MDR_RESULT_OK,
                 "voice setting remains readable after OFF notification");
    check(actual.enabled == MDR_FALSE && actual.volume == original.volume &&
          mdrHeadphonesIsReady(session.headphones) == MDR_TRUE,
          "MTK OFF updates only the enabled setting and leaves the session usable");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR_NO2, on, sizeof(on));
    device_run(&session, &device, MDR_EVENT_VOICE_GUIDANCE_CHANGED,
               "three-byte MTK ON notification is accepted without disconnecting");
    check_result(mdrHeadphonesGetVoiceGuidance(session.headphones, &actual), MDR_RESULT_OK,
                 "voice setting remains readable after ON notification");
    check(actual.enabled == MDR_TRUE && actual.volume == original.volume &&
          mdrHeadphonesIsInitialized(session.headphones) == MDR_TRUE,
          "MTK ON preserves volume and initialized state");
    session_close(&session);
}

static void test_advertised_auto_power_off_timers(MDRProtocolVersion family)
{
    static const unsigned char empty[] = {0x07,0,0};
    const unsigned char support1[] = {0x07,0,1,0xf4};
    const unsigned char support2[] = {0x07,0,1,0x24,0xff};
    const unsigned char wear_only[] = {0x07,0,1,0x25,0xff};
    const unsigned char timed[] = {family == MDR_PROTOCOL_V1 ? 0xf1 : 0x21,4,5,
                                  family == MDR_PROTOCOL_V1 ? 0 : 4,1,2,3,0x11};
    const unsigned char untimed1[] = {0xf1,4,2,0x10,0x11};
    const unsigned char untimed2[] = {0x21,4,1,0x11};
    static const unsigned char wearing[] = {0x21,5,2,0x10,0x11};
    const uint32_t expected[] = {family == MDR_PROTOCOL_V1 ? 5u : 15u,30,60,180,0};
    Session session;
    Device device;
    uint32_t values[5] = {777,777,777,777,777};
    uint32_t count = 0;
    if (!session_open_family(&session, family))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = family == MDR_PROTOCOL_V1;
    device.table1 = empty;
    device.table1_size = sizeof(empty);
    device.table2 = empty;
    device.table2_size = sizeof(empty);
    device_run_init(&session, &device);
    check_result(mdrHeadphonesGetAutoPowerOffTimers(session.headphones, NULL, &count), MDR_RESULT_OK,
                 "unsupported devices expose no invented timer choices");
    check(count == 0, "timer UI has no choices before advertised timer capability");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR,
                family == MDR_PROTOCOL_V1 ? support1 : support2,
                family == MDR_PROTOCOL_V1 ? sizeof(support1) : sizeof(support2));
    device_run(&session, &device, MDR_EVENT_IDENTITY_CHANGED, "auto-off support list is received");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR,
                family == MDR_PROTOCOL_V1 ? untimed1 : untimed2,
                family == MDR_PROTOCOL_V1 ? sizeof(untimed1) : sizeof(untimed2));
    device_run(&session, &device, MDR_EVENT_POWER_CHANGED, "non-timer capability is received");
    count = 0;
    mdrHeadphonesGetAutoPowerOffTimers(session.headphones, NULL, &count);
    check(count == 0, "removed/disabled-only options do not create an auto-off timer dropdown");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, timed, sizeof(timed));
    device_run(&session, &device, MDR_EVENT_POWER_CHANGED, "protocol-specific timed capability is received");
    count = 4;
    check_result(mdrHeadphonesGetAutoPowerOffTimers(session.headphones, values, &count),
                 MDR_RESULT_ERROR_BUFFER_TOO_SMALL, "timer options honor caller buffer capacity");
    check(count == 5 && values[0] == 777 && values[4] == 777, "short timer buffer is untouched");
    check_result(mdrHeadphonesGetAutoPowerOffTimers(session.headphones, values, &count), MDR_RESULT_OK,
                 "advertised timer options can be read");
    check(count == 5 && memcmp(values, expected, sizeof(expected)) == 0,
          "only advertised durations are exposed, in device order, with zero meaning disabled");
    device_run_init(&session, &device);
    count = 0;
    mdrHeadphonesGetAutoPowerOffTimers(session.headphones, NULL, &count);
    check(count == 0, "fresh initialization cannot retain a previous device's timer choices");
    if (family == MDR_PROTOCOL_V2)
    {
        device_send(&device, MDR_DATA_TYPE_DATA_MDR, wear_only, sizeof(wear_only));
        device_run(&session, &device, MDR_EVENT_IDENTITY_CHANGED, "wearing-only auto-off support is received");
        device_send(&device, MDR_DATA_TYPE_DATA_MDR, wearing, sizeof(wearing));
        device_run(&session, &device, MDR_EVENT_UNHANDLED, "wearing candidate capability remains separate");
        count = 0;
        mdrHeadphonesGetAutoPowerOffTimers(session.headphones, NULL, &count);
        check(count == 0, "wearing-based shutdown does not advertise a timed dropdown");
    }
    session_close(&session);
}

static void test_v1_wearing_shutdown_preserves_selected_timer(void)
{
    static const unsigned char empty[] = {0x07,0,0};
    static const unsigned char support[] = {0x07,0,1,0xf4};
    static const unsigned char capability[] = {0xf1,4,2,0x10,0x11};
    static const unsigned char original[] = {0xf7,4,1,0x10,1};
    Session session;
    Device device;
    MDRPower power;
    MDRFeatureAvailability availability;
    int iteration;
    uint32_t count = 0;
    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = 1;
    device.table1 = empty;
    device.table1_size = sizeof(empty);
    device_run_init(&session, &device);
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, support, sizeof(support));
    device_run(&session, &device, MDR_EVENT_IDENTITY_CHANGED, "V1 auto-off support arrives without pause support");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, capability, sizeof(capability));
    device_run(&session, &device, MDR_EVENT_POWER_CHANGED, "removed/disabled capability arrives");
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, original, sizeof(original));
    device_run(&session, &device, MDR_EVENT_POWER_CHANGED, "wearing shutdown state arrives");
    mdrHeadphonesGetFeature(session.headphones, MDR_FEATURE_WEARING_DETECTION, &availability);
    check(availability == MDR_AVAILABILITY_AVAILABLE, "removed candidate exposes wearing shutdown independently of auto-pause");
    mdrHeadphonesGetAutoPowerOffTimers(session.headphones, NULL, &count);
    check(count == 0, "wearing shutdown does not reintroduce unsupported timer choices");
    check_result(mdrHeadphonesGetPower(session.headphones, &power), MDR_RESULT_OK, "V1 wearing shutdown can be read");
    check(power.wearing_power == MDR_WEARING_POWER_WHEN_REMOVED && power.auto_power_off_minutes == 0,
          "active removed element maps to wearing shutdown, not a timed value");
    for (iteration = 0; iteration < 2; ++iteration)
    {
        const unsigned char active = iteration == 0 ? 0x11 : 0x10;
        const unsigned char response[] = {0xf9,4,1,active,1};
        TxFrame frame;
        size_t offset = session.transport.tx_size;
        int found = 0;
        power.wearing_power = iteration == 0 ? MDR_WEARING_POWER_DISABLED : MDR_WEARING_POWER_WHEN_REMOVED;
        check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_OK, "wearing shutdown toggle stages");
        check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "wearing shutdown toggle commits");
        device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "wearing shutdown command acknowledged");
        while (next_tx_frame(&session.transport, &offset, &frame))
            if (frame.type == MDR_DATA_TYPE_DATA_MDR && frame.payload_size >= 2 && frame.payload[1] == 4)
            {
                check(frame.payload_size == 5 && frame.payload[0] == 0xf8 && frame.payload[2] == 1 &&
                      frame.payload[3] == active && frame.payload[4] == 1,
                      "V1 toggle uses SYSTEM_SET_PARAM and preserves remembered thirty-minute selection");
                found = 1;
            }
        check(found, "wearing shutdown reaches the transport");
        device_send(&device, MDR_DATA_TYPE_DATA_MDR, response, sizeof(response));
        device_run(&session, &device, MDR_EVENT_POWER_CHANGED, "device reports toggled wearing mode");
        check_result(mdrHeadphonesGetPower(session.headphones, &power), MDR_RESULT_OK, "toggled wearing mode remains readable");
        check(power.wearing_power == (iteration == 0 ? MDR_WEARING_POWER_DISABLED : MDR_WEARING_POWER_WHEN_REMOVED),
              "received power state matches the selected wearing mode");
    }
    session_close(&session);
}

static void test_v1_user_shutdown_request(void)
{
    static const unsigned char empty[] = {0x07,0,0};
    static const unsigned char support[] = {0x07,0,1,0x21};
    Session session;
    Device device;
    MDRPower power;
    TxFrame frame;
    size_t offset;
    int found = 0;
    if (!session_open_family(&session, MDR_PROTOCOL_V1))
        return;
    memset(&device, 0, sizeof(device));
    device.transport = &session.transport;
    device.protocol_v1 = 1;
    device.table1 = empty;
    device.table1_size = sizeof(empty);
    device_run_init(&session, &device);
    device_send(&device, MDR_DATA_TYPE_DATA_MDR, support, sizeof(support));
    device_run(&session, &device, MDR_EVENT_IDENTITY_CHANGED, "V1 user-power-off support is received");
    check_result(mdrHeadphonesGetPower(session.headphones, &power), MDR_RESULT_OK, "power state is readable before shutdown");
    power.shutdown_requested = MDR_TRUE;
    offset = session.transport.tx_size;
    check_result(mdrHeadphonesSetPower(session.headphones, &power), MDR_RESULT_OK, "user shutdown stages");
    check_result(mdrHeadphonesRequestCommit(session.headphones), MDR_RESULT_OK, "user shutdown commits");
    device_run(&session, &device, MDR_EVENT_APPLY_COMPLETE, "user shutdown command is acknowledged");
    while (next_tx_frame(&session.transport, &offset, &frame))
        if (frame.type == MDR_DATA_TYPE_DATA_MDR && frame.payload_size > 0 && frame.payload[0] == 0x22)
        {
            check(frame.payload_size == 3 && frame.payload[1] == 0 && frame.payload[2] == 1,
                  "V1 shutdown requests USER_POWER_OFF rather than NO_USE");
            ++found;
        }
    check(found == 1, "a staged user shutdown sends one power-off request");
    check_result(mdrHeadphonesGetPower(session.headphones, &power), MDR_RESULT_OK, "shutdown request state is readable after ACK");
    check(power.shutdown_requested == MDR_FALSE && mdrHeadphonesIsDirty(session.headphones) == MDR_FALSE,
          "acknowledged shutdown is consumed rather than replayed by later commits");
    session_close(&session);
}

static void test_cold_initialization(void)
{
    test_init_skips_unadvertised_functions();
    test_init_requests_advertised_functions();
    test_discovery_response_barrier(MDR_PROTOCOL_V1, 0);
    test_discovery_response_barrier(MDR_PROTOCOL_V1, 1);
    test_discovery_response_barrier(MDR_PROTOCOL_V2, 0);
    test_discovery_response_barrier(MDR_PROTOCOL_V2, 1);
    test_completed_response_generation(MDR_PROTOCOL_V1);
    test_completed_response_generation(MDR_PROTOCOL_V2);
    test_live_response_timeout_generation(MDR_PROTOCOL_V1);
    test_live_response_timeout_generation(MDR_PROTOCOL_V2);
    test_cold_eq_variants();
    test_cold_playback_variants();
    test_cold_bgm_independent_gate();
    test_cold_voice_nested_gate();
    test_mtk_voice_toggle_notifications();
    test_advertised_auto_power_off_timers(MDR_PROTOCOL_V1);
    test_advertised_auto_power_off_timers(MDR_PROTOCOL_V2);
    test_v1_wearing_shutdown_preserves_selected_timer();
    test_v1_user_shutdown_request();
    test_v1_independent_coadvertisement();
    test_v2_independent_coadvertisement();
    test_v1_voice_nested_switches();
    test_v1_ebb_current_state();
    test_wearing_control_is_not_checker_support();
    test_v1_protocol_boundaries();
    test_v1_fixed_timeout_and_byte_strings();
    test_voice_detail_response_barrier();
    test_assignable_nested_capability_gate();
    test_v2_protocol_boundaries();
    test_v2_rejects_unaccepted_protocols();
}

int main(int argc, char** argv)
{
    if (argc == 2 && strcmp(argv[1], "--cold-init") == 0)
    {
        test_cold_initialization();
        if (g_failures != 0)
            fprintf(stderr, "%d cold-initialization assertion(s) failed\n", g_failures);
        return g_failures != 0 ? 1 : 0;
    }
    if (argc != 1)
    {
        fprintf(stderr, "Usage: %s [--cold-init]\n", argv[0]);
        return 2;
    }
    test_abi_version_handshake();
    test_struct_and_buffer_contracts();
    test_one_operation_at_a_time();
    test_committed_state_staging();
    test_playback_actions();
    test_poll_events();
    test_listening_modes();
    test_equalizer_availability_follows_the_device();
    test_equalizer_presets_follow_the_capability();
    test_preset_change_does_not_write_bands();
    test_listening_mode_rereads_what_it_takes_away();
    test_transmit_sequence_ignores_inbound_frames();
    test_v2_bootstrap();
    test_staging_is_rejected_during_apply();
    test_connection_mode_names_its_inquired_type();
    test_v1_voice_guidance_detail_is_not_the_switch();
    test_v1_reply_before_ack();
    test_v1_sync_asks_for_the_track_names();
    test_v2_sync_asks_for_the_track_names();
    test_v1_preset_and_curve_never_share_a_frame();
    test_wearing_status();
    test_v2_assignable_controls(0);
    test_v2_assignable_controls(1);
    test_v1_assignable_controls();
    test_v1_nc_optimizer();
    test_nc_optimizer_rejects_unsupported_families();
    test_cold_initialization();

    if (g_failures != 0)
        fprintf(stderr, "%d test assertion(s) failed\n", g_failures);
    return g_failures != 0 ? 1 : 0;
}
