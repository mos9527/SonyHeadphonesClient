#include "BluetoothWrapper.h"
#include <iostream>
BluetoothWrapper::BluetoothWrapper(std::unique_ptr<IBluetoothConnector> connector)
{
	this->connector.swap(connector);
}

BluetoothWrapper::BluetoothWrapper(BluetoothWrapper&& other) noexcept
{
	this->connector.swap(other.connector);
	this->_seqNumber = other._seqNumber;
}

BluetoothWrapper& BluetoothWrapper::operator=(BluetoothWrapper&& other) noexcept
{
	//self assignment
	if (this == &other) return *this;

	this->connector.swap(other.connector);
	this->_seqNumber = other._seqNumber;

	return *this;
}

int BluetoothWrapper::sendCommand(CommandSerializer::CommandMessage const& cmd)
{
    if (connector->isConnected()) {
        std::lock_guard guard(this->_connectorMtx);
        auto const &data = cmd.messageBytes;
        auto bytesSent = this->connector->send((char *) data.data(), data.size());
        return bytesSent;
    }
    return 0;
}

int BluetoothWrapper::sendCommand(const Buffer& command, DATA_TYPE dataType)
{
	return sendCommand(CommandSerializer::CommandMessage(dataType, command.data(), command.size(), this->_seqNumber));
}

int BluetoothWrapper::sendCommand(const uint8_t* data, size_t len, DATA_TYPE dataType)
{
	return sendCommand(CommandSerializer::CommandMessage(dataType, data, len, this->_seqNumber));
}

int BluetoothWrapper::sendAck(int seqNumber)
{
	return sendCommand(CommandSerializer::CommandMessage(DATA_TYPE::ACK, nullptr, 0, 1 - seqNumber));
}

bool BluetoothWrapper::isConnected() noexcept
{
	return this->connector->isConnected();
}

void BluetoothWrapper::connect(const std::string& addr)
{
	std::lock_guard guard(this->_connectorMtx);
	this->connector->connect(addr);
}

void BluetoothWrapper::disconnect() noexcept
{
	std::lock_guard guard(this->_connectorMtx);
	this->_seqNumber = 0;
	this->connector->disconnect();
}


std::vector<BluetoothDevice> BluetoothWrapper::getConnectedDevices()
{
	return this->connector->getConnectedDevices();
}

void BluetoothWrapper::recvCommand(CommandSerializer::CommandMessage& msg)
{
	// char buf[MAX_BLUETOOTH_MESSAGE_SIZE] = { 0 };

	msg.messageBytes.clear();
	msg.messageBytes.reserve(MAX_BLUETOOTH_MESSAGE_SIZE);
	msg.messageBytes.resize(7);

	const auto recvOne = [this]() {
		char buf;
		if (this->connector->recv(&buf, 1) != 1)
			throw RecoverableException("Connection closed", true);
		return buf;
	};

	while (this->connector->isConnected() && recvOne() != START_MARKER);
	msg.messageBytes[0] = START_MARKER;

	msg.messageBytes[1] = recvOne(); // dataType
	msg.messageBytes[2] = recvOne(); // seqNumber
	this->_seqNumber = msg.getSeqNumber();

	this->connector->recv(reinterpret_cast<char*>(&msg.messageBytes[3]), 4);
	while (msg.messageBytes.back() != END_MARKER) 
		msg.messageBytes.push_back(recvOne()); 

	// int msgSize = msg.getSize();
	msg.messageBytes = CommandSerializer::_unescapeSpecials(msg.messageBytes);

	if (!msg.verify())
		throw RecoverableException("Invalid checksum!", true);
#ifdef _DEBUG
	std::cout << "[recv] ";
	for (char c : msg.messageBytes) std::cout << std::hex << (int)c << ' ';
	std::cout << std::endl;
#endif // _DEBUG

	recvCV.notify_one();
}
