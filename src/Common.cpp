#include "Common.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

bool Common::ShowWarnings = false;
stack<string> Common::FileNames;
stack<uint8_t*> Common::Offsets;
vector<string> Common::Log;

int32_t ReadFixLen(uint8_t*& pos, size_t bytes, bool littleEndian, bool isSigned)
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

int32_t ReadVarLen(uint8_t*& pos)
{
	int32_t result = 0;

	do
	{
		result = (result << 7) | (*pos & 0x7F);
	} while (*pos++ & 0x80);

	return result;
}

void Common::Warning(uint8_t* pos, string msg)
{
	if (ShowWarnings)
	{
		cerr << hex << setfill('0') << uppercase << endl;
		cerr << "WARNING IN\t" << Common::FileNames.top() << endl;
		cerr << "AT POSITION\t0x" << setw(8) << pos - Common::Offsets.top() << endl;
		cerr << "MESSAGE\t\t" << msg << endl;
		cerr << endl;
	}
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
	Common::Log.push_back(FileNames.top() + "," + tag + "," + to_string(val));
}

void Common::Dump(string fileName)
{
	ofstream ofs(fileName);
	ofs << "fileName,tag,val" << endl;

	for (size_t i = 0; i < Common::Log.size(); ++i)
	{
		ofs << Common::Log[i] << endl;
	}

	ofs.close();
}
