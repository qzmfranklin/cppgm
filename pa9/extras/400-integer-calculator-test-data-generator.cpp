#include <vector>
#include <random>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

vector<string> opcodes = { "+", "-", "*", "/", "%", "==", "!=", "<", ">", "<=", ">=" };

vector<string> sizes = { "s1", "s2", "s4", "s8", "u1", "u2", "u4", "u8" };

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

string bound(string size, unsigned long int op)
{
	if (size == "s1") return to_string((signed char)op);
	if (size == "s2") return to_string((signed short int)op);
	if (size == "s4") return to_string((signed int)op);
	if (size == "s8") return to_string((signed long int)op);
	if (size == "u1") return to_string((unsigned char)op);
	if (size == "u2") return to_string((unsigned short int)op);
	if (size == "u4") return to_string((unsigned int)op);
	if (size == "u8") return to_string((unsigned long int)op);

	throw logic_error("unknown size");
}

bool unreasonable(const string& size, const string& op1, const string& opcode, const string& op2)
{
	if (opcode != "%" && opcode != "/")
		return false;

	if (op2 == "0")
		return true;

	if (op2 != "-1")
		return false;

	if (size == "s1" && op1 == "-128") return true;
	if (size == "s2" && op1 == "-32768") return true;
	if (size == "s4" && op1 == "-2147483648") return true;
	if (size == "s8" && op1 == "-9223372036854775808") return true;

	return false;
}

int main(int argc, char** argv)
{
	if (argc < 2)
		throw logic_error("usage: " + string(argv[0]) + " <num-test-inputs>");

	size_t N = stol(argv[1]);

	cout << N << endl;

	for (size_t i = 1000000; i < 1000000+N; i++)
	{
		string opcode = opcodes[R() % opcodes.size()];
		string size = sizes[R() % sizes.size()];

		string op1 = bound(size, R());
		string op2 = bound(size, R());

		if (unreasonable(size, op1, opcode, op2))
		{
			i--;
			continue;
		}

		cout << size << " " << op1 << " " << opcode << " " << op2 << endl;
	}
}

