#include	"Library/Property/AEndable.hh"

AEndable::AEndable():
	_end(false)
{}

AEndable::~AEndable() {}

bool	AEndable::mustEnd() const {
	return this->_end;
}

void	AEndable::mustEnd(bool state) {
	this->_end = state;
}
