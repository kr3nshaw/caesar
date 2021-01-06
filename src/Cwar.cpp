#include "Cwar.hpp"
#include "Common.hpp"
#include "Cwav.hpp"

#include <fstream>
#include <string>
#include <vector>

using namespace std;

Cwar::Cwar(const char* fileName) : FileName(fileName)
{
	ifstream ifs(FileName, ios::binary | ios::ate);

	Length = ifs.tellg();
	Data = new uint8_t[Length];

	Common::Push(FileName, Data);

	ifs.seekg(0, ios::beg);
	ifs.read(reinterpret_cast<char*>(Data), Length);
	ifs.close();
}

Cwar::~Cwar()
{
	for (auto cwav : Cwavs)
	{
		delete cwav;
	}

	Common::Pop();

	delete[] Data;
}

bool Cwar::Extract()
{
	uint8_t* pos = Data;

	if (!Common::Assert(pos, 0x43574152, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert(pos, 0xFEFF, ReadFixLen(pos, 2))) { return false; }
	if (!Common::Assert(pos, 0x40, ReadFixLen(pos, 2))) { return false; }

	uint32_t cwarVersion = ReadFixLen(pos, 4);

	if (!Common::Assert<uint64_t>(pos, Length, ReadFixLen(pos, 4))) { return false; }
	if (!Common::Assert(pos, 0x2, ReadFixLen(pos, 4))) { return false; }
	if (!Common::Assert(pos, 0x6800, ReadFixLen(pos, 4))) { return false; }

	uint32_t infoOffset = ReadFixLen(pos, 4);
	uint32_t infoLength = ReadFixLen(pos, 4);

	if (!Common::Assert(pos, 0x6801, ReadFixLen(pos, 4))) { return false; }

	uint32_t fileOffset = ReadFixLen(pos, 4);
	uint32_t fileLength = ReadFixLen(pos, 4);

	pos = Data + infoOffset;

	if (!Common::Assert(pos, 0x494E464F, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert<uint32_t>(pos, infoLength, ReadFixLen(pos, 4))) { return false; }

	uint32_t cwavCount = ReadFixLen(pos, 4);

	vector<CwarCwav> cwavs;

	for (uint32_t i = 0; i < cwavCount; ++i)
	{
		if (!Common::Assert(pos, 0x1F00, ReadFixLen(pos, 4))) { return false; }

		CwarCwav cwav{};
		cwav.Offset = Data + fileOffset + 8 + ReadFixLen(pos, 4);
		cwav.Length = ReadFixLen(pos, 4);

		cwavs.push_back(cwav);
	}

	pos = Data + fileOffset;

	if (!Common::Assert(pos, 0x46494C45, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert<uint32_t>(pos, fileLength, ReadFixLen(pos, 4))) { return false; }

	for (uint32_t i = 0; i < cwavCount; ++i)
	{
		ofstream ofs(string(to_string(i) + ".bcwav"), ofstream::binary);
		ofs.write(reinterpret_cast<const char*>(cwavs[i].Offset), cwavs[i].Length);
		ofs.close();

		Cwavs.push_back(new Cwav(string(to_string(i) + ".bcwav").c_str()));

		if (!Cwavs[i]->Convert())
		{
			return false;
		}
	}

	return true;
}
