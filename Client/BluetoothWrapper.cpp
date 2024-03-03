#include "BluetoothWrapper.h"

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

int BluetoothWrapper::sendCommand(const std::vector<char>& bytes)
{
	std::lock_guard guard(this->_connectorMtx);
	auto data = CommandSerializer::packageDataForBt(bytes, DATA_TYPE::DATA_MDR, this->_seqNumber++);
	auto bytesSent = this->connector->send(data.data(), data.size());

	this->_waitForAck();

	return bytesSent;
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

void BluetoothWrapper::_waitForAck()
{
	char buf[MAX_BLUETOOTH_MESSAGE_SIZE] = { 0 };
	
	Buffer msgBytes;
	msgBytes.reserve(MAX_BLUETOOTH_MESSAGE_SIZE);

	const auto recvOne = [=]() {
		char buf;
		if (this->connector->recv(&buf, 1) != 1)
			throw RecoverableException("Connection closed", true);
		return buf;
	};

	while (recvOne() != START_MARKER);

	msgBytes.push_back(recvOne()); // dataType
	msgBytes.push_back(recvOne()); // seq

	this->connector->recv(buf, 4);
	int size = (int)buf[3] | (int)buf[2] << 8 | (int)buf[1] << 16 | (int)buf[0] << 24;
	msgBytes.insert(msgBytes.end(), buf, buf + 4); // size (int BE)

	this->connector->recv(buf, size);
	msgBytes.insert(msgBytes.end(), buf, buf + size); // command

	msgBytes.push_back(recvOne()); // chksum

	if (recvOne() != END_MARKER) throw RecoverableException("Invalid message pack recevied", true);

	auto msg = CommandSerializer::unpackBtMessage(msgBytes);
	this->_seqNumber = msg.seqNumber;
}

