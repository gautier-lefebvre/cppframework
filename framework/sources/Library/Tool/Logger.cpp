#include  "Library/Tool/Logger.hpp"
#include  "Library/Tool/Macro.hh"

Logger::Logger(void):
  Threading::Lockable(),
  _level(Logger::Level::DEBUG),
  _offset(0),
  _file(nullptr)
{}

Logger::~Logger(void) {
  if (this->_file != nullptr) {
    delete this->_file;
  }
  this->_file = nullptr;
}

void  Logger::init(Logger::Level level) {
  SCOPELOCK(this);
  this->_level = level;
}

void  Logger::init(Logger::Level level, const std::string& filepath) {
  SCOPELOCK(this);

  if (this->_file != nullptr) {
    delete this->_file;
  }

  this->_level = level;
  this->_file = new std::ofstream(filepath);
}

void  Logger::setOffset(size_t off) {
  SCOPELOCK(this);
  this->_offset = off;
}

void  Logger::addOffset(size_t off) {
  SCOPELOCK(this);
  this->_offset += off;
}

void  Logger::delOffset(size_t off) {
  SCOPELOCK(this);
  this->_offset -= MIN(off, this->_offset);
}
