#include "Debugger.hpp"

#include "DebuggerDetails.hpp"

#include "Fonts/PlexSansIcon.h"
#include "Platform/Platform.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_iostream.h>
#include <SDL3/SDL_time.h>
#include <imgui.h>
#include <mdr/Command.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <limits>
#include <string_view>
#include <utility>

namespace
{
    using namespace debugger;

    ImFont* gMonospaceFont{};

    struct CapturedPacket
    {
        std::uint64_t id{};
        MDRPacketDirection direction{};
        mdr::MDRBuffer frame;
        mdr::MDRBuffer payload;
        mdr::MDRDataType type{mdr::MDRDataType::UNKNOWN};
        mdr::MDRCommandSeqNumber sequence{};
        mdr::Vector<const PacketDescriptor*> candidates;
        bool unpacked{};
    };

    MDRHeadphones* gHeadphones{};
    MDRProtocolVersion gProtocolVersion{};
    mdr::Deque<CapturedPacket> gHistory;
    bool gReplayHistory{};
    std::uint64_t gSelectedCaptureId{};
    PacketInstance gPacket;
    mdr::Vector<const PacketDescriptor*> gSelectedCandidates;
    mdr::MDRBuffer gOriginalPayload;
    mdr::MDRBuffer gEncodedPayload;
    mdr::MDRDataType gEnvelopeType{mdr::MDRDataType::DATA_MDR};
    mdr::MDRCommandSeqNumber gEnvelopeSequence{};
    bool gAwaitAck{true};
    ProtocolFamily gNewPacketFamily{ProtocolFamily::V2};
    int gNewPacketTable{};
    std::array<char, 128> gTypeFilter{};
    std::array<char, 128> gHistoryFilter{};
    mdr::String gStatus;
    bool gFollowMessages{true};
    bool gScrollToLatest{};

    SDL_Window* gWindow{};
    constexpr const char* kDefaultPacketExportPath = "mdr-debugger-packet.bin";
    struct ExportRequest
    {
        AppLocale locale{};
        mdr::String result;
    };

    // Only exports initiated inside Protocol Debugger may opt out of sanitization.
    bool gSanitizeExports{true};
    bool gExportDialogActive{};
    bool gExportResultReady{};
    mdr::String gExportResult;
    mdr::String gExportStatus;

    bool WriteCaptureFile(const char* path, mdr::Span<const mdr::UInt8> frame);

    void SDLCALL SetExportResult(void* userdata)
    {
        auto* request = static_cast<ExportRequest*>(userdata);
        gExportResult = std::move(request->result);
        gExportResultReady = true;
        gExportDialogActive = false;
        mdr::Destruct(request);
    }

    void PollExportResult()
    {
        if (!gExportResultReady)
            return;
        mdr::String result = std::move(gExportResult);
        gExportResult.clear();
        gExportResultReady = false;
        gExportStatus = result;
        gStatus = std::move(result);
    }

    void ExportCallback(void* userdata, ClientPlatformSaveBlobResult status, const char* path, const char* error)
    {
        auto* request = static_cast<ExportRequest*>(userdata);
        mdr::String result;
        const auto translate = [request](i18n::TextId id) { return i18n::Translate(id, request->locale); };
        switch (status)
        {
        case CLIENT_PLATFORM_SAVE_BLOB_SAVED:
            result = mdr::Format(fmt::runtime(translate(i18n::TextId::DebuggerExported)), path);
            break;
        case CLIENT_PLATFORM_SAVE_BLOB_DISPATCHED:
            result = mdr::Format(fmt::runtime(translate(i18n::TextId::DebuggerExportDispatched)), path);
            break;
        case CLIENT_PLATFORM_SAVE_BLOB_CANCELLED:
            result = translate(i18n::TextId::DebuggerExportCancelled);
            break;
        case CLIENT_PLATFORM_SAVE_BLOB_FAILED:
            result = mdr::Format(fmt::runtime(translate(i18n::TextId::DebuggerExportFailed)), path, error);
            break;
        case CLIENT_PLATFORM_SAVE_BLOB_FALLBACK_SAVED:
            result = mdr::Format(fmt::runtime(translate(i18n::TextId::DebuggerExportedFallback)), path, error);
            break;
        }
        request->result = std::move(result);
        if (!SDL_RunOnMainThread(SetExportResult, request, false))
        {
            MDR_LOG("Unable to dispatch debugger export result: {}", SDL_GetError());
            mdr::Destruct(request);
        }
    }

    bool RequestExport(mdr::Span<const mdr::UInt8> data, const char* defaultPath, const char* filterExtension)
    {
        PollExportResult();
        if (gExportDialogActive)
        {
            gStatus = Tr(i18n::TextId::DebuggerExportDialogOpen);
            return false;
        }

        auto* request = mdr::Construct<ExportRequest>();
        request->locale = clientGetAppLocale();
        gExportDialogActive = true;
        gExportStatus = gStatus = Tr(i18n::TextId::DebuggerChooseDestination);
        const char* mimeType =
            std::string_view(filterExtension) == "zip" ? "application/zip" : "application/octet-stream";
        const int result =
            clientPlatformSaveBlob(defaultPath, data.data(), data.size(), mimeType, gWindow, ExportCallback, request);
        if (result != MDR_RESULT_OK)
        {
            const mdr::String error =
                result == MDR_RESULT_ERROR_NOT_SUPPORTED ? "File saving is not supported" : SDL_GetError();
            ExportCallback(request, CLIENT_PLATFORM_SAVE_BLOB_FAILED, defaultPath, error.c_str());
            PollExportResult();
            return false;
        }
        PollExportResult();
        return true;
    }

    template <typename Function>
    void ForEachDescriptor(Function&& function)
    {
        for (const PacketDescriptor& descriptor : Getv1t1Packets())
            function(descriptor);
        for (const PacketDescriptor& descriptor : Getv1t2Packets())
            function(descriptor);
        for (const PacketDescriptor& descriptor : Getv2t1Packets())
            function(descriptor);
        for (const PacketDescriptor& descriptor : Getv2t2Packets())
            function(descriptor);
    }

    bool SameProtocolTable(mdr::MDRDataType lhs, mdr::MDRDataType rhs)
    {
        const auto table = [](mdr::MDRDataType type)
        {
            switch (type)
            {
            case mdr::MDRDataType::DATA_MDR:
            case mdr::MDRDataType::SHOT_MDR:
                return 1;
            case mdr::MDRDataType::DATA_MDR_NO2:
            case mdr::MDRDataType::SHOT_MDR_NO2:
                return 2;
            default:
                return 0;
            }
        };
        return table(lhs) != 0 && table(lhs) == table(rhs);
    }

    class CaptureScrubber
    {
        using Address = std::array<mdr::UInt8, 17>;
        mdr::Vector<Address> mAddresses;

        static void ReplaceText(mdr::Span<mdr::UInt8> value, std::string_view placeholder)
        {
            for (size_t index = 0; index < value.size(); ++index)
                value[index] = index < placeholder.size() ? static_cast<mdr::UInt8>(placeholder[index]) : '-';
        }

        void ReplaceAddress(mdr::Span<mdr::UInt8> value)
        {
            Address address{};
            std::transform(value.begin(), value.end(), address.begin(),
                           [](mdr::UInt8 byte) { return static_cast<mdr::UInt8>(std::toupper(byte)); });
            auto found = std::find(mAddresses.begin(), mAddresses.end(), address);
            const size_t identifier = static_cast<size_t>(found - mAddresses.begin()) + 1;
            if (found == mAddresses.end())
                mAddresses.push_back(address);
            // Stable within an export, including connection-control references to paired devices.
            const mdr::String placeholder =
                mdr::Format("02:00:{:02X}:{:02X}:{:02X}:{:02X}", (identifier >> 24) & 0xff, (identifier >> 16) & 0xff,
                            (identifier >> 8) & 0xff, identifier & 0xff);
            ReplaceText(value, placeholder);
        }

        void SweepAddresses(mdr::Span<mdr::UInt8> payload)
        {
            for (size_t offset = 0; offset + 17 <= payload.size(); ++offset)
            {
                const auto address = payload.subspan(offset, 17);
                bool matches = true;
                for (size_t index = 0; index < address.size(); ++index)
                    matches &= index % 3 == 2 ? address[index] == ':' : std::isxdigit(address[index]) != 0;
                if (matches)
                {
                    ReplaceAddress(address);
                    offset += 16;
                }
            }
        }

        struct NameSpan
        {
            size_t offset{};
            size_t length{};
        };

        static bool FindNames(mdr::Span<const mdr::UInt8> payload, size_t offset, size_t count,
                              std::array<NameSpan, 4>& names)
        {
            for (size_t index = 0; index < count; ++index)
            {
                if (offset + 2 > payload.size())
                    return false;
                const size_t length = payload[offset + 1];
                if (payload[offset] > 2 || length > payload.size() - offset - 2)
                    return false;
                names[index] = {offset + 2, length};
                offset += 2 + length;
            }
            return offset == payload.size();
        }

        static bool ScrubPlayback(mdr::Span<mdr::UInt8> payload)
        {
            // V2 carries four names; V1 carries a detail type followed by one name.
            // Short packets are volume values or name-change notifications, not text.
            if (payload.size() <= 4)
                return true;
            constexpr std::array<std::string_view, 4> placeholders{"Example Track", "Example Album", "Example Artist",
                                                                   "Example Genre"};
            std::array<NameSpan, 4> names{};
            if (FindNames(payload, 2, 4, names))
            {
                for (size_t index = 0; index < names.size(); ++index)
                    ReplaceText(payload.subspan(names[index].offset, names[index].length), placeholders[index]);
                return true;
            }
            const mdr::UInt8 detail = payload[2];
            if (detail == 0x20) // V1 volume
                return true;
            if ((detail > 3 && detail != 0x10) || !FindNames(payload, 3, 1, names))
                return false;
            ReplaceText(payload.subspan(names[0].offset, names[0].length),
                        detail == 0x10 ? "Example Player" : placeholders[detail]);
            return true;
        }

        static bool ScrubPeripherals(mdr::Span<mdr::UInt8> payload)
        {
            // V1 uses inquiry 1; V2 uses 0 (classic BT) or 2 (with class-of-device).
            // V2 inquiry 1 is source-switch control and contains no device names.
            if (payload.size() <= 4)
                return true;
            if (payload[1] > 2)
                return true;
            size_t offset = 3;
            const size_t nameLengthOffset = payload[1] == 2 ? 21 : 18;
            for (size_t index = 0; index < payload[2]; ++index)
            {
                if (offset + nameLengthOffset + 1 > payload.size())
                    return false;
                const size_t length = payload[offset + nameLengthOffset];
                const size_t nameOffset = offset + nameLengthOffset + 1;
                if (length > payload.size() - nameOffset)
                    return false;
                // Scrub all encodings, including non-printable names; do not leave binary names untouched.
                ReplaceText(payload.subspan(nameOffset, length), mdr::Format("Paired Device {}", index + 1));
                offset = nameOffset + length;
            }
            // One final byte identifies the device with playback rights.
            return offset + 1 == payload.size();
        }

    public:
        bool Frame(mdr::Span<const mdr::UInt8> original, mdr::MDRBuffer& output)
        {
            mdr::MDRBuffer payload;
            mdr::MDRDataType type{};
            mdr::MDRCommandSeqNumber sequence{};
            if (mdr::MDRUnpackCommand(original, payload, type, sequence) != mdr::MDRUnpackResult::OK)
                return false;
            const mdr::MDRBuffer originalPayload = payload;
            if (payload.size() >= 2)
            {
                if (SameProtocolTable(type, mdr::MDRDataType::DATA_MDR) &&
                    (payload[0] == 0xa7 || payload[0] == 0xa8 || payload[0] == 0xa9) && payload[1] == 1)
                {
                    if (!ScrubPlayback(payload))
                        return false;
                }
                if (SameProtocolTable(type, mdr::MDRDataType::DATA_MDR_NO2) &&
                    (payload[0] == 0x37 || payload[0] == 0x39) && !ScrubPeripherals(payload))
                    return false;
            }
            // Include the headphones' own address and addresses in otherwise unmodelled packets.
            SweepAddresses(payload);
            if (payload == originalPayload)
                output.assign(original.begin(), original.end());
            else
                output = mdr::MDRPackCommand(type, sequence, payload);
            return true;
        }
    };

    bool SanitizationFailed()
    {
        // Never silently fall back to exporting personal data when a frame cannot be scrubbed.
        gExportStatus = gStatus = Tr(i18n::TextId::DebuggerSanitizationFailed);
        return SDL_SetError("%s", gExportStatus.c_str());
    }

    bool RequestPacketExport(mdr::Span<const mdr::UInt8> frame, bool sanitize)
    {
        mdr::MDRBuffer sanitized;
        CaptureScrubber scrubber;
        if (sanitize && !scrubber.Frame(frame, sanitized))
            return SanitizationFailed();
        return RequestExport(sanitize ? mdr::Span<const mdr::UInt8>{sanitized} : frame, kDefaultPacketExportPath,
                             "bin");
    }

    bool ContainsCaseInsensitive(std::string_view value, std::string_view query)
    {
        if (query.empty())
            return true;
        return std::search(value.begin(), value.end(), query.begin(), query.end(),
                           [](char lhs, char rhs)
                           {
                               return std::tolower(static_cast<unsigned char>(lhs)) ==
                                   std::tolower(static_cast<unsigned char>(rhs));
                           }) != value.end();
    }

    bool ContainsAllTokens(std::string_view value, std::string_view query)
    {
        size_t start = 0;
        while (start < query.size())
        {
            while (start < query.size() && std::isspace(static_cast<unsigned char>(query[start])))
                ++start;
            size_t end = start;
            while (end < query.size() && !std::isspace(static_cast<unsigned char>(query[end])))
                ++end;
            if (start < end && !ContainsCaseInsensitive(value, query.substr(start, end - start)))
                return false;
            start = end;
        }
        return true;
    }

    bool MatchesActiveProtocolFamily(const PacketDescriptor& descriptor)
    {
        if (!gHeadphones)
            return true;

        switch (gProtocolVersion)
        {
        case MDR_PROTOCOL_V1:
            return descriptor.family == ProtocolFamily::V1;
        case MDR_PROTOCOL_V2:
            return descriptor.family == ProtocolFamily::V2;
        default:
            return true;
        }
    }

    void FindCandidates(CapturedPacket& packet)
    {
        packet.candidates.clear();
        if (!packet.unpacked || packet.payload.empty())
            return;

        const mdr::UInt8 command = packet.payload.front();
        mdr::Vector<const PacketDescriptor*> validated;
        mdr::Vector<const PacketDescriptor*> decoded;
        ForEachDescriptor(
            [&](const PacketDescriptor& descriptor)
            {
                if (!MatchesActiveProtocolFamily(descriptor) || !SameProtocolTable(descriptor.dataType, packet.type) ||
                    descriptor.command != command)
                {
                    return;
                }

                PacketInstance candidate{descriptor};
                if (descriptor.decode(candidate.Value(), packet.payload))
                {
                    decoded.push_back(&descriptor);
                    if (descriptor.validate(candidate.Value()))
                        validated.push_back(&descriptor);
                }
            });
        packet.candidates = validated.empty() ? std::move(decoded) : std::move(validated);
    }

    CapturedPacket* FindCapture(std::uint64_t id)
    {
        const auto found = std::find_if(gHistory.begin(), gHistory.end(),
                                        [id](const CapturedPacket& packet) { return packet.id == id; });
        return found == gHistory.end() ? nullptr : &*found;
    }

    bool DecodeIntoEditor(const PacketDescriptor& descriptor, mdr::Span<const mdr::UInt8> payload)
    {
        PacketInstance packet{descriptor};
        const auto decoded = descriptor.decode(packet.Value(), payload);
        if (!decoded)
        {
            gStatus = decoded.errMessage ? decoded.errMessage : mdrResultString(decoded.error);
            return false;
        }
        gPacket = std::move(packet);
        gOriginalPayload.assign(payload.begin(), payload.end());
        gStatus.clear();
        return true;
    }

    void SelectCapture(CapturedPacket& capture)
    {
        gSelectedCaptureId = capture.id;
        gSelectedCandidates = capture.candidates;
        gEnvelopeType = capture.type;
        gEnvelopeSequence = capture.sequence;
        gPacket.Reset();
        gOriginalPayload = capture.payload;
        if (!capture.candidates.empty())
            DecodeIntoEditor(*capture.candidates.front(), capture.payload);
    }

    void CreatePacket(const PacketDescriptor& descriptor)
    {
        gPacket = PacketInstance{descriptor};
        gSelectedCaptureId = 0;
        gSelectedCandidates.clear();
        gOriginalPayload.clear();
        gEnvelopeType = descriptor.dataType;
        gStatus.clear();
    }

    void DrawHex(i18n::TextId label, mdr::Span<const mdr::UInt8> bytes)
    {
        if (!ImGui::TreeNodeEx(TextLabel(label).c_str(), ImGuiTreeNodeFlags_DefaultOpen, "%s [%zu]", Tr(label),
                               bytes.size()))
            return;

        constexpr size_t bytesPerLine = 8;
        constexpr size_t offsetColumns = 7;
        constexpr size_t hexColumns = bytesPerLine * 3;
        if (gMonospaceFont)
            ImGui::PushFont(gMonospaceFont, ImGui::GetStyle().FontSizeBase);
        const ImU32 textColor = ImGui::GetColorU32(ImGuiCol_Text);
        const ImU32 mutedColor = ImGui::GetColorU32(ImGuiCol_TextDisabled);
        const float characterWidth = ImGui::CalcTextSize("0").x;
        const float lineHeight = ImGui::GetTextLineHeight();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        for (size_t offset = 0; offset < bytes.size(); offset += bytesPerLine)
        {
            const ImVec2 origin = ImGui::GetCursorScreenPos();
            const mdr::String offsetText = mdr::Format("{:04X}: ", offset);
            drawList->AddText(origin, mutedColor, offsetText.c_str());

            for (size_t column = 0; column < bytesPerLine; ++column)
            {
                const size_t index = offset + column;
                if (index >= bytes.size())
                    break;

                const mdr::UInt8 byte = bytes[index];
                const mdr::String byteText = mdr::Format("{:02X}", byte);
                const ImVec2 position{origin.x + (offsetColumns + column * 3) * characterWidth, origin.y};
                drawList->AddText(position, byte == 0 ? mutedColor : textColor, byteText.c_str());
            }

            const float asciiStart = origin.x + (offsetColumns + hexColumns + 1) * characterWidth;
            drawList->AddText({asciiStart, origin.y}, mutedColor, "|");
            for (size_t column = 0; column < bytesPerLine; ++column)
            {
                const size_t index = offset + column;
                if (index >= bytes.size())
                    break;

                const mdr::UInt8 byte = bytes[index];
                const bool printable = std::isprint(static_cast<unsigned char>(byte));
                const char character[] = {printable ? static_cast<char>(byte) : '.', '\0'};
                drawList->AddText({asciiStart + (column + 2) * characterWidth, origin.y},
                                  printable ? textColor : mutedColor, character);
            }
            drawList->AddText({asciiStart + (bytesPerLine + 2) * characterWidth, origin.y}, mutedColor, "|");
            ImGui::Dummy({(offsetColumns + hexColumns + bytesPerLine + 4) * characterWidth, lineHeight});
        }
        if (gMonospaceFont)
            ImGui::PopFont();
        ImGui::TreePop();
    }

    const char* ShortPacketName(const char* qualifiedName)
    {
        const std::string_view name{qualifiedName};
        const size_t separator = name.rfind("::");
        return separator == std::string_view::npos ? qualifiedName : qualifiedName + separator + 2;
    }

    bool IsAck(const CapturedPacket& packet)
    {
        return packet.unpacked && packet.type == mdr::MDRDataType::ACK;
    }

    mdr::String PacketSearchText(const CapturedPacket& packet)
    {
        const char* direction = packet.direction == MDR_PACKET_DIRECTION_RX ? "RX" : "TX";
        if (IsAck(packet))
            return mdr::Format(
                "#{} {} ACK seq {:02X} {}", packet.id, direction, packet.sequence,
                mdr::Format(fmt::runtime(Tr(i18n::TextId::DebuggerAckSummary)), packet.id, direction, packet.sequence));

        const mdr::UInt8 command = packet.payload.empty() ? 0 : packet.payload.front();
        const char* packetName =
            packet.candidates.empty() ? "Unknown packet" : ShortPacketName(packet.candidates.front()->name);
        mdr::String text = mdr::Format("#{} {} {} type {:02X} cmd {:02X} seq {:02X} {} {}", packet.id, direction,
                                       packetName, static_cast<mdr::UInt8>(packet.type), command, packet.sequence,
                                       packet.candidates.empty() ? Tr(i18n::TextId::DebuggerUnknownPacket) : "",
                                       mdr::Format(fmt::runtime(Tr(i18n::TextId::DebuggerPacketDetails)),
                                                   static_cast<mdr::UInt8>(packet.type), command, packet.sequence));
        for (const PacketDescriptor* descriptor : packet.candidates)
            text += mdr::Format(" {}", descriptor->name);
        return text;
    }

    bool MatchesHistoryFilter(const CapturedPacket& packet)
    {
        return ContainsAllTokens(PacketSearchText(packet), gHistoryFilter.data());
    }

    /**
     * Packets matching the current history filter, maintained incrementally so that drawing doesn't re-format
     * and re-filter the whole session every frame. Rebuilt when the filter or locale changes, or on Clear.
     */
    struct HistoryView
    {
        mdr::Vector<size_t> rows; // Indices into gHistory, in order
        mdr::Vector<size_t> ackPrefix{0}; // ackPrefix[k]: number of ACK rows in rows[0, k)
        size_t scanned{}; // gHistory entries already considered
        mdr::String filter;
        AppLocale locale{};
        bool valid{};
    } gHistoryView;

    void InvalidateHistoryView()
    {
        gHistoryView.valid = false;
    }

    void UpdateHistoryView()
    {
        const AppLocale locale = clientGetAppLocale();
        if (!gHistoryView.valid || gHistoryView.locale != locale || gHistoryView.filter != gHistoryFilter.data())
        {
            gHistoryView.rows.clear();
            gHistoryView.ackPrefix.assign(1, 0);
            gHistoryView.scanned = 0;
            gHistoryView.filter = gHistoryFilter.data();
            gHistoryView.locale = locale;
            gHistoryView.valid = true;
        }
        for (; gHistoryView.scanned < gHistory.size(); ++gHistoryView.scanned)
        {
            const CapturedPacket& packet = gHistory[gHistoryView.scanned];
            if (!MatchesHistoryFilter(packet))
                continue;
            gHistoryView.rows.push_back(gHistoryView.scanned);
            gHistoryView.ackPrefix.push_back(gHistoryView.ackPrefix.back() + (IsAck(packet) ? 1u : 0u));
        }
    }

    float HistoryCardHeight()
    {
        const float lineHeight = ImGui::GetTextLineHeight();
        const float rowPaddingY = lineHeight * 0.2f;
        return lineHeight * 2.0f + rowPaddingY * 2.0f;
    }

    void DrawHistoryRow(CapturedPacket& packet, const ImVec4& accent)
    {
        const char* direction = packet.direction == MDR_PACKET_DIRECTION_RX ? "RX" : "TX";
        if (IsAck(packet))
        {
            const mdr::String ackLabel =
                mdr::Format(fmt::runtime(Tr(i18n::TextId::DebuggerAckSummary)), packet.id, direction, packet.sequence);
            ImGui::BeginDisabled();
            ImGui::TextUnformatted(ackLabel.c_str());
            ImGui::EndDisabled();
            return;
        }

        const bool selected = packet.id == gSelectedCaptureId;
        ImVec4 cardColor = accent;
        cardColor.w = selected ? 0.55f : 0.28f;

        ImGui::PushID(static_cast<int>(packet.id));
        ImVec4 hoveredColor = cardColor;
        hoveredColor.w = std::min(1.0f, cardColor.w + 0.18f);
        ImGui::PushStyleColor(ImGuiCol_Header, cardColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, hoveredColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, hoveredColor);
        const mdr::UInt8 command = packet.payload.empty() ? 0 : packet.payload.front();
        const char* packetName = packet.candidates.empty() ? Tr(i18n::TextId::DebuggerUnknownPacket)
                                                           : ShortPacketName(packet.candidates.front()->name);
        mdr::String details = mdr::Format(fmt::runtime(Tr(i18n::TextId::DebuggerPacketDetails)),
                                          static_cast<mdr::UInt8>(packet.type), command, packet.sequence);
        if (packet.candidates.size() > 1)
            details += mdr::Format(fmt::runtime(Tr(i18n::TextId::DebuggerLayoutCount)), packet.candidates.size());
        const mdr::String label = mdr::Format("#{} {} - {}\n{}###capture", packet.id, direction, packetName, details);
        const auto& style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, {style.SelectableTextAlign.x, 0.5f});
        if (ImGui::Selectable(label.c_str(), selected, ImGuiSelectableFlags_None,
                              {ImGui::GetContentRegionAvail().x, HistoryCardHeight()}))
            SelectCapture(packet);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
        ImGui::PopID();
    }

    void DrawCenteredPlaceholder(const char* placeholder)
    {
        const ImVec2 textSize = ImGui::CalcTextSize(placeholder);
        ImGui::SetCursorPos({std::max(0.0f, (ImGui::GetContentRegionAvail().x - textSize.x) * 0.5f),
                             std::max(0.0f, (ImGui::GetContentRegionAvail().y - textSize.y) * 0.5f)});
        ImGui::TextDisabled("%s", placeholder);
    }

    void DrawHistory()
    {
        if (gHistory.empty())
        {
            gScrollToLatest = false;
            DrawCenteredPlaceholder(Tr(i18n::TextId::DebuggerHistoryEmpty));
            return;
        }

        UpdateHistoryView();
        const mdr::Vector<size_t>& rows = gHistoryView.rows;
        const mdr::Vector<size_t>& ackPrefix = gHistoryView.ackPrefix;
        if (rows.empty())
        {
            gScrollToLatest = false;
            DrawCenteredPlaceholder(Tr(i18n::TextId::DebuggerFilterEmpty));
            return;
        }

        // Rows come in two heights (one-line ACKs, two-line cards), so ImGuiListClipper's uniform-height model
        // doesn't fit. Row offsets are instead derived from the ACK prefix counts, which stay exact across font
        // and DPI changes, and the visible range is found by binary search.
        const float spacing = ImGui::GetStyle().ItemSpacing.y;
        const float ackHeight = ImGui::GetTextLineHeight() + spacing;
        const float cardHeight = HistoryCardHeight() + spacing;
        const auto rowTop = [&](size_t k)
        {
            const size_t acks = ackPrefix[k];
            return static_cast<float>(acks) * ackHeight + static_cast<float>(k - acks) * cardHeight;
        };

        const size_t count = rows.size();
        const float baseY = ImGui::GetCursorPosY();
        const float viewTop = ImGui::GetScrollY() - baseY;
        const float viewBottom = viewTop + ImGui::GetWindowHeight();

        // First row whose bottom edge is below the top of the view.
        size_t first = 0, high = count;
        while (first < high)
        {
            const size_t middle = first + (high - first) / 2;
            if (rowTop(middle + 1) <= viewTop)
                first = middle + 1;
            else
                high = middle;
        }

        const ImVec4 accent = ImGui::GetStyleColorVec4(ImGuiCol_Header);
        ImGui::SetCursorPosY(baseY + rowTop(first));
        for (size_t k = first; k < count && rowTop(k) < viewBottom; ++k)
            DrawHistoryRow(gHistory[rows[k]], accent);

        // Reserve the full content height so the scrollbar reflects the whole (filtered) session.
        ImGui::SetCursorPosY(baseY + rowTop(count) - spacing);
        ImGui::Dummy({0.0f, 0.0f});

        if (gScrollToLatest)
        {
            // Only follow when the newest packet is actually shown under the current filter.
            if (rows.back() + 1 == gHistory.size())
                ImGui::SetScrollHereY(1.0f);
            gScrollToLatest = false;
        }
    }

    bool MatchesNewPacketFilters(const PacketDescriptor& descriptor)
    {
        if (descriptor.family != gNewPacketFamily)
            return false;
        const mdr::MDRDataType wantedType =
            gNewPacketTable == 0 ? mdr::MDRDataType::DATA_MDR : mdr::MDRDataType::DATA_MDR_NO2;
        if (descriptor.dataType != wantedType)
            return false;
        return ContainsAllTokens(descriptor.name, gTypeFilter.data());
    }

    void DrawNewPacketPicker()
    {
        if (ImGui::Button(TextLabel(i18n::TextId::DebuggerCommand).c_str()))
            ImGui::OpenPopup("New Packet");
        if (!ImGui::BeginPopup("New Packet"))
            return;

        int family = gNewPacketFamily == ProtocolFamily::V1 ? 0 : 1;
        ImGui::RadioButton("V1", &family, 0);
        ImGui::SameLine();
        ImGui::RadioButton("V2", &family, 1);
        gNewPacketFamily = family == 0 ? ProtocolFamily::V1 : ProtocolFamily::V2;
        ImGui::RadioButton(TextLabel(i18n::TextId::DebuggerTable1).c_str(), &gNewPacketTable, 0);
        ImGui::SameLine();
        ImGui::RadioButton(TextLabel(i18n::TextId::DebuggerTable2).c_str(), &gNewPacketTable, 1);
        ImGui::InputTextWithHint("##type-filter", Tr(i18n::TextId::DebuggerFilterTypes), gTypeFilter.data(),
                                 gTypeFilter.size());
        ImGui::BeginChild("##packet-types", {600.0f, 320.0f}, ImGuiChildFlags_Borders);
        ForEachDescriptor(
            [&](const PacketDescriptor& descriptor)
            {
                if (MatchesNewPacketFilters(descriptor) && ImGui::Selectable(descriptor.name))
                {
                    CreatePacket(descriptor);
                    ImGui::CloseCurrentPopup();
                }
            });
        ImGui::EndChild();
        ImGui::EndPopup();
    }

    void DrawCandidatePicker()
    {
        if (gSelectedCandidates.size() <= 1)
            return;
        const char* preview = gPacket ? gPacket.Descriptor().name : Tr(i18n::TextId::DebuggerChooseLayout);
        if (ImGui::BeginCombo(TextLabel(i18n::TextId::DebuggerPacketLayout).c_str(), preview))
        {
            CapturedPacket* capture = FindCapture(gSelectedCaptureId);
            for (const PacketDescriptor* descriptor : gSelectedCandidates)
            {
                const bool selected = gPacket && descriptor == &gPacket.Descriptor();
                if (ImGui::Selectable(descriptor->name, selected) && capture)
                    DecodeIntoEditor(*descriptor, capture->payload);
            }
            ImGui::EndCombo();
        }
    }

    bool UpdateEncodedPayload()
    {
        if (!gPacket)
        {
            gEncodedPayload.clear();
            return false;
        }
        gEncodedPayload.resize(mdr::kMDRMaxPacketSize);
        const auto encoded =
            gPacket.Descriptor().encode(gPacket.Value(), gEncodedPayload.data(), gEncodedPayload.size());
        if (!encoded)
        {
            gEncodedPayload.clear();
            gStatus = encoded.errMessage ? encoded.errMessage : mdrResultString(encoded.error);
            return false;
        }
        gEncodedPayload.resize(encoded.value);
        return true;
    }

    bool SwitchToCommandDescriptor(mdr::UInt8 command, const mdr::MDRBuffer& previousPayload)
    {
        const PacketDescriptor& current = gPacket.Descriptor();
        if (command == current.command)
            return false;

        const PacketDescriptor* replacement{};
        ForEachDescriptor(
            [&](const PacketDescriptor& descriptor)
            {
                if (!replacement && descriptor.family == current.family &&
                    SameProtocolTable(descriptor.dataType, current.dataType) && descriptor.command == command)
                {
                    replacement = &descriptor;
                }
            });

        if (replacement)
        {
            CreatePacket(*replacement);
            gStatus = mdr::Format(fmt::runtime(Tr(i18n::TextId::DebuggerSwitchedLayout)), replacement->name);
            return true;
        }

        PacketInstance restored{current};
        if (previousPayload.empty() || !current.decode(restored.Value(), previousPayload))
            restored = PacketInstance{current};
        gPacket = std::move(restored);
        gStatus = mdr::Format(fmt::runtime(Tr(i18n::TextId::DebuggerNoMatchingLayout)), command);
        return false;
    }

    void ExportPacket(const mdr::MDRBuffer& frame)
    {
        RequestPacketExport(frame, gSanitizeExports);
    }

    void DrawPacketEditor()
    {
        DrawCandidatePicker();
        if (!gPacket)
        {
            if (const CapturedPacket* capture = FindCapture(gSelectedCaptureId))
            {
                ImGui::SeparatorText(Tr(i18n::TextId::DebuggerUnknownPacket));
                if (capture->unpacked)
                    DrawHex(i18n::TextId::DebuggerEncodedPayload, capture->payload);
                else
                    ImGui::TextColored({1.0f, 0.45f, 0.35f, 1.0f}, "%s", Tr(i18n::TextId::DebuggerUnpackFailed));
                DrawHex(i18n::TextId::DebuggerPackedFrame, capture->frame);
                return;
            }

            const char* placeholder = Tr(i18n::TextId::DebuggerEditorEmpty);
            const ImVec2 textSize = ImGui::CalcTextSize(placeholder);
            ImGui::SetCursorPosX(std::max(0.0f, (ImGui::GetContentRegionAvail().x - textSize.x) * 0.5f));
            ImGui::TextDisabled("%s", placeholder);
            return;
        }

        ImGui::SeparatorText(gPacket.Descriptor().name);
        UpdateEncodedPayload();
        const mdr::MDRBuffer previousPayload = gEncodedPayload;
        const bool changed = gPacket.Descriptor().draw(gPacket.Value());
        if (changed && UpdateEncodedPayload() && !gEncodedPayload.empty() &&
            SwitchToCommandDescriptor(gEncodedPayload.front(), previousPayload))
        {
            return;
        }

        const auto validation = gPacket.Descriptor().validate(gPacket.Value());
        if (validation)
        {
            ImGui::TextColored({0.3f, 0.9f, 0.4f, 1.0f}, "%s", Tr(i18n::TextId::DebuggerValidationValid));
        }
        else
        {
            ImGui::TextColored({1.0f, 0.45f, 0.35f, 1.0f}, Tr(i18n::TextId::DebuggerValidationError),
                               validation.errMessage ? validation.errMessage : mdrResultString(validation.error));
        }

        const bool encoded = UpdateEncodedPayload();
        if (encoded)
        {
            DrawHex(i18n::TextId::DebuggerEncodedPayload, gEncodedPayload);
            if (!gHeadphones)
            {
                const mdr::MDRBuffer frame = mdr::MDRPackCommand(gEnvelopeType, gEnvelopeSequence, gEncodedPayload);
                DrawHex(i18n::TextId::DebuggerPackedFrame, frame);
            }
            else if (const CapturedPacket* capture = FindCapture(gSelectedCaptureId))
            {
                DrawHex(i18n::TextId::DebuggerCapturedFrame, capture->frame);
            }
        }

        if (!gStatus.empty())
            ImGui::TextWrapped("%s", gStatus.c_str());
    }

    void ResetPacket()
    {
        if (!gPacket)
            return;
        if (!gOriginalPayload.empty())
            DecodeIntoEditor(gPacket.Descriptor(), gOriginalPayload);
        else
            CreatePacket(gPacket.Descriptor());
    }

    void SendPacket()
    {
        const MDRResult result =
            mdrDebugRequestCommand(gHeadphones, gEncodedPayload.data(), static_cast<uint32_t>(gEncodedPayload.size()),
                                   static_cast<uint32_t>(gEnvelopeType), gAwaitAck ? MDR_TRUE : MDR_FALSE);
        gStatus = result == MDR_RESULT_OK ? Tr(i18n::TextId::DebuggerPacketQueued) : mdrResultString(result);
    }

    float ButtonWidth(const char* label)
    {
        return ImGui::CalcTextSize(label, nullptr, true).x + ImGui::GetStyle().FramePadding.x * 2.0f;
    }

    void NextToolbarItem(float width, bool alignRight = false)
    {
        const float rightEdge = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x;
        const float nextX = ImGui::GetItemRectMax().x - ImGui::GetWindowPos().x + ImGui::GetStyle().ItemSpacing.x;
        if (nextX + width <= rightEdge)
            ImGui::SameLine(alignRight ? rightEdge - width : nextX);
        else if (alignRight)
            ImGui::SetCursorPosX(std::max(ImGui::GetCursorPosX(), rightEdge - width));
    }

    std::array<float, 7> ComposerItemWidths()
    {
        const ImGuiStyle& style = ImGui::GetStyle();
        const float scalarWidth = ImGui::CalcTextSize("255").x + style.FramePadding.x * 2.0f;
        const float sequenceWidth = gHeadphones ? ButtonWidth(Tr(i18n::TextId::Auto)) : scalarWidth;
        return {ButtonWidth(Tr(i18n::TextId::DebuggerCommand)),
                ButtonWidth(Tr(i18n::TextId::DebuggerReset)),
                scalarWidth + ImGui::GetFrameHeight() * 2.0f + style.ItemInnerSpacing.x * 3.0f +
                    ImGui::CalcTextSize(Tr(i18n::TextId::DebuggerType)).x,
                sequenceWidth + style.ItemInnerSpacing.x + ImGui::CalcTextSize(Tr(i18n::TextId::DebuggerSequence)).x,
                ImGui::GetFrameHeight() + style.ItemInnerSpacing.x + ImGui::CalcTextSize("ACK").x,
                ButtonWidth(Tr(gHeadphones ? i18n::TextId::DebuggerExportCaptured : i18n::TextId::DebuggerExport)),
                ButtonWidth(Tr(i18n::TextId::DebuggerSend))};
    }

    float ComposerHeight(float width)
    {
        const float availableWidth = std::max(1.0f, width - 20.0f);
        float rowWidth = 0.0f;
        float height = ImGui::GetFrameHeight() + 18.0f;
        for (const float itemWidth : ComposerItemWidths())
        {
            if (rowWidth > 0.0f && rowWidth + itemWidth > availableWidth)
            {
                rowWidth = 0.0f;
                height += ImGui::GetFrameHeightWithSpacing();
            }
            rowWidth += itemWidth + ImGui::GetStyle().ItemSpacing.x;
        }
        return height;
    }

    void DrawComposer(float height)
    {
        const auto widths = ComposerItemWidths();
        ImVec4 composerColor = ImGui::GetStyleColorVec4(ImGuiCol_CheckMark);
        composerColor.w = 0.13f;
        ImGui::PushStyleColor(ImGuiCol_ChildBg, composerColor);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ImGui::GetStyle().FrameRounding);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {10.0f, 9.0f});
        if (ImGui::BeginChild("##command-composer", {0.0f, height}, ImGuiChildFlags_AlwaysUseWindowPadding,
                              ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            DrawNewPacketPicker();
            NextToolbarItem(widths[1]);

            ImGui::BeginDisabled(!gPacket);
            if (ImGui::Button(TextLabel(i18n::TextId::DebuggerReset).c_str()))
                ResetPacket();
            NextToolbarItem(widths[2]);

            int type = static_cast<int>(gEnvelopeType);
            const float labelSpacing = ImGui::GetStyle().ItemInnerSpacing.x;
            ImGui::SetNextItemWidth(widths[2] - labelSpacing - ImGui::CalcTextSize(Tr(i18n::TextId::DebuggerType)).x);
            if (ImGui::InputInt(TextLabel(i18n::TextId::DebuggerType).c_str(), &type))
                gEnvelopeType = static_cast<mdr::MDRDataType>(type);
            NextToolbarItem(widths[3]);
            ImGui::SetNextItemWidth(widths[3] - labelSpacing -
                                    ImGui::CalcTextSize(Tr(i18n::TextId::DebuggerSequence)).x);
            if (gHeadphones)
                ImGui::LabelText(TextLabel(i18n::TextId::DebuggerSequence).c_str(), "%s", Tr(i18n::TextId::Auto));
            else
                DrawScalar(TextLabel(i18n::TextId::DebuggerSequence).c_str(), gEnvelopeSequence);
            NextToolbarItem(widths[4]);
            ImGui::Checkbox("ACK", &gAwaitAck);
            NextToolbarItem(widths[5]);

            const bool encoded = UpdateEncodedPayload();
            const CapturedPacket* capture = FindCapture(gSelectedCaptureId);
            const bool canExport = gHeadphones ? capture != nullptr : encoded;
            ImGui::BeginDisabled(!canExport || clientDebuggerExportInProgress());
            if (ImGui::Button(
                    TextLabel(gHeadphones ? i18n::TextId::DebuggerExportCaptured : i18n::TextId::DebuggerExport)
                        .c_str()))
            {
                if (gHeadphones)
                    ExportPacket(capture->frame);
                else
                    ExportPacket(mdr::MDRPackCommand(gEnvelopeType, gEnvelopeSequence, gEncodedPayload));
            }
            ImGui::EndDisabled();

            const bool canSend = encoded && gHeadphones && mdrHeadphonesIsReady(gHeadphones);
            NextToolbarItem(widths[6], true);
            ImGui::BeginDisabled(!canSend);
            if (ImGui::Button(TextLabel(i18n::TextId::DebuggerSend).c_str()))
                SendPacket();
            ImGui::EndDisabled();
            ImGui::EndDisabled();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();
    }

    void ClearHistory()
    {
        gHistory.clear();
        InvalidateHistoryView();
        gReplayHistory = false;
        if (gSelectedCaptureId)
            gPacket.Reset();
        gSelectedCaptureId = 0;
        gSelectedCandidates.clear();
        gOriginalPayload.clear();
    }

    void AppendPacket(MDRPacketDirection direction, uint64_t packetId, const unsigned char* frame, int frameSize)
    {
        if (!packetId || !frame || frameSize <= 0)
            return;

        CapturedPacket packet;
        packet.id = packetId;
        packet.direction = direction;
        packet.frame.assign(frame, frame + frameSize);
        packet.unpacked = mdr::MDRUnpackCommand(packet.frame, packet.payload, packet.type, packet.sequence) ==
            mdr::MDRUnpackResult::OK;
        FindCandidates(packet);
        gHistory.push_back(std::move(packet));
        if (gFollowMessages)
        {
            gScrollToLatest = true;
            if (!IsAck(gHistory.back()))
                SelectCapture(gHistory.back());
        }
    }

    const char* DumpCommandName(const CapturedPacket& packet)
    {
        if (IsAck(packet))
            return "ACK";
        if (packet.candidates.empty())
            return "NO_COMMAND";
        return ShortPacketName(packet.candidates.front()->name);
    }

    bool WriteCaptureFile(const char* path, mdr::Span<const mdr::UInt8> frame)
    {
        SDL_IOStream* output = SDL_IOFromFile(path, "wb");
        if (!output)
            return false;
        const size_t written = SDL_WriteIO(output, frame.data(), frame.size());
        const bool closed = SDL_CloseIO(output);
        return written == frame.size() && closed;
    }

    void AppendUInt16LE(mdr::MDRBuffer& output, std::uint16_t value)
    {
        output.push_back(static_cast<mdr::UInt8>(value));
        output.push_back(static_cast<mdr::UInt8>(value >> 8));
    }

    void AppendUInt32LE(mdr::MDRBuffer& output, std::uint32_t value)
    {
        for (unsigned int shift = 0; shift < 32; shift += 8)
            output.push_back(static_cast<mdr::UInt8>(value >> shift));
    }

    void AppendBytes(mdr::MDRBuffer& output, std::string_view value)
    {
        output.insert(output.end(), value.begin(), value.end());
    }

    std::uint32_t Crc32(mdr::Span<const mdr::UInt8> data)
    {
        std::uint32_t crc = 0xffffffffu;
        for (const mdr::UInt8 byte : data)
        {
            crc ^= byte;
            for (int bit = 0; bit < 8; ++bit)
                crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1u)));
        }
        return ~crc;
    }

    struct ZipEntry
    {
        mdr::String filename;
        std::uint32_t crc{};
        std::uint32_t size{};
        std::uint32_t localHeaderOffset{};
    };

    mdr::String CaptureFilename(const CapturedPacket& packet, std::int64_t timestamp)
    {
        const char* directionName = packet.direction == MDR_PACKET_DIRECTION_RX ? "rx" : "tx";
        const mdr::UInt8 command = packet.payload.empty() ? 0 : packet.payload.front();
        return mdr::Format("mdr-packet-{}-{:06}-{}.type-{}-{:02x}.seq-{:02x}.cmd-{}-{:02x}.bin", timestamp, packet.id,
                           directionName, packet.unpacked ? mdr::format_as(packet.type) : "INVALID",
                           static_cast<mdr::UInt8>(packet.type), packet.sequence, DumpCommandName(packet), command);
    }

    bool BuildHistoryZip(std::int64_t timestamp, bool sanitize, mdr::MDRBuffer& archive)
    {
        archive.clear();
        CaptureScrubber scrubber;
        mdr::Vector<ZipEntry> entries;
        entries.reserve(gHistory.size());
        for (const CapturedPacket& packet : gHistory)
        {
            mdr::MDRBuffer sanitized;
            if (sanitize && !scrubber.Frame(packet.frame, sanitized))
                return SanitizationFailed();
            const mdr::Span<const mdr::UInt8> frame = sanitize ? sanitized : packet.frame;
            ZipEntry& entry = entries.emplace_back();
            entry.filename = CaptureFilename(packet, timestamp);
            entry.crc = Crc32(frame);
            entry.size = static_cast<std::uint32_t>(frame.size());
            entry.localHeaderOffset = static_cast<std::uint32_t>(archive.size());

            AppendUInt32LE(archive, 0x04034b50u);
            AppendUInt16LE(archive, 20);
            AppendUInt16LE(archive, 0);
            AppendUInt16LE(archive, 0);
            AppendUInt16LE(archive, 0);
            AppendUInt16LE(archive, 0);
            AppendUInt32LE(archive, entry.crc);
            AppendUInt32LE(archive, entry.size);
            AppendUInt32LE(archive, entry.size);
            AppendUInt16LE(archive, static_cast<std::uint16_t>(entry.filename.size()));
            AppendUInt16LE(archive, 0);
            AppendBytes(archive, entry.filename);
            archive.insert(archive.end(), frame.begin(), frame.end());
        }

        const std::uint32_t centralDirectoryOffset = static_cast<std::uint32_t>(archive.size());
        for (const ZipEntry& entry : entries)
        {
            AppendUInt32LE(archive, 0x02014b50u);
            AppendUInt16LE(archive, 20);
            AppendUInt16LE(archive, 20);
            AppendUInt16LE(archive, 0);
            AppendUInt16LE(archive, 0);
            AppendUInt16LE(archive, 0);
            AppendUInt16LE(archive, 0);
            AppendUInt32LE(archive, entry.crc);
            AppendUInt32LE(archive, entry.size);
            AppendUInt32LE(archive, entry.size);
            AppendUInt16LE(archive, static_cast<std::uint16_t>(entry.filename.size()));
            AppendUInt16LE(archive, 0);
            AppendUInt16LE(archive, 0);
            AppendUInt16LE(archive, 0);
            AppendUInt16LE(archive, 0);
            AppendUInt32LE(archive, 0);
            AppendUInt32LE(archive, entry.localHeaderOffset);
            AppendBytes(archive, entry.filename);
        }

        const std::uint32_t centralDirectorySize = static_cast<std::uint32_t>(archive.size()) - centralDirectoryOffset;
        AppendUInt32LE(archive, 0x06054b50u);
        AppendUInt16LE(archive, 0);
        AppendUInt16LE(archive, 0);
        AppendUInt16LE(archive, static_cast<std::uint16_t>(entries.size()));
        AppendUInt16LE(archive, static_cast<std::uint16_t>(entries.size()));
        AppendUInt32LE(archive, centralDirectorySize);
        AppendUInt32LE(archive, centralDirectoryOffset);
        AppendUInt16LE(archive, 0);
        return true;
    }

    bool ExportTimestamp(std::int64_t& timestamp)
    {
        SDL_Time now{};
        if (!SDL_GetCurrentTime(&now))
            return false;
        timestamp = now / 1'000'000;
        return true;
    }

    bool RequestPacketCollectionExport(bool sanitize)
    {
        if (gHistory.empty())
        {
            gExportStatus = gStatus = Tr(i18n::TextId::DebuggerNoPackets);
            return false;
        }
        std::int64_t timestamp{};
        if (!ExportTimestamp(timestamp))
        {
            gExportStatus = gStatus =
                mdr::Format(fmt::runtime(Tr(i18n::TextId::DebuggerTimestampFailed)), SDL_GetError());
            return false;
        }
        mdr::MDRBuffer archive;
        if (!BuildHistoryZip(timestamp, sanitize, archive))
            return false;
        const mdr::String defaultPath = mdr::Format("mdr-debugger-packets-{}.zip", timestamp);
        return RequestExport(archive, defaultPath.c_str(), "zip");
    }

    struct ReplayCapture
    {
        mdr::String filename;
        mdr::String path;
        MDRPacketDirection direction{};
        mdr::MDRBuffer frame;
    };

    bool ReplayDirection(std::string_view filename, MDRPacketDirection& direction)
    {
        if (filename.find("-rx.") != std::string_view::npos)
        {
            direction = MDR_PACKET_DIRECTION_RX;
            return true;
        }
        if (filename.find("-tx.") != std::string_view::npos)
        {
            direction = MDR_PACKET_DIRECTION_TX;
            return true;
        }
        return false;
    }

    std::string_view ReplayFilename(std::string_view path)
    {
        const size_t separator = path.find_last_of("/\\");
        return separator == std::string_view::npos ? path : path.substr(separator + 1);
    }

    bool IsBinPath(std::string_view path)
    {
        constexpr std::string_view extension = ".bin";
        if (path.size() < extension.size())
            return false;
        const std::string_view suffix = path.substr(path.size() - extension.size());
        return std::equal(
            suffix.begin(), suffix.end(), extension.begin(), [](char lhs, char rhs)
            { return std::tolower(static_cast<unsigned char>(lhs)) == std::tolower(static_cast<unsigned char>(rhs)); });
    }

    void DrawHistoryToolbar(bool* open, bool replayMode)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted(Tr(gReplayHistory ? i18n::TextId::PacketReplayMode : i18n::TextId::DebuggerPackets));
        const mdr::String followLabel =
            mdr::Format("{} {}###follow-messages", PSI_SIGNAL,
                        Tr(gFollowMessages ? i18n::TextId::DebuggerFollowOn : i18n::TextId::DebuggerFollowOff));
        NextToolbarItem(ButtonWidth(followLabel.c_str()));
        if (ImGui::Button(followLabel.c_str()))
            gFollowMessages = !gFollowMessages;

        const mdr::String sanitizeLabel =
            mdr::Format("{}###sanitize-exports",
                        Tr(gSanitizeExports ? i18n::TextId::DebuggerSanitizeOn : i18n::TextId::DebuggerSanitizeOff));

        const mdr::String dumpLabel = TextLabel(i18n::TextId::ExportZip, PSI_SAVE);
        const mdr::String clearLabel = TextLabel(i18n::TextId::DebuggerClear, PSI_TRASH);
        const mdr::String closeLabel =
            TextLabel(replayMode ? i18n::TextId::DebuggerQuitReplay : i18n::TextId::DebuggerClose);
        const float buttonSpacing = ImGui::GetStyle().ItemSpacing.x;
        const float rightButtons = ButtonWidth(sanitizeLabel.c_str()) + buttonSpacing + ButtonWidth(dumpLabel.c_str()) +
            buttonSpacing + ButtonWidth(clearLabel.c_str()) +
            (open ? buttonSpacing + ButtonWidth(closeLabel.c_str()) : 0.0f);
        const float statusAvailable = ImGui::GetCursorScreenPos().x + ImGui::GetContentRegionAvail().x -
            ImGui::GetItemRectMax().x - rightButtons - buttonSpacing * 2.0f;
        const float statusWidth = gExportStatus.empty()
            ? 0.0f
            : std::min(ImGui::CalcTextSize(gExportStatus.c_str()).x, std::max(0.0f, statusAvailable));
        const float toolbarWidth = rightButtons + (statusWidth > 0.0f ? statusWidth + buttonSpacing : 0.0f);
        NextToolbarItem(std::min(toolbarWidth, ImGui::GetContentRegionAvail().x), true);
        if (statusWidth > 0.0f)
        {
            const ImVec2 position = ImGui::GetCursorScreenPos();
            const float frameHeight = ImGui::GetFrameHeight();
            const ImVec4 clipRect(position.x, position.y, position.x + statusWidth, position.y + frameHeight);
            ImGui::GetWindowDrawList()->AddText(
                ImGui::GetFont(), ImGui::GetFontSize(), {position.x, position.y + ImGui::GetStyle().FramePadding.y},
                ImGui::GetColorU32(ImGuiCol_TextDisabled), gExportStatus.c_str(), nullptr, 0.0f, &clipRect);
            ImGui::Dummy({statusWidth, frameHeight});
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip("%s", gExportStatus.c_str());
            ImGui::SameLine();
        }
        ImGui::BeginDisabled(clientDebuggerExportInProgress());
        if (ImGui::Button(sanitizeLabel.c_str()))
            gSanitizeExports = !gSanitizeExports;
        ImGui::EndDisabled();
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
            ImGui::SetTooltip("%s", Tr(i18n::TextId::DebuggerSanitizeExportsHint));
        NextToolbarItem(ButtonWidth(dumpLabel.c_str()));
        ImGui::BeginDisabled(gHistory.empty() || clientDebuggerExportInProgress());
        if (ImGui::Button(dumpLabel.c_str()))
            RequestPacketCollectionExport(gSanitizeExports);
        ImGui::EndDisabled();
        NextToolbarItem(ButtonWidth(clearLabel.c_str()));
        if (ImGui::Button(clearLabel.c_str()))
            ClearHistory();
        if (open)
        {
            NextToolbarItem(ButtonWidth(closeLabel.c_str()));
            ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(200, 0, 0, 255));
            if (ImGui::Button(closeLabel.c_str()))
            {
                *open = false;
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor();
        }
    }
} // namespace

bool clientDebuggerWritePacketFile(const char* path, const unsigned char* frame, size_t frameSize)
{
    if (!path || !*path || !frame || frameSize == 0)
        return SDL_SetError("%s", Tr(i18n::TextId::DebuggerInvalidExportArguments));
    mdr::MDRBuffer sanitized;
    CaptureScrubber scrubber;
    if (!scrubber.Frame({frame, frameSize}, sanitized))
        return SanitizationFailed();
    return WriteCaptureFile(path, sanitized);
}

void clientDebuggerSetMonospaceFont(ImFont* font)
{
    gMonospaceFont = font;
}

void clientDebuggerSetWindow(SDL_Window* window)
{
    gWindow = window;
}

void clientDebuggerAttach(MDRHeadphones* headphones, MDRProtocolVersion protocolVersion)
{
    ClearHistory();
    gHeadphones = headphones;
    gProtocolVersion = protocolVersion;
}

void clientDebuggerDetach()
{
    gHeadphones = nullptr;
    gProtocolVersion = {};
}

void clientDebuggerObservePacket(MDRPacketDirection direction, uint64_t packetId, const unsigned char* frame,
                                 int frameSize)
{
    if (!packetId || !frame || frameSize <= 0)
        return;
    if (gReplayHistory)
        ClearHistory();
    AppendPacket(direction, packetId, frame, frameSize);
}

bool clientDebuggerReplayPath(const char* path, size_t* packetCount)
{
    if (packetCount)
        *packetCount = 0;
    if (!path || !*path)
        return SDL_SetError("%s", Tr(i18n::TextId::DebuggerReplayPathEmpty));

    SDL_PathInfo pathInfo{};
    if (!SDL_GetPathInfo(path, &pathInfo))
        return false;
    if (pathInfo.type == SDL_PATHTYPE_DIRECTORY)
        return clientDebuggerReplayDirectory(path, packetCount);
    if (pathInfo.type != SDL_PATHTYPE_FILE)
        return SDL_SetError(Tr(i18n::TextId::DebuggerReplayNotFileOrFolder), path);
    if (!IsBinPath(path))
        return SDL_SetError(Tr(i18n::TextId::DebuggerReplayNotBin), path);

    size_t frameSize{};
    void* frame = SDL_LoadFile(path, &frameSize);
    if (!frame)
        return false;
    if (frameSize == 0 || frameSize > static_cast<size_t>(std::numeric_limits<int>::max()))
    {
        SDL_free(frame);
        return SDL_SetError(Tr(i18n::TextId::DebuggerInvalidPacketSize), path);
    }

    MDRPacketDirection direction = MDR_PACKET_DIRECTION_RX;
    ReplayDirection(ReplayFilename(path), direction);
    ClearHistory();
    gReplayHistory = true;
    MDRHeadphones* attachedHeadphones = gHeadphones;
    gHeadphones = nullptr;
    AppendPacket(direction, 1, static_cast<const unsigned char*>(frame), static_cast<int>(frameSize));
    gHeadphones = attachedHeadphones;
    SDL_free(frame);

    if (packetCount)
        *packetCount = 1;
    gStatus = mdr::Format(fmt::runtime(Tr(i18n::TextId::DebuggerReplayed)), 1, path);
    return true;
}

bool clientDebuggerHasPackets()
{
    return !gHistory.empty();
}

bool clientDebuggerExportInProgress()
{
    return gExportDialogActive;
}

bool clientDebuggerExportLatestPacket()
{
    if (gHistory.empty())
    {
        gExportStatus = gStatus = Tr(i18n::TextId::DebuggerNoPackets);
        return false;
    }
    return RequestPacketExport(gHistory.back().frame, true);
}

bool clientDebuggerExportPacketCollection()
{
    return RequestPacketCollectionExport(true);
}

bool clientDebuggerWritePacketCollectionFile(const char* path)
{
    if (!path || !*path)
        return SDL_SetError("%s", Tr(i18n::TextId::DebuggerExportPathEmpty));
    if (gHistory.empty())
        return SDL_SetError("%s", Tr(i18n::TextId::DebuggerNoPackets));
    std::int64_t timestamp{};
    if (!ExportTimestamp(timestamp))
        return false;
    mdr::MDRBuffer archive;
    if (!BuildHistoryZip(timestamp, true, archive))
        return false;
    return WriteCaptureFile(path, archive);
}

void clientDebuggerClearExportStatus()
{
    PollExportResult();
    gExportStatus.clear();
}

const char* clientDebuggerGetExportStatus()
{
    PollExportResult();
    return gExportStatus.c_str();
}

bool clientDebuggerReplayDirectory(const char* directory, size_t* packetCount)
{
    if (packetCount)
        *packetCount = 0;
    if (!directory || !*directory)
        return SDL_SetError("%s", Tr(i18n::TextId::DebuggerReplayFolderEmpty));

    SDL_PathInfo directoryInfo{};
    if (!SDL_GetPathInfo(directory, &directoryInfo))
        return false;
    if (directoryInfo.type != SDL_PATHTYPE_DIRECTORY)
        return SDL_SetError(Tr(i18n::TextId::DebuggerReplayNotFolder), directory);

    int entryCount{};
    char** entries = SDL_GlobDirectory(directory, "mdr-packet-*.bin", 0, &entryCount);
    if (!entries)
        return false;

    mdr::Vector<ReplayCapture> captures;
    captures.reserve(static_cast<size_t>(entryCount));
    for (int index = 0; index < entryCount; ++index)
    {
        MDRPacketDirection direction{};
        if (!ReplayDirection(entries[index], direction))
            continue;
        ReplayCapture& capture = captures.emplace_back();
        capture.filename = entries[index];
        capture.path = mdr::Format("{}/{}", directory, entries[index]);
        capture.direction = direction;
    }
    SDL_free(entries);

    std::sort(captures.begin(), captures.end(),
              [](const ReplayCapture& lhs, const ReplayCapture& rhs) { return lhs.filename < rhs.filename; });
    if (captures.empty())
        return SDL_SetError(Tr(i18n::TextId::DebuggerReplayNoFiles), directory);

    for (ReplayCapture& capture : captures)
    {
        size_t frameSize{};
        void* frame = SDL_LoadFile(capture.path.c_str(), &frameSize);
        if (!frame)
            return false;
        if (frameSize == 0 || frameSize > static_cast<size_t>(std::numeric_limits<int>::max()))
        {
            SDL_free(frame);
            return SDL_SetError(Tr(i18n::TextId::DebuggerInvalidPacketSize), capture.path.c_str());
        }
        const auto* bytes = static_cast<const mdr::UInt8*>(frame);
        capture.frame.assign(bytes, bytes + frameSize);
        SDL_free(frame);
    }

    ClearHistory();
    gReplayHistory = true;
    MDRHeadphones* attachedHeadphones = gHeadphones;
    gHeadphones = nullptr;
    uint64_t replayPacketId = 0;
    for (const ReplayCapture& capture : captures)
    {
        AppendPacket(capture.direction, ++replayPacketId, capture.frame.data(), static_cast<int>(capture.frame.size()));
    }
    gHeadphones = attachedHeadphones;
    if (packetCount)
        *packetCount = captures.size();
    gStatus = mdr::Format(fmt::runtime(Tr(i18n::TextId::DebuggerReplayed)), captures.size(), directory);
    return true;
}

void clientDebuggerDraw(bool* open, bool replayMode)
{
    PollExportResult();
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 workSize = viewport->WorkSize;
    if (!ImGui::IsPopupOpen("Debugger"))
    {
        gSanitizeExports = true;
        ImGui::OpenPopup("Debugger");
    }
    ImGui::SetNextWindowSize({workSize.x, workSize.y * 0.8f}, ImGuiCond_Appearing);
    ImGui::SetNextWindowSizeConstraints({workSize.x, 0.0f}, workSize);
    ImGui::SetNextWindowPos(viewport->GetWorkCenter(), ImGuiCond_Always, {0.5f, 0.5f});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {20.0f, 16.0f});
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {8.0f, 8.0f});
    if (!ImGui::BeginPopupModal("Debugger", open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove))
    {
        ImGui::PopStyleVar(2);
        return;
    }

    if (open && ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        *open = false;
        ImGui::CloseCurrentPopup();
    }

    DrawHistoryToolbar(open, replayMode);
    ImGui::Separator();

    const float composerHeight = ComposerHeight(ImGui::GetContentRegionAvail().x);
    const float panelsHeight =
        std::max(1.0f, ImGui::GetContentRegionAvail().y - composerHeight - ImGui::GetStyle().ItemSpacing.y);
    const float panelHeight = std::max(1.0f, panelsHeight - ImGui::GetStyle().CellPadding.y * 2.0f);
    if (ImGui::BeginTable("##debugger-panels", 2,
                          ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV |
                              ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_NoHostExtendY,
                          {0.0f, panelsHeight}))
    {
        ImGui::TableSetupColumn(Tr(i18n::TextId::DebuggerPackets), ImGuiTableColumnFlags_WidthStretch, 0.42f);
        ImGui::TableSetupColumn(Tr(i18n::TextId::Controls), ImGuiTableColumnFlags_WidthStretch, 0.58f);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        if (ImGui::BeginChild("##history-panel", {0.0f, panelHeight}, ImGuiChildFlags_None,
                              ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
            ImGui::SetNextItemWidth(-1.0f);
            const mdr::String filterHint = mdr::Format("{} {}", PSI_FILTER, Tr(i18n::TextId::DebuggerFilterPackets));
            ImGui::InputTextWithHint("##history-filter", filterHint.c_str(), gHistoryFilter.data(),
                                     gHistoryFilter.size());
            ImGui::Separator();
            if (ImGui::BeginChild("##command-history", {0.0f, 0.0f}))
                DrawHistory();
            ImGui::EndChild();
        }
        ImGui::EndChild();

        ImGui::TableSetColumnIndex(1);
        if (ImGui::BeginChild("##packet-editor", {0.0f, panelHeight}, ImGuiChildFlags_Borders))
            DrawPacketEditor();
        ImGui::EndChild();
        ImGui::EndTable();
    }
    DrawComposer(composerHeight);
    ImGui::EndPopup();
    ImGui::PopStyleVar(2);
}
