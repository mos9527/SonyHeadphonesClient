#include <mdr-c/Headphones.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MDR_ASSERT_U32(type) \
    _Static_assert(sizeof(type) == sizeof(uint32_t), #type " must be uint32_t")
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
MDR_ASSERT_U32(MDRDseeType);
MDR_ASSERT_U32(MDRPairedDeviceCommand);
MDR_ASSERT_U32(MDRGeneralSettingType);
MDR_ASSERT_U32(MDRAssignableAction);
MDR_ASSERT_U32(MDRWearingPowerMode);
MDR_ASSERT_U32(MDRAudioPriority);
#undef MDR_ASSERT_U32

#define MDR_ASSERT_C_STRUCT(type) \
    _Static_assert(offsetof(type, struct_size) == 0, #type " struct_size must be first")
MDR_ASSERT_C_STRUCT(MDRIdentity);
MDR_ASSERT_C_STRUCT(MDRBattery);
MDR_ASSERT_C_STRUCT(MDRPlayback);
MDR_ASSERT_C_STRUCT(MDRPlaybackCommand);
MDR_ASSERT_C_STRUCT(MDRNoiseControl);
MDR_ASSERT_C_STRUCT(MDRSpeakToChat);
MDR_ASSERT_C_STRUCT(MDRListening);
MDR_ASSERT_C_STRUCT(MDREqualizer);
MDR_ASSERT_C_STRUCT(MDRPairedDevice);
MDR_ASSERT_C_STRUCT(MDRPairedDeviceAction);
MDR_ASSERT_C_STRUCT(MDRPairing);
MDR_ASSERT_C_STRUCT(MDRGeneralSettingInfo);
MDR_ASSERT_C_STRUCT(MDRGeneralSetting);
MDR_ASSERT_C_STRUCT(MDRAssignableControls);
MDR_ASSERT_C_STRUCT(MDRPower);
MDR_ASSERT_C_STRUCT(MDRVoiceGuidance);
MDR_ASSERT_C_STRUCT(MDRConnectionMode);
MDR_ASSERT_C_STRUCT(MDRSafeListening);
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

static int session_open(Session* session)
{
    memset(session, 0, sizeof(*session));
    mock_init(&session->transport);
    check_result(
        mdrHeadphonesCreate(&session->transport.connection, &session->headphones),
        MDR_RESULT_OK,
        "opaque headphones session opens"
    );
    return session->headphones != NULL;
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

static size_t pack_ack(unsigned char output[FRAME_BUFFER_CAPACITY])
{
    return pack_frame(MDR_DATA_TYPE_ACK, 1, NULL, 0, output);
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
static const unsigned char k_v1_protocol_info[] = {0x01, 0x00, 0x01};

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
        mdrHeadphonesRequestFetch(session.headphones),
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
    staged.struct_size = (uint32_t)sizeof(staged);
    staged.status = MDR_PLAYBACK_UNKNOWN;
    staged.volume = 12;
    check_result(
        mdrHeadphonesSetPlayback(session.headphones, &staged),
        MDR_RESULT_OK,
        "playback volume stages"
    );

    memset(&current, 0, sizeof(current));
    current.struct_size = (uint32_t)sizeof(current);
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
        command.struct_size = (uint32_t)sizeof(command);
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
        ack_size = pack_ack(ack);
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
    command.struct_size = (uint32_t)sizeof(command);
    command.action = (MDRPlaybackAction)0xff;
    check_result(
        mdrHeadphonesPlayback(session.headphones, &command),
        MDR_RESULT_ERROR_INVALID_ARGUMENT,
        "unknown playback action is rejected"
    );

    memset(&unsupported_status, 0, sizeof(unsupported_status));
    unsupported_status.struct_size = (uint32_t)sizeof(unsupported_status);
    unsupported_status.status = MDR_PLAYBACK_PLAYING;
    unsupported_status.volume = 10;
    check_result(
        mdrHeadphonesSetPlayback(session.headphones, &unsupported_status),
        MDR_RESULT_ERROR_NOT_SUPPORTED,
        "playback status is not misrepresented as a staged volume change"
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
    MDRIdentity identity;
    MDREvent event;

    if (!session_open(&session))
        return;
    check_result(
        mdrHeadphonesRequestInit(session.headphones),
        MDR_RESULT_OK,
        "automatic initialization starts"
    );
    frame_size = pack_ack(frame);
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
    identity.struct_size = (uint32_t)sizeof(identity);
    check_result(
        mdrHeadphonesGetIdentity(session.headphones, &identity),
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

static void test_v1_bootstrap_not_supported(void)
{
    Session session;
    unsigned char frame[FRAME_BUFFER_CAPACITY];
    size_t frame_size;
    MDRIdentity identity;
    MDREvent state_changed;
    MDREvent completed;
    char* last_error;

    if (!session_open(&session))
        return;
    check_result(
        mdrHeadphonesRequestInit(session.headphones),
        MDR_RESULT_OK,
        "V1 probe starts"
    );
    frame_size = pack_ack(frame);
    mock_load(&session.transport, frame, frame_size);
    poll_event(session.headphones, &completed, "V1 probe ACK polls");

    frame_size = pack_data_frame(
        k_v1_protocol_info,
        sizeof(k_v1_protocol_info),
        0,
        frame
    );
    mock_load(&session.transport, frame, frame_size);
    poll_event(session.headphones, &state_changed, "three-byte V1 protocol-info polls");
    check_result(
        mdrHeadphonesPoll(session.headphones, &completed),
        MDR_RESULT_ERROR_GENERAL,
        "unsupported V1 initialization fails"
    );

    memset(&identity, 0, sizeof(identity));
    identity.struct_size = (uint32_t)sizeof(identity);
    check_result(
        mdrHeadphonesGetIdentity(session.headphones, &identity),
        MDR_RESULT_OK,
        "V1 identity is readable"
    );
    check(identity.protocol_version == 1, "three-byte payload is recognized as MDR V1");
    check(
        state_changed == MDR_EVENT_IDENTITY_CHANGED,
        "V1 recognition reports the identity change"
    );
    check(
        completed == MDR_EVENT_NONE,
        "failed initialization does not manufacture a completion event"
    );
    last_error = get_text(session.headphones, MDR_TEXT_LAST_ERROR);
    check(
        last_error != NULL && strstr(last_error, "not implemented") != NULL,
        "V1 failure text exposes the not-supported backend"
    );
    free(last_error);
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
    first.struct_size = (uint32_t)sizeof(first);
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
    newer.struct_size = (uint32_t)sizeof(newer);
    newer.status = MDR_PLAYBACK_UNKNOWN;
    newer.volume = 20;
    check_result(
        mdrHeadphonesSetPlayback(session.headphones, &newer),
        MDR_RESULT_OK,
        "newer playback value stages during apply"
    );

    ack_size = pack_ack(ack);
    mock_load(&session.transport, ack, ack_size);
    poll_event(session.headphones, &ack_event, "apply ACK polls");
    poll_event(session.headphones, &completion, "apply completion polls");
    check(
        ack_event == MDR_EVENT_UNHANDLED
            && completion == MDR_EVENT_APPLY_COMPLETE,
        "ACK and completion are reported in poll order"
    );

    memset(&current, 0, sizeof(current));
    current.struct_size = (uint32_t)sizeof(current);
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

int main(void)
{
    test_struct_and_buffer_contracts();
    test_one_operation_at_a_time();
    test_committed_state_staging();
    test_playback_actions();
    test_poll_events();
    test_v2_bootstrap();
    test_v1_bootstrap_not_supported();
    test_newer_staging_survives_apply();

    if (g_failures != 0)
        fprintf(stderr, "%d test assertion(s) failed\n", g_failures);
    return g_failures != 0 ? 1 : 0;
}
