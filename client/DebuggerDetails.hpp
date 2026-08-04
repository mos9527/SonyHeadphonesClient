#pragma once

#include <imgui.h>
#include <mdr/Command.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <limits>
#include <span>
#include <type_traits>
#include <utility>

#define MDR_DEBUGGER_TRY_READ(ResultType, Expression)                                                                  \
    do                                                                                                                 \
    {                                                                                                                  \
        const auto debuggerReadResult = (Expression);                                                                  \
        if (!debuggerReadResult)                                                                                       \
            return ::mdr::MDRResult<ResultType>::Failure(debuggerReadResult.error, debuggerReadResult.errMessage);     \
        remaining -= debuggerReadResult.value;                                                                         \
    }                                                                                                                  \
    while (false)

#define MDR_DEBUGGER_TRY_WRITE(ResultType, Expression)                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        const auto debuggerWriteResult = (Expression);                                                                 \
        if (!debuggerWriteResult)                                                                                      \
            return ::mdr::MDRResult<ResultType>::Failure(debuggerWriteResult.error, debuggerWriteResult.errMessage);   \
        remaining -= debuggerWriteResult.value;                                                                        \
    }                                                                                                                  \
    while (false)

namespace client::debugger
{
    enum class ProtocolFamily
    {
        V1,
        V2
    };

    struct PacketDescriptor
    {
        using CreateFn = void* (*)();
        using DestroyFn = void (*)(void*);
        using DecodeFn = mdr::MDRResult<void> (*)(void*, mdr::Span<const mdr::UInt8>);
        using EncodeFn = mdr::MDRResult<size_t> (*)(const void*, mdr::UInt8*, size_t);
        using ValidateFn = mdr::MDRResult<void> (*)(const void*);
        using DrawFn = bool (*)(void*);

        const char* name;
        ProtocolFamily family;
        mdr::MDRDataType dataType;
        mdr::UInt8 command;
        CreateFn create;
        DestroyFn destroy;
        DecodeFn decode;
        EncodeFn encode;
        ValidateFn validate;
        DrawFn draw;
    };

    class PacketInstance
    {
        const PacketDescriptor* mDescriptor{};
        void* mValue{};

    public:
        PacketInstance() = default;

        explicit PacketInstance(const PacketDescriptor& descriptor) :
            mDescriptor(&descriptor), mValue(descriptor.create())
        {
        }

        PacketInstance(const PacketInstance&) = delete;
        PacketInstance& operator=(const PacketInstance&) = delete;

        PacketInstance(PacketInstance&& other) noexcept :
            mDescriptor(std::exchange(other.mDescriptor, nullptr)), mValue(std::exchange(other.mValue, nullptr))
        {
        }

        PacketInstance& operator=(PacketInstance&& other) noexcept
        {
            if (this != &other)
            {
                Reset();
                mDescriptor = std::exchange(other.mDescriptor, nullptr);
                mValue = std::exchange(other.mValue, nullptr);
            }
            return *this;
        }

        ~PacketInstance() { Reset(); }

        void Reset()
        {
            if (mValue)
                mDescriptor->destroy(mValue);
            mDescriptor = nullptr;
            mValue = nullptr;
        }

        [[nodiscard]] explicit operator bool() const noexcept { return mValue != nullptr; }
        [[nodiscard]] const PacketDescriptor& Descriptor() const { return *mDescriptor; }
        [[nodiscard]] void* Value() { return mValue; }
        [[nodiscard]] const void* Value() const { return mValue; }
    };

    struct EnumOption
    {
        const char* name;
        std::uint64_t value;
    };

    inline bool DrawTree(const char* label, auto&& drawContents)
    {
        bool changed = false;
        if (ImGui::TreeNode(label))
        {
            changed = drawContents();
            ImGui::TreePop();
        }
        return changed;
    }

    template <typename T>
    constexpr ImGuiDataType ImGuiScalarType()
    {
        using Value = std::remove_cv_t<T>;
        if constexpr (std::is_same_v<Value, std::int8_t>)
            return ImGuiDataType_S8;
        else if constexpr (std::is_same_v<Value, std::uint8_t>)
            return ImGuiDataType_U8;
        else if constexpr (std::is_same_v<Value, std::int16_t>)
            return ImGuiDataType_S16;
        else if constexpr (std::is_same_v<Value, std::uint16_t>)
            return ImGuiDataType_U16;
        else if constexpr (std::is_same_v<Value, std::int32_t>)
            return ImGuiDataType_S32;
        else if constexpr (std::is_same_v<Value, std::uint32_t>)
            return ImGuiDataType_U32;
        else if constexpr (std::is_same_v<Value, std::int64_t>)
            return ImGuiDataType_S64;
        else if constexpr (std::is_same_v<Value, std::uint64_t>)
            return ImGuiDataType_U64;
        else if constexpr (std::is_same_v<Value, float>)
            return ImGuiDataType_Float;
        else if constexpr (std::is_same_v<Value, double>)
            return ImGuiDataType_Double;
        else
            return ImGuiDataType_COUNT;
    }

    template <typename T>
    bool DrawScalar(const char* label, T& value)
    {
        if constexpr (std::is_same_v<std::remove_cv_t<T>, bool>)
        {
            return ImGui::Checkbox(label, &value);
        }
        else
        {
            constexpr ImGuiDataType type = ImGuiScalarType<T>();
            if constexpr (type != ImGuiDataType_COUNT)
                return ImGui::InputScalar(label, type, &value);
            else
                return false;
        }
    }

    template <typename T>
    bool DrawEndian(const char* label, T& value)
    {
        if constexpr (std::is_same_v<T, mdr::Int16BE>)
        {
            std::int16_t native = value;
            if (!DrawScalar(label, native))
                return false;
            value = native;
        }
        else if constexpr (std::is_same_v<T, mdr::Int24BE> || std::is_same_v<T, mdr::Int32BE>)
        {
            std::int32_t native = value;
            if (!DrawScalar(label, native))
                return false;
            value = native;
        }
        else
        {
            std::uint64_t native = value;
            if (!DrawScalar(label, native))
                return false;
            value = native;
        }
        return true;
    }

    template <typename Enum>
    bool DrawEnum(const char* label, Enum& value, std::span<const EnumOption> options)
    {
        using Underlying = std::underlying_type_t<Enum>;
        std::uint64_t raw = static_cast<std::make_unsigned_t<Underlying>>(value);
        const char* preview = "Unknown";
        for (const EnumOption& option : options)
            if (option.value == raw)
                preview = option.name;

        bool changed = false;
        ImGui::PushID(label);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(label);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x * 0.55f);
        if (ImGui::BeginCombo("##enum", preview))
        {
            for (const EnumOption& option : options)
            {
                const bool selected = option.value == raw;
                if (ImGui::Selectable(option.name, selected))
                {
                    raw = option.value;
                    changed = true;
                }
                if (selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-1);
        changed |= ImGui::InputScalar("##raw", ImGuiDataType_U64, &raw, nullptr, nullptr, "%02llX",
                                      ImGuiInputTextFlags_CharsHexadecimal);
        ImGui::PopID();
        if (changed)
            value = static_cast<Enum>(static_cast<Underlying>(raw));
        return changed;
    }

    inline bool DrawString(const char* label, mdr::String& value, size_t maximumSize)
    {
        constexpr size_t kEditorCapacity = 4096;
        std::array<char, kEditorCapacity> buffer{};
        const size_t limit = std::min(maximumSize, buffer.size() - 1);
        const size_t copied = std::min(value.size(), limit);
        std::memcpy(buffer.data(), value.data(), copied);
        if (!ImGui::InputText(label, buffer.data(), limit + 1))
            return false;
        value.assign(buffer.data());
        return true;
    }

    inline bool DrawPrefixedString(const char* label, mdr::MDRPrefixedString& value)
    {
        return DrawString(label, value.value, UINT8_MAX);
    }

    template <typename Sequence, typename DrawElement>
    bool DrawSequence(const char* label, Sequence& sequence, bool resizable, DrawElement&& drawElement)
    {
        bool changed = false;
        if (!ImGui::TreeNode(label, "%s [%zu]", label, sequence.size()))
            return false;

        size_t removeIndex = std::numeric_limits<size_t>::max();
        for (size_t index = 0; index < sequence.size(); ++index)
        {
            ImGui::PushID(static_cast<int>(index));
            const mdr::String elementLabel = mdr::Format("[{}]", index);
            changed |= drawElement(elementLabel.c_str(), sequence[index]);
            if constexpr (requires { sequence.erase(sequence.begin()); })
            {
                if (resizable)
                {
                    ImGui::SameLine();
                    if (ImGui::SmallButton("-"))
                        removeIndex = index;
                }
            }
            ImGui::PopID();
        }

        if constexpr (requires {
                          sequence.erase(sequence.begin());
                          sequence.emplace_back();
                      })
        {
            if (resizable)
            {
                if (removeIndex != std::numeric_limits<size_t>::max())
                {
                    sequence.erase(sequence.begin() + static_cast<std::ptrdiff_t>(removeIndex));
                    changed = true;
                }
                ImGui::BeginDisabled(sequence.size() >= UINT8_MAX);
                if (ImGui::SmallButton("+"))
                {
                    sequence.emplace_back();
                    changed = true;
                }
                ImGui::EndDisabled();
            }
        }
        ImGui::TreePop();
        return changed;
    }

    template <typename Entries, typename DrawKey, typename DrawValue>
    bool DrawMap(const char* label, Entries& entries, DrawKey&& drawKey, DrawValue&& drawValue)
    {
        bool changed = false;
        if (!ImGui::TreeNode(label, "%s [%zu]", label, entries.size()))
            return false;

        size_t removeIndex = std::numeric_limits<size_t>::max();
        for (size_t index = 0; index < entries.size(); ++index)
        {
            ImGui::PushID(static_cast<int>(index));
            if (ImGui::TreeNode("entry", "[%zu]", index))
            {
                changed |= drawKey("key", entries[index].key);
                changed |= drawValue("value", entries[index].value);
                ImGui::TreePop();
            }
            ImGui::SameLine();
            if (ImGui::SmallButton("-"))
                removeIndex = index;
            ImGui::PopID();
        }
        if (removeIndex != std::numeric_limits<size_t>::max())
        {
            entries.erase(entries.begin() + static_cast<std::ptrdiff_t>(removeIndex));
            changed = true;
        }
        ImGui::BeginDisabled(entries.size() >= UINT8_MAX);
        if (ImGui::SmallButton("+"))
        {
            entries.emplace_back();
            changed = true;
        }
        ImGui::EndDisabled();
        ImGui::TreePop();
        return changed;
    }

    inline bool DrawOpaqueBytes(const char* label, void* data, size_t size)
    {
        return DrawTree(label,
                        [&]()
                        {
                            bool changed = false;
                            auto* bytes = static_cast<mdr::UInt8*>(data);
                            for (size_t index = 0; index < size; ++index)
                            {
                                ImGui::PushID(static_cast<int>(index));
                                const mdr::String byteLabel = mdr::Format("[{}]", index);
                                changed |=
                                    ImGui::InputScalar(byteLabel.c_str(), ImGuiDataType_U8, &bytes[index], nullptr,
                                                       nullptr, "%02X", ImGuiInputTextFlags_CharsHexadecimal);
                                ImGui::PopID();
                            }
                            return changed;
                        });
    }

    std::span<const PacketDescriptor> Getv1t1Packets();
    std::span<const PacketDescriptor> Getv1t2Packets();
    std::span<const PacketDescriptor> Getv2t1Packets();
    std::span<const PacketDescriptor> Getv2t2Packets();
} // namespace client::debugger
