#pragma once

#include <cstdint>
#include <ios>
#include <string>
#include <vector>

struct DspContext
{
	uint8_t PredScal;
	int16_t SampHist1;
	int16_t SampHist2;
};

struct CwavChan
{
	uint8_t* Offset;

	uint8_t* SampOffset;
	uint32_t AdpcmType;
	uint8_t* AdpcmOffset;

	int16_t DspCoeffs[16];
	DspContext DspCntx;
	DspContext DspLoopCntx;

	std::vector<int16_t> PcmSamples;
};

struct Cwav
{
	std::string FileName;
	std::streamoff Length;
	uint8_t* Data = nullptr;

	uint8_t SampleMode;

	Cwav(const char* fileName);
	~Cwav();
	bool Convert();
};
