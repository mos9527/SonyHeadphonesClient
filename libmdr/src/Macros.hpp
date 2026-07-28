// NOLINTBEGIN
/**
 * @brief Sends command through @ref SendCommandImpl<T>, and re-schedule ourselves to
 *        co_await for an @ref Await(AWAIT_ACK) on the coroutine.
 * @param Type Command payload of @ref MDRIsSerializable type
 * @note  This is ONLY meaningful within a @ref MDRTask coroutine, as this schedules
 *        the current task to wait on a @ref AWAIT_ACK event.
 *
 * As to _why_ this is here instead of a templated member function - instantiated
 * templates would create their own @ref MDRTask and generate code for EACH of them,
 * while all we need is merely a `co_await`...
 *
 * TL;DR, this helps with compiler bloats. Use it well.
 */
#define SendCommandACK(Type, ...)                                                                                      \
    {                                                                                                                  \
        int _retries;                                                                                                  \
        const int _maxRetries = kAwaitAckRetries;                                                                      \
        for (_retries = 0; _retries < _maxRetries; _retries++)                                                         \
        {                                                                                                              \
            const int _sendResult = SendCommandImpl<Type>(__VA_ARGS__);                                                \
            if (_sendResult != MDR_RESULT_OK)                                                                          \
            {                                                                                                          \
                SetLastError(_sendResult, "Unable to serialize command");                                              \
                co_return MDR_HEADPHONES_ERROR;                                                                        \
            }                                                                                                          \
            int res = co_await Await(AWAIT_ACK);                                                                       \
            if (res == MDR_RESULT_OK)                                                                                  \
                break;                                                                                                 \
            MDR_LOG("FIXME-ACK Timeout. Retry {}/{}", _retries, _maxRetries);                                          \
        }                                                                                                              \
        if (_retries == _maxRetries)                                                                                   \
        {                                                                                                              \
            SetLastError(MDR_RESULT_ERROR_TIMEOUT, "Timeout exceeded waiting for device to respond");                  \
            co_return MDR_HEADPHONES_ERROR;                                                                            \
        }                                                                                                              \
    }

/**
 * @brief Just a helper macro to deserialize a command payload.
 */
#define Deserialize(Type, Name, Command)                                                                               \
    auto Name##Result = Type::Deserialize((Command).data(), (Command).size());                                         \
    if (!Name##Result)                                                                                                 \
        return self->SetLastError(Name##Result.error, Name##Result.errMessage ? Name##Result.errMessage : "Unable to deserialize " #Type);   \
    auto& Name = Name##Result.value

// NOLINTEND
