#include	"Library/Tool/Logger.hpp"
#include	"Library/Tool/Macro.hh"

Logger::Logger():
	Threading::Lock(),
	_level(Logger::Level::DEBUG),
	_offset(0)
{}

Logger::~Logger() {}

void	Logger::setLevel(Logger::Level level) {
	SCOPELOCK(this);
	this->_level = level;
}

void	Logger::setOffset(size_t off) {
	SCOPELOCK(this);
	this->_offset = off;
}

void	Logger::addOffset(size_t off) {
	SCOPELOCK(this);
	this->_offset += off;
}

void	Logger::delOffset(size_t off) {
	SCOPELOCK(this);
	this->_offset -= MIN(off, this->_offset);
}