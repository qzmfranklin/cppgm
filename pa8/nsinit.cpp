// (C) 2013 CPPGM Foundation www.cppgm.org.  All rights reserved.

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char** argv)
{
	try
	{
		vector<string> args;

		for (int i = 1; i < argc; i++)
			args.emplace_back(argv[i]);

		if (args.size() < 3 || args[0] != "-o")
			throw logic_error("invalid usage");

		string outfile = args[1];
		size_t nsrcfiles = args.size() - 2;


		vector<char> program_image;

		for (size_t i = 0; i < nsrcfiles; i++)
		{
			string srcfile = args[i+2];

			ifstream in(srcfile);

			// ...

			program_image.push_back('?');
		}

		ofstream out(outfile);

		out.write(program_image.data(), program_image.size());
	}
	catch (exception& e)
	{
		cerr << "ERROR: " << e.what() << endl;
		return EXIT_FAILURE;
	}
}

