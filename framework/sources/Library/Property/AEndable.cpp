#include	"Library/Property/AEndable.hh"

AEndable::AEndable(void):
	_end(false)
{}

AEndable::~AEndable(void) {}

bool	AEndable::mustEnd(void) const {
	return this->_end;
}

void	AEndable::mustEnd(bool state) {
	this->_end = state;
}
