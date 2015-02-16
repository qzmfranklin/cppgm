#include <vector>
#include <random>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

vector<char> opcodes = { '~', '&', '|', '^', 'L', 'S', 'U' };

vector<size_t> sizes = { 1, 2, 4, 8 };

unsigned long int R()
{
	static random_device rd;
	static mt19937 gen(rd());
	static uniform_int_distribution<unsigned long int> dis(0x0000000000000000UL, 0xFFFFFFFFFFFFFFFFUL); 
	return dis(gen);
}

string hexdump(unsigned long int op, size_t op_size)
{
	ostringstream oss;

	for (size_t i = 0; i < op_size*2; i++)
	{
		int val = op % 16;
		if (val < 10)
			oss << char(val + '0');
		else
			oss << char(val - 10 + 'a');
		op = op >> 4;
	}

	string s = oss.str();
	reverse(s.begin(), s.end());
	return s;
}

int main(int argc, char** argv)
{
	if (argc < 2)
		throw logic_error("usage: " + string(argv[0]) + " <num-test-inputs>");

	size_t N = stol(argv[1]);

	cout << N << endl;

	for (size_t i = 0; i < N; i++)
	{
		char opcode = opcodes[R() % opcodes.size()];
		size_t size = sizes[R() % sizes.size()];
		unsigned long int op1 = R(), op2 = R();

		size_t op1_size, op2_size;

		switch (opcode)
		{
		case '~':
			op1_size = size;
			op2_size = 0;
			break;

		case '&':
		case '|':
		case '^':
			op1_size = size;
			op2_size = size;
			break;

		case 'L':
		case 'S':
		case 'U':
			op1_size = size;
			op2_size = 1;
			op2 %= size*8;
			break;
		}

		cout << opcode << size << " " << hexdump(op1, op1_size);
		if (op2_size)
			cout << " " << hexdump(op2, op2_size);
		cout << endl;
	}
}

	
