//
//  Csar.hpp
//  caesar
//
//  Created by Kai Renshaw on 6/6/18.
//  Copyright © 2018 Kai Renshaw. All rights reserved.
//

#ifndef Csar_hpp
#define Csar_hpp

#include "Cwar.hpp"
#include <cstdint>
#include <ios>
#include <string>
#include <vector>

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

	std::vector<Cwar*> Cwars;
	bool P;

	Csar(const char* fileName, bool p);
	~Csar();
	bool Extract();
} Csar;

#endif /* Csar_hpp */
