#pragma once

#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

int32_t ReadFixLen(uint8_t*& pos, size_t bytes, bool littleEndian = true, bool isSigned = false);
int32_t ReadVarLen(uint8_t*& pos);

struct Common
{
	static bool ShowWarnings;
	static std::stack<std::string> FileNames;
	static std::stack<uint8_t*> Offsets;
	static std::vector<std::string> Log;

	template<typename T>
	static bool Assert(uint8_t* pos, T expected, T found)
	{
		if (found != expected)
		{
			std::cerr << std::hex << std::setfill('0') << std::uppercase << std::endl;
			std::cerr << "ERROR IN\t" << Common::FileNames.top() << std::endl;
			std::cerr << "AT POSITION\t0x" << std::setw(8) << pos - Common::Offsets.top() << std::endl;
			std::cerr << "EXPECTED\t0x" << std::setw(8) << expected << std::endl;
			std::cerr << "INSTEAD GOT\t0x" << std::setw(8) << found << std::endl;
			std::cerr << std::endl;

			return false;
		}

		return true;
	}

	template<typename T>
	static void Error(uint8_t* pos, std::string expected, T found)
	{
		std::cerr << std::hex << std::setfill('0') << std::uppercase << std::endl;
		std::cerr << "ERROR IN\t" << Common::FileNames.top() << std::endl;
		std::cerr << "AT POSITION\t0x" << std::setw(8) << pos - Common::Offsets.top() << std::endl;
		std::cerr << "EXPECTED\t" << expected << std::endl;
		std::cerr << "INSTEAD GOT\t0x" << std::setw(8) << found << std::endl;
		std::cerr << std::endl;
	}

	static void Warning(uint8_t* pos, std::string msg);
	static void Push(std::string fileName, uint8_t* data);
	static void Pop();
	static void Analyse(std::string tag, uint32_t val);
	static void Dump(std::string fileName);
};
