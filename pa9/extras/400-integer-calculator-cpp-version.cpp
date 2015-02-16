#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <type_traits>

using namespace std;

template<typename T>
T convert(const string& s)
{
	if (is_signed<T>())
		return stol(s);
	else
		return stoul(s);
}

template<typename T>
void test(const string& x, const string& O, const string& y)
{
	T op1 = convert<T>(x);
	T op2 = convert<T>(y);

	if (is_signed<T>())
	{
		if (O == "+")  { cout << ((signed long int)(T(op1 + op2)))  << endl; return; }
		if (O == "-")  { cout << ((signed long int)(T(op1 - op2)))  << endl; return; }
		if (O == "*")  { cout << ((signed long int)(T(op1 * op2)))  << endl; return; }
		if (O == "/")  { cout << ((signed long int)(T(op1 / op2)))  << endl; return; }
		if (O == "%")  { cout << ((signed long int)(T(op1 % op2)))  << endl; return; }
		if (O == "==") { cout << ((signed long int)(T(op1 == op2))) << endl; return; }
		if (O == "!=") { cout << ((signed long int)(T(op1 != op2))) << endl; return; }
		if (O == "<")  { cout << ((signed long int)(T(op1 < op2)))  << endl; return; }
		if (O == ">")  { cout << ((signed long int)(T(op1 > op2)))  << endl; return; }
		if (O == "<=") { cout << ((signed long int)(T(op1 <= op2))) << endl; return; }
		if (O == ">=") { cout << ((signed long int)(T(op1 >= op2))) << endl; return; }
	}
	else
	{
		if (O == "+")  { cout << ((unsigned long int)(T(op1 + op2)))  << endl; return; }
		if (O == "-")  { cout << ((unsigned long int)(T(op1 - op2)))  << endl; return; }
		if (O == "*")  { cout << ((unsigned long int)(T(op1 * op2)))  << endl; return; }
		if (O == "/")  { cout << ((unsigned long int)(T(op1 / op2)))  << endl; return; }
		if (O == "%")  { cout << ((unsigned long int)(T(op1 % op2)))  << endl; return; }
		if (O == "==") { cout << ((unsigned long int)(T(op1 == op2))) << endl; return; }
		if (O == "!=") { cout << ((unsigned long int)(T(op1 != op2))) << endl; return; }
		if (O == "<")  { cout << ((unsigned long int)(T(op1 < op2)))  << endl; return; }
		if (O == ">")  { cout << ((unsigned long int)(T(op1 > op2)))  << endl; return; }
		if (O == "<=") { cout << ((unsigned long int)(T(op1 <= op2))) << endl; return; }
		if (O == ">=") { cout << ((unsigned long int)(T(op1 >= op2))) << endl; return; }
	}

	throw logic_error("unknown O: " + O);
}

int main()
{
	size_t N;
	cin >> N;

	for (size_t i = 0; i < N; i++)
	{
		string S, x, O, y;
		cin >> S >> x >> O >> y;

		if (S == "s1") { test<signed char>(x, O, y); continue; }
		if (S == "s2") { test<signed short int>(x, O, y); continue; }
		if (S == "s4") { test<signed int>(x, O, y); continue; }
		if (S == "s8") { test<signed long int>(x, O, y); continue; }
		if (S == "u1") { test<unsigned char>(x, O, y); continue; }
		if (S == "u2") { test<unsigned short int>(x, O, y); continue; }
		if (S == "u4") { test<unsigned int>(x, O, y); continue; }
		if (S == "u8") { test<unsigned long int>(x, O, y); continue; }
		throw logic_error("unknown S: " + S);
	}
}
