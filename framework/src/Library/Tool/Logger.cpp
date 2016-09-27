#include  "Library/Tool/Logger.hpp"
#include  "Library/Tool/Macro.hh"
#include  "Library/Exception.hh"

using namespace fwk;

Logger::Logger(const std::string& name, Lockable& lock):
    Lockable(),
    _name(name),
    _level(Logger::Level::INFO),
    _offset(0),
    _file(nullptr),
    _printLock(lock)
{}

Logger::Logger(const Logger& oth):
    Lockable(),
    _name(oth._name),
    _level(oth._level),
    _offset(oth._offset),
    _file(oth._file),
    _printLock(oth._printLock)
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
    Lockable(),
    _loggers(),
    _printLock()
{}

LoggerManager::~LoggerManager(void) {}

Logger&  LoggerManager::init(const std::string& loggerName, Logger::Level level) {
    SCOPELOCK(this);
    Logger& logger = this->getLogger(loggerName, true);
    logger.setLevel(level);
    return logger;
}

Logger&  LoggerManager::init(const std::string& loggerName, Logger::Level level, const std::string& filepath) {
    SCOPELOCK(this);

    Logger& logger = this->getLogger(loggerName, true);
    logger.setLevel(level);
    logger.setFile(filepath);
    return logger;
}

Logger& LoggerManager::getLogger(const std::string& loggerName, bool create) {
    SCOPELOCK(this);

    try {
        // returns logger if already in the list
        return this->_loggers.at(loggerName);
    } catch (const std::out_of_range&) {
        // if the logger is not in the list
        if (create) {
            // if create is true, creates it and returns it
            this->_loggers.emplace(std::make_pair(loggerName, Logger(loggerName, this->_printLock)));
            return this->_loggers.at(loggerName);
        } else {
            // else rethrow the exception
            throw;
        }
    }
}

void  LoggerManager::endLogger(const std::string& name) {
    SCOPELOCK(this);
    this->_loggers.erase(name);
}