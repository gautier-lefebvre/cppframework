#include  "Library/Tool/Logger.hpp"
#include  "Library/Tool/Macro.hh"
#include  "Library/Exception.hh"

Logger::Logger(const std::string& name):
  Threading::Lockable(),
  _name(name),
  _level(Logger::Level::INFO),
  _offset(0),
  _file(nullptr)
{}

Logger::Logger(const Logger& oth):
  Threading::Lockable(),
  _name(oth._name),
  _level(oth._level),
  _offset(oth._offset),
  _file(oth._file)
{}

Logger& Logger::operator=(const Logger& oth) {
  if (this != &oth) {
    this->_name = oth._name;
    this->_level = oth._level;
    this->_offset = oth._offset;
    this->_file = oth._file;
  }
  return *this;
}

Logger::~Logger(void) {
  this->closeFile();
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

void  Logger::setLevel(Logger::Level level) {
  SCOPELOCK(this);
  this->_level = level;
}

void  Logger::setFile(const std::string& filepath) {
  SCOPELOCK(this);
  this->closeFile();

  this->_file = new std::ofstream(filepath);

  if (this->_file->fail()) {
    this->_file = nullptr;
    throw ::Exception("Logger file could not be open");
  }
}

void Logger::closeFile(void) {
  SCOPELOCK(this);
  if (this->_file != nullptr) {
    try {
      this->_file->close();
    } catch (const std::exception&) {}

    delete this->_file;
  }
  this->_file = nullptr;
} 

LoggerManager::LoggerManager(void):
  Threading::Lockable(),
  _loggers()
{}

LoggerManager::~LoggerManager(void) {}

Logger&  LoggerManager::init(const std::string& loggerName, Logger::Level level) {
  SCOPELOCK(this);
  Logger& logger = this->getLogger(loggerName);
  logger.setLevel(level);
  return logger;
}

Logger&  LoggerManager::init(const std::string& loggerName, Logger::Level level, const std::string& filepath) {
  SCOPELOCK(this);

  Logger& logger = this->getLogger(loggerName);
  logger.setLevel(level);
  logger.setFile(filepath);
  return logger;
}

Logger& LoggerManager::getLogger(const std::string& loggerName) {
  SCOPELOCK(this);

  try {
    return this->_loggers.at(loggerName);
  } catch (const std::out_of_range&) {
    this->_loggers.emplace(std::make_pair(loggerName, Logger(loggerName)));
    return this->_loggers.at(loggerName);
  }
}

void  LoggerManager::endLogger(const std::string& name) {
  SCOPELOCK(this);
  this->_loggers.erase(name);
}