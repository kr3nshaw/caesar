#ifndef Cwar_hpp
#define Cwar_hpp

#include "Cwav.hpp"
#include <cstdint>
#include <ios>
#include <string>
#include <vector>

typedef struct
{
	uint8_t* Offset;
	uint32_t Length;
} CwarCwav;

typedef struct Cwar
{
	std::string FileName;
	std::streamoff Length;
	uint8_t* Data = nullptr;

	std::vector<Cwav*> Cwavs;

	Cwar(const char* fileName);
	~Cwar();
	bool Extract();
} Cwar;

#endif /* Cwar_hpp */
