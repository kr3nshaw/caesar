//
//  Common.cpp
//  caesar
//
//  Created by Kai Renshaw on 28/5/18.
//  Copyright © 2018 Kai Renshaw. All rights reserved.
//

#include "Common.hpp"
#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>
#include <iterator>
#include <map>
#include <stack>
#include <string>
#include <utility>

using namespace std;

stack<string> Common::FileNames;
stack<uint8_t*> Common::Offsets;
multimap<string, uint32_t> Common::Results;

int32_t ReadFixLen(uint8_t* &pos, size_t bytes, bool littleEndian, bool isSigned)
{
	int32_t result = 0;

	for (size_t i = 0; i < bytes; ++i)
	{
		result |= *pos++ << ((littleEndian ? i : bytes - i - 1) * 8);
	}

	if (isSigned && (result >= (1 << ((bytes * 8) - 1))))
	{
		result -= 1 << (bytes * 8);
	}

	return result;
}

int32_t ReadVarLen(uint8_t* &pos)
{
	int32_t result = 0;

	do
	{
		result = (result << 7) | (*pos & 0x7F);
	} while (*pos++ & 0x80);

	return result;
}

void Common::Push(string fileName, uint8_t* data)
{
	FileNames.push(fileName);
	Offsets.push(data);

	cout << FileNames.top() << endl;
}

void Common::Pop()
{
	Offsets.pop();
	FileNames.pop();
}

void Common::Analyse(string tag, uint32_t val)
{
	Common::Results.insert(pair<string, uint32_t>(tag, val));
}

void Common::Dump()
{
	// TODO (Medium): This doesn't actually work
	for (auto i = Common::Results.begin(), end = Common::Results.end(); i != end; i = Common::Results.upper_bound(i->first))
	{
		cout << hex << setfill('0') << uppercase << endl;
		cout << i->first << "\t" << setw(2) << i->second << endl;
	}
}
