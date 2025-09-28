#include "SingleInstanceFuture.h"
#include "BluetoothWrapper.h"
#include "Constants.h"

#include <mutex>
#include <iostream>
#include <map>
#include <variant>

template <class T>
struct Property
{
	T current;
	T desired;

	bool pendingRequest = false;

	void flagPending()
	{
		this->pendingRequest = true;
	};
	bool isPending()
	{
		return this->pendingRequest;
	};

	void fulfill()
	{
		this->current = this->desired;
		this->pendingRequest = false;
	};
	bool isFulfilled()
	{
		return this->desired == this->current;
	};
	void overwrite(T const &value)
	{
		current = value;
		desired = value;
		this->pendingRequest = false;
	};
};

template <class T>
struct ReadonlyProperty
{
	T current;

	void overwrite(T const &value)
	{
		current = value;
	};
};

enum class HeadphonesEvent
{
	MessageUnhandled = -1,

	NoMessage,
	NoChange,

	JSONMessage,
	Initialized,

	NcAsmParamUpdate,
	NcAmbButtonModeUpdate,
	BatteryLevelUpdate,

	PlaybackMetadataUpdate,
	PlaybackVolumeUpdate,
	PlaybackPlayPauseUpdate,

	SoundPressureUpdate,
	AutoPowerOffUpdate,
	AutoPauseUpdate,
	VoiceGuidanceEnabledUpdate,
	VoiceGuidanceVolumeUpdate,
	VoiceCaptureEnabledUpdate,

	SpeakToChatParamUpdate,
	SpeakToChatEnabledUpdate,
	TouchFunctionUpdate,

	EqualizerParamUpdate,

	MultipointDeviceSwitchUpdate,
	MultipointEnabledUpdate,
	TouchSensorControlPanelEnabledUpdate,

	ConnectedDeviceUpdate,

	InteractionUpdate,
};

using HeadphonesMessage = CommandSerializer::CommandMessage;

enum DeviceCapabilities
{
	DC_None = 0x0,

	DC_TrueWireless = 0x1,

	// Since WF-1000XM5
	DC_ConfigurableVoiceCaptureDuringCall = 0x10,
	DC_VoiceGuidanceVolumeAdjustment = 0x20,

	// Since WH-1000XM6
	DC_AutoAsm = 0x40,
};

HEADPHONES_DEFINE_ENUM_FLAG_OPERATORS(DeviceCapabilities);

class Headphones
{
public:
	/* The built-in EQ. All values should integers of range -10~+10 */
	struct EqualizerConfig
	{
		int bassLevel{};
		std::vector<int> bands;
		EqualizerConfig() : bassLevel(0), bands(0) {};
		EqualizerConfig(int bass, std::vector<int> const &bands) : bassLevel(bass), bands(bands) {};
		bool operator==(EqualizerConfig const &other) const
		{
			return bassLevel == other.bassLevel && bands == other.bands;
		}
	};
	Headphones(BluetoothWrapper &conn);

	ReadonlyProperty<HeadphonesMessage> rawMessage;

	// Is NC or Ambient sound enabled?
	Property<bool> asmEnabled{};

	// NC or Ambient sound mode?
	Property<NC_ASM_SETTING_TYPE> asmMode{};

	// Is Foucs On Voice enabled?
	Property<bool> asmFoucsOnVoice{};

	// Ambient sound level. 0 ~ 20.
	// 0 shouldn't be a possible value on the app.
	Property<int> asmLevel{};
	Property<bool> draggingAsmLevel{};

	// Is auto ambient sound enabled? (WH-1000XM6 onwards)
	Property<bool> autoAsmEnabled{};

	// Auto ambient sound sensitivity. 0 ~ 2. (WH-1000XM6 onwards)
	Property<AUTO_ASM_SENSITIVITY> autoAsmSensitivity{};

	// Is automatic power off enabled?
	Property<bool> autoPowerOffEnabled{};

	// Is "Pause when headphones are removed" enabled?
	Property<bool> autoPauseEnabled{};

	// Is voice guidance enabled?
	Property<bool> voiceGuidanceEnabled{};

	// Volume for voice guidance. -2 ~ 2
	Property<int> miscVoiceGuidanceVol{};

	// Battery levels 0 ~ 100
	Property<int> statBatteryL{}, statBatteryR{}, statBatteryCase{};

	// Volume. 0 ~ 30
	Property<int> volume{};

	// Play/Pause. true for play, false for pause
	ReadonlyProperty<bool> playPause{};

	// Plaintext messages
	ReadonlyProperty<std::string> deviceMessages{};

	// Headphone interaction message. Avalialbe after InteractionUpdate
	ReadonlyProperty<std::string> interactionMessage{};

	// Connected devices
	std::map<std::string, BluetoothDevice> connectedDevices;

	// Paired devices that are not connected
	std::map<std::string, BluetoothDevice> pairedDevices;

	// Is Multipoint enabled?
	Property<bool> mpEnabled{};

	// Capture Voice During a Phone Call thing
	Property<bool> voiceCapEnabled{};

	// Playback
	struct
	{
		std::string title;
		std::string album;
		std::string artist;
		int sndPressure{};
	} playback;

	// Multipoint
	Property<std::string> mpDeviceMac{};

	// Speak to chat
	Property<bool> stcEnabled{};

	// AUTO:0 HIGH:1 LOW:2
	Property<int> stcLevel{};

	// SHORT:0 STANDARD:1 LONG:2 OFF(Does not close automatically):3
	Property<int> stcTime{};

	// Equalizer
	Property<int> eqPreset;
	Property<EqualizerConfig> eqConfig;

	// [WF only] Touch sensor function
	Property<TOUCH_SENSOR_FUNCTION> touchLeftFunc{}, touchRightFunc{};

	// [WH only] Touch sensor control panel enabled
	Property<bool> touchSensorControlPanelEnabled{};

	// [WH only] [NC/AMB] Button Setting
	Property<NcAmbButtonMode> ncAmbButtonMode{};

	// Device model + capabilities
	enum class DeviceModel
	{
		WH1000XM5 = 0x2016,
		WF1000XM5 = 0x2017,
		WH1000XM6 = 0x3001,
	};
	DeviceModel deviceModel{};
	DeviceCapabilities deviceCapabilities{};

	bool isChanged();
	void setChanges();

	void waitForAck(int timeout = 1);

	void requestInit();
	void requestSync();
	void requestMultipointSwitch(const char *macString);
	void requestPlaybackControl(PLAYBACK_CONTROL control);
	void requestPowerOff();

	void recvAsync();
	BluetoothWrapper &getConn() { return _conn; }

	/*
	Asynchornously poll for incoming messages and (optionally) returns any event
	that has been triggered by the message.
	This function is non-blocking and thread-safe.
	*/
	HeadphonesEvent poll();

	bool is_connected() const { return _conn.isConnected(); }
	void disconnect();

	~Headphones();

	SingleInstanceFuture<std::optional<HeadphonesMessage>> _recvFuture;
	SingleInstanceFuture<void> _sendCommandFuture;
	SingleInstanceFuture<void> _requestFuture;

private:
	std::mutex _propertyMtx, _ackMtx;
	BluetoothWrapper &_conn;
	std::condition_variable _ackCV;

	// Helper functions for _handleMessage
	HeadphonesEvent _handleInitResponse(const HeadphonesMessage &msg);
	HeadphonesEvent _handleNcAsmParam(const HeadphonesMessage &msg);
	HeadphonesEvent _handleBatteryLevelRet(const HeadphonesMessage &msg);
	HeadphonesEvent _handlePlaybackStatus(const HeadphonesMessage &msg);
	HeadphonesEvent _handlePlaybackSndPressureRet(const HeadphonesMessage &msg);
	HeadphonesEvent _handleAutomaticPowerOffParam(const HeadphonesMessage &msg);
	HeadphonesEvent _handleVoiceGuidanceParam(const HeadphonesMessage &msg);
	HeadphonesEvent _handleMultipointDevice(const HeadphonesMessage &msg);
	HeadphonesEvent _handleConnectedDevices(const HeadphonesMessage &msg);
	HeadphonesEvent _handlePlaybackStatusControl(const HeadphonesMessage &msg);
	HeadphonesEvent _handleMultipointEtcEnable(const HeadphonesMessage &msg);
	HeadphonesEvent _handleAutomaticPowerOffButtonMode(const HeadphonesMessage &msg);
	HeadphonesEvent _handleSpeakToChat(const HeadphonesMessage &msg);
	HeadphonesEvent _handleEqualizer(const HeadphonesMessage &msg);
	HeadphonesEvent _handleMiscDataRet(const HeadphonesMessage &msg);
	HeadphonesEvent _handleMessage(HeadphonesMessage const &msg);

	bool hasInit = false;
};
