#include "Common.hpp"
#include "Csar.hpp"

#include <cstring>
#include <iostream>

using namespace std;

int main(int argc, char* argv[])
{
	bool p = false;

	if (argc == 1)
	{
		cout << "OVERVIEW: Caesar" << endl << endl;
		cout << "USAGE: caesar [options] <inputs>" << endl << endl;
		cout << "OPTIONS:" << endl;
		cout << "\t-p\tDo not ignore pan values of stereo samples" << endl;
		cout << "\t-w\tShow warnings" << endl;

		return 1;
	}
	else
	{
		for (int i = 1; i < argc; ++i)
		{
			if (!strcmp(argv[i], "-p"))
			{
				p = true;
			}
			else if (!strcmp(argv[i], "-w"))
			{
				Common::ShowWarnings = true;
			}
			else
			{
				Csar csar(argv[i], p);

				if (!csar.Extract())
				{
					return 1;
				}
			}
		}
	}

	return 0;
}
