# Logger library for C++17

### Simple logging
```cpp
#include <Logger.h>

int main() {
	Logger mainLogger("MAIN");


	Logger::console = Logger::openStaticOutputStream(std::cout);

	Logger::outputs[Logger::Level::ERROR] = Logger::openFileOutput("./errors.log");
	// or use: Logger::setAllOutputs to set every level

	// below this log level, everything will be printed and written to files
	Logger::logLevel = Logger::Level::VERBOSE2;

	mainLogger.log("This will get printed, but will not be written to errors.log");
	mainLogger.error("This is an error, in errors.log and the console");

	mainLogger.anonymous("This will be written to the console, but never to outputs");


	Logger::console = nullptr;
	mainLogger.anonymous("Nothing will appear in the console that is set to null");
}
```

### Integration with CMake FetchContent:
```cmake
Include(FetchContent)

FetchContent_Declare(
	logger
	GIT_REPOSITORY https://github.com/cccolda/logger.git
	GIT_TAG        main
	GIT_SHALLOW    TRUE
	GIT_PROGRESS   TRUE
)

FetchContent_MakeAvailable(logger)

target_link_libraries(my_target PUBLIC colda::logger)
```