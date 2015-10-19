#include	<cstring>

#include	"Library/Encryption/XOR.hh"

Encryption::XOR::XOR(void):
	_key(nullptr),
	_length(0)
{}

Encryption::XOR::~XOR(void) {
	this->clear();
}

void	Encryption::XOR::clear(void) {
	if (this->_key != nullptr) {
		delete[] (this->_key);
	}
	this->_key = nullptr;
	this->_length = 0;
}

void	Encryption::XOR::setKey(const void *ptr, size_t length) {
	this->clear();
	this->_key = new uint8_t[length];
	this->_length = length;
	memcpy(this->_key, reinterpret_cast<const uint8_t*>(ptr), length);
}

void	Encryption::XOR::xorify(void *addr, size_t size) const {
	if (this->_key != nullptr && this->_length != 0) {
		uint8_t* target = reinterpret_cast<uint8_t*>(addr);
		for (size_t i = 0 ; i < size ; ++i) {
			target[i] ^= this->_key[i % this->_length];
		}
	}
}