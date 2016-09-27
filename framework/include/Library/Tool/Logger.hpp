#ifndef    __LIBRARY_TOOL_LOGGER_HPP__
#define    __LIBRARY_TOOL_LOGGER_HPP__

#include  <map>
#include  <unordered_map>
#include  <string>
#include  <fstream>
#include  <iostream>
#include  <sstream>

#include  "Library/Collection/BidiMap.hpp"
#include  "Library/DesignPattern/Singleton.hpp"
#include  "Library/Threading/Lockable.hpp"
#include  "Library/Tool/Date.hh"

namespace fwk {
  /**
   *  \class Logger Library/Tool/Logger.hpp
   *  \brief A logger class, which can write logs to a file or to the terminal output.
   */
  class Logger :public Lockable {
  public:
    /*! Logging levels. */
    enum class Level {
      DEBUG    = 0, /*!< Used to debug. */
      INFO     = 1, /*!< Used to log information. */
      WARNING  = 2, /*!< Used to log minor errors. */
      ERROR    = 3, /*!< Used to log non-critical errors. */
      CRITICAL = 4, /*!< Used to log critical errors. */
      IGNORE   = 5  /*!< Used to ignore any message. */
    };

  private:
    /*!< Colors usable in a terminal. */
    enum class Color {
      WHITE,
      SKYBLUE,
      PURPLE,
      BLUE,
      YELLOW,
      GREEN,
      RED,
      NONE
    };

    const std::map<Logger::Color, std::string> ColorToString = {
      {Logger::Color::WHITE, std::string("\033[97m")},
      {Logger::Color::SKYBLUE, std::string("\033[96m")},
      {Logger::Color::PURPLE, std::string("\033[95m")},
      {Logger::Color::BLUE, std::string("\033[94m")},
      {Logger::Color::YELLOW, std::string("\033[93m")},
      {Logger::Color::GREEN, std::string("\033[92m")},
      {Logger::Color::RED, std::string("\033[91m")},
      {Logger::Color::NONE, std::string("\033[0m")}
    }; /*!< Used to init colors inside the terminal. */

    const std::map<Logger::Level, Logger::Color> LevelToColor = {
      {Logger::Level::CRITICAL, Logger::Color::RED},
      {Logger::Level::ERROR, Logger::Color::PURPLE},
      {Logger::Level::WARNING, Logger::Color::YELLOW},
      {Logger::Level::INFO, Logger::Color::GREEN},
      {Logger::Level::DEBUG, Logger::Color::WHITE}
    }; /*!< associates a level to a color. */

  private:
    std::string     _name; /*!< name of the logger. */
    Logger::Level   _level; /*!< lowest level of logging. */
    size_t          _offset; /*!< current number of tabulations between the timestamp and the message. */
    std::ofstream*  _file; /*!< file where the logs are written. */
    Lockable&       _printLock; /*!< lock used before printing. */

  public:
    /**
     *  \brief Constructor of Logger.
     *  Default lowest level of logging is INFO.
     *  \param name name of the logger.
     *  \param printLock lock used before printing.
     */
    Logger(const std::string& name, Lockable& printLock);

    /**
     *  \brief Copy constructor of Logger.
     *  \param oth logger to copy.
     */
    Logger(const Logger& oth);

    /**
     *  \brief Assignation constructor of Logger.
     *  \param oth logger to copy.
     */
    Logger& operator=(const Logger& oth);

    /**
     *  \brief Destructor of Logger.
     *  Closes the file if it is open.
     */
    ~Logger(void);

  public:
    /**
     *  \brief Logs the message only if its level is greater or equal to the lowest debugging level. Prints the message inside the file if specified in \a init.
     *  Prints in a "hh:mm:ss:µµµµµµ -- msg" format.
     *  \param msg the message to log.
     *  \param level the logging level.
     */
    template<typename T>
    void  log(const T &msg, Logger::Level level) {
      SCOPELOCK(this);
      if (this->_level != Logger::Level::IGNORE && level >= this->_level) {
        ScopeLock slprint(this->_printLock);

        std::ostream& os = (this->_file != nullptr ? *(this->_file) : (level >= Logger::Level::ERROR ? std::cerr : std::cout));

        os << Date::getTime() << " -- ";
        os << this->_name << " -- ";

        for (size_t i = 0 ; i < this->_offset ; ++i) {
          os << "\t";
        }

        if (this->_file == nullptr) {
          try {
            os << Logger::ColorToString.at(Logger::LevelToColor.at(level));
          } catch (const std::out_of_range&) {
            os << Logger::ColorToString.at(Logger::Color::NONE);
          }
        }

        os << msg;

        if (this->_file == nullptr) {
          os << Logger::ColorToString.at(Logger::Color::NONE);
        }

        os << std::endl;
      }
    }

    /**
     *  \brief Sets the offset to an absolute number.
     *  \param off the offset.
     */
    void  setOffset(size_t off);

    /**
     *  \brief Increments the offset by the given number.
     *  \param off the incrementation.
     */
    void  addOffset(size_t off);

    /**
     *  \brief Decrements the offset by the given number.
     *  \param off the decrementation.
     */
    void  delOffset(size_t off);

    /**
     *  \brief Sets the lowest level of logging.
     *  \param level lowest level of logging.
     */
    void  setLevel(Logger::Level level);

    /**
     *  \brief Sets the logging file.
     *  \throw Exception if the file could not be opened.
     *  \param filepath path to the logging file.
     */
    void  setFile(const std::string& filepath);

    /**
     *  \brief Closes the file. Any log will be written to the std output.
     *  If no file is attached to the logger, nothing happens.
     */
    void closeFile(void);
  };

  /**
   *  \class LoggerManager Library/Tool/Logger.hpp
   *  \brief Loggers module.
   */
  class LoggerManager :public Singleton<fwk::LoggerManager>, public Lockable {
    friend class Singleton<fwk::LoggerManager>;
  public:
    typedef std::unordered_map<std::string, Logger> NameLoggerMap; /*!< map Logger name to Logger. */

  private:
    NameLoggerMap _loggers; /*!< Current list of loggers. */
    Lockable      _printLock; /* !< Lock used by loggers before printing. */

  private:
    /*! Deleted copy constructor of LoggerManager. */
    LoggerManager(const LoggerManager&) = delete;

    /*! Deleted move constructor of LoggerManager. */
    LoggerManager(const LoggerManager&&) = delete;

    /*! Deleted assignement constructor of LoggerManager. */
    LoggerManager&  operator=(const LoggerManager&) = delete;

  private:
    /*! Default constructor of LoggerManager. */
    LoggerManager(void);

    /*! Destructor of LoggerManager. */
    virtual ~LoggerManager(void);

  public:
    /**
     *  \brief Inits a new logger or changes the info of a current logger.
     *  \param loggerName name of the logger.
     *  \param level the lowest level of logging for this logger.
     */
    Logger&  init(const std::string& loggerName, Logger::Level level);

    /**
     *  \brief Inits a new logger or changes the info of a current logger.
     *  \throw Exception if the file could not be opened.
     *  \param loggerName name of the logger.
     *  \param level the lowest level of logging.
     *  \param filepath the filepath of the logging file.
     */
    Logger&  init(const std::string& loggerName, Logger::Level level, const std::string& filepath);

    /**
     *  \brief Gets the Logger matching the specified name, or creates it and returns it if asked.
     *  \throw std::out_of_range if create is false and the logger was not initialized.
     *  \param name the logger name.
     *  \param create if true, will create the Logger if it does not exist.
     *  \return the logger.
     */
    Logger&  getLogger(const std::string& name, bool create = false);

    /**
     *  \brief Deletes a Logger.
     *  \param name name of the logger to remove.
     */
    void  endLogger(const std::string& name);

    /**
     *  \brief Logs the message to the specified logger. If the logger does not exist, creates it.
     *  \param msg the message to log.
     *  \param level the level of logging.
     *  \param loggerName the name of the logger.
     */
    template<typename T>
    void  log(const std::string& loggerName, const T &msg, Logger::Level level) {
      SCOPELOCK(this);
      try {
        // only log if the logger was initialized
        this->getLogger(loggerName).log(msg, level);
      } catch (const std::out_of_range&) {}
    }
  };

  static const BidiMap<Logger::Level, const std::string> LoggerLevelToString = {
    {Logger::Level::DEBUG, std::string("DEBUG")},
    {Logger::Level::INFO, std::string("INFO")},
    {Logger::Level::WARNING, std::string("WARNING")},
    {Logger::Level::ERROR, std::string("ERROR")},
    {Logger::Level::CRITICAL, std::string("CRITICAL")}
  }; /*!< Used to translate the enum Logging::Level to a string. */
}

#define LOG(x, y)              fwk::LoggerManager::get().log("cppframework", (x), (y))
#define DEBUG(x)               fwk::LoggerManager::get().log("cppframework", (x), fwk::Logger::Level::DEBUG)
#define INFO(x)                fwk::LoggerManager::get().log("cppframework", (x), fwk::Logger::Level::INFO)
#define WARNING(x)             fwk::LoggerManager::get().log("cppframework", (x), fwk::Logger::Level::WARNING)
#define ERROR(x)               fwk::LoggerManager::get().log("cppframework", (x), fwk::Logger::Level::ERROR)
#define CRITICAL(x)            fwk::LoggerManager::get().log("cppframework", (x), fwk::Logger::Level::CRITICAL)
#define LOGGER_SET_OFFSET(x)   fwk::LoggerManager::get().getLogger("cppframework").setOffset((x))
#define LOGGER_ADD_OFFSET(x)   fwk::LoggerManager::get().getLogger("cppframework").addOffset((x))
#define LOGGER_DEL_OFFSET(x)   fwk::LoggerManager::get().getLogger("cppframework").delOffset((x))
#define LOGGER_INIT(x)         fwk::LoggerManager::get().init("cppframework", (x))
#define LOGGER_INIT_FILE(x, y) fwk::LoggerManager::get().init("cppframework", (x), (y))
#define LOGGER_END(x)          fwk::LoggerManager::get().endLogger((x))
#define LOGGER_DESTROY         fwk::LoggerManager::destroy()

#endif    /* __LIBRARY_TOOL_LOGGER_HPP__ */
