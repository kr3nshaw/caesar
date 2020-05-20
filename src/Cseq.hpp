#pragma once

#include <cstdint>
#include <ios>
#include <string>
#include <vector>

enum SuffixType { _None, _Rnd, _Var, _Time, _TimeRnd, _TimeVar, _If };
enum ArgType { None, Uint8, Int8, Uint16, Int16, Rnd, Var, VarLen };

std::vector<int32_t> ReadArgs(uint8_t*& pos, ArgType argType);

struct CseqCmd
{
	SuffixType Suffix1 = _None;
	SuffixType Suffix2 = _None;
	SuffixType Suffix3 = _None;
	bool Extended = false;
	uint8_t Cmd;
	std::vector<int32_t> Args;

	ArgType Arg1 = None;
	ArgType Arg2 = None;

	std::string Label;
};

struct CseqLabl
{
	uint8_t* Offset;
	std::string Label;
};

struct Cseq
{
	std::string FileName;
	std::streamoff Length;
	uint8_t* Data = nullptr;

	Cseq(const char* fileName);
	~Cseq();
	bool Convert();
};
