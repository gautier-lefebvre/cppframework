#include  "Core/Network/Udp/AUdpSocketIO.hh"
#include  "Core/Network/Exception.hh"

using namespace fwk;

const size_t AUdpSocketIO::BUFFER_SIZE = 32768;

AUdpSocketIO::AUdpSocketIO(void):
    Lockable(),
    _addr(),
    _input(std::make_pair(std::queue<ByteArray*>(), 0)),
    _output(std::make_pair(std::queue<ByteArray*>(), 0))
{}

AUdpSocketIO::~AUdpSocketIO(void) {
    this->reinit();
}

void AUdpSocketIO::reinit(void) {
    SCOPELOCK(this);

    // reinit socket address
    memset(&(this->_addr), 0, sizeof(sockaddr_in));

    while (!(this->_input.first.empty())) {
        ByteArray* bytearray = this->_input.first.front();
        this->_input.first.pop();
        ByteArray::returnToPool(bytearray);
    }

    while (!(this->_output.first.empty())) {
        ByteArray* bytearray = this->_output.first.front();
        this->_output.first.pop();
        ByteArray::returnToPool(bytearray);
    }

    this->_input.second  = 0;
    this->_output.second = 0;
}

bool  AUdpSocketIO::hasDataToSend(void) const {
    return this->_output.second > 0;
}

void  AUdpSocketIO::push(ByteArray* datagram) {
    SCOPELOCK(this);

    if (this->_output.second + datagram->getSize() > AUdpSocketIO::BUFFER_SIZE) {
        throw NetworkException("AUdpSocketIO::push: buffer maximum size reached");
    }

    this->_output.first.push(datagram);
    this->_output.second += datagram->getSize();
}

ByteArray*  AUdpSocketIO::getData(void) {
    SCOPELOCK(this);

    if (!(this->_input.first.empty())) {
        ByteArray* datagram = this->_input.first.front();
        this->_input.first.pop();
        this->_input.second -= datagram->getSize();
        return datagram;
    } else {
        return nullptr;
    }
}

bool AUdpSocketIO::operator==(const sockaddr_in& oth) const {
    return (oth.sin_family      == AF_INET && 
                    oth.sin_family      == this->_addr.sin_family &&
                    oth.sin_addr.s_addr == this->_addr.sin_addr.s_addr &&
                    oth.sin_port        == this->_addr.sin_port);
}
