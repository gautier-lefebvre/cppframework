#include	"Library/Factory/AFactored.hh"

Factory::AFactored::AFactored():
	_valid(false)
{}

Factory::AFactored::~AFactored() {}

bool	Factory::AFactored::isValid() const { return this->_valid; }

void	Factory::AFactored::isValid(bool valid) { this->_valid = valid; }