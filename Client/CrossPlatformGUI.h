#pragma once

#include "imgui/imgui.h"
#include "imgui/misc/cpp/imgui_stdlib.h"

#include "Constants.h"
#include "IBluetoothConnector.h"
#include "BluetoothWrapper.h"
#include "CommandSerializer.h"
#include "Exceptions.h"
#include "TimedMessageQueue.h"
#include "SingleInstanceFuture.h"
#include "CascadiaCodeFont.h"
#include "Headphones.h"

#include <future>
#include <iostream>
#include <fstream>
#include <array>

#define TOML_HEADER_ONLY 0
#include <toml++/toml.hpp>

constexpr auto GUI_MAX_MESSAGES = -1;
constexpr auto GUI_MESSAGE_TIMEOUT = -1;
constexpr auto GUI_MESSAGE_BOX_SIZE = 2;
constexpr auto GUI_HEIGHT = 700;
constexpr auto GUI_WIDTH = 700;
constexpr auto FPS = 60;
constexpr auto MS_PER_FRAME = 1000 / FPS;
constexpr auto FONT_SIZE = 15.0f;
const auto WINDOW_COLOR = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// Executes a shell command asynchronusly. See respective *GUI.cpp implementation for details.
void ExecuteShellCommand(const std::string& command);

struct GUIConfig {
	const char* SAVE_NAME = "settings.ini";

	bool 
		showDisclaimers = true;
	std::map<std::string, std::string>
		headphoneInteractionShellCommands{};
	std::string 
		imguiSettings{};
	std::string
		imguiFontFile{};
	float imguiFontSize = -1;

	void loadSettings();
	void saveSettings();
};
//This class should be constructed after AFTER the Dear ImGUI context is initialized.
class CrossPlatformGUI
{
public:
	CrossPlatformGUI(BluetoothWrapper bt, const float font_size = FONT_SIZE);

	//Run the GUI code once. This function should be called from a loop from one of the GUI impls (Windows, OSX, Linux...)
	//O: true if the user wants to close the window
	bool performGUIPass();
private:
	ImFont* _applyFont(const std::string& fontFile, float font_size);

	void _drawMessages();
	void _drawDeviceDiscovery();
	void _drawControls();
	void _drawConfig();

	void _setHeadphoneSettings();

	GUIConfig _config;
	void _handleHeadphoneInteraction(std::string&& event);

	BluetoothDevice _connectedDevice;
	BluetoothWrapper _bt;
	SingleInstanceFuture<std::vector<BluetoothDevice>> _connectedDevicesFuture;
	SingleInstanceFuture<void> _sendCommandFuture;
	SingleInstanceFuture<void> _connectFuture;
	
	SingleInstanceFuture<void> _requestFuture;

	TimedMessageQueue _mq;

	std::unique_ptr<Headphones> _headphones;
};


