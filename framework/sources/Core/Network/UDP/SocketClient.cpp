#include	<netdb.h>

#include	"Core/Network/UDP/SocketClient.hh"
#include	"Core/Network/Exception.hh"

Core::Network::UDP::SocketClient::SocketClient():
	_addr(),
	_info(std::make_pair(0, 0)),
	_input(std::make_pair(std::queue<ByteArray*>(), 0)),
	_output(std::make_pair(std::queue<ByteArray*>(), 0))
{}

Core::Network::UDP::SocketClient::~SocketClient() {}

void	Core::Network::UDP::SocketClient::reinit() {
	// reinit socket address
	memset(&(this->_addr), 0, sizeof(sockaddr_in));

	// reinit socket information
	this->_info.first = 0;
	this->_info.second = 0;

	// reinit received datagrams
	// send back all bytearrays to pool
	while (!this->_input.first.empty()) {
		ByteArray::Pool::get().remove(this->_input.first.front());
		this->_input.first.pop();
	}
	this->_input.second = 0;

	// reinit datagrams to send
	// send back all bytearrays to pool
	while (!this->_output.first.empty()) {
		ByteArray::Pool::get().remove(this->_output.first.front());
		this->_output.first.pop();
	}
	this->_output.second = 0;
}

void	Core::Network::UDP::SocketClient::init(const sockaddr_in& addr) {
	if (&(addr) != &(this->_addr)) {
		memcpy(&(this->_addr), &(addr), sizeof(sockaddr_in));
		this->_info.first = static_cast<uint32_t>(this->_addr.sin_addr.s_addr);
		this->_info.second = ntohs(this->_addr.sin_port);
	}
}

bool	Core::Network::UDP::SocketClient::hasDataToSend() const {
	return !this->_output.first.empty();
}

void	Core::Network::UDP::SocketClient::push(ByteArray* datagram) {
	if (datagram->getSize() + this->_output.second > Core::Network::UDP::SocketClient::BUFFER_SIZE) {
		throw Core::Network::Exception("push: buffer is full");
	} else {
		this->_output.first.push(datagram);
		this->_output.second += datagram->getSize();
	}
}

ByteArray*	Core::Network::UDP::SocketClient::nextDatagram() {
	if (this->_output.first.empty()) {
		return nullptr;
	} else {
		ByteArray* datagram = this->_output.first.front();
		this->_output.first.pop();
		this->_output.second -= datagram->getSize();
		return datagram;
	}
}

void	Core::Network::UDP::SocketClient::received(ByteArray* datagram) {
	if (datagram->getSize() + this->_input.second > Core::Network::UDP::SocketClient::BUFFER_SIZE) {
		throw Core::Network::Exception("recvfrom: buffer is full");
	} else {
		this->_input.first.push(datagram);
		this->_input.second += datagram->getSize();
	}
}

const sockaddr_in& Core::Network::UDP::SocketClient::socketAddress() const {
	return this->_addr;
}

const std::pair<uint32_t, uint16_t>& Core::Network::UDP::SocketClient::clientInformation() const {
	return this->_info;
}

/**
 *	Core::Network::UDP::SocketClient pool
 */

Core::Network::UDP::SocketClient::Pool::Pool():
	Factory::BasicPool<Core::Network::UDP::SocketClient>()
{}

Core::Network::UDP::SocketClient::Pool::~Pool() {}

void	Core::Network::UDP::SocketClient::Pool::init() {
	this->initPool(Core::Network::UDP::SocketClient::Pool::ORIGINAL_SIZE,
		Core::Network::UDP::SocketClient::Pool::HYDRATE_SIZE,
		"Core::Network::UDP::SocketClient");
}
