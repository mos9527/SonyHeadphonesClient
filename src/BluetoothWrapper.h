#pragma once

#include "platform/IBluetoothConnector.h"
#include "CommandSerializer.h"
#include "Constants.h"
#include <memory>
#include <deque>
#include <string>
#include <mutex>
#include <condition_variable>

//Thread-safety: This class is thread-safe.
class BluetoothWrapper
{
public:
	BluetoothWrapper(std::unique_ptr<IBluetoothConnector> connector);

	BluetoothWrapper(const BluetoothWrapper&) = delete;
	BluetoothWrapper& operator=(const BluetoothWrapper&) = delete;

	BluetoothWrapper(BluetoothWrapper&& other) noexcept;
	BluetoothWrapper& operator=(BluetoothWrapper&& other) noexcept;

	int sendCommand(CommandSerializer::CommandMessage const& cmd);
	int sendCommand(const Buffer& command, DATA_TYPE dataType = DATA_TYPE::DATA_MDR);
	int sendCommand(const uint8_t* data, size_t len, DATA_TYPE dataType = DATA_TYPE::DATA_MDR);
	int sendAck(int seqNumber);

	bool isConnected() noexcept;
	//Try to connect to the headphones
	void connect(const std::string& addr);
	void disconnect() noexcept;

	std::vector<BluetoothDevice> getConnectedDevices();

	std::unique_ptr<IBluetoothConnector> connector;

	std::condition_variable recvCV;
	void recvCommand(CommandSerializer::CommandMessage& msg);

private:
	int _recvIntoBuffer();
	std::deque<char> _recvBuffer;
	std::mutex _connectorMtx;
	unsigned int _seqNumber = 0;
};