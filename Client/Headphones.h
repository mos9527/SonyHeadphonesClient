#include "SingleInstanceFuture.h"
#include "BluetoothWrapper.h"
#include "Constants.h"

#include <mutex>
#include <iostream>

template <class T>
struct Property {
	T current;
	T desired;

	void fulfill();
	bool isFulfilled();	
	void overwrite(T value);
};

class Headphones {
public:
	Headphones(BluetoothWrapper& conn);

	// Is NC & Ambient sound enabled?
	Property<bool> asmEnabled{};
	// Is Foucs On Voice enabled?
	Property<bool> asmFoucsOnVoice{};
	// Ambient sound level. 0-20. 0 for enabling Noise Cancelling
	Property<int> asmLevel{};

	// Volume for voice guidance. -2 - 2
	Property<int> miscVoiceGuidanceVol{};

	// Battery levels
	Property<int> statBatteryL{}, statBatteryR{}, statBatteryCase{};

	// Volume. 0-30
	Property<int> volume{};

	// Playback
	struct {
		std::string title;
		std::string album;
		std::string artist;
		int sndPressure{};
	} playback;

	bool isChanged();
	void setChanges();
		
	void waitForAck(int timeout = 1);

	void requestInit();
	void requestSync();

	void recvAsync();
	BluetoothWrapper& getConn() { return _conn; }

	void pollMessages();

	void disconnect();
	~Headphones();

	inline int const getAckCount() const { return _ackCount; }
	inline int const getCmdCount() const { return _cmdCount; }

private:
	std::mutex _propertyMtx, _ackMtx;
	BluetoothWrapper& _conn;
	std::condition_variable _ackCV;

	SingleInstanceFuture<std::optional<CommandSerializer::CommandMessage>> _recvFuture;

	void _handleMessage(CommandSerializer::CommandMessage const& msg);

	bool hasInit = false;

	int _ackCount{};
	int _cmdCount{};
};

template<class T>
inline void Property<T>::fulfill()
{
	this->current = this->desired;
}

template<class T>
inline bool Property<T>::isFulfilled()
{
	return this->desired == this->current;
}

template<class T>
inline void Property<T>::overwrite(T value)
{
	current = desired = value;
}
