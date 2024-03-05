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

int BluetoothWrapper::sendCommand(CommandSerializer::CommandMessage const& cmd)
{
	std::lock_guard guard(this->_connectorMtx);
	auto const& data = cmd.messageBytes;
	auto bytesSent = this->connector->send((char*)data.data(), data.size());

	return bytesSent;
}

int BluetoothWrapper::sendCommand(const Buffer& command, DATA_TYPE dataType)
{
	return sendCommand(CommandSerializer::CommandMessage(dataType, command, this->_seqNumber));
}

int BluetoothWrapper::sendAck(int seqNumber)
{	
	return sendCommand(CommandSerializer::CommandMessage(DATA_TYPE::ACK, {}, 1 - seqNumber));
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
	char buf[MAX_BLUETOOTH_MESSAGE_SIZE] = { 0 };

	msg.messageBytes.clear();
	msg.messageBytes.reserve(MAX_BLUETOOTH_MESSAGE_SIZE);
	msg.messageBytes.resize(7);

	const auto recvOne = [=]() {
		char buf;
		if (this->connector->recv(&buf, 1) != 1)
			throw RecoverableException("Connection closed", true);
		return buf;
	};

	while (recvOne() != START_MARKER);
	msg.messageBytes[0] = START_MARKER;

	msg.messageBytes[1] = recvOne(); // dataType
	msg.messageBytes[2] = recvOne(); // seqNumber
	this->_seqNumber = msg.getSeqNumber();

	this->connector->recv(&msg.messageBytes[3], 4);
	int msgSize = msg.getSize();

	int recvSize = this->connector->recv(buf, msgSize);
	if (msgSize != recvSize) throw RecoverableException("Recv size mismatch", true);
	if (msgSize)
		msg.messageBytes.insert(msg.messageBytes.end(), buf, buf + msgSize);

	msg.messageBytes.push_back(recvOne()); // chkSum
	
	// XXX: Some responses (i.e. INIT) has extra data before it?
	while (msg.messageBytes.back() != END_MARKER) 
		msg.messageBytes.push_back(recvOne()); 
	
	msg.messageBytes = CommandSerializer::_unescapeSpecials(msg.messageBytes);

	if (!msg.verify())
		throw RecoverableException("Invalid checksum!", true);

	recvCV.notify_one();
}
