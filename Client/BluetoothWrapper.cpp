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

int BluetoothWrapper::sendCommand(const std::vector<char>& bytes, DATA_TYPE dataType)
{
	std::lock_guard guard(this->_connectorMtx);
	auto data = CommandSerializer::packageDataForBt(bytes, dataType, this->_seqNumber++);
	auto bytesSent = this->connector->send(data.data(), data.size());

	return bytesSent;
}

void BluetoothWrapper::sendAck()
{
	auto data = CommandSerializer::packageDataForBt({}, DATA_TYPE::ACK, this->_seqNumber++);
	this->connector->send(data.data(), data.size());
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

void BluetoothWrapper::recvCommand(Command& msg)
{
	char buf[MAX_BLUETOOTH_MESSAGE_SIZE] = { 0 };

	msg.messageBytes.clear();
	msg.messageBytes.reserve(MAX_BLUETOOTH_MESSAGE_SIZE);
	msg.messageBytes.resize(6);

	const auto recvOne = [=]() {
		char buf;
		if (this->connector->recv(&buf, 1) != 1)
			throw RecoverableException("Connection closed", true);
		return buf;
	};

	while (recvOne() != START_MARKER);

	msg.messageBytes[0] = recvOne();
	msg.dataType = static_cast<DATA_TYPE>(msg.messageBytes[0]);

	msg.messageBytes[1] = recvOne();
	msg.seqNumber = msg.messageBytes[1];
	this->_seqNumber = msg.seqNumber;

	this->connector->recv(&msg.messageBytes[2], 4);
	msg.messageSize = bytesToIntBE(&msg.messageBytes[2]);

	int recvSize = this->connector->recv(buf, msg.messageSize);
	if (msg.messageSize != recvSize) throw RecoverableException("Recv size mismatch", true);
	if (msg.messageSize) 
		msg.messageBytes.insert(msg.messageBytes.end(), buf, buf + msg.messageSize);

	msg.messageBytes.push_back(recvOne());
	msg.chkSum = msg.messageBytes.back();	

	if (recvOne() != END_MARKER) 
		throw RecoverableException("Invalid message pack recevied", true);
	if (msg.chkSum != CommandSerializer::_sumChecksum(msg.messageBytes.data(), msg.messageBytes.size() - 1 /* chksum itself is not contained*/))
		throw RecoverableException("Invalid checksum!", true);

	return;
}
