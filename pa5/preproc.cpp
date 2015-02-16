// (C) 2013 CPPGM Foundation www.cppgm.org.  All rights reserved.

#include <utility>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>

using namespace std;

// For pragma once implementation:
// system-wide unique file id type `PA5FileId`
typedef pair<unsigned long int, unsigned long int> PA5FileId;

// bootstrap system call interface, used by PA5GetFileId
extern "C" long int syscall(long int n, ...) throw ();

// PA5GetFileId returns true iff file found at path `path`.
// out parameter `out_fileid` is set to file id
bool PA5GetFileId(const string& path, PA5FileId& out_fileid)
{
	struct
	{
			unsigned long int dev;
			unsigned long int ino;
			long int unused[16];
	} data;

	int res = syscall(4, path.c_str(), &data);

	out_fileid = make_pair(data.dev, data.ino);

	return res == 0;
}

// OPTIONAL: Also search `PA5StdIncPaths` on `--stdinc` command-line switch (not by default)
vector<string> PA5StdIncPaths =
{
    "/usr/include/c++/4.7/",
    "/usr/include/c++/4.7/x86_64-linux-gnu/",
    "/usr/include/c++/4.7/backward/",
    "/usr/lib/gcc/x86_64-linux-gnu/4.7/include/",
    "/usr/local/include/",
    "/usr/lib/gcc/x86_64-linux-gnu/4.7/include-fixed/",
    "/usr/include/x86_64-linux-gnu/",
    "/usr/include/"
};

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

		ofstream out(outfile);

		out << "preproc " << nsrcfiles << endl;

		for (size_t i = 0; i < nsrcfiles; i++)
		{
			string srcfile = args[i+2];

			out << "sof " << srcfile << endl;

			ifstream in(srcfile);

			// TODO: implement `preproc` as per PA5 description
			out << "not yet implemented" << endl;
	
			out << "eof" << endl;

		}
	}
	catch (exception& e)
	{
		cerr << "ERROR: " << e.what() << endl;
		return EXIT_FAILURE;
	}
}

