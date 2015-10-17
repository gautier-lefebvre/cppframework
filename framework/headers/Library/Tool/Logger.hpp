#ifndef		__LIBRARY_TOOL_LOGGER_HPP__
#define		__LIBRARY_TOOL_LOGGER_HPP__

#include	<map>
#include	<string>
#include	<sstream>
#include	<iostream>

#include	"Library/Collection/BidiMap.hpp"
#include	"Library/DesignPattern/Singleton.hpp"
#include	"Library/Threading/Lock.hpp"
#include	"Library/Tool/Date.hh"

/**
 *	\class Logger Library/Tool/Logger.hpp
 *	\brief A singleton object used for logging.
 */
class Logger :public Singleton<Logger>, public Threading::Lock {
	friend class Singleton<Logger>;
public:
	/*! Logging levels. */
	enum class Level {
		DEBUG    = 0, /*!< Used to debug. */
		INFO     = 1, /*!< Used to log information. */
		WARNING  = 2, /*!< Used to log minor errors. */
		ERROR    = 3, /*!< Used to log non-critical errors. */
		CRITICAL = 4  /*!< Used to log critical errors. */
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
	Logger::Level _level; /*!< Lowest level of logging. */
	size_t        _offset; /*!< Current number of \t to write before printing the log. */

private:
	/*! Deleted copy constructor of Logger. */
	Logger(const Logger&) = delete;

	/*! Deleted move constructor of Logger. */
	Logger(const Logger&&) = delete;

	/*! Deleted assignement constructor of Logger. */
	Logger& operator=(const Logger&) = delete;

private:
	/*! Default constructor of Logger. */
	Logger(void);

	/*! Destructor of Logger. */
	virtual ~Logger(void);

public:
	/**
	 *	\brief Sets the lowest level of logging.
	 *	\param level the lowest level of logging.
	 */
	void	setLevel(Logger::Level level);

	/**
	 *	\brief Logs the message only if its level is greater or equal to the lowest debugging level.
	 *	Prints in a "hh:mm:ss:µµµµµµ -- msg" format.
	 *	\param msg the message to log.
	 *	\param level the logging level.
	 */
	template<typename T>
	void	log(const T &msg, Logger::Level level) {
		if (level >= this->_level) {
			std::stringstream ss;
			ss << Date::getTime() << " -- ";
			for (size_t i = 0 ; i < this->_offset ; ++i) {
				ss << "\t";
			}
			try {
				ss << Logger::ColorToString.at(Logger::LevelToColor.at(level));
			} catch (const std::out_of_range&) {
				ss << Logger::ColorToString.at(Logger::Color::NONE);
			}
			ss << msg << Logger::ColorToString.at(Logger::Color::NONE) << std::endl;
			if (level >= Logger::Level::ERROR) {
				SCOPELOCK(this);
				std::cerr << ss.str();
			} else {
				SCOPELOCK(this);
				std::cout << ss.str();
			}
		}
	}

	/**
	 *	\brief Sets the offset to an absolute number.
	 *	\param off the offset.
	 */
	void	setOffset(size_t off);

	/**
	 *	\brief Increments the offset by the given number.
	 *	\param off the incrementation.
	 */
	void	addOffset(size_t off);

	/**
	 *	\brief Decrements the offset by the given number.
	 *	\param off the decrementation.
	 */
	void	delOffset(size_t off);
};

static const BidiMap<Logger::Level, const std::string> LoggerLevelToString = {
	{Logger::Level::DEBUG, std::string("DEBUG")},
	{Logger::Level::INFO, std::string("INFO")},
	{Logger::Level::WARNING, std::string("WARNING")},
	{Logger::Level::ERROR, std::string("ERROR")},
	{Logger::Level::CRITICAL, std::string("CRITICAL")}
}; /*!< Used to translate the enum Logging::Level to a string. */

#ifdef 		__DEBUG__
# define		LOG(x, y)				Logger::get().log(x, y)
# define		DEBUG(x)				Logger::get().log(x, Logger::Level::DEBUG)
# define		INFO(x)					Logger::get().log(x, Logger::Level::INFO)
# define		WARNING(x)				Logger::get().log(x, Logger::Level::WARNING)
# define		ERROR(x)				Logger::get().log(x, Logger::Level::ERROR)
# define		CRITICAL(x)				Logger::get().log(x, Logger::Level::CRITICAL)
# define		LOGGER_SET_OFFSET(x)	Logger::get().setOffset(x)
# define		LOGGER_ADD_OFFSET(x)	Logger::get().addOffset(x)
# define		LOGGER_DEL_OFFSET(x)	Logger::get().delOffset(x)
#else
# define		LOG(x, y)				(void)(x);(void)y
# define		DEBUG(x)				(void)(x)
# define		INFO(x)					(void)(x)
# define		WARNING(x)				(void)(x)
# define		ERROR(x)				(void)(x)
# define		CRITICAL(x)				(void)(x)
# define		LOGGER_SET_OFFSET(x)	(void)(x)
# define		LOGGER_ADD_OFFSET(x)	(void)(x)
# define		LOGGER_DEL_OFFSET(x)	(void)(x)
#endif		/* __DEBUG__ */


#endif		/* __LIBRARY_TOOL_LOGGER_HPP__ */