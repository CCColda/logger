#include "Logger.h"

#include <ctime>
#include <fstream>
#include <iostream>

#ifdef LOGGER_CONSOLE_ATTRIBUTES
#include <conio.h>
#include <windows.h>
#endif

namespace {
typedef std::ostream &(*ColorMacro)(std::ostream &stream);

struct LogStyle {
	ColorMacro sign_color;
	ColorMacro text_color;
};

#ifdef LOGGER_CONSOLE_ATTRIBUTES
void setColor(WORD color)
{
	static HANDLE stdoutHandle = NULL;
	if (!stdoutHandle)
		stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(stdoutHandle, color);
}

std::ostream &CLR_YELLOW(std::ostream &stream)
{
	setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	return stream;
}
std::ostream &CLR_RED(std::ostream &stream)
{
	setColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
	return stream;
}
std::ostream &CLR_GRAY(std::ostream &stream)
{
	setColor(FOREGROUND_INTENSITY);
	return stream;
}
std::ostream &CLR_WHITE(std::ostream &stream)
{
	setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE |
	         FOREGROUND_INTENSITY);
	return stream;
}
std::ostream &CLR_RESET(std::ostream &stream)
{
	setColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	return stream;
}
#else
std::ostream &CLR_YELLOW(std::ostream &stream)
{
	stream << "\x1B[33m";
	return stream;
}
std::ostream &CLR_RED(std::ostream &stream)
{
	stream << "\x1B[91m";
	return stream;
}
std::ostream &CLR_GRAY(std::ostream &stream)
{
	stream << "\x1B[90m";
	return stream;
}
std::ostream &CLR_WHITE(std::ostream &stream)
{
	stream << "\x1B[97m";
	return stream;
}
std::ostream &CLR_RESET(std::ostream &stream)
{
	stream << "\033[0m";
	return stream;
}
#endif

LogStyle getLogStyle(Logger::Level level)
{
	constexpr static const LogStyle LOG_STYLES[Logger::Level::count] = {
	    LogStyle{CLR_YELLOW, CLR_RED},
	    LogStyle{CLR_YELLOW, CLR_YELLOW},
	    LogStyle{CLR_YELLOW, CLR_WHITE},
	    LogStyle{CLR_WHITE, CLR_GRAY},
	    LogStyle{CLR_GRAY, CLR_GRAY}};

	return LOG_STYLES[level];
}

const char *getLogLevelString(Logger::Level level)
{
	constexpr static const char *LOG_LEVELS[Logger::Level::count] = {
	    "ERR",
	    "WRN",
	    "INF",
	    "VB1",
	    "VB2"};

	return LOG_LEVELS[level];
}

} // namespace

/* static */ std::array<Logger::Output, Logger::Level::count>
    Logger::outputs = {};

/* static */ Logger::Output Logger::console = nullptr;

/* static */ Logger::Level Logger::logLevel = Logger::Level::VERBOSE;

/* static */ void Logger::writeToOutput(const Output &out, const std::string &text)
{
	using namespace std::string_literals;

	(*out) << getTimeString() + " "s + text << std::endl;
}

/* static */ std::string Logger::getTimeString()
{
	const auto time = std::time(nullptr);
	const auto *utc = std::gmtime(&time);

	char buffer[32] = {};

	// yyyy-mm-dd hh:mm:ss
	std::size_t written =
	    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", utc);

	return std::string(buffer, written);
}

Logger::Logger(const std::string &sign) : m_sign(sign) {}

Logger::Logger(const Logger &other) : m_sign(other.m_sign) {}

Logger &Logger::level(Level ilevel, const std::string &text)
{
	using namespace std::string_literals;

	if (ilevel > logLevel)
		return *this;

	const auto [sign_color, text_color] = getLogStyle(ilevel);

	if (console)
		(*console) << CLR_RESET
		           << "[" << sign_color << m_sign << CLR_RESET << "]: "
		           << text_color << text << CLR_RESET << std::endl;

	if (outputs[ilevel])
		writeToOutput(
		    outputs[ilevel],
		    "["s + m_sign + "] "s + std::string(getLogLevelString(ilevel)) + ": "s + text);

	return *this;
}

Logger &Logger::anonymous(const std::string &text)
{
	if (Level::VERBOSE > logLevel)
		return *this;

	const auto [sign_color, text_color] = getLogStyle(VERBOSE);

	if (console)
		(*console) << CLR_RESET
		           << "[" << sign_color << m_sign << CLR_RESET << "]: "
		           << text_color << text << CLR_RESET << std::endl;

	return *this;
}

Logger &Logger::log(const std::string &text)
{
	return level(Level::INFO, text);
}

Logger &Logger::warning(const std::string &text)
{
	return level(Level::WARN, text);
}

Logger &Logger::error(const std::string &text)
{
	return level(Level::ERROR, text);
}

/* static */ Logger::Output Logger::openStaticOutputStream(std::ostream &stream)
{
	return std::shared_ptr<std::ostream>(&stream, [](std::ostream *) -> void {});
}

/* static */ void Logger::setAllOutputs(const Output &output)
{
	for (int i = 0; i < Logger::Level::count; ++i)
		outputs[i] = output;
}

/* static */ Logger::Output Logger::openFileOutput(const std::string &path)
{
	auto file =
	    std::shared_ptr<std::ofstream>(new std::ofstream(), [](std::ofstream *p) {
		    if (p->is_open())
			    p->close();
		    delete p;
	    });

	file->open(path, std::ios_base::out | std::ios_base::app);

	if (!file->is_open())
		return nullptr;

	return std::dynamic_pointer_cast<std::ostream>(file);
}

/* static */ void Logger::destroy()
{
	setAllOutputs(nullptr);
	console = nullptr;
}
