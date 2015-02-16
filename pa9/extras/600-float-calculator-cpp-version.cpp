#include "float-functions.h"

typedef long double LD;

template<typename F>
void do_test()
{
	string sop1, O, sop2;
	cin >> sop1 >> O >> sop2;

	F op1 = float_from_hex<F>(sop1), op2 = float_from_hex<F>(sop2);

	if (O == "+") { cout << float_to_hex<F>(LD(op1) + LD(op2)) << endl; }
	else if (O == "-") { cout << float_to_hex<F>(LD(op1) - LD(op2)) << endl; }
	else if (O == "/") { cout << float_to_hex<F>(LD(op1) / LD(op2)) << endl; }
	else if (O == "*") { cout << float_to_hex<F>(LD(op1) * LD(op2)) << endl; }
	else if (O == "==") { cout << bool(LD(op1) == LD(op2)) << endl; }
	else if (O == "!=") { cout << bool(LD(op1) != LD(op2)) << endl; }
	else if (O == "<") { cout << bool(LD(op1) < LD(op2)) << endl; }
	else if (O == ">") { cout << bool(LD(op1) > LD(op2)) << endl; }
	else if (O == "<=") { cout << bool(LD(op1) <= LD(op2)) << endl; }
	else if (O == ">=") { cout << bool(LD(op1) >= LD(op2)) << endl; }
	else throw logic_error("unknown O: " + O);
}

int main()
{
	size_t N;
	cin >> N;

	for (size_t i = 0; i < N; i++)
	{
		string S;
		cin >> S;

		switch (S[0])
		{
		case '0' : do_test<long double>(); continue;
		case '4' : do_test<float>(); continue;
		case '8' : do_test<double>(); continue;
		}
	}
}
