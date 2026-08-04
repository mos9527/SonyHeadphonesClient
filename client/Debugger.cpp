#include "Debugger.hpp"

#include "DebuggerDetails.hpp"
#include "Details.hpp"

#include "Fonts/PlexSansIcon.h"

#include <SDL3/SDL_iostream.h>
#include <imgui.h>
#include <mdr/Command.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <iterator>
#include <string_view>

namespace
{
    using namespace client::debugger;

    constexpr size_t kMaximumHistory = 512;

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
    mdr::Deque<CapturedPacket> gHistory;
    std::uint64_t gNextPacketId{};
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
    mdr::String gStatus;
    bool gFollowMessages{true};
    bool gScrollToLatest{};

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

    bool ContainsCaseInsensitive(std::string_view value, std::string_view query)
    {
        return std::ranges::search(value, query,
                                   [](char lhs, char rhs)
                                   {
                                       return std::tolower(static_cast<unsigned char>(lhs)) ==
                                           std::tolower(static_cast<unsigned char>(rhs));
                                   })
                   .begin() != value.end();
    }

    bool MatchesActiveProtocolFamily(const PacketDescriptor& descriptor)
    {
        if (!gHeadphones)
            return true;

        switch (mdr::detail::HeadphonesImpl(gHeadphones)->mProtocolFamily)
        {
        case mdr::MDRHeadphones::ProtocolFamily::V1:
            return descriptor.family == ProtocolFamily::V1;
        case mdr::MDRHeadphones::ProtocolFamily::V2:
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
                if (!MatchesActiveProtocolFamily(descriptor) ||
                    !SameProtocolTable(descriptor.dataType, packet.type) || descriptor.command != command)
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
        const auto found = std::ranges::find(gHistory, id, &CapturedPacket::id);
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
        if (gHeadphones)
        {
            gEnvelopeSequence = mdr::detail::HeadphonesImpl(gHeadphones)->CurrentSequenceNumber();
        }
        gStatus.clear();
    }

    void DrawHex(std::string_view label, mdr::Span<const mdr::UInt8> bytes)
    {
        if (!ImGui::TreeNodeEx(label.data(), ImGuiTreeNodeFlags_DefaultOpen, "%.*s [%zu]",
                               static_cast<int>(label.size()), label.data(), bytes.size()))
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
                const bool padding = index >= bytes.size();
                const mdr::UInt8 byte = padding ? 0 : bytes[index];
                const mdr::String byteText = mdr::Format("{:02X}", byte);
                const ImVec2 position{origin.x + (offsetColumns + column * 3) * characterWidth, origin.y};
                drawList->AddText(position, padding || byte == 0 ? mutedColor : textColor, byteText.c_str());
            }

            const float asciiStart = origin.x + (offsetColumns + hexColumns + 1) * characterWidth;
            drawList->AddText({asciiStart, origin.y}, mutedColor, "|");
            for (size_t column = 0; column < bytesPerLine; ++column)
            {
                const size_t index = offset + column;
                const bool padding = index >= bytes.size();
                const mdr::UInt8 byte = padding ? 0 : bytes[index];
                const bool printable = !padding && std::isprint(static_cast<unsigned char>(byte));
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

    bool IsAck(const CapturedPacket& packet) { return packet.unpacked && packet.type == mdr::MDRDataType::ACK; }

    void DrawHistory()
    {
        if (gHistory.empty())
        {
            constexpr const char* placeholder = "Commands from the headphones will appear here.";
            const ImVec2 textSize = ImGui::CalcTextSize(placeholder);
            ImGui::SetCursorPos({std::max(0.0f, (ImGui::GetContentRegionAvail().x - textSize.x) * 0.5f),
                                 std::max(0.0f, (ImGui::GetContentRegionAvail().y - textSize.y) * 0.5f)});
            ImGui::TextDisabled("%s", placeholder);
            return;
        }

        const ImVec4 accent = ImGui::GetStyleColorVec4(ImGuiCol_Header);
        for (auto iterator = gHistory.begin(); iterator != gHistory.end(); ++iterator)
        {
            CapturedPacket& packet = *iterator;
            const char* direction = packet.direction == MDR_PACKET_DIRECTION_RX ? "RX" : "TX";
            if (IsAck(packet))
            {
                const mdr::String ackLabel =
                    mdr::Format("#{}  {} ACK  seq {:02X}", packet.id, direction, packet.sequence);
                ImGui::BeginDisabled();
                ImGui::Text(ackLabel.c_str());
                ImGui::EndDisabled();
                if (gScrollToLatest && std::next(iterator) == gHistory.end())
                {
                    ImGui::SetScrollHereY(1.0f);
                    gScrollToLatest = false;
                }
                continue;
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
            const char* packetName =
                packet.candidates.empty() ? "Unknown packet" : ShortPacketName(packet.candidates.front()->name);
            mdr::String details = mdr::Format("type {:02X}  cmd {:02X}  seq {:02X}",
                                              static_cast<mdr::UInt8>(packet.type), command, packet.sequence);
            if (packet.candidates.size() > 1)
                details += mdr::Format("  {} layouts", packet.candidates.size());
            const mdr::String label = mdr::Format("#{} {} - {}\n{}", packet.id, direction, packetName, details);
            const float rowHeight = ImGui::GetTextLineHeightWithSpacing() + ImGui::GetStyle().FramePadding.y * 2;
            if (ImGui::Selectable(label.c_str(), selected, ImGuiSelectableFlags_None,
                                  {ImGui::GetContentRegionAvail().x, rowHeight}))
                SelectCapture(packet);
            ImGui::PopStyleColor(3);
            ImGui::PopID();
            ImGui::Dummy({0.0f, 3.0f});

            if (gScrollToLatest && std::next(iterator) == gHistory.end())
            {
                ImGui::SetScrollHereY(1.0f);
                gScrollToLatest = false;
            }
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
        return ContainsCaseInsensitive(descriptor.name, gTypeFilter.data());
    }

    void DrawNewPacketPicker()
    {
        if (ImGui::Button("Command..."))
            ImGui::OpenPopup("New Packet");
        if (!ImGui::BeginPopup("New Packet"))
            return;

        int family = gNewPacketFamily == ProtocolFamily::V1 ? 0 : 1;
        ImGui::RadioButton("V1", &family, 0);
        ImGui::SameLine();
        ImGui::RadioButton("V2", &family, 1);
        gNewPacketFamily = family == 0 ? ProtocolFamily::V1 : ProtocolFamily::V2;
        ImGui::RadioButton("Table 1", &gNewPacketTable, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Table 2", &gNewPacketTable, 1);
        ImGui::InputTextWithHint("##type-filter", "Filter packet types", gTypeFilter.data(), gTypeFilter.size());
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
        const char* preview = gPacket ? gPacket.Descriptor().name : "Choose a layout";
        if (ImGui::BeginCombo("Packet layout", preview))
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
            gStatus = mdr::Format("Switched to {}", replacement->name);
            return true;
        }

        PacketInstance restored{current};
        if (previousPayload.empty() || !current.decode(restored.Value(), previousPayload))
            restored = PacketInstance{current};
        gPacket = std::move(restored);
        gStatus = mdr::Format("No packet layout matches command {:02X}", command);
        return false;
    }

    void ExportPacket(const mdr::MDRBuffer& frame)
    {
        constexpr const char* path = "mdr-debugger-packet.bin";
        SDL_IOStream* output = SDL_IOFromFile(path, "wb");
        if (!output)
        {
            gStatus = SDL_GetError();
            return;
        }
        const size_t written = SDL_WriteIO(output, frame.data(), frame.size());
        const bool closed = SDL_CloseIO(output);
        if (written != frame.size() || !closed)
            gStatus = SDL_GetError();
        else
            gStatus = mdr::Format("Exported {}", path);
    }

    void DrawPacketEditor()
    {
        DrawCandidatePicker();
        if (!gPacket)
        {
            if (const CapturedPacket* capture = FindCapture(gSelectedCaptureId))
            {
                ImGui::SeparatorText("Unknown packet");
                if (capture->unpacked)
                    DrawHex("Encoded payload", capture->payload);
                else
                    ImGui::TextColored({1.0f, 0.45f, 0.35f, 1.0f}, "The captured frame could not be unpacked.");
                DrawHex("Packed frame", capture->frame);
                return;
            }

            constexpr const char* placeholder = "Select a command above or create a new packet below.";
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
            ImGui::TextColored({0.3f, 0.9f, 0.4f, 1.0f}, "Validation: valid");
        }
        else
        {
            ImGui::TextColored({1.0f, 0.45f, 0.35f, 1.0f}, "Validation: %s",
                               validation.errMessage ? validation.errMessage : mdrResultString(validation.error));
        }

        const bool encoded = UpdateEncodedPayload();
        if (encoded)
        {
            DrawHex("Encoded payload", gEncodedPayload);
            const mdr::MDRBuffer frame = mdr::MDRPackCommand(gEnvelopeType, gEnvelopeSequence, gEncodedPayload);
            DrawHex("Packed frame", frame);
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
        auto* engine = mdr::detail::HeadphonesImpl(gHeadphones);
        gEnvelopeSequence = engine->CurrentSequenceNumber();
        const int result =
            engine->Invoke(engine->RequestDebugCommand(gEncodedPayload, gEnvelopeType, gEnvelopeSequence, gAwaitAck));
        gStatus = result == MDR_RESULT_OK ? "Packet queued" : mdrResultString(result);
    }

    void DrawComposer()
    {
        ImVec4 composerColor = ImGui::GetStyleColorVec4(ImGuiCol_CheckMark);
        composerColor.w = 0.13f;
        ImGui::PushStyleColor(ImGuiCol_ChildBg, composerColor);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, ImGui::GetStyle().FrameRounding);
        if (ImGui::BeginChild("##command-composer", {-1.0f, ImGui::GetFrameHeightWithSpacing() + 18.0f},
                              ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar))
        {
            ImGui::SetCursorPos({10.0f, 9.0f});
            DrawNewPacketPicker();
            ImGui::SameLine();

            ImGui::BeginDisabled(!gPacket);
            if (ImGui::Button("Reset"))
                ResetPacket();
            ImGui::SameLine();

            int type = static_cast<int>(gEnvelopeType);
            ImGui::SetNextItemWidth(70.0f);
            if (ImGui::InputInt("Type", &type))
                gEnvelopeType = static_cast<mdr::MDRDataType>(type);
            ImGui::SameLine();
            if (gHeadphones)
                gEnvelopeSequence = mdr::detail::HeadphonesImpl(gHeadphones)->CurrentSequenceNumber();
            ImGui::SetNextItemWidth(55.0f);
            ImGui::BeginDisabled(gHeadphones != nullptr);
            DrawScalar("Seq", gEnvelopeSequence);
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::Checkbox("ACK", &gAwaitAck);
            ImGui::SameLine();

            const bool encoded = UpdateEncodedPayload();
            ImGui::BeginDisabled(!encoded);
            if (ImGui::Button("Export"))
            {
                ExportPacket(mdr::MDRPackCommand(gEnvelopeType, gEnvelopeSequence, gEncodedPayload));
            }
            ImGui::EndDisabled();

            const bool canSend = encoded && gHeadphones && mdrHeadphonesIsReady(gHeadphones);
            const float sendWidth = ImGui::CalcTextSize("Send").x + ImGui::GetStyle().FramePadding.x * 2.0f;
            ImGui::SameLine(std::max(ImGui::GetCursorPosX(), ImGui::GetWindowWidth() - sendWidth - 12.0f));
            ImGui::BeginDisabled(!canSend);
            if (ImGui::Button("Send"))
                SendPacket();
            ImGui::EndDisabled();
            ImGui::EndDisabled();
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    void ClearHistory()
    {
        gHistory.clear();
        if (gSelectedCaptureId)
            gPacket.Reset();
        gSelectedCaptureId = 0;
        gSelectedCandidates.clear();
        gOriginalPayload.clear();
    }

    void DrawHistoryToolbar(bool* open)
    {
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Packets");
        ImGui::SameLine();
        if (ImGui::Button(gFollowMessages ? "Auto-follow: On" : "Auto-follow: Off"))
            gFollowMessages = !gFollowMessages;

        const float clearWidth =
            ImGui::CalcTextSize(PSI_TRASH " Clear history").x + ImGui::GetStyle().FramePadding.x * 2.0f;
        const float closeWidth = ImGui::CalcTextSize("Close").x + ImGui::GetStyle().FramePadding.x * 2.0f;
        const float buttonSpacing = ImGui::GetStyle().ItemSpacing.x;
        const float rightEdge = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x;
        ImGui::SameLine(std::max(ImGui::GetCursorPosX(), rightEdge - clearWidth - closeWidth - buttonSpacing));
        if (ImGui::Button("Clear history"))
            ClearHistory();
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(200, 0, 0, 255));
        if (ImGui::Button("Close"))
        {
            *open = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor();
    }
} // namespace

void clientDebuggerSetMonospaceFont(ImFont* font) { gMonospaceFont = font; }

void clientDebuggerAttach(MDRHeadphones* headphones) { gHeadphones = headphones; }

void clientDebuggerDetach() { gHeadphones = nullptr; }

void clientDebuggerObservePacket(MDRPacketDirection direction, const unsigned char* frame, int frameSize)
{
    if (!frame || frameSize <= 0)
        return;

    CapturedPacket packet;
    packet.id = ++gNextPacketId;
    packet.direction = direction;
    packet.frame.assign(frame, frame + frameSize);
    packet.unpacked =
        mdr::MDRUnpackCommand(packet.frame, packet.payload, packet.type, packet.sequence) == mdr::MDRUnpackResult::OK;
    FindCandidates(packet);
    gHistory.push_back(std::move(packet));
    if (gHistory.size() > kMaximumHistory)
    {
        if (gSelectedCaptureId == gHistory.front().id)
            gSelectedCaptureId = 0;
        gHistory.pop_front();
    }
    if (gFollowMessages)
    {
        gScrollToLatest = true;
        if (!IsAck(gHistory.back()))
            SelectCapture(gHistory.back());
    }
}

void clientDebuggerDraw(bool* open)
{
    const ImVec2 displaySize = ImGui::GetIO().DisplaySize;
    if (!ImGui::IsPopupOpen("Command Playground"))
        ImGui::OpenPopup("Command Playground");
    ImGui::SetNextWindowSize({displaySize.x, displaySize.y * 0.8f}, ImGuiCond_Appearing);
    ImGui::SetNextWindowPos({displaySize.x * 0.5f, displaySize.y * 0.5f}, ImGuiCond_Appearing, {0.5f, 0.5f});
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {20.0f, 16.0f});
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {8.0f, 8.0f});
    if (!ImGui::BeginPopupModal("Command Playground", open, ImGuiWindowFlags_NoTitleBar))
    {
        ImGui::PopStyleVar(2);
        return;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_Escape))
    {
        *open = false;
        ImGui::CloseCurrentPopup();
    }

    if (ImGui::BeginTable("##debugger-panels", 2,
                          ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV |
                              ImGuiTableFlags_SizingStretchProp))
    {
        ImGui::TableSetupColumn("Packets", ImGuiTableColumnFlags_WidthStretch, 0.42f);
        ImGui::TableSetupColumn("Controls", ImGuiTableColumnFlags_WidthStretch, 0.58f);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        DrawHistoryToolbar(open);
        ImGui::Separator();
        if (ImGui::BeginChild("##command-history", {-1.0f, -1.0f}))
            DrawHistory();
        ImGui::EndChild();

        ImGui::TableSetColumnIndex(1);
        const float composerHeight = ImGui::GetFrameHeightWithSpacing() + 18.0f;
        const float editorHeight = std::max(100.0f, ImGui::GetContentRegionAvail().y - composerHeight - 8.0f);
        if (ImGui::BeginChild("##packet-editor", {-1.0f, editorHeight}, ImGuiChildFlags_Borders))
            DrawPacketEditor();
        ImGui::EndChild();
        ImGui::Spacing();
        DrawComposer();
        ImGui::EndTable();
    }
    ImGui::EndPopup();
    ImGui::PopStyleVar(2);
}
