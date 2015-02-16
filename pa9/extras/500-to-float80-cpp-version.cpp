#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>

using namespace std;

template<typename T, size_t n = sizeof(T)>
string to_hex(T t)
{
	ostringstream oss;

	unsigned char* begin = (unsigned char*) &t;
	unsigned char* end = begin + n;

	for (size_t i = 0; i < n; i++)
	{
		int d0 = begin[i] >> 4;
		int d1 = begin[i] & 0xF;

		cout << char(d0 < 10 ? '0' + d0 : d0 - 10 + 'a');
		cout << char(d1 < 10 ? '0' + d1 : d1 - 10 + 'a');
	}

	return oss.str();
}

template<typename T, size_t n = sizeof(T)>
T from_hex(const string& h)
{
	T t = 0;
	unsigned char* begin = (unsigned char*) &t;
	unsigned char* end = begin + n;

	for (size_t i = 0; i < n; i++)
		begin[i] = 
			(h[2*i+0] >= 'a' ? h[2*i+0] - 'a' + 10 : h[2*i+0] - '0') << 4 | 
			(h[2*i+1] >= 'a' ? h[2*i+1] - 'a' + 10 : h[2*i+1] - '0');

	return t;
}

template<typename T>
void do_test(const string& op)
{
	T in = from_hex<T>(op);
	long double out = in;

	cout << to_hex<long double, 10>(out);
}

int main()
{
	size_t N;
	cin >> N;
	cout << N << endl;

	for (size_t i = 0; i < N; i++)
	{
		string size, op;
		cin >> size >> op;
		if (size.size() != 2)
			throw logic_error(to_string(i) + ": invalid start token");

		cout << size << " ";

		if (size == "s1") do_test<signed char>(op);
		else if (size == "s2") do_test<signed short int>(op);
		else if (size == "s4") do_test<signed int>(op);
		else if (size == "s8") do_test<signed long int>(op);
		else if (size == "u1") do_test<unsigned char>(op);
		else if (size == "u2") do_test<unsigned short int>(op);
		else if (size == "u4") do_test<unsigned int>(op);
		else if (size == "u8") do_test<unsigned long int>(op);
		else if (size == "f4") do_test<float>(op);
		else if (size == "f8") do_test<double>(op);
		else throw logic_error("unknown size: " + size);

		cout << endl;
	}
}
