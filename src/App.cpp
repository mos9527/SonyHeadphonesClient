#include "App.h"

#include "imgui_internal.h"

bool App::OnUpdate()
{
    bool shouldUpdate = true;
    if (_headphones)
    {
        // Polling & state updates are only done on the main thread
        HeadphonesEvent event{};
        try
        {
            event = _headphones->poll();
        } catch (RecoverableException& exc) {
            _logs.push_back(exc.what());
            if (_headphones && exc.shouldDisconnect)
                _headphones.reset();
            else
                throw exc;
            return true;
        }
        switch (event)
        {
        case HeadphonesEvent::NoChange:
        case HeadphonesEvent::NoMessage:
        case HeadphonesEvent::MessageUnhandled:
            shouldUpdate = false;
            break;        
        case HeadphonesEvent::Initialized:
            this->_logs.push_back("Initialized: " + this->_connectedDevice.name);
            break;
#ifdef _DEBUG
        case HeadphonesEvent::JSONMessage:
            _logs.push_back(_headphones->deviceMessages.current);
            // JSON data sent by the headphones after issuing MISC_DATA_GET with arguments {0x00,0x00}
            // Not very useful for now, disabled unless debugging
            break;
#endif
        case HeadphonesEvent::InteractionUpdate:
            _handleHeadphoneInteraction(_headphones->interactionMessage.current);
            break;
        case HeadphonesEvent::PlaybackMetadataUpdate:        
            _logs.push_back("Now Playing: " + _headphones->playback.title);
            break;        
        }
        // TODO: Properly handle requests with queues
        if (_headphones->_requestFuture.ready()) {
            // Request status updates at fixed intervals
            // This updates battery levels, sound pressure, etc.
            // Most of the other headphone states are updated automatically (see previous poll() call)
            if (ImGui::GetTime() - _lastStateSyncTime >= _config.headphoneStateSyncInterval) {
                _lastStateSyncTime = ImGui::GetTime();
                _headphones->_requestFuture.get();
                _headphones->_requestFuture.setFromAsync([this]() {
                    if (this->_headphones)
                        this->_headphones->requestSync();
                });
            }
            // Handle shutdown requests
            else if (_requestShutdown)
            {
                _headphones->_requestFuture.get();
                _headphones->_requestFuture.setFromAsync([this]() {
                    _headphones->requestPowerOff();
                    // Device should now disconnect and sever the connection
                    // poll() will raise an exception and properly let the owner
                    // destruct ourselves.
                });
            }
            else if (_requestPlaybackControl != THMSGV2T1::PlaybackControl::KEY_OFF)
            {
                auto control = _requestPlaybackControl;
                _headphones->_requestFuture.get();
                _headphones->_requestFuture.setFromAsync([this, control]() {
                    _headphones->requestPlaybackControl(control);
                });
                _requestPlaybackControl = THMSGV2T1::PlaybackControl::KEY_OFF;
            }
            /*else if (_headphones->_requestFixedMessageMessageType != static_cast<THMSGV2T1::AlertMessageType>(0xFF)
                && _headphones->_requestFixedMessageAlertAction != static_cast<THMSGV2T1::AlertAction>(0xFF))
            {
                auto messageType = _headphones->_requestFixedMessageMessageType;
                auto action = _headphones->_requestFixedMessageAlertAction;
                _headphones->_requestFuture.get();
                _headphones->_requestFuture.setFromAsync([this, messageType, action]() -> void
                {
                    _headphones->respondToFixedMessageAlert(messageType, action);
                });
                _headphones->_requestFixedMessageMessageType = static_cast<THMSGV2T1::AlertMessageType>(0xFF);
                _headphones->_requestFixedMessageAlertAction = static_cast<THMSGV2T1::AlertAction>(0xFF);
            }*/
        }
    }
    else {
        shouldUpdate = false;
    }
    return shouldUpdate;
}

bool App::OnFrame()
{
    ImGui::SetNextWindowPos({ 0,0 });
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    // Disable any interaction if the headphones are shutting down
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, _requestShutdown);
    {
        static bool open = true;
        ImGui::Begin("Sony Headphones", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
        //Legal disclaimer
        if (_config.showDisclaimers) {
            ImGui::Separator();
            ImGui::Text("! This product is not affiliated with Sony. Use at your own risk. !");
            ImGui::Text("Source (original) : https://github.com/Plutoberth/SonyHeadphonesClient");
            ImGui::Text("Source (this fork): https://github.com/mos9527/SonyHeadphonesClient");
            ImGui::Separator();
        }        
        _drawConfig();
        try {            
            if (_headphones)
            {                
                _drawControls();
                _drawModalAlerts();
                _setHeadphoneSettings();
            }
            else {
                // Nothing connected yet
                _drawDeviceDiscovery();            
            }
        }
        catch (RecoverableException& exc) {
            _logs.push_back(exc.what());
            if (_headphones && exc.shouldDisconnect)
                _headphones.reset();
            else
                throw exc;
        }              
        ImGui::End();
    }
    ImGui::PopItemFlag();
    return true;
}

void App::_drawDeviceDiscovery()
{
    if (ImGui::CollapsingHeader("Device Discovery", ImGuiTreeNodeFlags_DefaultOpen))
    {
        static std::vector<BluetoothDevice> connectedDevices;
        static int selectedDevice = -1;
        auto request_connect = [&](int index) {
            _connectedDevice = connectedDevices[index];
            _connectFuture.setFromAsync([this]() {
                try
                {
                    this->_bt.connect(this->_connectedDevice.mac);
                    if (this->_bt.isConnected()) {
                        this->_headphones = std::make_unique<Headphones>(this->_bt);
                        if (this->_headphones->_requestFuture.valid())
                            this->_headphones->_requestFuture.get();
                        this->_headphones->_requestFuture.setFromAsync([&]() {
                            this->_headphones->requestInit();                            
                        });
                    } else {
                        throw RecoverableException("Failed to connect", true);
                    }
                } catch (RecoverableException& exc)
                {
                    this->_logs.push_back(exc.what());
                }

            });
        };
        ImGui::Text("Select from one of the available devices: ");

        int temp = 0;
        for (const auto& device : connectedDevices)
        {
            ImGui::RadioButton(device.name.c_str(), &selectedDevice, temp++);
        }

        ImGui::Spacing();

        if (_connectFuture.valid())
        {
            if (this->_connectFuture.ready())
            {
                _connectFuture.get();
            }
            else
            {
                ImGui::Text("Connecting %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
            }
        }
        else
        {
            if (ImGui::Button("Connect"))
            {
                if (selectedDevice != -1) request_connect(selectedDevice);
            }
        }

        ImGui::SameLine();

        if (_connectedDevicesFuture.valid())
        {
            if (_connectedDevicesFuture.ready())
            {
                connectedDevices = _connectedDevicesFuture.get();                
                if (!_autoConnectAttempted){
                    _autoConnectAttempted = true;
                    auto index = std::find_if(connectedDevices.begin(), connectedDevices.end(), [&](const BluetoothDevice& device) {
                        return device.mac == _config.autoConnectDeviceMac;
                    });
                    if (index != connectedDevices.end()){
                        _logs.push_back("Auto connecting " + index->name + " (" + index->mac + ")");
                        selectedDevice = index - connectedDevices.begin();
                        request_connect(selectedDevice);
                    }
                }
            }
            else
            {
                ImGui::Text("Discovering Devices %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
            }
        }
        else
        {            
            ImGui::SameLine();
            if (ImGui::Button("Refresh"))
            {
                selectedDevice = -1;
                _connectedDevicesFuture.setFromAsync([this]() { return this->_bt.getConnectedDevices(); });
            }
            if (connectedDevices.size()) {
                bool isAutoConnect = selectedDevice >= 0 && _config.autoConnectDeviceMac.length() && connectedDevices[selectedDevice].mac == _config.autoConnectDeviceMac;
                if (selectedDevice >= 0 && ImGui::Checkbox("Auto Connect On Startup", &isAutoConnect)) {
                    _config.autoConnectDeviceMac = isAutoConnect ? connectedDevices[selectedDevice].mac : "";                    
                }
            }
        }
    }    
}

void App::_drawControls()
{
    if (ImGui::CollapsingHeader(_connectedDevice.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        const ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable;        
        if (ImGui::BeginTable("##Stats", 2, flags)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                auto drawBatteryProgressBar = [](const Headphones::BatteryData& battery, const char* label) {
                    ImGui::Text("%s:", label); ImGui::SameLine();
                    std::string levelStr = std::to_string(battery.level) + "%";
                    THMSGV2T1::BatteryChargingStatus cs = battery.chargingStatus;
                    if (cs != THMSGV2T1::BatteryChargingStatus::NOT_CHARGING && cs != THMSGV2T1::BatteryChargingStatus::UNKNOWN) {
                        static const std::map<THMSGV2T1::BatteryChargingStatus, const char*> statusMap = {
                            { THMSGV2T1::BatteryChargingStatus::CHARGING, " (Charging)" },
                            { THMSGV2T1::BatteryChargingStatus::CHARGED, " (Charged)" }
                        };
                        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 0.8f, 0.0f, 1.0f));
                        ImGui::ProgressBar(battery.level / 100.0f, ImVec2(-1, 0), (levelStr + statusMap.at(cs)).c_str());
                        ImGui::PopStyleColor();
                    } else {
                        ImGui::ProgressBar(battery.level / 100.0f, ImVec2(-1, 0), levelStr.c_str());
                    }
                };
                if (_headphones->supports(MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_INDICATOR)
                    || _headphones->supports(MessageMdrV2FunctionType_Table1::BATTERY_LEVEL_WITH_THRESHOLD)) {
                    drawBatteryProgressBar(_headphones->statBatteryL.current, "Battery");
                }
                if (_headphones->supports(MessageMdrV2FunctionType_Table1::LEFT_RIGHT_BATTERY_LEVEL_INDICATOR)
                    || _headphones->supports(MessageMdrV2FunctionType_Table1::LR_BATTERY_LEVEL_WITH_THRESHOLD)) {
                    drawBatteryProgressBar(_headphones->statBatteryL.current, "L");
                    drawBatteryProgressBar(_headphones->statBatteryR.current, "R");
                }
                if (_headphones->supports(MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_INDICATOR)
                    || _headphones->supports(MessageMdrV2FunctionType_Table1::CRADLE_BATTERY_LEVEL_WITH_THRESHOLD)) {
                    drawBatteryProgressBar(_headphones->statBatteryCase.current, "Case");
                }
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::Text("Title:  %s", _headphones->playback.title.c_str());
                ImGui::Text("Album:  %s", _headphones->playback.album.c_str());
                ImGui::Text("Artist: %s", _headphones->playback.artist.c_str());
                if (_headphones->supportsSafeListening()) {
                    ImGui::Separator();

                    bool measuringSoundPressure = _headphones->safeListening.preview.current;

                    ImGui::AlignTextToFramePadding();
                    if (measuringSoundPressure && _headphones->playback.sndPressure != 0xFF) {
                        ImGui::Text("Sound Pressure: %d dB", _headphones->playback.sndPressure);
                    } else {
                        ImGui::Text("Sound Pressure: N/A");
                    }

                    ImGui::SameLine();
                    if (_headphones->safeListening.preview.current) {
                        if (ImGui::Button("Stop")) {
                            _headphones->safeListening.preview.desired = false;
                        }
                    } else {
                        if (ImGui::Button("Measure")) {
                            _headphones->safeListening.preview.desired = true;
                        }
                    }
                }
            }
            ImGui::EndTable();
        }
        if (ImGui::Button("Disconnect")) {
            throw RecoverableException("Requested Disconnect", true);
        }

        if (_headphones->supports(MessageMdrV2FunctionType_Table1::POWER_OFF)) {
            ImGui::SameLine();
            if (!_requestShutdown) {
                if (ImGui::Button("Power off"))
                    _requestShutdown = true;
            } else {
                // OnFrame should disable UI interactions
                ImGui::Button("...Please wait");
            }
        }
    }
    ImGui::SeparatorText("Controls");
    {
        if (ImGui::BeginTabBar("##Controls")) {
            if (ImGui::BeginTabItem("Playback")) {
                using enum THMSGV2T1::PlaybackControl;
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, _requestPlaybackControl != KEY_OFF);
                ImGui::SliderInt("Volume", &_headphones->volume.desired, 0, 30);
                if (ImGui::Button("Prev")) _requestPlaybackControl = TRACK_DOWN;
                ImGui::SameLine();
                if (_headphones->playPause.current == true /*playing*/) {
                    if (ImGui::Button("Pause")) _requestPlaybackControl = PAUSE;
                }
                else {
                    if (ImGui::Button("Play")) _requestPlaybackControl = PLAY;
                }
                ImGui::SameLine();
                if (ImGui::Button("Next")) _requestPlaybackControl = TRACK_UP;
                ImGui::EndTabItem();
                ImGui::PopItemFlag();
            }

            bool supportsNc = _headphones->supportsNc();
            bool supportsAsm = _headphones->supportsAsm();
            if ((supportsNc || supportsAsm) && ImGui::BeginTabItem("Ambient Sound")) {
                bool supportsAutoAsm = _headphones->supports(MessageMdrV2FunctionType_Table1::MODE_NC_ASM_NOISE_CANCELLING_DUAL_AMBIENT_SOUND_MODE_LEVEL_ADJUSTMENT_NOISE_ADAPTATION);

                if (supportsNc) {
                    if (ImGui::RadioButton("Noise Cancelling", _headphones->asmEnabled.desired && (!supportsAsm || _headphones->asmMode.desired == THMSGV2T1::NcAsmMode::NC))) {
                        _headphones->asmEnabled.desired = true;
                        _headphones->asmMode.desired = THMSGV2T1::NcAsmMode::NC;
                    }
                    ImGui::SameLine();
                }

                if (supportsAsm) {
                    if (ImGui::RadioButton("Ambient Sound", _headphones->asmEnabled.desired && (!supportsNc || _headphones->asmMode.desired == THMSGV2T1::NcAsmMode::ASM))) {
                        _headphones->asmEnabled.desired = true;
                        _headphones->asmMode.desired = THMSGV2T1::NcAsmMode::ASM;
                        if (_headphones->asmLevel.desired == 0)
                            _headphones->asmLevel.desired = 20;
                    }
                    ImGui::SameLine();
                }

                if (ImGui::RadioButton("Off", !_headphones->asmEnabled.desired)) {
                    _headphones->asmEnabled.desired = false;
                }

                ImGui::Separator();

                if (supportsAsm && _headphones->asmEnabled.current && (!supportsNc || _headphones->asmMode.current == THMSGV2T1::NcAsmMode::ASM)) {
                    ImGui::SliderInt("Ambient Strength", &_headphones->asmLevel.desired, 1, 20);
                    _headphones->changingAsmLevel.desired = ImGui::IsItemActive()
                        ? THMSGV2T1::ValueChangeStatus::UNDER_CHANGING : THMSGV2T1::ValueChangeStatus::CHANGED;
                    if (supportsAutoAsm) {
                        ImGui::Checkbox("Auto Ambient Sound", &_headphones->autoAsmEnabled.desired);

                        ImGui::BeginDisabled(!_headphones->autoAsmEnabled.current);
                        static const std::map<THMSGV2T1::NoiseAdaptiveSensitivity, const char*> NoiseAdaptiveSensitivity_STR = {
                            {THMSGV2T1::NoiseAdaptiveSensitivity::STANDARD, "Standard"},
                            {THMSGV2T1::NoiseAdaptiveSensitivity::HIGH, "High"},
                            {THMSGV2T1::NoiseAdaptiveSensitivity::LOW, "Low"},
                        };
                        auto it = NoiseAdaptiveSensitivity_STR.find(_headphones->autoAsmSensitivity.current);
                        const char* currentStr = it != NoiseAdaptiveSensitivity_STR.end() ? it->second : "Unknown";
                        if (ImGui::BeginCombo("Sensitivity", currentStr)) {
                            for (auto const& [k, v] : NoiseAdaptiveSensitivity_STR) {
                                bool is_selected = k == _headphones->autoAsmSensitivity.current;
                                if (ImGui::Selectable(v, is_selected))
                                    _headphones->autoAsmSensitivity.desired = k;
                                if (is_selected)
                                    ImGui::SetItemDefaultFocus();
                            }

                            ImGui::EndCombo();
                        }

                        ImGui::EndDisabled();
                    }

                    ImGui::Checkbox("Voice Passthrough", &_headphones->asmFoucsOnVoice.desired);
                }

                ImGui::EndTabItem();
            }

            if (_headphones->supports(MessageMdrV2FunctionType_Table1::SMART_TALKING_MODE_TYPE2)
                && ImGui::BeginTabItem("Speak to Chat")) {
                ImGui::Checkbox("Enabled", &_headphones->stcEnabled.desired);
                ImGui::BeginDisabled(!_headphones->stcEnabled.current);
                {
                    static const std::map<THMSGV2T1::DetectSensitivity, const char*> STC_SENSITIVITY_STR = {
                        { THMSGV2T1::DetectSensitivity::AUTO, "Auto" },
                        { THMSGV2T1::DetectSensitivity::HIGH, "High" },
                        { THMSGV2T1::DetectSensitivity::LOW, "Low" },
                    };
                    auto it = STC_SENSITIVITY_STR.find(_headphones->stcLevel.current);
                    const char* currentStr = it != STC_SENSITIVITY_STR.end() ? it->second : "Unknown";
                    if (ImGui::BeginCombo("Voice Detect Sensitivity", currentStr)) {
                        for (auto const& [k, v] : STC_SENSITIVITY_STR) {
                            bool is_selected = k == _headphones->stcLevel.current;
                            if (ImGui::Selectable(v, is_selected))
                                _headphones->stcLevel.desired = k;
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                }
                {
                    static const std::map<THMSGV2T1::ModeOutTime, const char*> STC_TIME_STR = {
                        { THMSGV2T1::ModeOutTime::FAST, "Short (~5s)" },
                        { THMSGV2T1::ModeOutTime::MID, "Standard (~15s)" },
                        { THMSGV2T1::ModeOutTime::SLOW, "Long (~30s)" },
                        { THMSGV2T1::ModeOutTime::NONE, "Don't end automatically" },
                    };
                    auto it = STC_TIME_STR.find(_headphones->stcTime.current);
                    const char* currentStr = it != STC_TIME_STR.end() ? it->second : "Unknown";
                    if (ImGui::BeginCombo("Mode Duration", currentStr)) {
                        for (auto const& [k, v] : STC_TIME_STR)
                        {
                            bool is_selected = k == _headphones->stcTime.current;
                            if (ImGui::Selectable(v, is_selected))
                                _headphones->stcTime.desired = k;
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                }
                ImGui::EndDisabled();
                ImGui::EndTabItem();
            }

            if (_headphones->supports(MessageMdrV2FunctionType_Table1::LISTENING_OPTION) &&
                ImGui::BeginTabItem("Listening Mode")) {
                ListeningMode effectiveMode = _headphones->listeningModeConfig.current.getEffectiveMode();

                // Standard
                bool radioChanged = false;
                if (ImGui::RadioButton("Standard", effectiveMode == ListeningMode::Standard)) {
                    radioChanged |= true;
                    effectiveMode = ListeningMode::Standard;
                }

                // BGM
                if (ImGui::RadioButton("BGM", effectiveMode == ListeningMode::BGM)) {
                    radioChanged |= true;
                    effectiveMode = ListeningMode::BGM;
                }
                ImGui::Indent();
                ImGui::BeginDisabled(!_headphones->listeningModeConfig.current.bgmActive);

                // Distance combo box
                static const std::map<THMSGV2T1::RoomSize, const char*> BGM_DISTANCE_MODE_STR = {
                    { THMSGV2T1::RoomSize::SMALL, "My Room" },
                    { THMSGV2T1::RoomSize::MIDDLE, "Living Room" },
                    { THMSGV2T1::RoomSize::LARGE, "Cafe" },
                };
                auto it = BGM_DISTANCE_MODE_STR.find(_headphones->listeningModeConfig.current.bgmDistanceMode);
                const char* currentStr = it != BGM_DISTANCE_MODE_STR.end() ? it->second : "Unknown";
                if (ImGui::BeginCombo("Distance", currentStr)) {
                    for (auto const& [k, v] : BGM_DISTANCE_MODE_STR) {
                        bool is_selected = k == _headphones->listeningModeConfig.current.bgmDistanceMode;
                        if (ImGui::Selectable(v, is_selected))
                            _headphones->listeningModeConfig.desired.bgmDistanceMode = k;
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                ImGui::EndDisabled();
                ImGui::Unindent();

                // Cinema
                if (ImGui::RadioButton("Cinema", effectiveMode == ListeningMode::Cinema)) {
                    radioChanged |= true;
                    effectiveMode = ListeningMode::Cinema;
                }

                if (radioChanged) {
                    _headphones->listeningModeConfig.desired.bgmActive = effectiveMode == ListeningMode::BGM;
                    if (!_headphones->listeningModeConfig.desired.bgmActive) {
                        _headphones->listeningModeConfig.desired.nonBgmMode = effectiveMode;
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Equalizer")) {
                bool eqAvailable = (_headphones->deviceCapabilities & DC_EqualizerAvailableCommand) == 0 || _headphones->eqAvailable.current;
                if (!eqAvailable && _headphones->supports(MessageMdrV2FunctionType_Table1::LISTENING_OPTION)) {
                    ImGui::PushTextWrapPos(0.0f);
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Please set Listening Mode to Standard to use Equalizer.");
                    ImGui::PopTextWrapPos();
                }
                ImGui::BeginDisabled(!eqAvailable);
                static const std::map<THMSGV2T1::EqPresetId, const char*> EQ_PRESET_NAMES = {
                    { THMSGV2T1::EqPresetId::OFF, "Off" },
                    { THMSGV2T1::EqPresetId::ROCK, "Rock" },
                    { THMSGV2T1::EqPresetId::POP, "Pop" },
                    { THMSGV2T1::EqPresetId::JAZZ, "Jazz" },
                    { THMSGV2T1::EqPresetId::DANCE, "Dance" },
                    { THMSGV2T1::EqPresetId::EDM, "EDM" },
                    { THMSGV2T1::EqPresetId::R_AND_B_HIP_HOP, "R&B/Hip-Hop" },
                    { THMSGV2T1::EqPresetId::ACOUSTIC, "Acoustic" },
                    // 8-15 reserved for future use
                    { THMSGV2T1::EqPresetId::BRIGHT, "Bright" },
                    { THMSGV2T1::EqPresetId::EXCITED, "Excited" },
                    { THMSGV2T1::EqPresetId::MELLOW, "Mellow" },
                    { THMSGV2T1::EqPresetId::RELAXED, "Relaxed" },
                    { THMSGV2T1::EqPresetId::VOCAL, "Vocal" },
                    { THMSGV2T1::EqPresetId::TREBLE, "Treble" },
                    { THMSGV2T1::EqPresetId::BASS, "Bass" },
                    { THMSGV2T1::EqPresetId::SPEECH, "Speech" },
                    // 24-31 reserved for future use
                    { THMSGV2T1::EqPresetId::CUSTOM, "Custom" },
                    { THMSGV2T1::EqPresetId::USER_SETTING1, "User Setting 1" },
                    { THMSGV2T1::EqPresetId::USER_SETTING2, "User Setting 2" },
                    { THMSGV2T1::EqPresetId::USER_SETTING3, "User Setting 3" },
                    { THMSGV2T1::EqPresetId::USER_SETTING4, "User Setting 4" },
                    { THMSGV2T1::EqPresetId::USER_SETTING5, "User Setting 5" }
                };
                std::string presetName = "Unknown";
                auto it = EQ_PRESET_NAMES.find(_headphones->eqPreset.current);
                if (it != EQ_PRESET_NAMES.end())
                    presetName = it->second;
                if (ImGui::BeginCombo("Preset", presetName.c_str()))
                {
                    for (auto const& [k, v] : EQ_PRESET_NAMES)
                    {
                        bool is_selected = k == _headphones->eqPreset.current;
                        if (ImGui::Selectable(v, is_selected))
                            _headphones->eqPreset.desired = k;
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                const float width = ImGui::GetContentRegionAvail().x;
                const float padding = ImGui::GetStyle().ItemSpacing.x;
                auto eqSliders = [&](const char* const labels[], int count, int minValue, int maxValue) -> void {
                    float totalSpacing = (count - 1) * padding;
                    float columnWidth = (width - totalSpacing) / count;

                    for (int i = 0; i < count; i++) {
                        ImGui::BeginGroup();

                        ImGui::PushID(i);
                        ImGui::VSliderInt("##v", ImVec2(columnWidth, 160), &_headphones->eqConfig.desired.bands[i], minValue, maxValue);
                        ImGui::PopID();

                        float textWidth = ImGui::CalcTextSize(labels[i]).x;
                        float textOffset = (columnWidth - textWidth) * 0.5f;
                        if (textOffset > 0.0f)
                            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
                        ImGui::TextUnformatted(labels[i]);

                        ImGui::EndGroup();

                        if (i < count - 1)
                            ImGui::SameLine(0.0f, padding);
                    }
                };

                size_t numBands = _headphones->eqConfig.current.bands.size();
                if (numBands == 5) {
                    ImGui::SeparatorText("5-Band EQ");
                    static const char* const bandNames[] = { "400","1k","2.5k","6.3k","16k" };
                    eqSliders(bandNames, 5, -10, 10);
                    ImGui::SeparatorText("Clear Bass");
                    ImGui::SetNextItemWidth(width);
                    ImGui::SliderInt("##", &_headphones->eqConfig.desired.bassLevel, -10, 10);
                } else if (numBands == 10) {
                    ImGui::SeparatorText("10-Band EQ");
                    static const char* const bandNames[] = { "31","63","125","250","500","1k","2k","4k","8k","16k" };
                    eqSliders(bandNames, 10, -6, 6);
                } else {
                    ImGui::Text("Unknown EQ configuration");
                }

                ImGui::EndDisabled();
                ImGui::EndTabItem();
            }

            if (_headphones->supportsMultipoint() && ImGui::BeginTabItem("Devices")) {
                if (_headphones->supportsPairingDeviceManagement()) {
                    enum class DeviceEntryClickAction {
                        None,
                        DeviceEntry, // Switch playback for connected, attempt connection for paired
                        Disconnect,
                        Unpair,
                        Fix,
                    };
                    auto drawDevice = [](const BluetoothDevice& device, uint8_t connectedIndex, bool selected) -> DeviceEntryClickAction {
                        DeviceEntryClickAction clicked = DeviceEntryClickAction::None;
                        ImGui::PushID(&device);

                        bool notConnected = device.mac.empty();
                        ImGui::BeginDisabled(notConnected);

                        static const std::string NOT_CONNECTED = "Not connected";
                        const std::string& deviceName = notConnected ? NOT_CONNECTED
                            : !device.name.empty() ? device.name : device.mac;

                        if (!notConnected) {
                            ImGui::SetNextItemAllowOverlap();
                        }

                        bool selectableSelected;
                        if (connectedIndex > 0)
                            selectableSelected = ImGui::Selectable((std::to_string(connectedIndex) + ". " + deviceName).c_str(), selected);
                        else
                            selectableSelected = ImGui::Selectable(deviceName.c_str(), selected);
                        if (selectableSelected && clicked == DeviceEntryClickAction::None) {
                            clicked = DeviceEntryClickAction::DeviceEntry;
                        }

                        if (!notConnected && ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
                            ImGui::OpenPopup("DeviceOptions");
                        }

                        if (!notConnected) {
                            int buttonWidth = ImGui::CalcTextSize("...").x + 2 * ImGui::GetStyle().FramePadding.x;
                            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - buttonWidth);
                            if (ImGui::SmallButton("...")) {
                                ImGui::OpenPopup("DeviceOptions");
                            }

                            if (ImGui::BeginPopup("DeviceOptions")) {
                                if (connectedIndex != 0) {
                                    if (ImGui::MenuItem("Disconnect") && clicked == DeviceEntryClickAction::None) {
                                        clicked = DeviceEntryClickAction::Disconnect;
                                    }
                                }

                                bool selfDevice = false; // TODO later
                                if (!selfDevice) {
                                    if (ImGui::MenuItem("Unpair") && clicked == DeviceEntryClickAction::None) {
                                        clicked = DeviceEntryClickAction::Unpair;
                                    }
                                }

                                // Note: App mentions this as "Fix" which is not a suitable translation from Japanese word "固定".
                                //       "Pin" is more appropriate in this context.
                                /*if (connectedIndex != 0) {
                                    if (selected) {
                                        if (ImGui::MenuItem("Pin playback to this device") && clicked == DeviceEntryClickAction::None) {
                                            clicked = DeviceEntryClickAction::Fix;
                                        }
                                    } else {
                                        if (ImGui::MenuItem("Switch playback device and pin") && clicked == DeviceEntryClickAction::None) {
                                            clicked = DeviceEntryClickAction::Fix;
                                        }
                                    }
                                }*/ // TODO Implement

                                ImGui::EndPopup();
                            }
                        }

                        ImGui::EndDisabled();
                        ImGui::PopID();
                        return clicked;
                    };

                    if (ImGui::TreeNodeEx("Connected", ImGuiTreeNodeFlags_DefaultOpen)) {
                        for (const auto& [connectedIndex, device] : _headphones->connectedDevices) {
                            DeviceEntryClickAction clicked = drawDevice(device, connectedIndex, connectedIndex == _headphones->playbackDevice);
                            switch (clicked) {
                                case DeviceEntryClickAction::DeviceEntry: {
                                    _headphones->mpDeviceMac.desired = device.mac;
                                    break;
                                }
                                case DeviceEntryClickAction::Disconnect: {
                                    _headphones->disconnectDevice(device.mac);
                                    break;
                                }
                                case DeviceEntryClickAction::Unpair: {
                                    _headphones->unpairDevice(device.mac);
                                    break;
                                }
                                case DeviceEntryClickAction::Fix: {
                                    // TODO
                                    break;
                                }
                            }
                        }
                        ImGui::TreePop();
                    }

                    if (!_headphones->pairedDevices.empty() && ImGui::TreeNodeEx("Paired", ImGuiTreeNodeFlags_DefaultOpen)) {
                        for (const auto& [mac, device] : _headphones->pairedDevices) {
                            DeviceEntryClickAction clicked = drawDevice(device, 0, false);
                            switch (clicked) {
                                case DeviceEntryClickAction::DeviceEntry: {
                                    _headphones->connectDevice(device.mac);
                                    break;
                                }
                                case DeviceEntryClickAction::Unpair: {
                                    _headphones->unpairDevice(device.mac);
                                    break;
                                }
                            }
                        }
                        ImGui::TreePop();
                    }

                    // Connect to New Device
                    if (!_headphones->pairingMode.current) {
                        if (ImGui::Button("Connect to New Device")) {
                            _headphones->pairingMode.desired = true;
                        }
                    } else {
                        if (ImGui::Button("Stop")) {
                            _headphones->pairingMode.desired = false;
                        }
                        ImGui::SameLine();
                        ImGui::Text("Searching %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
                    }
                } else {
                    ImGui::PushTextWrapPos(0.0f);
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Please enable \"Connect to 2 devices simultaneously\" in System settings to manage devices.");
                    ImGui::PopTextWrapPos();
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("System")) {
                // General settings
                static const std::map<std::string, const char*> GS_SUBJECT_STRINGS = {
                    { "SIDETONE_SETTING", "Capture Voice During a Phone Call" },
                    { "MULTIPOINT_SETTING", "Connect to 2 devices simultaneously" },
                    { "TOUCH_PANEL_SETTING", "Touch sensor control panel" }
                };
                static const std::map<std::string, const char*> GS_SUMMARY_STRINGS = {
                    { "SIDETONE_SETTING_SUMMARY", "Your own voice will be easier to hear during calls.\nIf your voice sounds too loud or background noise is distracting, please turn off this feature." },
                    { "MULTIPOINT_SETTING_SUMMARY", "For example, when using the audio device with both a PC and a smartphone, you can use it comfortably without needing to switch connections. During simultaneous connections, playback with the LDAC codec is not possible even if Prioritize Sound Quality is selected." },
                    { "MULTIPOINT_SETTING_SUMMARY_LDAC_AVAILABLE", "For example, when using the audio device with both a PC and a smartphone, you can use it comfortably without needing to switch connections." },
                };
                auto conditionalDrawGeneralSetting = [&](const Headphones::GsCapability& gsc, Property<bool>& gsv, const char* fallbackSubject) -> void {
                    if (gsc.type == THMSGV2T1::GsSettingType::BOOLEAN_TYPE) {
                        const char* subjectString = fallbackSubject;
                        if (!gsc.info.subject.empty()) {
                            if (gsc.info.stringFormat == THMSGV2T1::GsStringFormat::ENUM_NAME) {
                                auto it = GS_SUBJECT_STRINGS.find(gsc.info.subject);
                                subjectString = it == GS_SUBJECT_STRINGS.end() ? gsc.info.subject.c_str() : it->second;
                            } else {
                                subjectString = gsc.info.subject.c_str();
                            }
                        }
                        ImGui::BeginDisabled(gsc.info.subject.empty());
                        ImGui::Checkbox(subjectString, &gsv.desired);
                        if (!gsc.info.summary.empty() && ImGui::IsItemHovered()) {
                            ImGui::BeginTooltip();
                            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
                            const char* summaryString;
                            if (gsc.info.stringFormat == THMSGV2T1::GsStringFormat::ENUM_NAME) {
                                auto it = GS_SUMMARY_STRINGS.find(gsc.info.summary);
                                summaryString = it == GS_SUMMARY_STRINGS.end() ? gsc.info.summary.c_str() : it->second;
                            } else {
                                summaryString = gsc.info.summary.c_str();
                            }
                            ImGui::TextUnformatted(summaryString);
                            ImGui::PopTextWrapPos();
                            ImGui::EndTooltip();
                        }
                        ImGui::EndDisabled();
                    }
                };

                if (_headphones->supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_1)) {
                    conditionalDrawGeneralSetting(_headphones->gs1c.current, _headphones->gs1, "General Setting 1");
                }
                if (_headphones->supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_2)) {
                    conditionalDrawGeneralSetting(_headphones->gs2c.current, _headphones->gs2, "General Setting 2");
                }
                if (_headphones->supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_3)) {
                    conditionalDrawGeneralSetting(_headphones->gs3c.current, _headphones->gs3, "General Setting 3");
                }
                if (_headphones->supports(MessageMdrV2FunctionType_Table1::GENERAL_SETTING_4)) {
                    conditionalDrawGeneralSetting(_headphones->gs4c.current, _headphones->gs4, "General Setting 4");
                }

                if (_headphones->supports(MessageMdrV2FunctionType_Table1::ASSIGNABLE_SETTING)
                    && ImGui::TreeNodeEx("Touch Sensor", ImGuiTreeNodeFlags_DefaultOpen)) {
                    static const std::map<THMSGV2T1::Preset, const char*> TOUCH_SENSOR_FUNCTION_STR = {
                        { THMSGV2T1::Preset::PLAYBACK_CONTROL, "Playback Control" },
                        { THMSGV2T1::Preset::AMBIENT_SOUND_CONTROL_QUICK_ACCESS, "Ambient Sound / Noise Cancelling" },
                        { THMSGV2T1::Preset::NO_FUNCTION, "Not Assigned "}
                    };

                    const auto draw_touch_sensor_combo = [&](auto& prop, const char* label) {
                        auto it = TOUCH_SENSOR_FUNCTION_STR.lower_bound({prop.desired});
                        if (it == TOUCH_SENSOR_FUNCTION_STR.end() || it->first != prop.desired)  {
                            ImGui::TextUnformatted(label);
                            ImGui::SameLine();
                            ImGui::Text("Unknown Function: %d", static_cast<int>(prop.desired));
                            return;
                        }
                        if (ImGui::BeginCombo(label, TOUCH_SENSOR_FUNCTION_STR.at(prop.desired))) {
                            for (auto& [k, v] : TOUCH_SENSOR_FUNCTION_STR) {
                                const bool is_selected = (prop.desired == k);
                                if (ImGui::Selectable(v, is_selected)) {
                                    prop.desired = k;
                                }
                                if (is_selected) {
                                    ImGui::SetItemDefaultFocus();
                                }
                            }
                            ImGui::EndCombo();
                        }
                    };
                    draw_touch_sensor_combo(_headphones->touchLeftFunc, "Left Touch Sensor");
                    draw_touch_sensor_combo(_headphones->touchRightFunc, "Right Touch Sensor");
                    ImGui::TreePop();
                }

                if (_headphones->supports(MessageMdrV2FunctionType_Table1::AMBIENT_SOUND_CONTROL_MODE_SELECT)
                    && ImGui::TreeNodeEx("[NC/AMB] Button Setting", ImGuiTreeNodeFlags_DefaultOpen)) {
                    bool ncActive;
                    bool ambActive;
                    bool offActive;
                    NcAmbButtonMode_ToStates(_headphones->ncAmbButtonMode.desired, &ncActive, &ambActive, &offActive);

                    bool changed = ImGui::Checkbox("Noise Cancelling", &ncActive); ImGui::SameLine();
                    changed |= ImGui::Checkbox("Ambient Sound", &ambActive); ImGui::SameLine();
                    changed |= ImGui::Checkbox("Off", &offActive);
                    if (changed) {
                        THMSGV2T1::Function mode;
                        if (NcAmbButtonMode_FromStates(ncActive, ambActive, offActive, &mode)) {
                            _headphones->ncAmbButtonMode.desired = mode;
                        }
                    }

                    ImGui::TreePop();
                }

                if (_headphones->supportsAutoPowerOff()) {
                    if (_headphones->supports(MessageMdrV2FunctionType_Table1::AUTO_POWER_OFF_WITH_WEARING_DETECTION)) {
                        ImGui::Checkbox("Automatic Power Off", &_headphones->autoPowerOffEnabled.desired);
                    }
                    // TODO This should be a combobox or select box not a checkbox.
                    //      A model may support configuring the auto off time.
                }

                if (_headphones->supports(MessageMdrV2FunctionType_Table1::PLAYBACK_CONTROL_BY_WEARING_REMOVING_HEADPHONE_ON_OFF)) {
                    ImGui::Checkbox("Pause when headphones are removed", &_headphones->autoPauseEnabled.desired);
                }

                if (ImGui::TreeNodeEx("Notification & Voice Guide", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::Checkbox("Enabled", &_headphones->voiceGuidanceEnabled.desired);
                    if (_headphones->supports(MessageMdrV2FunctionType_Table2::VOICE_GUIDANCE_SETTING_MTK_TRANSFER_WITHOUT_DISCONNECTION_SUPPORT_LANGUAGE_SWITCH_AND_VOLUME_ADJUSTMENT)) {
                        ImGui::SliderInt("Volume", &_headphones->miscVoiceGuidanceVol.desired, -2, 2);
                    }
                    ImGui::TreePop();
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("About")) {
                ImGui::Text("Model: %s", _headphones->modelName.current.c_str());
                ImGui::Text("MAC: %s", _headphones->uniqueId.current.c_str());
                ImGui::Text("Firmware Version: %s", _headphones->fwVersion.current.c_str());
                ImGui::Text("Protocol Version: 0x%08X", _headphones->protocolVersion);

                static const std::map<THMSGV2T1::ModelSeries, const char*> MODEL_SERIES_STR = {
                    { THMSGV2T1::ModelSeries::EXTRA_BASS, "Extra Bass" },
                    { THMSGV2T1::ModelSeries::ULT_POWER_SOUND, "ULT Power Sound" },
                    { THMSGV2T1::ModelSeries::HEAR, "Hear" },
                    { THMSGV2T1::ModelSeries::PREMIUM, "Premium" },
                    { THMSGV2T1::ModelSeries::SPORTS, "Sports" },
                    { THMSGV2T1::ModelSeries::CASUAL, "Casual" },
                    { THMSGV2T1::ModelSeries::LINK_BUDS, "LinkBuds" },
                    { THMSGV2T1::ModelSeries::NECKBAND, "Neckband" },
                    { THMSGV2T1::ModelSeries::LINKPOD, "LinkPod" },
                    { THMSGV2T1::ModelSeries::GAMING, "Gaming" },
                };
                auto seriesIt = MODEL_SERIES_STR.find(_headphones->modelSeries.current);
                ImGui::Text("Series: %s", seriesIt != MODEL_SERIES_STR.end() ? seriesIt->second : "Unknown");

                static const std::map<ModelColor, const char*> MODEL_COLOR_STR = {
                    { ModelColor::Default, "Default" },
                    { ModelColor::Black, "Black" },
                    { ModelColor::White, "White" },
                    { ModelColor::Silver, "Silver" },
                    { ModelColor::Red, "Red" },
                    { ModelColor::Blue, "Blue" },
                    { ModelColor::Pink, "Pink" },
                    { ModelColor::Yellow, "Yellow" },
                    { ModelColor::Green, "Green" },
                    { ModelColor::Gray, "Gray" },
                    { ModelColor::Gold, "Gold" },
                    { ModelColor::Cream, "Cream" },
                    { ModelColor::Orange, "Orange" },
                    { ModelColor::Brown, "Brown" },
                    { ModelColor::Violet, "Violet" },
                };
                auto colorIt = MODEL_COLOR_STR.find(_headphones->modelColor.current);
                ImGui::Text("Color: %s", colorIt != MODEL_COLOR_STR.end() ? colorIt->second : "Unknown");

                if (ImGui::TreeNode("Supported features (table 1)")) {
                    if (ImGui::Button("Copy")) {
                        ImGui::SetClipboardText(_headphones->supportFunctionString1.current.c_str());
                    }

                    ImGui::PushTextWrapPos(0.0f);
                    ImGui::TextUnformatted(_headphones->supportFunctionString1.current.c_str());
                    ImGui::PopTextWrapPos();

                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Supported features (table 2)")) {
                    if (ImGui::Button("Copy")) {
                        ImGui::SetClipboardText(_headphones->supportFunctionString2.current.c_str());
                    }

                    ImGui::PushTextWrapPos(0.0f);
                    ImGui::TextUnformatted(_headphones->supportFunctionString2.current.c_str());
                    ImGui::PopTextWrapPos();

                    ImGui::TreePop();
                }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
    }
}

void App::_drawModalAlerts()
{
    auto& modalAlerts = _headphones->modalAlerts;

    if (modalAlerts.empty())
        return;

    Headphones::ModalAlert& dlg = modalAlerts.back();

    if (dlg.open)
        ImGui::OpenPopup((dlg.title + dlg.id).c_str());

    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal((dlg.title + dlg.id).c_str(), nullptr,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        if (!dlg.message.empty())
        {
            ImGui::PushTextWrapPos(ImGui::GetMainViewport()->Size.x * 0.7f);
            ImGui::TextUnformatted(dlg.message.c_str());
            ImGui::PopTextWrapPos();
        }

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            dlg.onClose(true);
            dlg.open = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            dlg.onClose(false);
            dlg.open = false;
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    _headphones->postModalAlertHandling();
}

void App::_drawConfig()
{
    if (ImGui::CollapsingHeader("App Config")) {
        if (ImGui::TreeNode("Shell Command")) {
            ImGui::Text("NOTE: Headphones may send Events (displayed in the Messages section as Headphone Event: ...)");
            ImGui::Text("NOTE: You may bind them to shell commands here.");
            const ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
            auto& cmds = _config.headphoneInteractionShellCommands;
            if (ImGui::BeginTable("Commands", 3, flags)) {
                ImGui::TableSetupColumn("Event");
                ImGui::TableSetupColumn("Shell");
                ImGui::TableSetupColumn("Action");
                ImGui::TableHeadersRow();
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                for (
                    auto it = cmds.begin();it != cmds.end();it != cmds.end() ? it++ : cmds.end()) {
                    ImGui::TableNextRow();
                    ImGui::PushID(&it->first);
                    ImGui::TableSetColumnIndex(0);
                    ImGui::InputText("##Event", &it->first);
                    ImGui::PopID();

                    ImGui::PushID(&it->second);
                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##Shell", &it->second);
                    ImGui::TableSetColumnIndex(2);
                    if (ImGui::Button("Remove##")) {
                        it = cmds.erase(it);
                    }
                    ImGui::PopID();
                }
                ImGui::PopStyleColor();
                ImGui::EndTable();
            }
            if (ImGui::Button("Add Command")) {
                cmds.push_back({});
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("UI")) {
            ImGui::SeparatorText("Font");
            ImGui::SliderInt("Font Size", &_config.imguiFontSize, 10.0f, 64.0f);
            ImGui::InputText("Font File (full path)", &_config.imguiFontFile);
            ImGui::SeparatorText("Misc");
            ImGui::Checkbox("Show Disclaimers", &_config.showDisclaimers);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Messages")) {
            if (_logs.size()) {
                if (ImGui::BeginListBox("##Messages", ImVec2(-1, GUI_MESSAGE_BOX_SIZE * ImGui::GetTextLineHeightWithSpacing()))) {
                    for (auto const& message : _logs) {
                        ImGui::Text("%s", message.c_str());
                    }
                    if (_logs.size() > _prevMessageCnt)
                        _prevMessageCnt = _logs.size(), ImGui::SetScrollHereY();
                    ImGui::EndListBox();
                }
            }
            else {
                ImGui::Text("No messages yet.");
            }
            ImGui::TreePop();
        }
    }
}

void App::_setHeadphoneSettings() {      
    if (_headphones->_sendCommandFuture.ready())
    {        
        _headphones->_sendCommandFuture.get();
    }
    //This means that we're waiting
    else if (_headphones->_sendCommandFuture.valid())
    {
        ImGui::Text("Sending command %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
    }
    //We're not waiting, and there's no event in the air, so we can evaluate sending a new event
    else if (_headphones->isChanged())
    {
        _headphones->_sendCommandFuture.setFromAsync([=, this]() {
            return _headphones->setChanges();
        });
    }
}

void App::_handleHeadphoneInteraction(std::string const& event)
{
    _logs.push_back("Headphone Event: " + event);
    auto& cmds = _config.headphoneInteractionShellCommands;
    auto cmd = std::find_if(cmds.begin(),cmds.end(),[&](auto& p) { return p.first == event; });
    if (cmd != cmds.end()){
        if (int ret = system(cmd->second.c_str()); ret != 0) {
            _logs.push_back("Command " + cmd->second + " returned " + std::to_string(ret));
        }
    }
}

App::App(BluetoothWrapper&& bt, AppConfig& config) : _config(config), _bt(std::move(bt)),  _connectFuture("connect"), _connectedDevicesFuture("connected devices")
{     
    this->_connectedDevicesFuture.setFromAsync([this]() { return this->_bt.getConnectedDevices(); });
}

///
bool AppConfig::load(std::string const& configPath)
{
    std::cout << "Loading config:" << configPath << std::endl;
    std::ifstream file(configPath);
    if (!file.is_open())
        return false;

    toml::table table = toml::parse(file);
    showDisclaimers = table["showDisclaimers"].value<bool>().value_or(true);

    imguiSettings = table["imguiSettings"].value<std::string>().value_or("");
    autoConnectDeviceMac = table["autoConnectDeviceMac"].value<std::string>().value_or("");
    imguiFontFile = table["imguiFontFile"].value<std::string>().value_or("");
    imguiFontSize = table["imguiFontSize"].value<float>().value_or(DEFAULT_FONT_SIZE);
    auto& [imguiWindowSizeWidth, imguiWindowSizeHeight] = imguiWindowSize;
	imguiWindowSizeWidth = table["imguiWindowSizeWidth"].value<int>().value_or(GUI_DEFAULT_WIDTH);
	imguiWindowSizeHeight = table["imguiWindowSizeHeight"].value<int>().value_or(GUI_DEFAULT_HEIGHT);
	headphoneStateSyncInterval = table["headphoneStateSyncInterval"].value<float>().value_or(1.0f);
    if (table["shellCommands"].as_table())
    {
        headphoneInteractionShellCommands.clear();
        for (auto& [k, v] : *table["shellCommands"].as_table()) {
            std::string ks{ k.str() };
            std::string vs = v.value<std::string>().value_or("");
            headphoneInteractionShellCommands.push_back({ks,vs});
        }
    }
    return true;
}

bool AppConfig::save(std::string const& configPath)
{
    std::cout << "Saving config:" << configPath << std::endl;
    std::ofstream file(configPath);
    if (!file.is_open())
        return false;

    toml::table table;
    table.insert("showDisclaimers", showDisclaimers);
    imguiSettings = ImGui::SaveIniSettingsToMemory();
    table.insert("imguiSettings", imguiSettings);
    table.insert("imguiFontSize", imguiFontSize);
    table.insert("shellCommands", toml::table{});
    for (auto& [k, v] : headphoneInteractionShellCommands) 
        table["shellCommands"].as_table()->insert(k.data(), v.data());	

    table.insert("imguiFontFile", imguiFontFile);
    table.insert("imguiWindowSizeWidth", imguiWindowSize.first);
    table.insert("imguiWindowSizeHeight", imguiWindowSize.second);
    table.insert("autoConnectDeviceMac", autoConnectDeviceMac);
    table.insert("headphoneStateSyncInterval", headphoneStateSyncInterval);    
    file << toml::toml_formatter{ table };
    return true;
}
