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
            } else if (_requestPlaybackControl != PLAYBACK_CONTROL::NONE)
            {
                auto control = _requestPlaybackControl;
                _headphones->_requestFuture.get();
                _headphones->_requestFuture.setFromAsync([this, control]() {
                    _headphones->requestPlaybackControl(control);
                });
                _requestPlaybackControl = PLAYBACK_CONTROL::NONE;
            }
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
                if ((_headphones->deviceCapabilities & DC_TrueWireless) != 0) {
                    ImGui::Text("L:"); ImGui::SameLine();
                    ImGui::ProgressBar(_headphones->statBatteryL.current / 100.0f);
                    ImGui::Text("R:"); ImGui::SameLine();
                    ImGui::ProgressBar(_headphones->statBatteryR.current / 100.0f);
                    ImGui::Text("Case:"); ImGui::SameLine();
                    ImGui::ProgressBar(_headphones->statBatteryCase.current / 100.0f);
                } else {
                    ImGui::Text("Battery:"); ImGui::SameLine();
                    ImGui::ProgressBar(_headphones->statBatteryL.current / 100.0f);
                }
            }
            ImGui::TableSetColumnIndex(1);
            {
                ImGui::Text("Title:  %s", _headphones->playback.title.c_str());
                ImGui::Text("Album:  %s", _headphones->playback.album.c_str());
                ImGui::Text("Artist: %s", _headphones->playback.artist.c_str());
                ImGui::Separator();
                ImGui::Text("Sound Pressure: %d", _headphones->playback.sndPressure);
            }
            ImGui::EndTable();
        }
        if (ImGui::Button("Disconnect")) {
            throw RecoverableException("Requested Disconnect", true);
        }
        ImGui::SameLine();
        if (!_requestShutdown)
        {
            if (ImGui::Button("Shutdown"))
                _requestShutdown = true;
        } else
        {
            // OnFrame should disable UI interactions
            ImGui::Button("...Please wait");
        }
    }	
    ImGui::SeparatorText("Controls");
    {
        if (ImGui::BeginTabBar("##Controls")) {
            if (ImGui::BeginTabItem("Playback Control")) {
                using enum PLAYBACK_CONTROL;
                ImGui::PushItemFlag(ImGuiItemFlags_Disabled, _requestPlaybackControl != NONE);
                ImGui::SliderInt("Volume", &_headphones->volume.desired, 0, 30);
                if (ImGui::Button("Prev")) _requestPlaybackControl = PREV;
                ImGui::SameLine();
                if (_headphones->playPause.current == true /*playing*/) {
                    if (ImGui::Button("Pause")) _requestPlaybackControl = PAUSE;
                }
                else {
                    if (ImGui::Button("Play")) _requestPlaybackControl = PLAY;
                }
                ImGui::SameLine();
                if (ImGui::Button("Next")) _requestPlaybackControl = NEXT;
                ImGui::EndTabItem();
                ImGui::PopItemFlag();
            }

            if (ImGui::BeginTabItem("AMB / ANC")) {
                bool supportsAutoAsm = (_headphones->deviceCapabilities & DC_AutoAsm) != 0;

                if (ImGui::RadioButton("Noise Cancelling", _headphones->asmEnabled.desired && _headphones->asmMode.desired == NC_ASM_SETTING_TYPE::NOISE_CANCELLING)) {
                    _headphones->asmEnabled.desired = true;
                    _headphones->asmMode.desired = NC_ASM_SETTING_TYPE::NOISE_CANCELLING;
                }

                ImGui::SameLine();
                if (ImGui::RadioButton("Ambient Sound", _headphones->asmEnabled.desired && _headphones->asmMode.desired == NC_ASM_SETTING_TYPE::AMBIENT_SOUND)) {
                    _headphones->asmEnabled.desired = true;
                    _headphones->asmMode.desired = NC_ASM_SETTING_TYPE::AMBIENT_SOUND;
                    if (_headphones->asmLevel.desired == 0)
                        _headphones->asmLevel.desired = 20;
                }

                ImGui::SameLine();
                if (ImGui::RadioButton("Off", !_headphones->asmEnabled.desired)) {
                    _headphones->asmEnabled.desired = false;
                }

                ImGui::Separator();

                if (_headphones->asmEnabled.current && _headphones->asmMode.current == NC_ASM_SETTING_TYPE::AMBIENT_SOUND) {
                    ImGui::SliderInt("Ambient Strength", &_headphones->asmLevel.desired, 1, 20);
                    _headphones->draggingAsmLevel = ImGui::IsItemActive();
                    if (ImGui::IsItemDeactivatedAfterEdit()) {
                        _forceSendChanges = true;
                    }
                    if (supportsAutoAsm) {
                        ImGui::Checkbox("Auto Ambient Sound", &_headphones->autoAsmEnabled.desired);

                        ImGui::BeginDisabled(!_headphones->autoAsmEnabled.current);
                        static const std::map<AUTO_ASM_SENSITIVITY, const char*> AUTO_ASM_SENSITIVITY_STR = {
                            {AUTO_ASM_SENSITIVITY::STANDARD, "Standard"},
                            {AUTO_ASM_SENSITIVITY::HIGH, "High"},
                            {AUTO_ASM_SENSITIVITY::LOW, "Low"},
                        };
                        auto it = AUTO_ASM_SENSITIVITY_STR.find(_headphones->autoAsmSensitivity.current);
                        std::string currentStr = it != AUTO_ASM_SENSITIVITY_STR.end() ? it->second : "Unknown";
                        if (ImGui::BeginCombo("Sensitivity", currentStr.c_str())) {
                            for (auto const& [k, v] : AUTO_ASM_SENSITIVITY_STR) {
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

            if (ImGui::BeginTabItem("Speak to Chat")) {
                ImGui::Checkbox("Enabled", &_headphones->stcEnabled.desired);
                ImGui::SliderInt("STC Level", &_headphones->stcLevel.desired, 0, 2);
                ImGui::SliderInt("STC Time", &_headphones->stcTime.desired, 0, 3);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Equalizer")) {
                static const std::map<int, const char*> EQ_PRESET_NAMES = {
                    { 0, "Off" },
                    { 1, "Rock" },
                    { 2, "Pop" },
                    { 3, "Jazz" },
                    { 4, "Dance" },
                    { 5, "EDM" },
                    { 6, "R&B/Hip-Hop" },
                    { 7, "Acoustic" },
                    // 8-15 reserved for future use
                    { 16, "Bright" },
                    { 17, "Excited" },
                    { 18, "Mellow" },
                    { 19, "Relaxed" },
                    { 20, "Vocal" },
                    { 21, "Treble" },
                    { 22, "Bass" },
                    { 23, "Speech" },
                    // 24-31 reserved for future use
                    { 0xa0, "Custom" },
                    { 0xa1, "User Setting 1" },
                    { 0xa2, "User Setting 2" },
                    { 0xa3, "User Setting 3" },
                    { 0xa4, "User Setting 4" },
                    { 0xa5, "User Setting 5" }
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
                ImGui::SeparatorText("5-Band EQ");
                for (int i = 0; i < 5; i++) {
                    const char* bandNames[] = { "400","1k","2.5k","6.3k","16k" };
                    ImGui::PushID(i);
                    ImGui::VSliderInt("##", ImVec2(width / 5 - padding, 160), &_headphones->eqConfig.desired.bands[i], -10, 10);
                    if (ImGui::IsItemActive() || ImGui::IsItemHovered())
                        ImGui::SetTooltip("%s", bandNames[i]);
                    ImGui::PopID();
                    if (i != 4) ImGui::SameLine();
                }
                ImGui::SeparatorText("Clear Bass");
                ImGui::SetNextItemWidth(width);
                ImGui::SliderInt("##", &_headphones->eqConfig.desired.bassLevel, -10, 10);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Multipoint")) {
                ImGui::Checkbox("Multipoint Connect", &_headphones->mpEnabled.desired);
                ImGui::Text("NOTE: Can't toggle yet. Sorry!");
                const auto default_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                for (auto& [mac, device] : _headphones->connectedDevices) {
                    if (mac == _headphones->mpDeviceMac.current)
                        ImGui::TreeNodeEx(&mac, default_flags | ImGuiTreeNodeFlags_Selected, "%s", device.name.c_str());
                    else
                        ImGui::TreeNodeEx(&mac, default_flags, "%s", device.name.c_str());
                    if (ImGui::IsItemClicked()) {
                        _headphones->mpDeviceMac.desired = mac;
                    }
                }
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Misc")) {
                ImGui::Checkbox("Capture Voice During a Phone Call", &_headphones->voiceCapEnabled.desired);
                ImGui::SliderInt("Voice Guidance Volume", &_headphones->miscVoiceGuidanceVol.desired, -2, 2);

                if (ImGui::TreeNode("Touch Sensor")) {
                    const std::map<TOUCH_SENSOR_FUNCTION, const char*> TOUCH_SENSOR_FUNCTION_STR = {
                        {TOUCH_SENSOR_FUNCTION::PLAYBACK_CONTROL, "Playback Control"},
                        {TOUCH_SENSOR_FUNCTION::AMBIENT_NC_CONTROL, "Ambient Sound / Noise Cancelling"},
                        {TOUCH_SENSOR_FUNCTION::NOT_ASSIGNED, "Not Assigned"}
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

                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
    }
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
    else if (_forceSendChanges || _headphones->isChanged())
    {
        _forceSendChanges = false;
        _headphones->_sendCommandFuture.setFromAsync([=, this]() {
            return this->_headphones->setChanges();
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
