#pragma once

#include "Cwar.hpp"

#include <cstdint>
#include <ios>
#include <map>
#include <string>

struct CsarStrg
{
	uint8_t* Offset;
	uint32_t Length;

	std::string String;
};

struct CsarFile
{
	uint8_t* Offset;
	uint32_t Length;

	std::string Location = "";
};

struct CsarCbnk
{
	uint8_t* Offset;

	uint32_t Id;
	std::string FileName;
};

struct CsarCseq
{
	uint8_t* Offset;

	std::string FileName;
};

struct CsarCgrp
{
	uint8_t* Offset;

	uint32_t Id;
	std::string FileName;
};

struct Csar
{
	std::string FileName;
	std::streamoff Length;
	uint8_t* Data = nullptr;

	std::map<int, Cwar*> Cwars;
	bool P;

	Csar(const char* fileName, bool p);
	~Csar();
	bool Extract();
};
