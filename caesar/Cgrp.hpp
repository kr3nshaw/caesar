//
//  Cgrp.hpp
//  caesar
//
//  Created by Kai Renshaw on 24/6/18.
//  Copyright © 2018 Kai Renshaw. All rights reserved.
//

#ifndef Cgrp_hpp
#define Cgrp_hpp

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
	bool P;

	Cgrp(const char* fileName, bool p);
	~Cgrp();
	bool Extract();
} Cgrp;

#endif /* Cgrp_hpp */