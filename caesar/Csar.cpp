//
//  Csar.cpp
//  caesar
//
//  Created by Kai Renshaw on 6/6/18.
//  Copyright © 2018 Kai Renshaw. All rights reserved.
//

#include "Csar.hpp"
#include "Cbnk.hpp"
#include "Cgrp.hpp"
#include "Common.hpp"
#include "Cseq.hpp"
#include "Cwar.hpp"
#include <cstdint>
#include <fstream>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

using namespace std;

Csar::Csar(const char* fileName, bool p) : FileName(fileName), P(p)
{
	ifstream ifs(FileName, ios::binary | ios::ate);

	Length = ifs.tellg();
	Data = new uint8_t[Length];

	Common::Push(FileName, Data);

	ifs.seekg(0, ios::beg);
	ifs.read(reinterpret_cast<char*>(Data), Length);
	ifs.close();
}

Csar::~Csar()
{
	for (auto cwar : Cwars)
	{
		if (cwar)
		{
			delete cwar;
		}
	}

	Common::Pop();

	if (Data)
	{
		delete[] Data;
	}
}

bool Csar::Extract()
{
	uint8_t* pos = Data;

	if (!Common::Assert(pos, 0x43534152, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert(pos, 0xFEFF, ReadFixLen(pos, 2))) { return false; }
	if (!Common::Assert(pos, 0x40, ReadFixLen(pos, 2))) { return false; }

	uint32_t csarVersion = ReadFixLen(pos, 4);

	if (!Common::Assert<uint64_t>(pos, Length, ReadFixLen(pos, 4))) { return false; }
	if (!Common::Assert(pos, 0x3, ReadFixLen(pos, 4))) { return false; }
	if (!Common::Assert(pos, 0x2000, ReadFixLen(pos, 4))) { return false; }

	uint32_t strgOffset = ReadFixLen(pos, 4);
	uint32_t strgLength = ReadFixLen(pos, 4);

	if (!Common::Assert(pos, 0x2001, ReadFixLen(pos, 4))) { return false; }

	uint32_t infoOffset = ReadFixLen(pos, 4);
	uint32_t infoLength = ReadFixLen(pos, 4);

	if (!Common::Assert(pos, 0x2002, ReadFixLen(pos, 4))) { return false; }

	uint32_t fileOffset = ReadFixLen(pos, 4);
	uint32_t fileLength = ReadFixLen(pos, 4);

	pos = Data + strgOffset;

	if (!Common::Assert(pos, 0x53545247, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert<uint32_t>(pos, strgLength, ReadFixLen(pos, 4))) { return false; }
	if (!Common::Assert(pos, 0x2400, ReadFixLen(pos, 4))) { return false; }

	uint32_t strgStringsOffset = ReadFixLen(pos, 4);

	if (!Common::Assert(pos, 0x2401, ReadFixLen(pos, 4))) { return false; }

	uint32_t strgUnknownOffset = ReadFixLen(pos, 4);
	uint32_t strgCount = ReadFixLen(pos, 4);

	vector<CsarStrg> strgs;

	for (uint32_t i = 0; i < strgCount; ++i)
	{
		if (!Common::Assert(pos, 0x1F01, ReadFixLen(pos, 4))) { return false; }

		CsarStrg strg;
		strg.Offset = Data + strgOffset + 24 + ReadFixLen(pos, 4);
		strg.Length = ReadFixLen(pos, 4);

		strgs.push_back(strg);
	}

	for (uint32_t i = 0; i < strgCount; ++i)
	{
		strgs[i].String = string(reinterpret_cast<const char*>(pos), strgs[i].Length - 1);

		pos = i != (strgCount - 1) ? strgs[i + 1].Offset : pos + strgs[i].Length;
	}

	pos = Data + infoOffset;

	if (!Common::Assert(pos, 0x494E464F, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert<uint32_t>(pos, infoLength, ReadFixLen(pos, 4))) { return false; }

	uint32_t infoCseqOffset = 0;
	uint32_t infoCbnkOffset = 0;
	uint32_t infoPlayerOffset = 0;
	uint32_t infoCwarOffset = 0;
	uint32_t infoSetOffset = 0;
	uint32_t infoCgrpOffset = 0;
	uint32_t infoFileOffset = 0;
	uint32_t infoEndOffset = 0;

	for (uint8_t i = 0; i < 8; ++i)
	{
		switch (ReadFixLen(pos, 4))
		{
			case 0x2100:
				infoCseqOffset = ReadFixLen(pos, 4); break;

			case 0x2101:
				infoCbnkOffset = ReadFixLen(pos, 4); break;

			case 0x2102:
				infoPlayerOffset = ReadFixLen(pos, 4); break;

			case 0x2103:
				infoCwarOffset = ReadFixLen(pos, 4); break;

			case 0x2104:
				infoSetOffset = ReadFixLen(pos, 4); break;

			case 0x2105:
				infoCgrpOffset = ReadFixLen(pos, 4); break;

			case 0x2106:
				infoFileOffset = ReadFixLen(pos, 4); break;

			case 0x220B:
				infoEndOffset = ReadFixLen(pos, 4); break;

			default:
				return false;
		}
	}

	pos = Data + infoOffset + 8 + infoFileOffset;

	uint32_t fileCount = ReadFixLen(pos, 4);

	vector<uint8_t*> fileOffsets;

	for (uint32_t i = 0; i < fileCount; ++i)
	{
		if (!Common::Assert(pos, 0x220A, ReadFixLen(pos, 4))) { return false; }

		fileOffsets.push_back(Data + infoOffset + 8 + infoFileOffset + ReadFixLen(pos, 4));
	}

	vector<CsarFile> files;

	for (uint32_t i = 0; i < fileCount; ++i)
	{
		pos = fileOffsets[i];

		CsarFile file;

		switch (ReadFixLen(pos, 4))
		{
			case 0x220C:
			{
				if (!Common::Assert(pos, 0xC, ReadFixLen(pos, 8))) { return false; }
				Common::Analyse("0x220C 0x08", ReadFixLen(pos, 4));

				file.Offset = Data + fileOffset + 8 + ReadFixLen(pos, 4);
				file.Length = ReadFixLen(pos, 4);

				break;
			}

			case 0x220D:
			{
				if (!Common::Assert(pos, 0xC, ReadFixLen(pos, 8))) { return false; }

				while (*pos != 0x00)
				{
					file.Location += *pos++;
				}

				break;
			}

			case 0: // Actually 0xFFFFFFFF
				file.Offset = nullptr;
				file.Length = 0;

				break;

			default:
				return false;
		}

		files.push_back(file);
	}

	pos = Data + infoOffset + 8 + infoCwarOffset;

	uint32_t cwarCount = ReadFixLen(pos, 4);

	vector<uint8_t*> cwarOffsets;

	for (uint32_t i = 0; i < cwarCount; ++i)
	{
		if (!Common::Assert(pos, 0x2207, ReadFixLen(pos, 4))) { return false; }

		cwarOffsets.push_back(Data + infoOffset + 8 + infoCwarOffset + ReadFixLen(pos, 4));
	}

	for (uint32_t i = 0; i < cwarCount; ++i)
	{
		pos = cwarOffsets[i];

		uint32_t id = ReadFixLen(pos, 4);

		if (files[id].Offset != nullptr)
		{
			pos = files[id].Offset + 12;

			uint32_t cwarLength = ReadFixLen(pos, 4);

			pos -= 16;

#ifdef _WIN32
			_mkdir(to_string(i).c_str());
			_chdir(to_string(i).c_str());
#else
			mkdir(to_string(i).c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
			chdir(to_string(i).c_str());
#endif

			ofstream ofs(string(to_string(i) + ".cwar"), ofstream::binary);
			ofs.write(reinterpret_cast<const char*>(pos), cwarLength);
			ofs.close();

			Cwars.push_back(new Cwar(string(to_string(i) + ".cwar").c_str()));

			if (!Cwars[i]->Extract())
			{
				return false;
			}

#ifdef _WIN32
			_chdir("..");
#else
			chdir("..");
#endif
		}
		else
		{
			Cwars.push_back(nullptr);
		}
	}

	pos = Data + infoOffset + 8 + infoCbnkOffset;

	uint32_t cbnkCount = ReadFixLen(pos, 4);

	vector<CsarCbnk> cbnks;

	for (uint32_t i = 0; i < cbnkCount; ++i)
	{
		if (!Common::Assert(pos, 0x2206, ReadFixLen(pos, 4))) { return false; }

		CsarCbnk cbnk;
		cbnk.Offset = Data + infoOffset + 8 + infoCbnkOffset + ReadFixLen(pos, 4);

		cbnks.push_back(cbnk);
	}

	for (uint32_t i = 0; i < cbnkCount; ++i)
	{
		pos = cbnks[i].Offset;

		cbnks[i].Id = ReadFixLen(pos, 4);

		pos = cbnks[i].Offset + 16;

		cbnks[i].FileName = strgs[ReadFixLen(pos, 4)].String;

#ifdef _WIN32
		_mkdir(cbnks[i].FileName.c_str());
		_chdir(cbnks[i].FileName.c_str());
#else
		mkdir(cbnks[i].FileName.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
		chdir(cbnks[i].FileName.c_str());
#endif

		if (files[cbnks[i].Id].Offset != nullptr)
		{
			pos = files[cbnks[i].Id].Offset + 12;

			uint32_t cbnkLength = ReadFixLen(pos, 4);

			pos -= 16;

			ofstream ofs(string(cbnks[i].FileName + ".cbnk"), ofstream::binary);
			ofs.write(reinterpret_cast<const char*>(pos), cbnkLength);
			ofs.close();

			Cbnk cbnk(string(cbnks[i].FileName + ".cbnk").c_str(), Cwars, P);

			if (!cbnk.Convert(".."))
			{
				return false;
			}
		}

#ifdef _WIN32
		_chdir("..");
#else
		chdir("..");
#endif
	}

	pos = Data + infoOffset + 8 + infoCseqOffset;

	uint32_t cseqCount = ReadFixLen(pos, 4);

	vector<CsarCseq> cseqs;

	for (uint32_t i = 0; i < cseqCount; ++i)
	{
		if (!Common::Assert(pos, 0x2200, ReadFixLen(pos, 4))) { return false; }

		CsarCseq cseq;
		cseq.Offset = Data + infoOffset + 8 + infoCseqOffset + ReadFixLen(pos, 4);

		cseqs.push_back(cseq);
	}

	for (uint32_t i = 0; i < cseqCount; ++i)
	{
		pos = cseqs[i].Offset;

		uint32_t id = ReadFixLen(pos, 4);

		Common::Analyse("Cseq 0x04", ReadFixLen(pos, 4));
		Common::Analyse("Cseq 0x08", ReadFixLen(pos, 4));

		uint32_t type = ReadFixLen(pos, 4);
		uint32_t cbnkOffset = ReadFixLen(pos, 4);

		Common::Analyse("Cseq 0x14", ReadFixLen(pos, 4));

		cseqs[i].FileName = strgs[ReadFixLen(pos, 4)].String;

		switch (type)
		{
			// TODO (Medium): Implement ability to read external streams and CWSDs
			case 0x2201:
			case 0x2202:
				break;

			case 0x2203:
			{
				pos += cbnkOffset;

				uint32_t cbnk = ReadFixLen(pos, 2);

				pos = files[id].Offset + 12;

				uint32_t cseqLength = ReadFixLen(pos, 4);

				pos -= 16;

#ifdef _WIN32
				_chdir(cbnks[cbnk].FileName.c_str());
#else
				chdir(cbnks[cbnk].FileName.c_str());
#endif

				ofstream ofs(string(cseqs[i].FileName + ".cseq"), ofstream::binary);
				ofs.write(reinterpret_cast<const char*>(pos), cseqLength);
				ofs.close();

				Cseq cseq(string(cseqs[i].FileName + ".cseq").c_str());

				if (!cseq.Convert())
				{
					return false;
				}

#ifdef _WIN32
				_chdir("..");
#else
				chdir("..");
#endif

				break;
			}

			default:
				return false;
		}
	}

	pos = Data + infoOffset + 8 + infoPlayerOffset;

	uint32_t playerCount = ReadFixLen(pos, 4);

	vector<uint8_t*> playerOffsets;

	for (uint32_t i = 0; i < playerCount; ++i)
	{
		if (!Common::Assert(pos, 0x2209, ReadFixLen(pos, 4))) { return false; }

		playerOffsets.push_back(Data + infoOffset + 8 + infoPlayerOffset + ReadFixLen(pos, 4));
	}

	pos = Data + infoOffset + 8 + infoSetOffset;

	uint32_t setCount = ReadFixLen(pos, 4);

	vector<uint8_t*> setOffsets;

	for (uint32_t i = 0; i < setCount; ++i)
	{
		if (!Common::Assert(pos, 0x2204, ReadFixLen(pos, 4))) { return false; }

		setOffsets.push_back(Data + infoOffset + 8 + infoSetOffset + ReadFixLen(pos, 4));
	}

	pos = Data + infoOffset + 8 + infoCgrpOffset;

	uint32_t cgrpCount = ReadFixLen(pos, 4);

	vector<CsarCgrp> cgrps;

	for (uint32_t i = 0; i < cgrpCount; ++i)
	{
		if (!Common::Assert(pos, 0x2208, ReadFixLen(pos, 4))) { return false; }

		CsarCgrp cgrp;
		cgrp.Offset = Data + infoOffset + 8 + infoCgrpOffset + ReadFixLen(pos, 4);

		cgrps.push_back(cgrp);
	}

	for (uint32_t i = 0; i < cgrpCount; ++i)
	{
		pos = cgrps[i].Offset;

		cgrps[i].Id = ReadFixLen(pos, 4);

		if (!Common::Assert(pos, 0x1, ReadFixLen(pos, 4))) { return false; }

		cgrps[i].FileName = strgs[ReadFixLen(pos, 4)].String;

#ifdef _WIN32
		_mkdir(cgrps[i].FileName.c_str());
		_chdir(cgrps[i].FileName.c_str());
#else
		mkdir(cgrps[i].FileName.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
		chdir(cgrps[i].FileName.c_str());
#endif

		if (files[cgrps[i].Id].Offset != nullptr)
		{
			pos = files[cgrps[i].Id].Offset + 12;

			uint32_t cgrpLength = ReadFixLen(pos, 4);

			pos -= 16;

			ofstream ofs(string(cgrps[i].FileName + ".cgrp"), ofstream::binary);
			ofs.write(reinterpret_cast<const char*>(pos), cgrpLength);
			ofs.close();

			Cgrp cgrp(string(cgrps[i].FileName + ".cgrp").c_str(), P);

			if (!cgrp.Extract())
			{
				return false;
			}
		}

#ifdef _WIN32
		_chdir("..");
#else
		chdir("..");
#endif
	}

	Common::Dump();

	return true;
}
