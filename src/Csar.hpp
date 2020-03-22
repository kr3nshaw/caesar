#ifndef Csar_hpp
#define Csar_hpp

#include "Cwar.hpp"
#include <cstdint>
#include <ios>
#include <map>
#include <string>

typedef struct
{
	uint8_t* Offset;
	uint32_t Length;

	std::string String;
} CsarStrg;

typedef struct
{
	uint8_t* Offset;
	uint32_t Length;

	std::string Location = "";
} CsarFile;

typedef struct
{
	uint8_t* Offset;

	uint32_t Id;
	std::string FileName;
} CsarCbnk;

typedef struct
{
	uint8_t* Offset;

	std::string FileName;
} CsarCseq;

typedef struct
{
	uint8_t* Offset;

	uint32_t Id;
	std::string FileName;
} CsarCgrp;

typedef struct Csar
{
	std::string FileName;
	std::streamoff Length;
	uint8_t* Data = nullptr;

	std::map<int, Cwar*> Cwars;
	bool P;

	Csar(const char* fileName, bool p);
	~Csar();
	bool Extract();
} Csar;

#endif /* Csar_hpp */
