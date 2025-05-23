#pragma once
#include <string>
#include <vector>
#include "Exceptions.h"
#include "Constants.h"

inline constexpr auto NO_BLUETOOTH_DEVICES_ERROR = "No Bluetooth radios were found - is your adapter connected *and* turned on?";

struct BluetoothDevice
{
	//UTF-8
	std::string name;
	std::string mac;

	BluetoothDevice() = default;
	BluetoothDevice(const std::string& name, const std::string& mac) : name(name), mac(mac) {}
};

/*
General notes: Please look at the implementation of WindowsBluetoothConnector.
* Functions should throw RecoverableExceptions if they're indeed recoverable, and throw std::runtime_error otherwise.
* RecoverableException can force a disconnection of the socket with an additional param (a call to disconnect()).
* See notes below
*/

//Thread-safety: IBluetoothConnector implementations don't need to be thread safe, except calls to isConnected;
class IBluetoothConnector
{
public:
	IBluetoothConnector() = default;
	virtual ~IBluetoothConnector() = default;

	IBluetoothConnector(const IBluetoothConnector&) = delete;
	IBluetoothConnector& operator=(const IBluetoothConnector&) = delete;

	//send, recv and connect can block.
	//O: The number of bytes sent.
	virtual int send(char* buf, size_t length) noexcept(false) = 0;
	virtual int recv(char* buf, size_t length) noexcept(false) = 0;
	virtual void connect(const std::string& addrStr) noexcept(false) = 0;
	
	//This function should not block.
	virtual void disconnect() noexcept = 0;
	
	//Cost directive: This function must be as cheap as possible.
	//!!! Thread-safety: This function must be thread safe. !!!
	virtual bool isConnected() noexcept = 0;

	//getConnectedDevices can block.
	virtual std::vector<BluetoothDevice> getConnectedDevices() = 0;
};
