// (C) 2013 CPPGM Foundation www.cppgm.org.  All rights reserved.

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <fstream>

using namespace std;

struct ElfHeader
{
    unsigned char ident[16] =
    {
        0x7f, 'E', 'L', 'F', // magic bytes
        2, // 64-bit architecture
        1, // two's compliment, little-endian
        1, // ELF specification version 1.0
        0, // System V ABI
        0, // ABI Version
        0, 0, 0, 0, 0, 0, 0 // Unused padding
    };

    short int type = 2; // executable file type
    short int machine = 0x3E; // x86-64 Architecture
    int version = 1; // ELF specification version 1.0
    long int entry; // entry point virtual memory address

    long int phoff = 64; // start of program segment header array file offset
    long int shoff = 0; // no sections

    int processor_flags = 0; // no processor-specific flags
    short int ehsize = 64; // ELF header is 64 bytes long
    short int phentsize = 56; // program header table entry size
    short int phnum = 1; // number of program headers       
    short int shentsize = 0; // no section header table entry size
    short int shnum = 0; // no sections
    short int shstrndx = 0; // no section header string table index
};

struct ProgramSegmentHeader
{
    int type = 1; // PT_LOAD: loadable segment

    static constexpr int executable = 1 << 0;
    static constexpr int writable = 1 << 1;
    static constexpr int readable = 1 << 2;

    int flags = executable | writable | readable; // segment permissions

    long int offset = 0; // source file offset
    long int vaddr = 0x400000; // destination (virtual) memory address
    long int paddr = 0; // unused, doesn't use physical memory
    long int filesz; // source length
    long int memsz; // destination length
    long int align = 0; // unused, alignment of file/memory
};

// bootstrap system call interface, used by RABSetFileExecutable
extern "C" long int syscall(long int n, ...) throw ();

// PA9SetFileExecutable: sets file at `path` executable
// returns true on success
bool PA9SetFileExecutable(const string& path)
{
    int res = syscall(/* chmod */ 90, path.c_str(), 0755);

    return res == 0;
}

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

		for (size_t i = 0; i < nsrcfiles; i++)
		{
			string srcfile = args[i+2];

			ifstream in(srcfile);

			// TODO: parse / semantically analyze / generate code for srcfile
		}

		ElfHeader elf_header;
		ProgramSegmentHeader program_segment_header;

		// TODO: Replace this with assembled x86 machine code / data from above

		char data[] = "TODO\n"; // 6 bytes

		unsigned char code[] =
		{
            // ==== write(stdout, "TODO\n") ====
			// mov rax, 1 ... system call `write`
			0x48, 0xc7, 0xc0, 0x01, 0x00, 0x00, 0x00,

			// mov rdi, 1 ... stdout fd
			0x48, 0xc7, 0xc7, 0x01, 0x00, 0x00, 0x00,
			
			// mov rsi, 0x400000 + 64 + 56 ... address of "TODO" string
			0x48, 0xbe,
			    0x78, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,

			// mov rdx, 5 ... num bytes to write
			0x48, 0xc7, 0xc2, 0x05, 0x00, 0x00, 0x00,

			// syscall
			0x0f, 0x05,
            // =====================


            // ===== exit(0) =======
			// mov rax, 60 ... system call `exit`
			0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00, 0x00,

			// mov rdi, 0 ... exit status 0
			0x48, 0xc7, 0xc7, 0x00, 0x00, 0x00, 0x00,

			// syscall
			0x0f, 0x05
            // =====================
		};

		elf_header.entry = 0x400000 + 64 + 56 + sizeof(data);
		program_segment_header.filesz = 64 + 56 + 6 + sizeof(data) + sizeof(code);
		program_segment_header.memsz = program_segment_header.filesz;

		{
			ofstream out(outfile);
			out.write((char*) &elf_header, 64);
			out.write((char*) &program_segment_header, 56);
			out.write((char*) data, sizeof(data));
			out.write((char*) code, sizeof(code));
		}

		PA9SetFileExecutable(outfile);
	}
	catch (exception& e)
	{
		cerr << "ERROR: " << e.what() << endl;
		return EXIT_FAILURE;
	}
}

