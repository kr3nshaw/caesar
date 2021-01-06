#include "Cbnk.hpp"
#include "Common.hpp"
#include "Cwar.hpp"

#include <sf2cute.hpp>

#include <cmath>
#include <fstream>
#include <ios>
#include <map>
#include <memory>
#include <string>
#include <vector>

using namespace sf2cute;
using namespace std;

const double AttackTable[] = { 13122, 6546, 4356, 3261, 2604, 2163, 1851, 1617, 1434, 1287, 1167, 1068, 984, 912, 849, 795, 747, 702, 666, 630, 600, 570, 543, 519, 498, 477, 459, 441, 426, 411, 396, 384, 372, 360, 348, 336, 327, 318, 309, 300, 294, 285, 279, 270, 264, 258, 252, 246, 240, 234, 231, 225, 219, 216, 210, 207, 201, 198, 195, 192, 186, 183, 180, 177, 174, 171, 168, 165, 162, 159, 156, 153.5, 153, 150, 147, 144, 141.5, 141, 138, 135.5, 135, 132, 129.5, 129, 126, 123.5, 123, 120.5, 120, 117, 114.5, 114, 111.5, 111, 108.5, 108, 105.7, 105.35, 105, 102.5, 102, 99.5, 99, 96.7, 96.35, 96, 93.5, 93, 90, 87, 81, 75, 72, 69, 63, 60, 54, 48, 45, 39, 36, 30, 24, 21, 15, 12, 9, 6.1e-6 };
const double HoldTable[] = { 6e-6, 1, 2, 4, 6, 9, 12, 16, 20, 25, 30, 36, 42, 49, 56, 64, 72, 81, 90, 100, 110, 121, 132, 144, 156, 169, 182, 196, 210, 225, 240, 256, 272, 289, 306, 324, 342, 361, 380, 400, 420, 441, 462, 484, 506, 529, 552, 576, 600, 625, 650, 676, 702, 729, 756, 784, 812, 841, 870, 900, 930, 961, 992, 1024, 1056, 1089, 1122, 1156, 1190, 1225, 1260, 1296, 1332, 1369, 1406, 1444, 1482, 1521, 1560, 1600, 1640, 1681, 1722, 1764, 1806, 1849, 1892, 1936, 1980, 2025, 2070, 2116, 2162, 2209, 2256, 2304, 2352, 2401, 2450, 2500, 2550, 2601, 2652, 2704, 2756, 2809, 2862, 2916, 2970, 3025, 3080, 3136, 3192, 3249, 3306, 3364, 3422, 3481, 3540, 3600, 3660, 3721, 3782, 3844, 3906, 3969, 4032, 4096 };
const double DecayTable[] = { -0.00016, -0.00047, -0.00078, -0.00109, -0.00141, -0.00172, -0.00203, -0.00234, -0.00266, -0.00297, -0.00328, -0.00359, -0.00391, -0.00422, -0.00453, -0.00484, -0.00516, -0.00547, -0.00578, -0.00609, -0.00641, -0.00672, -0.00703, -0.00734, -0.00766, -0.00797, -0.00828, -0.00859, -0.00891, -0.00922, -0.00953, -0.00984, -0.01016, -0.01047, -0.01078, -0.01109, -0.01141, -0.01172, -0.01203, -0.01234, -0.01266, -0.01297, -0.01328, -0.01359, -0.01391, -0.01422, -0.01453, -0.01484, -0.01516, -0.01547, -0.01579, -0.016, -0.01622, -0.01644, -0.01667, -0.0169, -0.01714, -0.01739, -0.01765, -0.01791, -0.01818, -0.01846, -0.01875, -0.01905, -0.01935, -0.01967, -0.02, -0.02034, -0.02069, -0.02105, -0.02143, -0.02182, -0.02222, -0.02264, -0.02308, -0.02353, -0.024, -0.02449, -0.025, -0.02553, -0.02609, -0.02667, -0.02727, -0.02791, -0.02857, -0.02927, -0.03, -0.03077, -0.03158, -0.03243, -0.03333, -0.03429, -0.03529, -0.03636, -0.0375, -0.03871, -0.04, -0.04138, -0.04286, -0.04444, -0.04615, -0.048, -0.05, -0.05217, -0.05455, -0.05714, -0.06, -0.06316, -0.06667, -0.07059, -0.075, -0.08, -0.08571, -0.09231, -1, -0.10909, -0.12, -0.13333, -0.15, -0.17143, -2, -2.4, -3, -4, -6, -12, -24, -65535 };

double ChangeLogBase(double x, double base)
{
	return log(x) / log(base);
}

double ConvertTime(double time)
{
	double timeCents = 1200 * ChangeLogBase(time, 2);

	return timeCents < -12000 ? -12000 : timeCents;
}

double ConvertVolume(uint32_t volume)
{
	return 200 * abs(ChangeLogBase(pow((static_cast<double>(volume) / 127.0f), 2), 10));
}

double ConvertPan(uint32_t pan)
{
	double sf2Pan = (static_cast<double>(pan) - 64.0f) * (500.0f / 63.0f);

	return sf2Pan < -500 ? -500 : sf2Pan;
}

double ConvertAttack(uint8_t attack)
{
	return ConvertTime(AttackTable[attack] / 1000);
}

double ConvertHold(uint8_t hold)
{
	return ConvertTime(HoldTable[hold] / 1000);
}

double ConvertDecay(uint8_t decay, uint8_t sustain)
{
	double sustainVolume = 20 * ChangeLogBase(pow((static_cast<double>(sustain) / 127.0f), 2), 10);

	if (decay == 127)
	{
		return -12000;
	}
	else
	{
		if (sustain == 0)
		{
			return ConvertTime(-90.25 / DecayTable[decay] / 1000);
		}
		else
		{
			return ConvertTime(sustainVolume / DecayTable[decay] / 1000);
		}
	}
}

double ConvertRelease(uint8_t release, uint8_t sustain)
{
	double sustainVolume = 20 * ChangeLogBase(pow((static_cast<double>(sustain) / 127.0f), 2), 10);

	if (release == 127)
	{
		return -12000;
	}
	else
	{
		if (sustain == 0)
		{
			return ConvertTime(-90.25 / DecayTable[release] / 1000);
		}
		else
		{
			return ConvertTime((-90.25 - sustainVolume) / DecayTable[release] / 1000);
		}
	}
}

double ConvertSustain(uint8_t sustain)
{
	if (sustain == 0)
	{
		return 900;
	}
	else
	{
		return 200 * abs(ChangeLogBase(pow((static_cast<double>(sustain) / 127.0f), 2), 10));
	}
}

Cbnk::Cbnk(const char* fileName, map<int, Cwar*>* cwars, bool p) : FileName(fileName), Cwars(cwars), P(p)
{
	ifstream ifs(FileName, ios::binary | ios::ate);

	Length = ifs.tellg();
	Data = new uint8_t[Length];

	Common::Push(FileName, Data);

	ifs.seekg(0, ios::beg);
	ifs.read(reinterpret_cast<char*>(Data), Length);
	ifs.close();
}

Cbnk::~Cbnk()
{
	Common::Pop();

	delete[] Data;
}

bool Cbnk::Convert(string cwarPath)
{
	uint8_t* pos = Data;

	if (!Common::Assert(pos, 0x43424E4B, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert(pos, 0xFEFF, ReadFixLen(pos, 2))) { return false; }
	if (!Common::Assert(pos, 0x20, ReadFixLen(pos, 2))) { return false; }

	uint32_t cbnkVersion = ReadFixLen(pos, 4);

	if (!Common::Assert<uint64_t>(pos, Length, ReadFixLen(pos, 4))) { return false; }
	if (!Common::Assert(pos, 0x1, ReadFixLen(pos, 4))) { return false; }
	if (!Common::Assert(pos, 0x5800, ReadFixLen(pos, 4))) { return false; }

	uint32_t infoOffset = ReadFixLen(pos, 4);
	uint32_t infoLength = ReadFixLen(pos, 4);

	if (!Common::Assert(pos, 0x494E464F, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert<uint32_t>(pos, infoLength, ReadFixLen(pos, 4))) { return false; }
	if (!Common::Assert(pos, 0x100, ReadFixLen(pos, 4))) { return false; }

	uint32_t cwavOffset = ReadFixLen(pos, 4);

	if (!Common::Assert(pos, 0x101, ReadFixLen(pos, 4))) { return false; }

	uint32_t instOffset = ReadFixLen(pos, 4);

	pos = Data + infoOffset + 8 + cwavOffset;

	uint32_t cwavCount = ReadFixLen(pos, 4);

	vector<CbnkCwav> cwavs;

	for (uint32_t i = 0; i < cwavCount; ++i)
	{
		pos = Data + infoOffset + 8 + cwavOffset + 4 + (i * 8);

		CbnkCwav cwav;
		cwav.Cwar = ReadFixLen(pos, 4) - 0x5000000;
		cwav.Id = ReadFixLen(pos, 4);

		size_t j = 0;
		auto it = Cwars->begin();

		for (; it != Cwars->end(); ++it, ++j)
		{
			if (j == cwav.Cwar)
			{
				break;
			}
		}

		if (cwav.Id < 0xF000)
		{
			ifstream ifs(cwarPath + "/" + it->second->FileName.substr(0, it->second->FileName.length() - 6) + "/" + to_string(cwav.Id) + ".wav", ios::binary | ios::ate);

			streamoff cwavLength = ifs.tellg();
			uint8_t* cwavData = new uint8_t[cwavLength];

			Common::Push(string(to_string(cwav.Id) + ".wav"), cwavData);

			ifs.seekg(0, ios::beg);
			ifs.read(reinterpret_cast<char*>(cwavData), cwavLength);
			ifs.close();

			pos = cwavData;

			if (!Common::Assert(pos, 0x52494646, ReadFixLen(pos, 4, false))) { return false; }
			if (!Common::Assert<uint64_t>(pos, cwavLength - 8, ReadFixLen(pos, 4))) { return false; }
			if (!Common::Assert(pos, 0x57415645, ReadFixLen(pos, 4, false))) { return false; }
			if (!Common::Assert(pos, 0x666D7420, ReadFixLen(pos, 4, false))) { return false; }
			if (!Common::Assert(pos, 0x10, ReadFixLen(pos, 4))) { return false; }
			if (!Common::Assert(pos, 0x1, ReadFixLen(pos, 2))) { return false; }

			cwav.ChanCount = ReadFixLen(pos, 2);
			cwav.SampleRate = ReadFixLen(pos, 4);
			uint32_t byteRate = ReadFixLen(pos, 4);
			uint16_t blockAlign = ReadFixLen(pos, 2);
			uint16_t bitsPerSample = ReadFixLen(pos, 2);

			if (!Common::Assert(pos - 8, byteRate, (cwav.SampleRate * cwav.ChanCount) * (bitsPerSample / 8))) { return false; }
			if (!Common::Assert<uint16_t>(pos - 4, blockAlign, cwav.ChanCount * (bitsPerSample / 8))) { return false; }
			if (!Common::Assert(pos, 0x64617461, ReadFixLen(pos, 4, false))) { return false; }

			uint32_t cwavDataLength = ReadFixLen(pos, 4);

			while (pos < (cwavData + 44 + cwavDataLength))
			{
				cwav.LeftSamples.push_back(ReadFixLen(pos, 2));

				if (cwav.ChanCount == 2)
				{
					cwav.RightSamples.push_back(ReadFixLen(pos, 2));
				}
			}

			for (; pos < (cwavData + cwavLength); pos -= 3)
			{
				if (ReadFixLen(pos, 4, false) == 0x736D706C)
				{
					uint32_t smplLength = ReadFixLen(pos, 4);
					uint32_t manufacturer = ReadFixLen(pos, 4);
					uint32_t product = ReadFixLen(pos, 4);
					uint32_t samplePeriod = ReadFixLen(pos, 4);
					uint32_t midiUnityNote = ReadFixLen(pos, 4);
					uint32_t midiPitchFraction = ReadFixLen(pos, 4);
					uint32_t smpteFormat = ReadFixLen(pos, 4);
					uint32_t smpteOffset = ReadFixLen(pos, 4);
					uint32_t sampleLoops = ReadFixLen(pos, 4);
					uint32_t samplerData = ReadFixLen(pos, 4);

					vector<WaveSmpl> smpls;

					for (uint32_t j = 0; j < sampleLoops; ++j)
					{
						WaveSmpl smpl{};
						smpl.CuePointId = ReadFixLen(pos, 4);
						smpl.Type = ReadFixLen(pos, 4);
						smpl.Start = ReadFixLen(pos, 4);
						smpl.End = ReadFixLen(pos, 4);
						smpl.Fraction = ReadFixLen(pos, 4);
						smpl.PlayCount = ReadFixLen(pos, 4);

						smpls.push_back(smpl);
					}

					cwav.Loop = true;
					cwav.LoopStart = smpls[0].Start;
					cwav.LoopEnd = smpls[0].End;

					break;
				}
			}

			if (!cwav.Loop)
			{
				cwav.LoopStart = 0;
				cwav.LoopEnd = cwav.LeftSamples.size();
			}

			Common::Pop();

			delete[] cwavData;
		}

		cwavs.push_back(cwav);
	}

	pos = Data + infoOffset + 8 + instOffset;

	uint32_t instCount = ReadFixLen(pos, 4);

	vector<CbnkInst> insts;

	for (uint32_t i = 0; i < instCount; ++i)
	{
		CbnkInst inst;

		if (ReadFixLen(pos, 4) != 0x5900)
		{
			inst.Exists = false;
		}

		inst.Offset = Data + infoOffset + 24 + ReadFixLen(pos, 4);

		insts.push_back(inst);
	}

	for (uint32_t i = 0; i < instCount; ++i)
	{
		if (!insts[i].Exists)
		{
			continue;
		}

		pos = insts[i].Offset;

		uint32_t instType = ReadFixLen(pos, 4);

		if (!Common::Assert(pos, 0x8, ReadFixLen(pos, 4))) { return false; }

		switch (instType)
		{
			case 0x6000:
			{
				insts[i].NoteCount = 1;

				CbnkNote note{};
				note.StartNote = 0;
				note.EndNote = 127;

				insts[i].Notes.push_back(note);

				break;
			}

			case 0x6001:
			{
				insts[i].NoteCount = ReadFixLen(pos, 4);

				for (uint32_t j = 0; j < insts[i].NoteCount; ++j)
				{
					CbnkNote note{};
					note.StartNote = j == 0 ? 0 : insts[i].Notes[j - 1].EndNote + 1;
					note.EndNote = ReadFixLen(pos, 1);

					insts[i].Notes.push_back(note);
				}

				uint8_t padding = insts[i].NoteCount % 4;

				if (padding)
				{
					if (!Common::Assert(pos, 0x0, ReadFixLen(pos, 4 - padding))) { return false; }
				}

				break;
			}

			case 0x6002:
			{
				insts[i].NoteCount = ReadFixLen(pos, 2, false) + 1;

				for (uint32_t j = 0; j < insts[i].NoteCount; ++j)
				{
					CbnkNote note{};
					note.StartNote = j;
					note.EndNote = j;

					insts[i].Notes.push_back(note);
				}

				if (!Common::Assert(pos, 0x0, ReadFixLen(pos, 2))) { return false; }

				insts[i].IsDrumKit = true;

				break;
			}

			default:
			{
				Common::Error(pos - 8, "A valid instrument type", instType);

				return false;
			}
		}

		for (uint32_t j = 0; j < insts[i].NoteCount; ++j)
		{
			if (ReadFixLen(pos, 4) != 0x5901)
			{
				insts[i].Notes[j].Exists = false;
			}

			insts[i].Notes[j].Offset = insts[i].Offset + 8 + ReadFixLen(pos, 4);
		}

		for (uint32_t j = 0; j < insts[i].NoteCount; ++j)
		{
			if (!insts[i].Notes[j].Exists)
			{
				continue;
			}

			pos = insts[i].Notes[j].Offset;

			uint32_t id = ReadFixLen(pos, 4);

			if (!Common::Assert(pos, 0x8, ReadFixLen(pos, 4))) { return false; }
			Common::Analyse("Note 0x08", ReadFixLen(pos, 4));
			Common::Analyse("Note 0x0C", ReadFixLen(pos, 4));

			if (id == 0x6001)
			{
				Common::Analyse("Note 0x6001 0x10", ReadFixLen(pos, 4));
				Common::Analyse("Note 0x6001 0x14", ReadFixLen(pos, 4));
				Common::Analyse("Note 0x6001 0x18", ReadFixLen(pos, 4));
				Common::Analyse("Note 0x6001 0x1C", ReadFixLen(pos, 4));
			}

			uint32_t cwav = ReadFixLen(pos, 4);

			if (cwav < cwavs.size())
			{
				insts[i].Notes[j].Cwav = &cwavs[cwav];
			}
			else
			{
				Common::Warning(pos - 4, "CWAV " + to_string(cwav) + " does not exist");

				insts[i].Notes[j].Cwav = &cwavs[0];
			}

			Common::Analyse("Note 0x14", ReadFixLen(pos, 4));

			insts[i].Notes[j].RootKey = ReadFixLen(pos, 4);
			insts[i].Notes[j].Cwav->Key = insts[i].Notes[j].RootKey;
			insts[i].Notes[j].Volume = ReadFixLen(pos, 4);
			insts[i].Notes[j].Pan = ReadFixLen(pos, 4);

			Common::Analyse("Note 0x24", ReadFixLen(pos, 4));
			Common::Analyse("Note 0x28", ReadFixLen(pos, 2));

			insts[i].Notes[j].Interpolation = ReadFixLen(pos, 1);

			if (!Common::Assert(pos, 0x0, ReadFixLen(pos, 1))) { return false; }
			Common::Analyse("Note 0x2C", ReadFixLen(pos, 4));
			Common::Analyse("Note 0x30", ReadFixLen(pos, 4));
			Common::Analyse("Note 0x34", ReadFixLen(pos, 4));

			insts[i].Notes[j].Attack = ReadFixLen(pos, 1);
			insts[i].Notes[j].Decay = ReadFixLen(pos, 1);
			insts[i].Notes[j].Sustain = ReadFixLen(pos, 1);
			insts[i].Notes[j].Hold = ReadFixLen(pos, 1);
			insts[i].Notes[j].Release = ReadFixLen(pos, 1);

			if (!Common::Assert(pos, 0x0, ReadFixLen(pos, 3))) { return false; }
		}
	}

	SoundFont sf2;
	sf2.set_sound_engine("EMU8000");
	sf2.set_bank_name(FileName.substr(0, FileName.length() - 6));
	sf2.set_rom_name("ROM");
	sf2.set_software("Caesar");

	map<uint32_t, shared_ptr<SFSample>> leftSamples;
	map<uint32_t, shared_ptr<SFSample>> rightSamples;

	for (uint32_t i = 0; i < cwavCount; ++i)
	{
		if (cwavs[i].Id >= 0xF000)
		{
			continue;
		}

		if (cwavs[i].ChanCount == 1)
		{
			leftSamples[cwavs[i].Id] = sf2.NewSample(to_string(cwavs[i].Id), cwavs[i].LeftSamples, cwavs[i].LoopStart, cwavs[i].LoopEnd, cwavs[i].SampleRate, cwavs[i].Key, 0);
		}
		else
		{
			leftSamples[cwavs[i].Id] = sf2.NewSample(to_string(cwavs[i].Id) + "l", cwavs[i].LeftSamples, cwavs[i].LoopStart, cwavs[i].LoopEnd, cwavs[i].SampleRate, cwavs[i].Key, 0);
			rightSamples[cwavs[i].Id] = sf2.NewSample(to_string(cwavs[i].Id) + "r", cwavs[i].RightSamples, cwavs[i].LoopStart, cwavs[i].LoopEnd, cwavs[i].SampleRate, cwavs[i].Key, 0);

			leftSamples[cwavs[i].Id]->set_link(rightSamples[cwavs[i].Id]);
			rightSamples[cwavs[i].Id]->set_link(leftSamples[cwavs[i].Id]);

			leftSamples[cwavs[i].Id]->set_type(SFSampleLink::kLeftSample);
			rightSamples[cwavs[i].Id]->set_type(SFSampleLink::kRightSample);
		}
	}

	vector<shared_ptr<SFInstrument>> instruments;

	for (uint32_t i = 0; i < instCount; ++i)
	{
		if (insts[i].Exists)
		{
			vector<SFInstrumentZone> instrumentZones;

			for (uint32_t j = 0; j < insts[i].NoteCount; ++j)
			{
				if ((insts[i].Notes[j].Exists) && (insts[i].Notes[j].Cwav->Id < 0xF000))
				{
					size_t k = 0;
					auto it = Cwars->begin();

					for (; it != Cwars->end(); ++it, ++k)
					{
						if (k == insts[i].Notes[j].Cwav->Cwar)
						{
							break;
						}
					}

					SFGeneratorItem keyRange(SFGenerator::kKeyRange, RangesType(insts[i].Notes[j].StartNote, insts[i].Notes[j].EndNote));
					SFGeneratorItem overridingRootKey(SFGenerator::kOverridingRootKey, insts[i].Notes[j].RootKey);
					SFGeneratorItem initialAttenuation(SFGenerator::kInitialAttenuation, ConvertVolume(insts[i].Notes[j].Volume));
					SFGeneratorItem pan(SFGenerator::kPan, ConvertPan(insts[i].Notes[j].Pan));
					SFGeneratorItem attackVolEnv(SFGenerator::kAttackVolEnv, ConvertAttack(insts[i].Notes[j].Attack));
					SFGeneratorItem holdVolEnv(SFGenerator::kHoldVolEnv, ConvertHold(insts[i].Notes[j].Hold));
					SFGeneratorItem decayVolEnv(SFGenerator::kDecayVolEnv, ConvertDecay(insts[i].Notes[j].Decay, insts[i].Notes[j].Sustain));
					SFGeneratorItem releaseVolEnv(SFGenerator::kReleaseVolEnv, ConvertRelease(insts[i].Notes[j].Release, insts[i].Notes[j].Sustain));
					SFGeneratorItem sustainVolEnv(SFGenerator::kSustainVolEnv, ConvertSustain(insts[i].Notes[j].Sustain));
					SFGeneratorItem sampleModes(SFGenerator::kSampleModes, it->second->Cwavs[insts[i].Notes[j].Cwav->Id]->SampleMode);

					if (insts[i].Notes[j].Cwav->ChanCount == 1)
					{
						instrumentZones.push_back(SFInstrumentZone(leftSamples[insts[i].Notes[j].Cwav->Id], vector<SFGeneratorItem> { keyRange, overridingRootKey, initialAttenuation, pan, attackVolEnv, holdVolEnv, decayVolEnv, releaseVolEnv, sustainVolEnv, sampleModes }, vector<SFModulatorItem> { }));
					}
					else
					{
						if (!P)
						{
							SFGeneratorItem left(SFGenerator::kPan, -500);
							SFGeneratorItem right(SFGenerator::kPan, 500);

							instrumentZones.push_back(SFInstrumentZone(leftSamples[insts[i].Notes[j].Cwav->Id], vector<SFGeneratorItem> { keyRange, overridingRootKey, initialAttenuation, left, attackVolEnv, holdVolEnv, decayVolEnv, releaseVolEnv, sustainVolEnv, sampleModes }, vector<SFModulatorItem> { }));
							instrumentZones.push_back(SFInstrumentZone(rightSamples[insts[i].Notes[j].Cwav->Id], vector<SFGeneratorItem> { keyRange, overridingRootKey, initialAttenuation, right, attackVolEnv, holdVolEnv, decayVolEnv, releaseVolEnv, sustainVolEnv, sampleModes }, vector<SFModulatorItem> { }));
						}
						else
						{
							SFGeneratorItem left(SFGenerator::kPan, ((static_cast<double>(insts[i].Notes[j].Pan) / 128.0f) * 500) - 500);
							SFGeneratorItem right(SFGenerator::kPan, (static_cast<double>(insts[i].Notes[j].Pan) / 128.0f) * 500);

							instrumentZones.push_back(SFInstrumentZone(leftSamples[insts[i].Notes[j].Cwav->Id], vector<SFGeneratorItem> { keyRange, overridingRootKey, initialAttenuation, left, attackVolEnv, holdVolEnv, decayVolEnv, releaseVolEnv, sustainVolEnv, sampleModes }, vector<SFModulatorItem> { }));
							instrumentZones.push_back(SFInstrumentZone(rightSamples[insts[i].Notes[j].Cwav->Id], vector<SFGeneratorItem> { keyRange, overridingRootKey, initialAttenuation, right, attackVolEnv, holdVolEnv, decayVolEnv, releaseVolEnv, sustainVolEnv, sampleModes }, vector<SFModulatorItem> { }));
						}
					}
				}
			}

			if (!instrumentZones.empty())
			{
				instruments.push_back(sf2.NewInstrument(to_string(i), instrumentZones));
			}
			else
			{
				instruments.push_back(nullptr);
			}
		}
		else
		{
			instruments.push_back(nullptr);
		}
	}

	for (uint32_t i = 0; i < instCount; ++i)
	{
		if (insts[i].Exists && (instruments[i] != nullptr))
		{
			sf2.NewPreset(instruments[i]->name(), i, !insts[i].IsDrumKit ? 0 : 128, vector<SFPresetZone> { SFPresetZone(instruments[i]) });
		}
	}

	ofstream ofs(FileName.substr(0, FileName.length() - 5).append("sf2"), ios::binary);
	sf2.Write(ofs);
	ofs.close();

	return true;
}
