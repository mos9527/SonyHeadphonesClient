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

	Property<bool> asmEnabled{};
	Property<bool> asmFoucsOnVoice{};
	Property<int> asmLevel{};

	Property<int> miscVoiceGuidanceVol{};

	bool isChanged();
	void setChanges();
	
	void handleMessage(CommandSerializer::CommandMessage const& msg);
	BluetoothWrapper& getConn() { return _conn; }

	void disconnect();
	~Headphones();

	inline int const getAckCount() { return _ackCount; }
	inline int const getCmdCount() { return _cmdCount; }

private:
	std::mutex _propertyMtx;
	BluetoothWrapper& _conn;

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
