#include "Logger.h"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

TEST_CASE("Logger", "[Logger]")
{
	SECTION("console initialization")
	{
		Logger::console = Logger::openStaticOutputStream(std::cout);

		REQUIRE(Logger::console != nullptr);
	}

	SECTION("file output initialization")
	{
		char outputText[17] = {};
		for (int i = 0; i < 16; ++i)
			outputText[i] = GENERATE('A', 'z');

		outputText[17] = '\0';

		{
			auto fileOutput = Logger::openFileOutput("./logger_test_file.tmp");

			REQUIRE(fileOutput != nullptr);

			fileOutput->write(outputText, 16);
		}

		std::ifstream file("./logger_test_file.tmp", std::ios_base::in);

		REQUIRE(file.is_open());

		char inputText[17] = {};

		file.read(inputText, 16);
		inputText[16] = '\0';
		file.close();

		REQUIRE_THAT(inputText, Catch::Matchers::Equals(outputText));

		REQUIRE(std::filesystem::remove("./logger_test_file.tmp"));
	}

	SECTION("logging")
	{
		std::stringstream ss;

		Logger::setAllOutputs(Logger::openStaticOutputStream(ss));

		Logger logger("test");

		logger.log("Hello!");

		ss.clear();

		REQUIRE_THAT(ss.str(), Catch::Matchers::Matches("[0-9]{4}\\-[0-9]{2}\\-[0-9]{2} ..?\\:..\\:.. \\[test\\] INF: Hello\\!\\s*"));

		ss.str("");
		ss.clear();

		logger.warning("Hello!");

		REQUIRE_THAT(ss.str(), Catch::Matchers::Matches("[0-9]{4}\\-[0-9]{2}\\-[0-9]{2} ..?\\:..\\:.. \\[test\\] WRN: Hello\\!\\s*"));

		ss.str("");
		ss.clear();

		logger.error("Hello!");

		REQUIRE_THAT(ss.str(), Catch::Matchers::Matches("[0-9]{4}\\-[0-9]{2}\\-[0-9]{2} ..?\\:..\\:.. \\[test\\] ERR: Hello\\!\\s*"));

		ss.str("");
		ss.clear();
	}
}
