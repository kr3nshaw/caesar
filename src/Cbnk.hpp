#pragma once

#include "Cwar.hpp"

#include <ios>
#include <cstdint>
#include <string>
#include <map>
#include <vector>

struct CbnkCwav
{
	uint32_t Cwar;
	uint32_t Id;
	uint32_t Key;
	
	uint16_t ChanCount;
	uint32_t SampleRate;

	std::vector<int16_t> LeftSamples;
	std::vector<int16_t> RightSamples;

	bool Loop = false;
	uint32_t LoopStart;
	uint32_t LoopEnd;
};

struct WaveSmpl
{
	uint32_t CuePointId;
	uint32_t Type;
	uint32_t Start;
	uint32_t End;
	uint32_t Fraction;
	uint32_t PlayCount;
};

struct CbnkNote
{
	bool Exists = true;
	uint8_t* Offset;

	CbnkCwav* Cwav;
	uint8_t StartNote;
	uint8_t EndNote;
	uint32_t RootKey;
	uint32_t Volume;
	uint32_t Pan;
	uint8_t Interpolation;
	uint8_t Attack;
	uint8_t Decay;
	uint8_t Sustain;
	uint8_t Hold;
	uint8_t Release;
};

struct CbnkInst
{
	bool Exists = true;
	uint8_t* Offset;

	uint32_t NoteCount;
	std::vector<CbnkNote> Notes;

	bool IsDrumKit = false;
};

struct Cbnk
{
	std::string FileName;
	std::streamoff Length;
	uint8_t* Data = nullptr;

	std::map<int, Cwar*>* Cwars;
	bool P;

	Cbnk(const char* fileName, std::map<int, Cwar*>* cwars, bool p);
	~Cbnk();
	bool Convert(std::string cwarPath);
};
