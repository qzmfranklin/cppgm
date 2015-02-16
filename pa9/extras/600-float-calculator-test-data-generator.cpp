#include "float-functions.h"

vector<string> opcodes = { "+", "-", "*", "/", "==", "!=", "<", ">", "<=", ">=" };

vector<string> sizes = { "4", "8", "0" };

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

bool unreasonable(const string& size, const string& op1, const string& opcode, const string& op2)
{
	return false;
}

template<class F>
void do_test(const string& S, const string& O, size_t size)
{
	F op1 = gen_floating<F>();
	F op2 = gen_floating<F>();

	cout << S << " " << float_to_hex<F>(op1) << " " << O << " " << float_to_hex<F>(op2) << endl;
}

int main(int argc, char** argv)
{
	if (argc < 2)
		throw logic_error("usage: " + string(argv[0]) + " <num-test-inputs>");

	size_t N = stol(argv[1]);

	cout << N << endl;

	for (size_t i = 1000000; i < 1000000+N; i++)
	{
		string S = sizes[R() % sizes.size()];

		string O = opcodes[R() % opcodes.size()];

		size_t size;

		switch (S[0])
		{
		case '4': do_test<float>(S, O, 4); break;
		case '8': do_test<double>(S, O, 8); break;
		case '0': do_test<long double>(S, O, 10); break;
		default: throw logic_error("internal error");
		}
	}
}
