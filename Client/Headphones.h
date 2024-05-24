#include "SingleInstanceFuture.h"
#include "BluetoothWrapper.h"
#include "Constants.h"

#include <mutex>
#include <iostream>
#include <map>
#include <variant>

template <class T>
struct Property {
	T current;
	T desired;

	bool pendingRequest = false;

	void flagPending();
	bool isPending();

	void fulfill();
	bool isFulfilled();
	void overwrite(T const& value);
};

template <class T>
struct ReadonlyProperty {
	T current;

	void overwrite(T const& value);
};

struct HeadphonesEvent {
	enum EventType {
		None,

		JSONMessage,
		HeadphoneInteractionEvent,

		PlaybackMetadataUpdate,
		PlaybackVolumeUpdate,
		PlaybackPlayPauseUpdate,

		MultipointSwitch,

		ConnectedDeviceUpdate,

	} type;
	std::variant<std::string> message;

	HeadphonesEvent() : type(None) {};
	HeadphonesEvent(EventType type) : type(type) {};
	HeadphonesEvent(EventType type, auto const& message) : type(type), message(message) {};
	~HeadphonesEvent() {};
};

class Headphones {
public:
	/* The built-in EQ. All values should integers of range -10~+10 */
	struct EqualizerConfig {
		int bassLevel{};
		std::vector<int> bands;
		EqualizerConfig() : bassLevel(0), bands(5) {};
		EqualizerConfig(int bass, std::vector<int> const& bands) : bassLevel(bass), bands(bands) {};
		bool operator==(EqualizerConfig const& other) const {
			return bassLevel == other.bassLevel && bands == other.bands;
		}
	};
	Headphones(BluetoothWrapper& conn);

	// Is NC & Ambient sound enabled?
	Property<bool> asmEnabled{};

	// Is Foucs On Voice enabled?
	Property<bool> asmFoucsOnVoice{};

	// Ambient sound level. 0 ~ 20.
	// 0 shouldn't be a possible value on the app. It's used here as a fallback to Noise Cancelling
	Property<int> asmLevel{};

	// Volume for voice guidance. -2 ~ 2
	Property<int> miscVoiceGuidanceVol{};

	// Battery levels 0 ~ 100
	Property<int> statBatteryL{}, statBatteryR{}, statBatteryCase{};

	// Volume. 0 ~ 30
	Property<int> volume{};

	// Play/Pause. true for play, false for pause
	ReadonlyProperty<bool> playPause{};

	// Connected devices
	std::map<std::string, BluetoothDevice> connectedDevices;
	
	// Paired devices that are not connected
	std::map<std::string, BluetoothDevice> pairedDevices;

	// Is Multipoint enabled?
	Property<bool> mpEnabled{};

	// Capture Voice During a Phone Call thing
	Property<bool> voiceCapEnabled{};

	// Playback
	struct {
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
	Property<EqualizerConfig> eqConfig;

	// Touch sensor function
	Property<TOUCH_SENSOR_FUNCTION> touchLeftFunc{}, touchRightFunc{};

	bool isChanged();
	void setChanges();
		
	void waitForAck(int timeout = 1);

	void requestInit();
	void requestSync();
	void requestMultipointSwitch(const char* macString);
	void requestPlaybackControl(PLAYBACK_CONTROL control);
	void requestPowerOff();

	void recvAsync();
	BluetoothWrapper& getConn() { return _conn; }

	/*
	Asynchornously poll for incoming messages and (optionally) returns any event 
	that has been triggered by the message.
	This function is non-blocking and thread-safe.
	*/
	HeadphonesEvent poll();

	void disconnect();
	~Headphones();

private:
	std::mutex _propertyMtx, _ackMtx;
	BluetoothWrapper& _conn;
	std::condition_variable _ackCV;

	SingleInstanceFuture<std::optional<CommandSerializer::CommandMessage>> _recvFuture;

	HeadphonesEvent _handleMessage(CommandSerializer::CommandMessage const& msg);

	bool hasInit = false;
};

template<class T>
inline void Property<T>::flagPending()
{
	this->pendingRequest = true;
}


template<class T>
inline bool Property<T>::isPending()
{
	return this->pendingRequest;
}


template<class T>
inline void Property<T>::fulfill()
{
	this->current = this->desired;
	this->pendingRequest = false;
}

template<class T>
inline bool Property<T>::isFulfilled()
{
	return this->desired == this->current;
}

template<class T>
inline void Property<T>::overwrite(T const& value)
{
	current = value;
	desired = value;
	this->pendingRequest = false;
}

template<class T>
inline void ReadonlyProperty<T>::overwrite(T const& value)
{
	current = value;
}
