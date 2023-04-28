#ifndef LOGGER_H
#define LOGGER_H

#include <array>
#include <memory>
#include <ostream>
#include <string>

/** @brief Class for managing single-threaded logging */
class Logger {
public:
	enum Level {
		LVL_ERROR = 0,
		LVL_WARN = 1,
		LVL_INFO = 2,
		LVL_VERBOSE = 3,
		LVL_VERBOSE2 = 4,
		count
	};

	using Output = std::shared_ptr<std::ostream>;

	static Level logLevel;
	static std::array<Output, Level::count> outputs;
	static Output console;

private:
	std::string m_sign;

	static void writeToOutput(const Output &output, const std::string &text);
	static std::string getTimeString();

public:
	Logger(const std::string &sign);
	Logger(const Logger &other);

	//! @brief Logs a string with a given log level
	Logger &level(Level ilevel, const std::string &text);

	//! @brief Logs a string to the console, but not the log file
	Logger &anonymous(const std::string &text);

	//! @brief Logs a string
	Logger &log(const std::string &text);

	//! @brief Logs a warning string
	Logger &warning(const std::string &text);

	//! @brief Logs an error string
	Logger &error(const std::string &text);

	//! @brief Converts a statically defined output stream, such as std::cout to Output
	static Output openStaticOutputStream(std::ostream &stream);

	//! @brief Sets all @c outputs to @c output
	static void setAllOutputs(const Output &output);

	//! @brief Initializes the logger to a file
	static Output openFileOutput(const std::string &path);

	//! @brief Disconnects the logfile
	static void destroy();
};

#endif // !defined(LOGGER_H)