#include "CrossPlatformGUI.h"

bool CrossPlatformGUI::performGUIPass()
{
	ImGui::NewFrame();

	static bool isConnected = false;

	bool open = true;

	ImGui::SetNextWindowPos({ 0,0 });
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	{
		//TODO: Figure out how to get rid of the Windows window, make everything transparent, and just use ImGui for everything.
		//TODO: ImGuiWindowFlags_AlwaysAutoResize causes some flickering. Figure out how to stop it
		ImGui::Begin("Sony Headphones", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		//Legal disclaimer
		ImGui::Text("! This product is not affiliated with Sony. Use at your own risk. !");
		ImGui::Text("Source: https://github.com/Plutoberth/SonyHeadphonesClient");
		ImGui::Spacing();


		this->_drawErrors();
		this->_drawDeviceDiscovery();

		if (_headphones)
		{
			ImGui::Spacing();
			this->_drawControls();			
			this->_setHeadphoneSettings();
			if (this->_recvFuture.ready()) {
				auto cmd = this->_recvFuture.get();
				if (cmd.has_value()) {
					this->_headphones->handleMessage(cmd.value());
					this->_recvAsync();
				}
			}
		}
	}

	ImGui::End();
	ImGui::Render();

	return open;
}

void CrossPlatformGUI::_drawErrors()
{
	//There's a slight race condition here but I don't care, it'd only be for one frame.
	if (this->_mq.begin() != this->_mq.end())
	{
		ImGui::Text("Errors:");
		ImGui::Spacing();

		for (auto&& message : this->_mq)
		{
			ImGui::Text(message.message.c_str());
		}
		
		ImGui::Spacing();
	}
}

void CrossPlatformGUI::_drawDeviceDiscovery()
{
	if (ImGui::CollapsingHeader("Device Discovery   ", ImGuiTreeNodeFlags_DefaultOpen))
	{
		static std::vector<BluetoothDevice> connectedDevices;
		static int selectedDevice = -1;

		if (this->_headphones)
		{
			ImGui::Text("Connected to %s", this->_connectedDevice.name.c_str());
			if (ImGui::Button("Disconnect"))
			{
				selectedDevice = -1;				
				this->_headphones.reset();
			}			
		}
		else
		{
			ImGui::Text("Select from one of the available devices: ");

			int temp = 0;
			for (const auto& device : connectedDevices)
			{
				ImGui::RadioButton(device.name.c_str(), &selectedDevice, temp++);
			}

			ImGui::Spacing();

			if (this->_connectFuture.valid())
			{
				if (this->_connectFuture.ready())
				{
					try
					{
						this->_connectFuture.get();
					}
					catch (const RecoverableException& exc)
					{
						if (exc.shouldDisconnect)
						{
							this->_headphones.reset();
						}
						this->_mq.addMessage(exc.what());
					}
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
					if (selectedDevice != -1)
					{
						this->_connectedDevice = connectedDevices[selectedDevice];
						this->_connectFuture.setFromAsync([this]() { 
							this->_bt.connect(this->_connectedDevice.mac);
							this->_headphones = std::make_unique<Headphones>(this->_bt);
							this->_recvAsync();
						});
					}
				}
			}

			ImGui::SameLine();

			if (this->_connectedDevicesFuture.valid())
			{
				if (this->_connectedDevicesFuture.ready())
				{
					try
					{
						connectedDevices = this->_connectedDevicesFuture.get();
					}
					catch (const RecoverableException& exc)
					{
						if (exc.shouldDisconnect)
						{
							this->_headphones.reset();
						}
						this->_mq.addMessage(exc.what());
					}
				}
				else
				{
					ImGui::Text("Discovering Devices %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
				}
			}
			else
			{
				if (ImGui::Button("Refresh devices"))
				{
					selectedDevice = -1;
					this->_connectedDevicesFuture.setFromAsync([this]() { return this->_bt.getConnectedDevices(); });
				}
			}
		}
	}
}

void CrossPlatformGUI::_drawControls()
{
	assert(_headphones);
	if (ImGui::CollapsingHeader("Ambient Sound / Noise Cancelling", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::Checkbox("Enabled", &_headphones->asmEnabled.desired);
		ImGui::Checkbox("Voice Passthrough", &_headphones->asmFoucsOnVoice.desired);
		ImGui::SliderInt("Ambient Strength", &_headphones->asmLevel.desired, 0, 20);
	}
	if (ImGui::CollapsingHeader("Misc", ImGuiTreeNodeFlags_DefaultOpen)) {
		ImGui::SliderInt("Voice Guidance Volume", &_headphones->miscVoiceGuidanceVol.desired, -2, 2);
	}
}

void CrossPlatformGUI::_setHeadphoneSettings() {
	assert(_headphones);
	//Don't show if the command only takes a few frames to send
	static int commandLinger = 0;
	if (this->_sendCommandFuture.ready())
	{
		commandLinger = 0;
		try
		{
			this->_sendCommandFuture.get();
		}
		catch (const RecoverableException& exc)
		{
			std::string excString;
			//We kinda have to do it here and not in the wrapper, due to async causing timing issues. To fix it, the messagequeue can be made
			//static, but I'm not sure if I wanna do that.
			if (exc.shouldDisconnect)
			{
				this->_bt.disconnect();
				excString = "Disconnected due to: ";
			}
			this->_mq.addMessage(excString + exc.what());
		}
	}
	//This means that we're waiting
	else if (this->_sendCommandFuture.valid())
	{
		if (commandLinger++ > (FPS / 10))
		{
			ImGui::Text("Sending command %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]);
		}
	}
	//We're not waiting, and there's no command in the air, so we can evaluate sending a new command
	else if (this->_headphones->isChanged())
	{
		this->_sendCommandFuture.setFromAsync([=, this]() {
			return this->_headphones->setChanges();
		});
	}
}

void CrossPlatformGUI::_recvAsync()
{
	assert(_headphones);
	this->_recvFuture.setFromAsync([=]() -> std::optional<CommandSerializer::CommandMessage> {
		if (this->_headphones) {
			auto& conn = this->_headphones->getConn();
			CommandSerializer::CommandMessage cmd;
			try
			{
				conn.recvCommand(cmd);
				return cmd;
			}
			catch (const RecoverableException& exc)
			{
				if (exc.shouldDisconnect)
				{
					this->_bt.disconnect();
					this->_mq.addMessage(exc.what());
					return std::nullopt;
				}
				else {
					throw exc;
				}
			}
		}
	});
}

CrossPlatformGUI::CrossPlatformGUI(BluetoothWrapper bt, const float font_size) : _bt(std::move(bt))
{
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	this->_mq = TimedMessageQueue(GUI_MAX_MESSAGES);
	this->_connectedDevicesFuture.setFromAsync([this]() { return this->_bt.getConnectedDevices(); });

	io.IniFilename = nullptr;
	io.WantSaveIniSettings = false;

	//AddFontFromMemory will own the pointer, so there's no leak
	char* fileData = new char[sizeof(CascadiaCodeTTF)];
	memcpy(fileData, CascadiaCodeTTF, sizeof(CascadiaCodeTTF));
	ImFont* font = io.Fonts->AddFontFromMemoryTTF(reinterpret_cast<void*>(fileData), sizeof(CascadiaCodeTTF), font_size);
	IM_ASSERT(font != NULL);
}
