#ifndef Cbnk_hpp
#define Cbnk_hpp

#include "Cwar.hpp"
#include <ios>
#include <cstdint>
#include <string>
#include <map>
#include <vector>

typedef struct
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
} CbnkCwav;

typedef struct
{
	uint32_t CuePointId;
	uint32_t Type;
	uint32_t Start;
	uint32_t End;
	uint32_t Fraction;
	uint32_t PlayCount;
} WaveSmpl;

typedef struct
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
} CbnkNote;

typedef struct
{
	bool Exists = true;
	uint8_t* Offset;

	uint32_t NoteCount;
	std::vector<CbnkNote> Notes;

	bool IsDrumKit = false;
} CbnkInst;

typedef struct BnkToSf2
{
	static double AttackTable[128];
	static double HoldTable[128];
	static double DecayTable[128];

	static double ChangeLogBase(double x, double base);
	static double ConvertTime(double time);
	static double ConvertVolume(uint32_t volume);
	static double ConvertPan(uint32_t pan);
	static double ConvertAttack(uint8_t attack);
	static double ConvertHold(uint8_t hold);
	static double ConvertDecay(uint8_t decay, uint8_t sustain);
	static double ConvertRelease(uint8_t release, uint8_t sustain);
	static double ConvertSustain(uint8_t sustain);
} BnkToSf2;

typedef struct Cbnk
{
	std::string FileName;
	std::streamoff Length;
	uint8_t* Data = nullptr;

	std::map<int, Cwar*>* Cwars;
	bool P;

	Cbnk(const char* fileName, std::map<int, Cwar*>* cwars, bool p);
	~Cbnk();
	bool Convert(std::string cwarPath);
} Cbnk;

#endif /* Cbnk_hpp */
