#include	<cstring>

#include	"Library/Collection/ByteArray.hpp"
#include	"Library/Tool/Converter.hpp"
#include	"Library/Tool/Macro.hh"

Factory::Pool<ByteArray>* ByteArray::Pool::_pool = nullptr;

ByteArray::ByteArray(size_t s):
	Factory::AFactored(),
	Threading::Lock(),
	_bytearray(new uint8_t[s]),
	_size(0),
	_sizemax(s) {
	this->reinit();
}

ByteArray::ByteArray(const ByteArray& oth):
	_bytearray(new uint8_t[oth._size]),
	_size(oth._size),
	_sizemax(oth._size) {
	memcpy(this->_bytearray, oth._bytearray, oth._size);
}

ByteArray&	ByteArray::operator=(const ByteArray& oth) {
	if (this != &oth) {
		if (this->_sizemax < oth._size) {
			delete[] this->_bytearray;
			this->_bytearray  = new uint8_t[oth._size];
			this->_sizemax = oth._size;
		}
		this->_size    = oth._size;
		memcpy(this->_bytearray, oth._bytearray, oth._size);
	}
	return (*this);
}

ByteArray::~ByteArray() {
	if (this->_bytearray != nullptr) {
		delete[] this->_bytearray;
	}
}

uint8_t		ByteArray::operator[](size_t idx) const {
	if (idx < this->_size) {
		return this->_bytearray[idx];
	} else {
		throw std::out_of_range("ByteArray: index " + StringOfSize(idx) + " out of range");
	}
}

void		ByteArray::reinit() {
	this->_size = 0;
	memset(this->_bytearray, 0, this->_sizemax);
}

void	ByteArray::resize(size_t size, bool force, bool keep) {
	this->_size = MIN(size, this->_size);
	if (force || this->_sizemax < size) {
		uint8_t* ptr   = this->_bytearray;
		this->_bytearray  = new uint8_t[size];
		this->_sizemax = size;
		if (keep) {
			memcpy(this->_bytearray, ptr, this->_size);
		} else {
			this->_size = 0;
		}
		if (ptr) {
			delete[] ptr;
		}
	}
}

const uint8_t*	ByteArray::getBytes() const {
	return (this->_bytearray);
}

uint8_t*	ByteArray::atEnd() {
	return (this->_bytearray + this->_size);
}

uint8_t*	ByteArray::atStart() {
	return (this->_bytearray);
}

uint8_t*	ByteArray::atIndex(size_t idx) {
	if (idx > this->_size) {
		throw std::out_of_range("ByteArray: index " + StringOfSize(idx) + " out of range");
	}
	return (this->_bytearray + idx);
}

void		ByteArray::moveEnd(size_t off) {
	if (this->_size + off < this->_sizemax) {
		this->_size += off;
	} else {
		throw std::out_of_range("ByteArray::moveEnd : the offset is too high");
	}
}

void		ByteArray::clearStart(size_t size) {
	size_t	toclear = MIN(size, this->_size);
	memmove(this->_bytearray, this->_bytearray + toclear, this->_size);
	this->_size -= toclear;
}

size_t		ByteArray::find(const void* ptr, size_t size, size_t start_idx) const {
	if (this->_size + start_idx < size) {
		return std::string::npos;
	}
	bool	found;
	const uint8_t *tofind = reinterpret_cast<const uint8_t*>(ptr);
	for (size_t idx = start_idx ; idx < this->_size - size ; ++idx) {
		found = true;
		for (size_t i = 0 ; i < size ; ++i) {
			if (this->_bytearray[idx+i] != tofind[i]) {
				found = false;
			}
		}
		if (found) {
			return idx;
		}
	}
	return std::string::npos;
}

size_t		ByteArray::get(void *ptr, size_t size, size_t offset, bool remove) {
	size_t	tomove = MIN(size, this->_size - offset);
	memcpy(ptr, this->_bytearray + offset, tomove);
	if (remove) {
		memmove(this->_bytearray + offset, this->_bytearray + tomove + offset, this->_size - offset - tomove);
		this->_size -= tomove;
	}
	return tomove;
}

size_t		ByteArray::get(void *ptr, size_t size, size_t offset) const {
	size_t	tomove = MIN(size, this->_size - offset);
	memcpy(ptr, this->_bytearray + offset, tomove);
	return tomove;
}

void		ByteArray::pushStr(const std::string& str, bool resize) {
	this->push(str.c_str(), str.length(), resize);
}

void		ByteArray::push_frontStr(const std::string& str, bool resize) {
	this->push_front(str.c_str(), str.length(), resize);
}

size_t		ByteArray::availableSpace() const {
	return (this->getSizeMax() - this->getSize());
}

size_t		ByteArray::getSize() const {
	return this->_size;
}

size_t		ByteArray::getSizeMax() const {
	return (this->_sizemax);
}

bool		ByteArray::empty() const {
	return (this->_size == 0);
}

bool		ByteArray::full() const {
	return (this->availableSpace() == 0);
}

std::ostream&	operator<<(std::ostream& os, const ByteArray& bytearray) {
	for (size_t i = 0 ; i < bytearray.getSize() ; ++i) {
		os << bytearray[i];
	}
	return os;
}

ByteArrayExtractor::ByteArrayExtractor(const ByteArray * bytearray, size_t offset):
	_bytearray(bytearray),
	_offset(offset)
{}

ByteArrayExtractor::ByteArrayExtractor(const ByteArrayExtractor& oth):
	_bytearray(oth._bytearray),
	_offset(oth._offset)
{}

ByteArrayExtractor& ByteArrayExtractor::operator=(const ByteArrayExtractor& oth) {
	if (this != &oth) {
		this->_bytearray = oth._bytearray;
		this->_offset = oth._offset;
	}
	return *this;
}

ByteArrayExtractor::~ByteArrayExtractor() {}

void	ByteArrayExtractor::extractString(std::string& data, size_t length) {
	if (this->_bytearray->getSize() - this->_offset < length) {
		throw std::out_of_range("ByteArrayExtractor::extract: There are not enough bytes left in ByteArray to extract successfully");
	} else {
		ByteArray::Pool::Guard guard(ByteArray::Pool::create(length));
		this->_bytearray->get(guard.bytearray->atStart(), length, this->_offset);
		data = reinterpret_cast<const char*>(guard.bytearray->getBytes());
		this->_offset += length;
	}
}

void	ByteArray::Pool::init() {
	if (ByteArray::Pool::_pool == nullptr) {
		ByteArray::Pool::_pool = new Factory::Pool<ByteArray>(
			ByteArray::Pool::ORIGINAL_SIZE,
			ByteArray::Pool::HYDRATE_SIZE,
			"ByteArray");
	}
}

void	ByteArray::Pool::destroy() {
	if (ByteArray::Pool::_pool != nullptr) {
		delete ByteArray::Pool::_pool;
	}
	ByteArray::Pool::_pool = nullptr;
}

ByteArray* ByteArray::Pool::create() {
	return ByteArray::Pool::_pool->get();
}

ByteArray* ByteArray::Pool::create(size_t size) {
	ByteArray* bytearray = ByteArray::Pool::_pool->get();
	bytearray->resize(size);
	return bytearray;
}

void ByteArray::Pool::remove(ByteArray* element) {
	if (element != nullptr) {
		if (element->getSizeMax() > ByteArray::Pool::MAX_BUFFER_SIZE) {
			element->resize(ByteArray::Pool::MAX_BUFFER_SIZE, true, false);
		}
		ByteArray::Pool::_pool->push(element);
	}
}

ByteArray::Pool::Guard::Guard(ByteArray* bytearray): bytearray(bytearray) {}

ByteArray::Pool::Guard::~Guard() { ByteArray::Pool::remove(this->bytearray); }