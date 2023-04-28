cmake_minimum_required(VERSION 3.0.0)
project(logger
	VERSION     1.0.0
	DESCRIPTION "Logger utility for the Neon engine"
	LANGUAGES   "CXX"
)

option(LOGGER_BUILD_TEST "Build the tests using Catch2" OFF)
option(LOGGER_CONSOLE_ATTRIBUTES_WIN32 "Use SetConsoleTextAttribute on Windows" OFF)

if(LOGGER_CONSOLE_ATTRIBUTES_WIN32 AND NOT WIN32)
	message(WARNING "LOGGER_CONSOLE_ATTRIBUTES_WIN32 is intended for Win32.")
endif()

set(LOGGER_INC_DIR include)
set(LOGGER_SRC_DIR src)
set(LOGGER_TST_DIR tests)

set(LOGGER_INC_FILES
	${LOGGER_INC_DIR}/Logger.h
)

set(LOGGER_SRC_FILES
	${LOGGER_SRC_DIR}/Logger.cpp
)

set(LOGGER_TST_FILES
	${LOGGER_TST_DIR}/test.cpp
)

add_library(logger STATIC)
add_library(colda::logger ALIAS logger)

if (LOGGER_CONSOLE_ATTRIBUTES_WIN32)
	target_compile_definitions(logger PRIVATE LOGGER_CONSOLE_ATTRIBUTES=1)
endif()

target_include_directories(logger PUBLIC ${LOGGER_INC_DIR})
target_compile_features(logger PRIVATE cxx_std_17)
target_sources(logger PRIVATE ${LOGGER_INC_FILES} ${LOGGER_SRC_FILES})

if (LOGGER_BUILD_TEST)

	Include(FetchContent)

	FetchContent_Declare(
		catch2
		GIT_REPOSITORY https://github.com/catchorg/Catch2.git
		GIT_TAG        v3.0.1
		GIT_SHALLOW    TRUE
		GIT_PROGRESS   TRUE
	)

	FetchContent_MakeAvailable(catch2)

	add_executable(logger_test)
	target_sources(logger_test PRIVATE ${LOGGER_TST_FILES})
	target_compile_features(logger_test PRIVATE cxx_std_17)
	target_link_libraries(logger_test PRIVATE colda::logger Catch2::Catch2WithMain)

	list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
	include(CTest)
	include(Catch)

	catch_discover_tests(logger_test)

endif()