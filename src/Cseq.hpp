#pragma once

#include <cstdint>
#include <ios>
#include <string>
#include <vector>

enum class SuffixType { None, Rnd, Var, Time, TimeRnd, TimeVar, If };
enum class ArgType { None, Uint8, Int8, Uint16, Int16, Rnd, Var, VarLen };

std::vector<int32_t> ReadArgs(uint8_t*& pos, ArgType argType);

struct CseqCmd
{
	SuffixType Suffix1 = SuffixType::None;
	SuffixType Suffix2 = SuffixType::None;
	SuffixType Suffix3 = SuffixType::None;
	bool Extended = false;
	uint8_t Cmd;
	std::vector<int32_t> Args;

	ArgType Arg1 = ArgType::None;
	ArgType Arg2 = ArgType::None;

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
