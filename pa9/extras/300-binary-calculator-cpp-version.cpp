#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>

using namespace std;

template<typename T>
string to_hex(T t)
{
	ostringstream oss;

	for (size_t i = sizeof(T)*2; i > 0; i--)
	{
		T x = t;
		x >>= 4*(i-1);
		int y = x & 0xF;
		if (y >= 10)
			oss << char(y-10+'a');
		else
			oss << char(y+'0');
	}

	return oss.str();
}

template<typename T>
T from_hex(const string& h)
{
	T t = 0;

	for (char c : h)
	{
		t <<= 4;

		T x;
		if (c >= 'a')
			x = c-'a'+10;
		else
			x = c-'0';

		t |= x;
	}

	return t;
}

template<typename T>
void do_not(const string& op1s)
{
	T op1i = from_hex<T>(op1s);

	T res = ~op1i;

	cout << to_hex<T>(res) << endl;
}

template<typename T>
void do_binary(char O, const string& op1s, const string& op2s)
{
	T op1i = from_hex<T>(op1s);
	T op2i = from_hex<T>(op2s);

	T res;

	switch (O)
	{
	case '&': res = op1i & op2i; break;
	case '|': res = op1i | op2i; break;
	case '^': res = op1i ^  op2i; break;
	default: throw logic_error("internal error");
	}

	cout << to_hex<T>(res) << endl;
}

template<typename T>
void do_shift(char O, const string& op1s, const string& op2s)
{
	T op1i = from_hex<T>(op1s);
	int op2i = from_hex<int>(op2s);
	
	T res;

	using Z = typename make_signed<T>::type;

	switch (O)
	{
	case 'L': res = op1i << op2i; break;
	case 'S': res = Z(op1i) >> op2i; break;
	case 'U': res = op1i >> op2i; break;
	default: throw logic_error("internal error");
	}

	cout << to_hex<T>(res) << endl;
}

void read_op(string& op)
{
	cin >> op;
}

int main()
{
	size_t N;
	cin >> N;

	for (size_t i = 0; i < N; i++)
	{
		string s;
		cin >> s;
		if (s.size() != 2)
			throw logic_error(to_string(i) + ": invalid start token");
		char O = s[0];
		char S = s[1];

		size_t bytes = S - '0';

		string op1, op2;

		switch (O)
		{
		case '~':
			read_op(op1);
			if (op1.size() != bytes*2)
				throw logic_error(to_string(i) + ": op1 size incorrect: " + to_string(op1.size()));

			switch (bytes)
			{
			case 1: do_not<unsigned char>(op1); break;
			case 2: do_not<unsigned short int>(op1); break;
			case 4: do_not<unsigned int>(op1); break;
			case 8: do_not<unsigned long int>(op1); break;
			default: throw logic_error(to_string(i) + "unknown size " + to_string(bytes));
			}
		
			break;

		case '&':
		case '|':
		case '^':
			read_op(op1);
			read_op(op2);
			if (op1.size() != bytes*2 || op2.size() != bytes*2)
				throw logic_error(to_string(i) + ": op size incorrect");

			switch (bytes)
			{
			case 1: do_binary<unsigned char>(O, op1,op2); break;
			case 2: do_binary<unsigned short int>(O, op1,op2); break;
			case 4: do_binary<unsigned int>(O, op1,op2); break;
			case 8: do_binary<unsigned long int>(O, op1,op2); break;
			default: throw logic_error(to_string(i) + "unknown size " + to_string(bytes));
			}

			break;

		case 'L':
		case 'S':
		case 'U':
			read_op(op1);
			read_op(op2);
			if (op1.size() != bytes*2 || op2.size() != 2)
				throw logic_error(to_string(i) + ": op size incorrect");

			switch (bytes)
			{
			case 1: do_shift<unsigned char>(O, op1,op2); break;
			case 2: do_shift<unsigned short int>(O, op1,op2); break;
			case 4: do_shift<unsigned int>(O, op1,op2); break;
			case 8: do_shift<unsigned long int>(O, op1,op2); break;
			default: throw logic_error(to_string(i) + "unknown size " + to_string(bytes));
			}

			break;

		default:
			throw logic_error(to_string(i) + ": unknown O = " + string(1, O));
		}
	}
}
