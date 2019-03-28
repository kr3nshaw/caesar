#ifndef Cwav_hpp
#define Cwav_hpp

#include <cstdint>
#include <ios>
#include <string>
#include <vector>

typedef struct
{
	uint8_t PredScal;
	int16_t SampHist1;
	int16_t SampHist2;
} DspContext;

typedef struct
{
	uint8_t* Offset;

	uint8_t* SampOffset;
	uint32_t AdpcmType;
	uint8_t* AdpcmOffset;

	int16_t DspCoeffs[16];
	DspContext DspCntx;
	DspContext DspLoopCntx;

	std::vector<int16_t> PcmSamples;
} CwavChan;

typedef struct Cwav
{
	std::string FileName;
	std::streamoff Length;
	uint8_t* Data = nullptr;

	uint8_t SampleMode;

	Cwav(const char* fileName);
	~Cwav();
	bool Convert();
} Cwav;

#endif /* Cwav_hpp */
