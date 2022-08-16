#pragma once

#include "Cwav.hpp"

#include <cstdint>
#include <ios>
#include <string>
#include <vector>

struct CwarCwav
{
	uint8_t* Offset;
	uint32_t Length;
};

struct Cwar
{
	std::string FileName;
	std::streamoff Length;
	uint8_t* Data = nullptr;

	std::vector<Cwav*> Cwavs;

	Cwar(const char* fileName);
	~Cwar();
	bool Extract();
};
