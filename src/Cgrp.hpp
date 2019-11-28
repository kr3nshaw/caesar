#ifndef Cgrp_hpp
#define Cgrp_hpp

#include "Cbnk.hpp"
#include "Cseq.hpp"
#include "Cwar.hpp"
#include <cstdint>
#include <ios>
#include <string>

typedef struct
{
	uint32_t Id;
	uint8_t* Offset;
	uint32_t Length;
} CgrpFile;

typedef struct Cgrp
{
	std::string FileName;
	std::streamoff Length;
	uint8_t* Data = nullptr;

	std::vector<Cwar*> Cwars;
	std::vector<Cbnk*> Cbnks;
	std::vector<Cseq*> Cseqs;
	bool P;

	Cgrp(const char* fileName, bool p);
	~Cgrp();
	bool Extract();
} Cgrp;

#endif /* Cgrp_hpp */
