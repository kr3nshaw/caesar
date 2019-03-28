#include "Cseq.hpp"
#include "Common.hpp"
#include "libsmfc/libsmfc.h"
#include "libsmfc/libsmfcx.h"
#include <cstdint>
#include <fstream>
#include <iterator>
#include <map>
#include <stack>
#include <string>
#include <vector>

using namespace std;

vector<int32_t> ReadArgs(uint8_t* &pos, ArgType argType)
{
	if (argType == Uint8)
	{
		return { ReadFixLen(pos, 1) };
	}
	else if (argType == Int8)
	{
		return { ReadFixLen(pos, 1, false, true) };
	}
	else if (argType == Uint16)
	{
		return { ReadFixLen(pos, 2, false) };
	}
	else if (argType == Int16)
	{
		return { ReadFixLen(pos, 2, false, true) };
	}
	else if (argType == Rnd)
	{
		return { ReadFixLen(pos, 2, false, true), ReadFixLen(pos, 2, false, true) };
	}
	else if (argType == Var)
	{
		return { ReadFixLen(pos, 1) };
	}
	else if (argType == VarLen)
	{
		return { ReadVarLen(pos) };
	}

	return { };
}

Cseq::Cseq(const char* fileName) : FileName(fileName)
{
	ifstream ifs(FileName, ios::binary | ios::ate);

	Length = ifs.tellg();
	Data = new uint8_t[Length];

	Common::Push(FileName, Data);

	ifs.seekg(0, ios::beg);
	ifs.read(reinterpret_cast<char*>(Data), Length);
	ifs.close();
}

Cseq::~Cseq()
{
	Common::Pop();

	if (Data)
	{
		delete[] Data;
	}
}

bool Cseq::Convert()
{
	uint8_t* pos = Data;

	if (!Common::Assert(pos, 0x43534551, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert(pos, 0xFEFF, ReadFixLen(pos, 2))) { return false; }
	if (!Common::Assert(pos, 0x40, ReadFixLen(pos, 2))) { return false; }

	uint32_t cseqVersion = ReadFixLen(pos, 4);

	if (!Common::Assert<uint64_t>(pos, Length, ReadFixLen(pos, 4))) { return false; }
	if (!Common::Assert(pos, 0x2, ReadFixLen(pos, 4))) { return false; }
	if (!Common::Assert(pos, 0x5000, ReadFixLen(pos, 4))) { return false; }

	uint32_t dataOffset = ReadFixLen(pos, 4);
	uint32_t dataLength = ReadFixLen(pos, 4);

	if (!Common::Assert(pos, 0x5001, ReadFixLen(pos, 4))) { return false; }

	uint32_t lablOffset = ReadFixLen(pos, 4);
	uint32_t lablLength = ReadFixLen(pos, 4);

	pos = Data + lablOffset;

	if (!Common::Assert(pos, 0x4C41424C, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert<uint32_t>(pos, lablLength, ReadFixLen(pos, 4))) { return false; }

	uint32_t lablCount = ReadFixLen(pos, 4);

	vector<uint8_t*> lablOffsets;

	for (uint32_t i = 0; i < lablCount; ++i)
	{
		if (!Common::Assert(pos, 0x5100, ReadFixLen(pos, 4))) { return false; }

		lablOffsets.push_back(Data + lablOffset + 8 + ReadFixLen(pos, 4));
	}

	map<uint8_t*, CseqLabl> labls;

	for (uint32_t i = 0; i < lablCount; ++i)
	{
		pos = lablOffsets[i];

		if (!Common::Assert(pos, 0x1F00, ReadFixLen(pos, 4))) { return false; }

		CseqLabl labl;
		labl.Offset = Data + dataOffset + 8 + ReadFixLen(pos, 4);
		uint32_t lablLength = ReadFixLen(pos, 4);
		labl.Label = string(reinterpret_cast<const char*>(pos), lablLength);

		labls[labl.Offset] = labl;
	}

	pos = Data + dataOffset;

	if (!Common::Assert(pos, 0x44415441, ReadFixLen(pos, 4, false))) { return false; }
	if (!Common::Assert<uint32_t>(pos, dataLength, ReadFixLen(pos, 4))) { return false; }

	map<uint32_t, CseqCmd> commands;

	while (pos < (Data + dataOffset + dataLength))
	{
		uint32_t offset = pos - 8 - dataOffset - Data;
		CseqCmd cmd;

		if (labls.count(pos))
		{
			cmd.Label = labls[pos].Label;
		}

		uint8_t statusByte = ReadFixLen(pos, 1);

		if (statusByte == 0xA2)
		{
			cmd.Suffix3 = _If;

			statusByte = ReadFixLen(pos, 1);
		}

		if (statusByte == 0xA3)
		{
			cmd.Suffix2 = _Time;
			cmd.Arg2 = Int16;

			statusByte = ReadFixLen(pos, 1);
		}
		else if (statusByte == 0xA4)
		{
			cmd.Suffix2 = _TimeRnd;
			cmd.Arg2 = Rnd;

			statusByte = ReadFixLen(pos, 1);
		}
		else if (statusByte == 0xA5)
		{
			cmd.Suffix2 = _TimeVar;
			cmd.Arg2 = Var;

			statusByte = ReadFixLen(pos, 1);
		}

		if (statusByte == 0xA0)
		{
			cmd.Suffix1 = _Rnd;
			cmd.Arg1 = Rnd;

			statusByte = ReadFixLen(pos, 1);
		}
		else if (statusByte == 0xA1)
		{
			cmd.Suffix1 = _Var;
			cmd.Arg1 = Var;

			statusByte = ReadFixLen(pos, 1);
		}

		cmd.Cmd = statusByte;

		if (statusByte < 0x80)
		{
			cmd.Args.push_back(ReadFixLen(pos, 1));

			if (cmd.Arg1 == None)
			{
				cmd.Arg1 = VarLen;
			}

			vector<int32_t> args = ReadArgs(pos, cmd.Arg1);

			cmd.Args.insert(cmd.Args.end(), args.begin(), args.end());
		}
		else if ((statusByte == 0x80) || (statusByte == 0x81))
		{
			if (cmd.Arg1 == None)
			{
				cmd.Arg1 = VarLen;
			}

			vector<int32_t> args = ReadArgs(pos, cmd.Arg1);

			cmd.Args.insert(cmd.Args.end(), args.begin(), args.end());
		}
		else if (statusByte == 0x88)
		{
			cmd.Args.push_back(ReadFixLen(pos, 1));
			cmd.Args.push_back(ReadFixLen(pos, 3, false));
		}
		else if ((statusByte == 0x89) || (statusByte == 0x8A))
		{
			cmd.Args.push_back(ReadFixLen(pos, 3, false));
		}
		else if (statusByte == 0x90)
		{
			Common::Analyse("Cseq Cmd 0x90", ReadFixLen(pos, 2, false));
		}
		else if (statusByte == 0x96)
		{
			Common::Analyse("Cseq Cmd 0x96", ReadFixLen(pos, 2, false));
		}
		else if ((statusByte >= 0xB0) && (statusByte <= 0xDF))
		{
			if ((statusByte != 0xB7) && (statusByte != 0xB8) && (statusByte != 0xB9) && (statusByte != 0xBA) && (statusByte != 0xBB) && (statusByte != 0xBC) && (statusByte != 0xDE))
			{
				if ((statusByte == 0xB1) || (statusByte == 0xC3) || (statusByte == 0xC4) || (statusByte == 0xD0) || (statusByte == 0xD1) || (statusByte == 0xD2) || (statusByte == 0xD3))
				{
					if (cmd.Arg1 == None)
					{
						cmd.Arg1 = Int8;
					}

					vector<int32_t> args = ReadArgs(pos, cmd.Arg1);

					cmd.Args.insert(cmd.Args.end(), args.begin(), args.end());
				}
				else if ((statusByte == 0xB2) || (statusByte == 0xBF) || (statusByte = 0xC7) || (statusByte == 0xC8) || (statusByte == 0xC9) || (statusByte == 0xCE) || (statusByte == 0xDF))
				{
					cmd.Args.push_back(ReadFixLen(pos, 1));
				}
				else if (statusByte == 0xCC)
				{
					cmd.Args.push_back(ReadFixLen(pos, 1));

					if (cmd.Args.back() > 2)
					{
						Common::Error(pos - 1, "A valid modulation type", cmd.Args.back());

						return false;
					}
				}
				else if (statusByte == 0xD6)
				{
					vector<int32_t> args = ReadArgs(pos, Var);

					cmd.Args.insert(cmd.Args.end(), args.begin(), args.end());
				}
			}
			else
			{
				if (cmd.Arg1 == None)
				{
					cmd.Arg1 = Uint8;
				}

				vector<int32_t> args = ReadArgs(pos, cmd.Arg1);

				cmd.Args.insert(cmd.Args.end(), args.begin(), args.end());
			}

			if (cmd.Arg2 != None)
			{
				vector<int32_t> args = ReadArgs(pos, cmd.Arg2);

				cmd.Args.insert(cmd.Args.end(), args.begin(), args.end());
			}
		}
		else if ((statusByte == 0xE0) || (statusByte == 0xE1) || (statusByte == 0xE3) || (statusByte == 0xE4))
		{
			if (cmd.Arg1 == None)
			{
				cmd.Arg1 = Int16;
			}

			vector<int32_t> args = ReadArgs(pos, cmd.Arg1);

			cmd.Args.insert(cmd.Args.end(), args.begin(), args.end());
		}
		else if (statusByte == 0xF0)
		{
			cmd.Extended = true;

			statusByte = ReadFixLen(pos, 1);

			if (((statusByte >= 0x80) && (statusByte <= 0x8B)) || ((statusByte >= 0x90) && (statusByte <= 0x95)))
			{
				vector<int32_t> args1 = ReadArgs(pos, Var);

				cmd.Args.insert(cmd.Args.end(), args1.begin(), args1.end());

				if (cmd.Arg1 == None)
				{
					cmd.Arg1 = Int16;
				}

				vector<int32_t> args2 = ReadArgs(pos, cmd.Arg1);

				cmd.Args.insert(cmd.Args.end(), args2.begin(), args2.end());
			}
			else if (statusByte == 0xA4)
			{
				cmd.Args.push_back(ReadFixLen(pos, 1));

				if (cmd.Args.back() > 2)
				{
					Common::Error(pos - 1, "A valid modulation type", cmd.Args.back());

					return false;
				}
			}
			else if (statusByte == 0xAA)
			{
				cmd.Args.push_back(ReadFixLen(pos, 1));

				if (cmd.Args.back() > 2)
				{
					Common::Error(pos - 1, "A valid modulation type", cmd.Args.back());

					return false;
				}
			}
			else if (statusByte == 0xB0)
			{
				cmd.Args.push_back(ReadFixLen(pos, 1));

				if (cmd.Args.back() > 2)
				{
					Common::Error(pos - 1, "A valid modulation type", cmd.Args.back());

					return false;
				}
			}
			else if ((statusByte >= 0xA0) && (statusByte <= 0xB1))
			{
				if (cmd.Arg1 == None)
				{
					cmd.Arg1 = Uint8;
				}

				vector<int32_t> args = ReadArgs(pos, cmd.Arg1);

				cmd.Args.insert(cmd.Args.end(), args.begin(), args.end());
			}
			else if (statusByte == 0xE0)
			{
				if (cmd.Arg1 == None)
				{
					cmd.Arg1 = Uint16;
				}

				vector<int32_t> args = ReadArgs(pos, cmd.Arg1);

				cmd.Args.insert(cmd.Args.end(), args.begin(), args.end());
			}
			else if ((statusByte >= 0xE1) && (statusByte <= 0xE6))
			{
				if (cmd.Arg1 == None)
				{
					cmd.Arg1 = Int16;
				}

				vector<int32_t> args = ReadArgs(pos, cmd.Arg1);

				cmd.Args.insert(cmd.Args.end(), args.begin(), args.end());
			}
			else
			{
				Common::Error(pos - 1, "A valid extended command", statusByte);

				return false;
			}
		}
		else if ((statusByte == 0xFB) || (statusByte == 0xFC) || (statusByte == 0xFD) || (statusByte == 0xFF))
		{
			// These commands take no arguments
		}
		else if (statusByte == 0xFE)
		{
			cmd.Args.push_back(ReadFixLen(pos, 2, false));
		}
		else
		{
		Common::Error(pos - 1, "A valid command", statusByte);

			return false;
		}

		commands[offset] = cmd;
	}

	Smf* smf = smfCreate();
	uint32_t absTime = 0;
	uint8_t track = 0;
	bool noteWait = false;
	uint32_t trackOffsets[16] = { 0 };
	stack<uint32_t> sp;
	bool trackEnabled[16] = { false };

	for (auto i = commands.begin(); i != commands.end(); ++i)
	{
		if (!i->second.Label.empty())
		{
			smfInsertMetaText(smf, absTime, track, SMF_META_TEXT, i->second.Label.c_str());
		}

		if (!i->second.Extended)
		{
			if (i->second.Cmd < 0x80)
			{
				smfInsertNote(smf, absTime, track, track, i->second.Cmd, i->second.Args[0], i->second.Args[1]);

				if (noteWait)
				{
					absTime += i->second.Args[1];
				}
			}
			else if (i->second.Cmd == 0x80)
			{
				absTime += i->second.Args[0];
			}
			else if (i->second.Cmd == 0x81)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_BANKSELM, (i->second.Args[0] / 128 / 128) % 128);
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_BANKSELL, (i->second.Args[0] / 128) % 128);
				smfInsertProgram(smf, absTime, track, track, i->second.Args[0]);
			}
			else if (i->second.Cmd == 0x88)
			{
				trackOffsets[i->second.Args[0]] = i->second.Args[1];
			}
			else if (i->second.Cmd == 0x89)
			{
				// TODO (Low): Implement jump
			}
			else if (i->second.Cmd == 0x8A)
			{
				sp.push(next(i, 1)->first);

				i = commands.find(i->second.Args[0]);
				--i;
			}
			else if (i->second.Cmd == 0xB0)
			{
				smfSetTimebase(smf, i->second.Args[0]);
			}
			else if (i->second.Cmd == 0xB1)
			{
				// TODO (Low): Implement envelope hold
			}
			else if (i->second.Cmd == 0xB2)
			{
				smfInsertControl(smf, absTime, track, track, i->second.Args[0] ? SMF_CONTROL_MONO : SMF_CONTROL_POLY, 0);
			}
			else if (i->second.Cmd == 0xB3)
			{
				// TODO (Low): Implement velocity range
			}
			else if (i->second.Cmd == 0xB4)
			{
				// TODO (Low): Implement biquad type
			}
			else if (i->second.Cmd == 0xB5)
			{
				// TODO (Low): Implement biquad value
			}
			else if (i->second.Cmd == 0xB6)
			{
				// TODO (Low): Implement bank select
			}
			else if (i->second.Cmd == 0xBD)
			{
				// TODO (Low): Implement mod phase
			}
			else if (i->second.Cmd == 0xBE)
			{
				// TODO (Low): Implement mod curve
			}
			else if (i->second.Cmd == 0xBF)
			{
				// TODO (Low): Implement front bypass
			}
			else if (i->second.Cmd == 0xC0)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_PANPOT, i->second.Args[0]);
			}
			else if (i->second.Cmd == 0xC1)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_VOLUME, i->second.Args[0]);
			}
			else if (i->second.Cmd == 0xC2)
			{
				smfInsertMasterVolume(smf, absTime, 0, track, i->second.Args[0]);
			}
			else if (i->second.Cmd == 0xC3)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_RPNM, 0);
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_RPNL, 2);
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_DATAENTRYM, i->second.Args[0] + 64);
			}
			else if (i->second.Cmd == 0xC4)
			{
				smfInsertPitchBend(smf, absTime, track, track, i->second.Args[0] * 64);
			}
			else if (i->second.Cmd == 0xC5)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_RPNM, 0);
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_RPNL, 0);
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_DATAENTRYM, i->second.Args[0]);
			}
			else if (i->second.Cmd == 0xC6)
			{
				// TODO (Low): Implement priority
			}
			else if (i->second.Cmd == 0xC7)
			{
				noteWait = i->second.Args[0];
			}
			else if (i->second.Cmd == 0xC8)
			{
				// TODO (Low): Implement tie
			}
			else if (i->second.Cmd == 0xC9)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_PORTAMENTOCTRL, i->second.Args[0]);
			}
			else if (i->second.Cmd == 0xCA)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_MODULATION, i->second.Args[0]);
			}
			else if (i->second.Cmd == 0xCB)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_VIBRATORATE, (i->second.Args[0] / 2) + 64);
			}
			else if (i->second.Cmd == 0xCC)
			{
				// TODO (Low): Implement mod type
			}
			else if (i->second.Cmd == 0xCD)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_VIBRATODEPTH, (i->second.Args[0] / 2) + 64);
			}
			else if (i->second.Cmd == 0xCE)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_PORTAMENTO, i->second.Args[0] ? 127 : 0);
			}
			else if (i->second.Cmd == 0xCF)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_PORTAMENTOTIME, i->second.Args[0]);
			}
			else if (i->second.Cmd == 0xD0)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_ATTACKTIME, (i->second.Args[0] / 2) + 64);
			}
			else if (i->second.Cmd == 0xD1)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_DECAYTIME, (i->second.Args[0] / 2) + 64);
			}
			else if (i->second.Cmd == 0xD2)
			{
				// TODO (Low): Implement sustain
			}
			else if (i->second.Cmd == 0xD3)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_RELEASETIME, (i->second.Args[0] / 2) + 64);
			}
			else if (i->second.Cmd == 0xD4)
			{
				smfInsertControl(smf, absTime, track, track, 116, 0);
			}
			else if (i->second.Cmd == 0xD5)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_EXPRESSION, i->second.Args[0]);
			}
			else if (i->second.Cmd == 0xD6)
			{
				// TODO (Low): Implement print var
			}
			else if (i->second.Cmd == 0xD7)
			{
				// TODO (Low): Implement span
			}
			else if (i->second.Cmd == 0xD8)
			{
				// TODO (Low): Implement lpf cutoff
			}
			else if (i->second.Cmd == 0xD9)
			{
				// TODO (Low): Implement fx send a
			}
			else if (i->second.Cmd == 0xDA)
			{
				// TODO (Low): Implement fx send b
			}
			else if (i->second.Cmd == 0xDB)
			{
				// TODO (Low): Implement main send
			}
			else if (i->second.Cmd == 0xDC)
			{
				// TODO (Low): Implement init pan
			}
			else if (i->second.Cmd == 0xDD)
			{
				// TODO (Low): Implement mute
			}
			else if (i->second.Cmd == 0xDF)
			{
				// TODO (Low): Implement damper
			}
			else if (i->second.Cmd == 0xE0)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_VIBRATODELAY, (i->second.Args[0] / 2) + 64);
			}
			else if (i->second.Cmd == 0xE1)
			{
				smfInsertTempoBPM(smf, absTime, track, i->second.Args[0]);
			}
			else if (i->second.Cmd == 0xE3)
			{
				smfInsertControl(smf, absTime, track, track, SMF_CONTROL_VIBRATODELAY, i->second.Args[0]);
			}
			else if (i->second.Cmd == 0xE4)
			{
				// TODO (Low): Implement mod_period
			}
			else if (i->second.Cmd == 0xFB)
			{
				// TODO (Low): Implement envelope reset
			}
			else if (i->second.Cmd == 0xFC)
			{
				smfInsertControl(smf, absTime, track, track, 117, 0);
			}
			else if (i->second.Cmd == 0xFD)
			{
				if (!sp.empty())
				{
					i = commands.find(sp.top());
					--i;

					sp.pop();
				}
				else
				{
					// TODO (High): Work out how sequences can return without ever having been called
					smfDelete(smf);

					return true;
				}
			}
			else if (i->second.Cmd == 0xFE)
			{
				for (uint8_t j = 0; j < 16; ++j)
				{
					trackEnabled[j] = (i->second.Args[0] >> j) & 0x1;
				}
			}
			else if (i->second.Cmd == 0xFF)
			{
				smfSetEndTimingOfTrack(smf, track, absTime);

				uint8_t j;

				for (j = track + 1; j < 16; ++j)
				{
					if (trackOffsets[j] != 0)
					{
						absTime = 0;
						track = j;
						noteWait = false;

						i = commands.find(trackOffsets[j]);
						--i;

						break;
					}
				}

				if (j == 16)
				{
					break;
				}
			}
		}
		else
		{
			if (i->second.Cmd == 0x80)
			{
				// TODO (Low): Implement setvar
			}
			else if (i->second.Cmd == 0x81)
			{
				// TODO (Low): Implement addvar
			}
			else if (i->second.Cmd == 0x82)
			{
				// TODO (Low): Implement subvar
			}
			else if (i->second.Cmd == 0x83)
			{
				// TODO (Low): Implement mulvar
			}
			else if (i->second.Cmd == 0x84)
			{
				// TODO (Low): Implement divvar
			}
			else if (i->second.Cmd == 0x85)
			{
				// TODO (Low): Implement shiftvar
			}
			else if (i->second.Cmd == 0x86)
			{
				// TODO (Low): Implement randvar
			}
			else if (i->second.Cmd == 0x87)
			{
				// TODO (Low): Implement andvar
			}
			else if (i->second.Cmd == 0x88)
			{
				// TODO (Low): Implement orvar
			}
			else if (i->second.Cmd == 0x89)
			{
				// TODO (Low): Implement xorvar
			}
			else if (i->second.Cmd == 0x8A)
			{
				// TODO (Low): Implement notvar
			}
			else if (i->second.Cmd == 0x8B)
			{
				// TODO (Low): Implement modvar
			}
			else if (i->second.Cmd == 0x90)
			{
				// TODO (Low): Implement cmp_eq
			}
			else if (i->second.Cmd == 0x91)
			{
				// TODO (Low): Implement cmp_ge
			}
			else if (i->second.Cmd == 0x92)
			{
				// TODO (Low): Implement cmp_gt
			}
			else if (i->second.Cmd == 0x93)
			{
				// TODO (Low): Implement cmp_le
			}
			else if (i->second.Cmd == 0x94)
			{
				// TODO (Low): Implement cmp_lt
			}
			else if (i->second.Cmd == 0x95)
			{
				// TODO (Low): Implement cmp_ne
			}
			else if (i->second.Cmd == 0xA0)
			{
				// TODO (Low): Implement mod2_curve
			}
			else if (i->second.Cmd == 0xA1)
			{
				// TODO (Low): Implement mod2_phase
			}
			else if (i->second.Cmd == 0xA2)
			{
				// TODO (Low): Implement mod2_depth
			}
			else if (i->second.Cmd == 0xA3)
			{
				// TODO (Low): Implement mod2_speed
			}
			else if (i->second.Cmd == 0xA4)
			{
				// TODO (Low): Implement mod2_type
			}
			else if (i->second.Cmd == 0xA5)
			{
				// TODO (Low): Implement mod2_range
			}
			else if (i->second.Cmd == 0xA6)
			{
				// TODO (Low): Implement mod3_curve
			}
			else if (i->second.Cmd == 0xA7)
			{
				// TODO (Low): Implement mod3_phase
			}
			else if (i->second.Cmd == 0xA8)
			{
				// TODO (Low): Implement mod3_depth
			}
			else if (i->second.Cmd == 0xA9)
			{
				// TODO (Low): Implement mod3_speed
			}
			else if (i->second.Cmd == 0xAA)
			{
				// TODO (Low): Implement mod3_type
			}
			else if (i->second.Cmd == 0xAB)
			{
				// TODO (Low): Implement mod3_range
			}
			else if (i->second.Cmd == 0xAC)
			{
				// TODO (Low): Implement mod4_range
			}
			else if (i->second.Cmd == 0xAD)
			{
				// TODO (Low): Implement mod4_curve
			}
			else if (i->second.Cmd == 0xAE)
			{
				// TODO (Low): Implement mod4_phase
			}
			else if (i->second.Cmd == 0xAF)
			{
				// TODO (Low): Implement mod4_depth
			}
			else if (i->second.Cmd == 0xB0)
			{
				// TODO (Low): Implement mod4_speed
			}
			else if (i->second.Cmd == 0xB1)
			{
				// TODO (Low): Implement mod4_type
			}
			else if (i->second.Cmd == 0xE0)
			{
				// TODO (Low): Implement userproc
			}
			else if (i->second.Cmd == 0xE1)
			{
				// TODO (Low): Implement mod2_delay
			}
			else if (i->second.Cmd == 0xE2)
			{
				// TODO (Low): Implement mod2_period
			}
			else if (i->second.Cmd == 0xE3)
			{
				// TODO (Low): Implement mod3_delay
			}
			else if (i->second.Cmd == 0xE4)
			{
				// TODO (Low): Implement mod3_period
			}
			else if (i->second.Cmd == 0xE5)
			{
				// TODO (Low): Implement mod4_delay
			}
			else if (i->second.Cmd == 0xE6)
			{
				// TODO (Low): Implement mod4_period
			}
		}
	}

	if (smf->timebase == 0)
	{
		smfSetTimebase(smf, 48);
	}

	smfWriteFile(smf, FileName.substr(0, FileName.size() - 4).append("mid").c_str());
	smfDelete(smf);

	return true;
}
