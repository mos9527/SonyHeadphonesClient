#include "App.h"

bool App::OnFrame()
{
    bool open = true;

    ImGui::SetNextWindowPos({ 0,0 });
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    {
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
                // Polling & state updates are only done on the main thread
                auto event = _headphones->poll();
                switch (event)
                {
#ifdef _DEBUG
                    case HeadphonesEvent::JSONMessage:
                        _logs.push_back(_headphones->deviceMessages);
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
                default:
                    break;
                }
                _drawControls();
                // Timed sync
                {
                    static const double syncInterval = 1.0;
                    static double lastSync = -syncInterval;
                    if (_headphones->_requestFuture.ready()) {
                        if (ImGui::GetTime() - lastSync >= syncInterval) {
                            _headphones->_requestFuture.get();
                            lastSync = ImGui::GetTime();
                            _headphones->_requestFuture.setFromAsync([this]() {
                                if (this->_headphones)
                                    this->_headphones->requestSync();
                                });
                        }
                    }
                }
                _setHeadphoneSettings();
            }
            else {
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
    return open;
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
                            this->_logs.push_back("Initialized: " + this->_connectedDevice.name);
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
                static bool autoConnectAttempted = false;
                if (!autoConnectAttempted){
                    autoConnectAttempted = true;
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
                    _config.save();
                }
            }
        }
    }    
}

void App::_drawControls()
{
    if (ImGui::CollapsingHeader(_connectedDevice.name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
        static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable;        
        if (ImGui::BeginTable("##Stats", 2, flags)) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            {
                ImGui::Text("L:"); ImGui::SameLine();
                ImGui::ProgressBar(_headphones->statBatteryL.current / 100.0);
                ImGui::Text("R:"); ImGui::SameLine();
                ImGui::ProgressBar(_headphones->statBatteryR.current / 100.0);
                ImGui::Text("Case:"); ImGui::SameLine();
                ImGui::ProgressBar(_headphones->statBatteryCase.current / 100.0);
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
        if (ImGui::Button("Shutdown")) {
            if (_headphones->_requestFuture.ready()) {
                _headphones->_requestFuture.get();
                _headphones->_requestFuture.setFromAsync([this]() {
                    _headphones->requestPowerOff();
                    throw RecoverableException("Requested Shutdown", true);
                });
            }
        }
    }	
    ImGui::SeparatorText("Controls");
    {
        if (ImGui::BeginTabBar("##Controls")) {
            if (ImGui::BeginTabItem("Playback Control")) {
                using enum PLAYBACK_CONTROL;
                PLAYBACK_CONTROL control = NONE;
                ImGui::SliderInt("Volume", &_headphones->volume.desired, 0, 30);
                if (ImGui::Button("Prev")) control = PREV;
                ImGui::SameLine();
                if (_headphones->playPause.current == true /*playing*/) {
                    if (ImGui::Button("Pause")) control = PAUSE;
                }
                else {
                    if (ImGui::Button("Play")) control = PLAY;
                }
                ImGui::SameLine();
                if (ImGui::Button("Next")) control = NEXT;

                if (_headphones->_requestFuture.ready()) {
                    if (control != NONE) {
                        _headphones->_requestFuture.get();
                        _headphones->_requestFuture.setFromAsync([this, control]() {
                            _headphones->requestPlaybackControl(control);
                            });
                    }
                }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("AMB / ANC")) {
                ImGui::Checkbox("Enabled", &_headphones->asmEnabled.desired);
                ImGui::Checkbox("Voice Passthrough", &_headphones->asmFoucsOnVoice.desired);
                ImGui::Text("NOTE: Set to 0 to enable Noise Cancelling.");
                ImGui::SliderInt("Ambient Strength", &_headphones->asmLevel.desired, 0, 20);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Speak to Chat")) {
                ImGui::Checkbox("Enabled", &_headphones->stcEnabled.desired);
                ImGui::SliderInt("STC Level", &_headphones->stcLevel.desired, 0, 2);
                ImGui::SliderInt("STC Time", &_headphones->stcTime.desired, 0, 3);
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Equalizer")) {
                static const std::map<int, std::string> EQ_PRESET_NAMES = {
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
                static std::string presetName = "Unknown";
                auto it = EQ_PRESET_NAMES.find(_headphones->eqPreset.current);
                if (it != EQ_PRESET_NAMES.end())
                    presetName = it->second;
                if (ImGui::BeginCombo("Preset", presetName.c_str()))
                {
                    for (auto const& [k, v] : EQ_PRESET_NAMES)
                    {
                        bool is_selected = k == _headphones->eqPreset.current;
                        if (ImGui::Selectable(v.c_str(), is_selected))
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
                    static const std::map<TOUCH_SENSOR_FUNCTION, const char*> TOUCH_SENSOR_FUNCTION_STR = {
                        {TOUCH_SENSOR_FUNCTION::PLAYBACK_CONTROL, "Playback Control"},
                        {TOUCH_SENSOR_FUNCTION::AMBIENT_NC_CONTROL, "Ambient Sound / Noise Cancelling"},
                        {TOUCH_SENSOR_FUNCTION::NOT_ASSIGNED, "Not Assigned"}
                    };

                    const auto draw_touch_sensor_combo = [&](auto& prop, const char* label) {
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
    if (ImGui::CollapsingHeader("App Config", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (ImGui::BeginListBox("##Messages", ImVec2(-1, GUI_MESSAGE_BOX_SIZE * ImGui::GetTextLineHeightWithSpacing()))) {
            static int prevMessageCnt = 0;
            for (auto const& message : _logs) {
                ImGui::Text("%s", message.c_str());
            }
            if (_logs.size() > prevMessageCnt)
                prevMessageCnt = _logs.size(), ImGui::SetScrollHereY();
            ImGui::EndListBox();
        }
        else {
            ImGui::Text("No messages yet.");
        }
        if (ImGui::TreeNode("UI")) {
            ImGui::SeparatorText("Misc");
            ImGui::Checkbox("Show Disclaimers", &_config.showDisclaimers);
            ImGui::SeparatorText("Font");
            ImGui::SliderInt("Font Size", &_config.imguiFontSize, 10.0f, 64.0f);
            ImGui::InputText("Custom Font (filename)", &_config.imguiFontFile);
            ImGui::Text("NOTE: You'll need a custom, CJK compliant font to see those characters.");
            ImGui::Text("NOTE: All font-related changes will only be applied on restart.");
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Shell Command")) {
            ImGui::Text("NOTE: Headphones may send Events (displayed in the Messages section as Headphone Event: ...)");
            ImGui::Text("NOTE: You may bind them to shell commands here.");
            static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV;
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
                    ImGui::PushID(it->first.data());
                    ImGui::TableSetColumnIndex(0);
                    ImGui::InputText("##", &it->first);
                    ImGui::PopID();

                    ImGui::PushID(it->second.data());
                    ImGui::TableSetColumnIndex(1);
                    ImGui::InputText("##", &it->second);
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
        ImGui::Separator();
        if (ImGui::Button("Save Config"))
            _config.save();
        ImGui::SameLine();
        if (ImGui::Button("Load Config"))
            _config.load();
    }
}

void App::_setHeadphoneSettings() {
    //Don't show if the event only takes a few frames to send
    static int commandLinger = 0;
    if (_headphones->_sendCommandFuture.ready())
    {
        commandLinger = 0;
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
            return this->_headphones->setChanges();
        });
    }
}

void App::_handleHeadphoneInteraction(std::string const& event)
{
    auto& cmds = _config.headphoneInteractionShellCommands;
    auto cmd = std::find_if(cmds.begin(),cmds.end(),[&](auto& p) { return p.first == event; });
    if (cmd != cmds.end()){
        system(cmd->second.c_str());
    }
    _logs.push_back("Headphone Event: " + event);
}

App::App(BluetoothWrapper&& bt, AppConfig& config) : _config(config), _bt(std::move(bt)),  _connectFuture("connect"), _connectedDevicesFuture("connected devices")
{
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO();
    this->_connectedDevicesFuture.setFromAsync([this]() { return this->_bt.getConnectedDevices(); });

    io.IniFilename = nullptr;
    io.WantSaveIniSettings = false;
}

///
bool AppConfig::load()
{
    std::ifstream file(_configPath);
    if (!file.is_open())
        return false;

    toml::table table = toml::parse(file);
    showDisclaimers = table["showDisclaimers"].value<bool>().value_or(true);

    imguiSettings = table["imguiSettings"].value<std::string>().value_or("");
    ImGui::LoadIniSettingsFromMemory(imguiSettings.c_str(), imguiSettings.size());

    autoConnectDeviceMac = table["autoConnectDeviceMac"].value<std::string>().value_or("");
    imguiFontFile = table["imguiFontFile"].value<std::string>().value_or("");
    imguiFontSize = table["imguiFontSize"].value<float>().value_or(-1);
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

bool AppConfig::save()
{
    std::ofstream file(_configPath);
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
    table.insert("autoConnectDeviceMac", autoConnectDeviceMac);

    file << toml::toml_formatter{ table };
    return true;
}
