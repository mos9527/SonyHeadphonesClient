#pragma once

#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "Constants.h"
#include "BluetoothWrapper.h"
#include "CommandSerializer.h"
#include "Exceptions.h"
#include "SingleInstanceFuture.h"
#include "Headphones.h"

#include <future>
#include <iostream>
#include <fstream>
#include <array>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <filesystem>
#include <memory>

#define TOML_HEADER_ONLY 0
#include <toml++/toml.hpp>

#include "platform/IBluetoothConnector.h"
constexpr auto BACKGROUND_UPDATE_INTERVAL_MS = 100; // ms
constexpr auto GUI_FONT_CHANGE_DELAY = 1; // s
constexpr auto GUI_MAX_MESSAGES = -1;
constexpr auto GUI_MESSAGE_TIMEOUT = -1;
constexpr auto GUI_MESSAGE_BOX_SIZE = 5;
constexpr auto GUI_DEFAULT_HEIGHT = 400;
constexpr auto GUI_DEFAULT_WIDTH = 600;
constexpr auto DEFAULT_FONT_SIZE = 15.0f;
constexpr auto WINDOW_COLOR = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
struct AppConfig {
public:
    bool showDisclaimers = true;
    std::string autoConnectDeviceMac{};
    std::vector<std::pair<std::string, std::string>> headphoneInteractionShellCommands{};
    std::string imguiSettings{};
    std::string imguiFontFile{};
    std::pair<int, int> imguiWindowSize{GUI_DEFAULT_WIDTH, GUI_DEFAULT_HEIGHT};
    int imguiFontSize = DEFAULT_FONT_SIZE;
	float headphoneStateSyncInterval = 1.0f;    
    bool load(std::string const& configPath);
    bool save(std::string const& configPath);
};
//This class should be constructed AFTER the Dear ImGUI context is initialized.
class App
{
public:
    App(BluetoothWrapper&& bt, AppConfig& config);
	// This function should be called from the main thread
    // O: true if the window should be redrawn due to state update
    bool OnUpdate();
    // This function should be called from the main thread
    // Run the GUI code once. This function should be called from a loop from one of the GUI impls (Windows, OSX, Linux...)
    // O: true if the user wants to close the window
    bool OnFrame();
private:   
    void _drawDeviceDiscovery();
    void _drawControls();
    void _drawConfig();
    void _setHeadphoneSettings();
    void _handleHeadphoneInteraction(std::string const& event);
    
    double _lastStateSyncTime = 0.0f;
    bool _autoConnectAttempted = false;
    int _prevMessageCnt = 0;

    AppConfig & _config;

    bool _requestShutdown = false;
    bool _forceSendChanges = false;
    PLAYBACK_CONTROL _requestPlaybackControl = PLAYBACK_CONTROL::NONE;

    std::vector<std::string> _logs;
    BluetoothDevice _connectedDevice;
    BluetoothWrapper _bt;
    std::unique_ptr<Headphones> _headphones;
    SingleInstanceFuture<std::vector<BluetoothDevice>> _connectedDevicesFuture;
    SingleInstanceFuture<void> _connectFuture;
};


