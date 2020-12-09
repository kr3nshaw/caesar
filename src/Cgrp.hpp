#pragma once

#include "Cbnk.hpp"
#include "Cseq.hpp"
#include "Cwar.hpp"

#include <cstdint>
#include <ios>
#include <map>
#include <string>
#include <vector>

struct CgrpFile
{
	uint32_t Id;
	uint8_t* Offset;
	uint32_t Length;
};

struct Cgrp
{
	std::string FileName;
	std::streamoff Length;
	uint8_t* Data = nullptr;

	std::map<int, Cwar*>* Cwars;
	std::vector<Cbnk*> Cbnks;
	std::vector<Cseq*> Cseqs;
	std::map<int, bool> CseqsFromCsar;
	bool P;

	Cgrp(const char* fileName, std::map<int, Cwar*>* cwars, const std::map<int, bool>& cseqsFromCsar, bool p);
	~Cgrp();
	bool Extract();
};
