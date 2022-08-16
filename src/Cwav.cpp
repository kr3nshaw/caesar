#include "Cwav.hpp"
#include "Common.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <vector>

using namespace std;

const int8_t nibbles[] = { 0, 1, 2, 3, 4, 5, 6, 7, -8, -7, -6, -5, -4, -3, -2, -1 };

Cwav::Cwav(const char* fileName) : FileName(fileName)
{
	ifstream ifs(FileName, ios::binary | ios::ate);

	Length = ifs.tellg();
	Data = new uint8_t[Length];

	Common::Push(FileName, Data);

	ifs.seekg(0, ios::beg);
	ifs.read(reinterpret_cast<char*>(Data), Length);
	ifs.close();
}

Cwav::~Cwav()
{
	Common::Pop();

	delete[] Data;
}

bool Cwav::Convert()
{
	uint8_t* pos = Data;

	if (!Common::Assert(pos, 0x43574156, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert(pos, 0xFEFF, ReadFixLen(pos, 2))) { return false; }
	if (!Common::Assert(pos, 0x40, ReadFixLen(pos, 2))) { return false; }

	uint32_t cwavVersion = ReadFixLen(pos, 4);

	if (!Common::Assert<uint64_t>(pos, Length, ReadFixLen(pos, 4))) { return false; }
	if (!Common::Assert(pos, 0x2, ReadFixLen(pos, 4))) { return false; }
	if (!Common::Assert(pos, 0x7000, ReadFixLen(pos, 4))) { return false; }

	uint32_t infoOffset = ReadFixLen(pos, 4);
	uint32_t infoLength = ReadFixLen(pos, 4);

	if (!Common::Assert(pos, 0x7001, ReadFixLen(pos, 4))) { return false; }

	uint32_t dataOffset = ReadFixLen(pos, 4);
	uint32_t dataLength = ReadFixLen(pos, 4);

	pos = Data + infoOffset;

	if (!Common::Assert(pos, 0x494E464F, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert<uint32_t>(pos, infoLength, ReadFixLen(pos, 4))) { return false; }

	uint8_t codec = ReadFixLen(pos, 1);
	SampleMode = ReadFixLen(pos, 1);

	if (!Common::Assert(pos, 0x0, ReadFixLen(pos, 2))) { return false; }

	uint32_t sampleRate = ReadFixLen(pos, 4);
	uint32_t loopStart = ReadFixLen(pos, 4);
	uint32_t loopEnd = ReadFixLen(pos, 4);
	uint32_t unalignedLoopStart = ReadFixLen(pos, 4);
	uint16_t chanCount = ReadFixLen(pos, 2);

	if (!Common::Assert(pos, 0x0, ReadFixLen(pos, 2))) { return false; }

	vector<CwavChan> chans;

	for (uint16_t i = 0; i < chanCount; ++i)
	{
		if (!Common::Assert(pos, 0x7100, ReadFixLen(pos, 4))) { return false; }

		CwavChan chan;
		chan.Offset = Data + infoOffset + 28 + ReadFixLen(pos, 4);

		chans.push_back(chan);
	}

	for (uint16_t i = 0; i < chanCount; ++i)
	{
		pos = chans[i].Offset;

		if (!Common::Assert(pos, 0x1F00, ReadFixLen(pos, 4))) { return false; }

		chans[i].SampOffset = Data + dataOffset + 8 + ReadFixLen(pos, 4);
		chans[i].AdpcmType = ReadFixLen(pos, 4);
		uint32_t adpcmOffset = ReadFixLen(pos, 4);

		switch (codec)
		{
			case 0:
			{
				pos = chans[i].SampOffset;

				for (uint32_t j = 0; j < loopEnd; ++j)
				{
					chans[i].PcmSamples.push_back(ReadFixLen(pos, 1) << 8);
				}

				break;
			}

			case 1:
			{
				pos = chans[i].SampOffset;

				for (uint32_t j = 0; j < loopEnd; ++j)
				{
					chans[i].PcmSamples.push_back(ReadFixLen(pos, 2, true, true));
				}

				break;
			}

			case 2:
			{
				chans[i].AdpcmOffset = chans[i].Offset + adpcmOffset;

				pos = chans[i].AdpcmOffset;

				for (uint8_t j = 0; j < 16; ++j)
				{
					chans[i].DspCoeffs[j] = ReadFixLen(pos, 2, true, true);
				}

				DspContext dspCntx{};
				dspCntx.PredScal = ReadFixLen(pos, 1);

				if (!Common::Assert(pos, 0x0, ReadFixLen(pos, 1))) { return false; }

				dspCntx.SampHist1 = ReadFixLen(pos, 2, true, true);
				dspCntx.SampHist2 = ReadFixLen(pos, 2, true, true);

				DspContext dspLoopCntx{};
				dspLoopCntx.PredScal = ReadFixLen(pos, 1);

				if (!Common::Assert(pos, 0x0, ReadFixLen(pos, 1))) { return false; }

				dspLoopCntx.SampHist1 = ReadFixLen(pos, 2, true, true);
				dspLoopCntx.SampHist2 = ReadFixLen(pos, 2, true, true);

				chans[i].DspCntx = dspCntx;
				chans[i].DspLoopCntx = dspLoopCntx;

				pos = chans[i].SampOffset;

				int8_t predScal = chans[i].DspCntx.PredScal;
				int16_t hist1 = chans[i].DspCntx.SampHist1;
				int16_t hist2 = chans[i].DspCntx.SampHist2;

				for (uint32_t j = 0; j < ceil(loopEnd / 14.0f); ++j)
				{
					predScal = ReadFixLen(pos, 1);
					int32_t pred = (predScal >> 4) & 0xF;
					int32_t scal = 1 << (predScal & 0xF);
					int16_t coef1 = chans[i].DspCoeffs[pred * 2];
					int16_t coef2 = chans[i].DspCoeffs[(pred * 2) + 1];

					uint32_t samplesToRead = min<uint32_t>(14, loopEnd - chans[i].PcmSamples.size());

					for (uint32_t k = 0; k < samplesToRead; ++k)
					{
						int32_t adpcm = k % 2 == 0 ? nibbles[*pos >> 4] : nibbles[ReadFixLen(pos, 1) & 0xF];
						int32_t distance = (scal * adpcm) << 11;
						int32_t predicted = (coef1 * hist1) + (coef2 * hist2);
						int32_t corrected = predicted + distance;
						int32_t scaled = (corrected + 1024) >> 11;

						if (scaled < -32768)
						{
							chans[i].PcmSamples.push_back(-32768);
						}
						else if (scaled > 32767)
						{
							chans[i].PcmSamples.push_back(32767);
						}
						else
						{
							chans[i].PcmSamples.push_back(scaled);
						}

						hist2 = hist1;
						hist1 = chans[i].PcmSamples.back();
					}
				}

				break;
			}

			case 3:
			{
				Common::Warning(Data + infoOffset + 8, "IMA ADPCM decoding not implemented");

				break;
			}

			default:
			{
				Common::Error(Data + infoOffset + 8, "A valid codec identifier", codec);

				return false;
			}
		}
	}

	uint32_t fmtLength = 16;
	uint16_t waveCodec = 1;
	uint16_t bitsPerSample = 16;
	uint32_t byteRate = (sampleRate * chanCount) * (bitsPerSample / 8);
	uint16_t blockAlign = chanCount * (bitsPerSample / 8);
	uint32_t waveDataLength = (chans[0].PcmSamples.size() * chanCount) * (bitsPerSample / 8);
	uint32_t length = 36 + waveDataLength;

	uint32_t smplLength = 60;
	uint32_t zero = 0;
	uint32_t sampleLoops = 1;

	if ((SampleMode % 2) != 0)
	{
		length += 8 + smplLength;
	}

	ofstream ofs(FileName.substr(0, FileName.length() - 5).append("wav"), ofstream::binary);

	ofs.write("RIFF", 4);
	ofs.write(reinterpret_cast<const char*>(&length), 4);
	ofs.write("WAVE", 4);
	ofs.write("fmt ", 4);
	ofs.write(reinterpret_cast<const char*>(&fmtLength), 4);
	ofs.write(reinterpret_cast<const char*>(&waveCodec), 2);
	ofs.write(reinterpret_cast<const char*>(&chanCount), 2);
	ofs.write(reinterpret_cast<const char*>(&sampleRate), 4);
	ofs.write(reinterpret_cast<const char*>(&byteRate), 4);
	ofs.write(reinterpret_cast<const char*>(&blockAlign), 2);
	ofs.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
	ofs.write("data", 4);
	ofs.write(reinterpret_cast<const char*>(&waveDataLength), 4);

	for (size_t i = 0; i < chans[0].PcmSamples.size(); ++i)
	{
		for (uint16_t j = 0; j < chanCount; ++j)
		{
			ofs.write(reinterpret_cast<const char*>(&chans[j].PcmSamples[i]), 2);
		}
	}

	if ((SampleMode % 2) != 0)
	{
		ofs.write("smpl", 4);
		ofs.write(reinterpret_cast<const char*>(&smplLength), 4);

		for (uint8_t i = 0; i < 7; ++i)
		{
			ofs.write(reinterpret_cast<const char*>(&zero), 4);
		}

		ofs.write(reinterpret_cast<const char*>(&sampleLoops), 4);

		for (uint8_t i = 0; i < 3; ++i)
		{
			ofs.write(reinterpret_cast<const char*>(&zero), 4);
		}

		ofs.write(reinterpret_cast<const char*>(&loopStart), 4);
		ofs.write(reinterpret_cast<const char*>(&loopEnd), 4);

		for (uint8_t i = 0; i < 2; ++i)
		{
			ofs.write(reinterpret_cast<const char*>(&zero), 4);
		}
	}

	ofs.close();

	return true;
}
