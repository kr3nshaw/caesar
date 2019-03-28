#include "Cgrp.hpp"
#include "Cbnk.hpp"
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

Cgrp::Cgrp(const char* fileName, bool p) : FileName(fileName), P(p)
{
	ifstream ifs(FileName, ios::binary | ios::ate);

	Length = ifs.tellg();
	Data = new uint8_t[Length];

	Common::Push(FileName, Data);

	ifs.seekg(0, ios::beg);
	ifs.read(reinterpret_cast<char*>(Data), Length);
	ifs.close();
}

Cgrp::~Cgrp()
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

bool Cgrp::Extract()
{
	uint8_t* pos = Data;

	if (!Common::Assert(pos, 0x43475250, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert(pos, 0xFEFF, ReadFixLen(pos, 2))) { return false; }
	if (!Common::Assert(pos, 0x40, ReadFixLen(pos, 2))) { return false; }

	uint32_t cgrpVersion = ReadFixLen(pos, 4);

	if (!Common::Assert<uint64_t>(pos, Length, ReadFixLen(pos, 4))) { return false; }

	uint32_t chunkCount = ReadFixLen(pos, 4);

	uint32_t infoOffset = 0;
	uint32_t infoLength = 0;

	uint32_t fileOffset = 0;
	uint32_t fileLength = 0;

	uint32_t infxOffset = 0;
	uint32_t infxLength = 0;

	for (uint32_t i = 0; i < chunkCount; ++i)
	{
		uint32_t chunkId = ReadFixLen(pos, 4);

		switch (chunkId)
		{
			case 0x7800:
				infoOffset = ReadFixLen(pos, 4);
				infoLength = ReadFixLen(pos, 4);

				break;

			case 0x7801:
				fileOffset = ReadFixLen(pos, 4);
				fileLength = ReadFixLen(pos, 4);

				break;

			case 0x7802:
				infxOffset = ReadFixLen(pos, 4);
				infxLength = ReadFixLen(pos, 4);

				break;

			default:
			{
				Common::Error(pos - 4, "A valid chunk type", chunkId);

				return false;
			}
		}
	}

	pos = Data + infoOffset;

	if (!Common::Assert(pos, 0x494E464F, ReadFixLen(pos, 4, false))) { return false; }
	
	Common::Analyse("Cgrp 0x44", ReadFixLen(pos, 4));

	uint32_t fileCount = ReadFixLen(pos, 4);

	vector<uint8_t*> fileOffsets;

	for (uint32_t i = 0; i < fileCount; ++i)
	{
		if (!Common::Assert(pos, 0x7900, ReadFixLen(pos, 4))) { return false; }

		fileOffsets.push_back(Data + infoOffset + 8 + ReadFixLen(pos, 4));
	}

	vector<CgrpFile> files;

	for (uint32_t i = 0; i < fileCount; ++i)
	{
		CgrpFile file;
		file.Id = ReadFixLen(pos, 4);
		file.Offset = ReadFixLen(pos, 4) == 0x1F00 ? Data + fileOffset + 8 + ReadFixLen(pos, 4) : nullptr;
		file.Length = ReadFixLen(pos, 4);

		files.push_back(file);
	}

	for (uint32_t i = 0; i < fileCount; ++i)
	{
		if (files[i].Offset == nullptr)
		{
			continue;
		}

		pos = files[i].Offset;

		uint32_t fileId = ReadFixLen(pos, 4, false);

		switch (fileId)
		{
			// TODO (High): The CWARs have IDs, because they're referenced in the CBNKs...where do these IDs come from?
			case 0x43574152:
			{
				pos += 8;

				uint32_t cwarLength = ReadFixLen(pos, 4);

				pos -= 16;

#ifdef _WIN32
				_mkdir(to_string(files[i].Id).c_str());
				_chdir(to_string(files[i].Id).c_str());
#else
				mkdir(to_string(files[i].Id).c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
				chdir(to_string(files[i].Id).c_str());
#endif

				ofstream ofs(string(to_string(files[i].Id) + ".cwar"), ofstream::binary);
				ofs.write(reinterpret_cast<const char*>(pos), cwarLength);
				ofs.close();

				Cwars.push_back(new Cwar(string(to_string(files[i].Id) + ".cwar").c_str()));

				if (!Cwars.back()->Extract())
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

			case 0x43424E4B:
			{
				pos += 8;

				uint32_t cbnkLength = ReadFixLen(pos, 4);

				pos -= 16;

#ifdef _WIN32
				_mkdir(to_string(files[i].Id).c_str());
				_chdir(to_string(files[i].Id).c_str());
#else
				mkdir(to_string(files[i].Id).c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
				chdir(to_string(files[i].Id).c_str());
#endif

				ofstream ofs(string(to_string(files[i].Id) + ".cbnk"), ofstream::binary);
				ofs.write(reinterpret_cast<const char*>(pos), cbnkLength);
				ofs.close();

				Cbnk cbnk(string(to_string(files[i].Id) + ".cbnk").c_str(), Cwars, P);

				if (!cbnk.Convert(".."))
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

			// TODO (High): Where is the bank for each sequence stored?
			case 0x43534551:
			{
				pos += 8;

				uint32_t cseqLength = ReadFixLen(pos, 4);

				pos -= 16;

/*#ifdef _WIN32
				_chdir(cbnks[cbnk].FileName.c_str());
#else
				chdir(cbnks[cbnk].FileName.c_str());
#endif*/

				ofstream ofs(string(to_string(files[i].Id) + ".cseq"), ofstream::binary);
				ofs.write(reinterpret_cast<const char*>(pos), cseqLength);
				ofs.close();

				Cseq cseq(string(to_string(files[i].Id) + ".cseq").c_str());

				if (!cseq.Convert())
				{
					return false;
				}

/*#ifdef _WIN32
				_chdir("..");
#else
				chdir("..");
#endif*/

				break;
			}

			case 0x43575344:
			{
				// TODO (Medium): Implement CWSD conversion

				break;
			}

			default:
			{
				Common::Error(pos - 4, "A valid file type", fileId);

				return false;
			}
		}
	}

	// TODO (Medium): Process INFX chunk

	return true;
}
