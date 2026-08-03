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
    auto& Name = Name##Result.value;                                                                                   \
    MDR_LOG_DEBUG("{}", format_as(Name))

// NOLINTEND

namespace mdr
{
    namespace detail
    {
        class JsonFormatter
        {
            struct Frame
            {
                char close;
                bool first{true};
            };

            String mOutput;
            Vector<Frame> mFrames;

            void AppendIndent(size_t depth) { mOutput.append(depth * 4, ' '); }

            void AppendEscaped(std::string_view value)
            {
                static constexpr char kHex[] = "0123456789ABCDEF";
                mOutput.push_back('"');
                for (const unsigned char character : value)
                {
                    switch (character)
                    {
                    case '"':
                        mOutput += "\\\"";
                        break;
                    case '\\':
                        mOutput += "\\\\";
                        break;
                    case '\b':
                        mOutput += "\\b";
                        break;
                    case '\f':
                        mOutput += "\\f";
                        break;
                    case '\n':
                        mOutput += "\\n";
                        break;
                    case '\r':
                        mOutput += "\\r";
                        break;
                    case '\t':
                        mOutput += "\\t";
                        break;
                    default:
                        if (character < 0x20)
                        {
                            mOutput += "\\u00";
                            mOutput.push_back(kHex[character >> 4]);
                            mOutput.push_back(kHex[character & 0x0F]);
                        }
                        else
                        {
                            mOutput.push_back(static_cast<char>(character));
                        }
                        break;
                    }
                }
                mOutput.push_back('"');
            }

            void PrefixElement()
            {
                Frame& frame = mFrames.back();
                if (!frame.first)
                    mOutput.push_back(',');
                mOutput.push_back('\n');
                AppendIndent(mFrames.size());
                frame.first = false;
            }

            void PrefixField(std::string_view name)
            {
                PrefixElement();
                AppendEscaped(name);
                mOutput += ": ";
            }

            void BeginContainer(char open, char close)
            {
                mOutput.push_back(open);
                mFrames.push_back(Frame{close});
            }

            void EndContainer()
            {
                const Frame frame = mFrames.back();
                mFrames.pop_back();
                if (!frame.first)
                {
                    mOutput.push_back('\n');
                    AppendIndent(mFrames.size());
                }
                mOutput.push_back(frame.close);
            }

            void AppendNested(std::string_view value)
            {
                for (const char character : value)
                {
                    mOutput.push_back(character);
                    if (character == '\n')
                        AppendIndent(mFrames.size());
                }
            }

            void AppendValue(const String& value) { AppendEscaped(std::string_view(value.data(), value.size())); }

            void AppendValue(const MDRPrefixedString& value) { AppendValue(value.value); }

            void AppendValue(Int16BE value)
            {
                fmt::format_to(std::back_inserter(mOutput), "{}", static_cast<int16_t>(value));
            }

            void AppendValue(Int24BE value)
            {
                fmt::format_to(std::back_inserter(mOutput), "{}", static_cast<int32_t>(value));
            }

            void AppendValue(Int32BE value)
            {
                fmt::format_to(std::back_inserter(mOutput), "{}", static_cast<int32_t>(value));
            }

            void AppendValue(UInt64BE value)
            {
                fmt::format_to(std::back_inserter(mOutput), "{}", static_cast<uint64_t>(value));
            }

            template <typename K, typename V>
            void AppendValue(const MDRMap<K, V>& value)
            {
                BeginContainer('[', ']');
                for (const auto& entry : value.entries)
                {
                    PrefixElement();
                    BeginContainer('{', '}');
                    Field("key", entry.key);
                    Field("value", entry.value);
                    EndContainer();
                }
                EndContainer();
            }

            template <typename T>
                requires requires(const T& value) {
                    value.begin();
                    value.end();
                }
            void AppendValue(const T& value)
            {
                BeginContainer('[', ']');
                for (const auto& element : value)
                {
                    PrefixElement();
                    AppendValue(element);
                }
                EndContainer();
            }

            template <typename T>
            void AppendValue(const T& value)
            {
                if constexpr (std::is_same_v<T, bool>)
                {
                    mOutput += value ? "true" : "false";
                }
                else if constexpr (std::is_enum_v<T>)
                {
                    AppendEscaped(format_as(value));
                }
                else if constexpr (std::is_integral_v<T>)
                {
                    if constexpr (std::is_signed_v<T>)
                    {
                        fmt::format_to(std::back_inserter(mOutput), "{}", static_cast<int64_t>(value));
                    }
                    else
                    {
                        fmt::format_to(std::back_inserter(mOutput), "{}", static_cast<uint64_t>(value));
                    }
                }
                else if constexpr (std::is_floating_point_v<T>)
                {
                    fmt::format_to(std::back_inserter(mOutput), "{}", value);
                }
                else
                {
                    const String nested = format_as(value);
                    AppendNested(std::string_view(nested.data(), nested.size()));
                }
            }

        public:
            void BeginObject() { BeginContainer('{', '}'); }

            void EndObject() { EndContainer(); }

            void BeginObjectField(std::string_view name)
            {
                PrefixField(name);
                BeginContainer('{', '}');
            }

            void BeginArrayField(std::string_view name)
            {
                PrefixField(name);
                BeginContainer('[', ']');
            }

            void BeginObjectElement()
            {
                PrefixElement();
                BeginContainer('{', '}');
            }

            void EndArray() { EndContainer(); }

            template <typename T>
            void Field(std::string_view name, const T& value)
            {
                PrefixField(name);
                AppendValue(value);
            }

            template <typename T>
            void Element(const T& value)
            {
                PrefixElement();
                AppendValue(value);
            }

            [[nodiscard]] String Take() && { return std::move(mOutput); }
        };
    } // namespace detail

}
