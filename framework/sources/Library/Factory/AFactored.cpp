#include	"Library/Factory/AFactored.hh"

Factory::AFactored::AFactored():
	_valid(false),
	_lastOutOfPoolTimePoint()
{}

Factory::AFactored::~AFactored() {}

void	Factory::AFactored::isValid(bool valid) { this->_valid = valid; }

bool	Factory::AFactored::isValid() const { return this->_valid; }

void	Factory::AFactored::updateLastOutOfPoolTimePoint() {
	this->_lastOutOfPoolTimePoint = std::chrono::steady_clock::now();
}

const std::chrono::steady_clock::time_point& Factory::AFactored::lastOutOfPoolTimePoint() const { return this->_lastOutOfPoolTimePoint; }
