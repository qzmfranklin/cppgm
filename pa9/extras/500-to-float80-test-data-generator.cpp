#include <vector>
#include <random>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <algorithm>

using namespace std;

vector<string> sizes = { "s1", "s2", "s4", "s8", "u1", "u2", "u4", "u8", "f4", "f8" };

unsigned long int R()
{
	static random_device rd;
	static mt19937 gen(rd());
	static uniform_int_distribution<unsigned long int> dis(0x0000000000000000UL, 0xFFFFFFFFFFFFFFFFUL); 
	return dis(gen);
}

void write_hex(unsigned char* begin, unsigned char* end)
{
	for (auto p = begin; p < end; p++)
	{
		unsigned char c = *p;
		c >>= 4;
		cout << char(c < 10 ? '0' + c : 'a' + c - 10);
		c = *p;
		c &= 0xF;
		cout << char(c < 10 ? '0' + c : 'a' + c - 10);
	}
}


template<typename T>
void gen_int()
{
	T t = R();
	unsigned char* begin = (unsigned char*)&t;
	unsigned char* end = begin + sizeof(T);
	write_hex(begin, end);

}

void set_bits(unsigned char* p, size_t b, size_t e, unsigned long int v)
{
	size_t n = e - b;

	for (size_t i = 0; i < n; i++)
	{
		bool B = v & (1 << i);

		size_t P = e - 1 - i;

		size_t Q = P / 8;
		size_t R = 7 - P % 8;

		p[Q] &= ~(1 << R);
		p[Q] |= B * (1 << R);
	}
}

template<typename F, size_t E, size_t T>
void gen_floating()
{
	size_t bS = 0;
	size_t eS = 1;
	size_t bE = eS;
	size_t eE = bE + E;
	size_t bT = eE;
	size_t eT = bT + T;

	F t;
	unsigned char* begin = (unsigned char*)&t;
	unsigned char* end = begin + sizeof(F);

	set_bits(begin, bS, eS, R());

	switch (R() % 3)
	{
	case 0:
		set_bits(begin, bE, eE, 0);
		if (R() % 2)
		{
			set_bits(begin, bT, eT, 0); // +/- zero
		}
		else
		{
			set_bits(begin, bT, eT, 1 + R() % ((1UL<<T) -1)); // subnormal
		}
		break;

	case 1:
		set_bits(begin, bE, eE, (1UL << E) - 1);
		set_bits(begin, bT, eT, 0); // +/- inf
		break;

	case 2:
		set_bits(begin, bE, eE, 1 + R() % ((1UL << E) - 2));
		set_bits(begin, bT, eT, R() % (1UL << T)); // normal
		break;
	}

	reverse(begin, end);

	write_hex(begin, end);
}

int main(int argc, char** argv)
{
	if (argc < 2)
		throw logic_error("usage: " + string(argv[0]) + " <num-test-inputs>");

	size_t N = stol(argv[1]);

	cout << N << endl;

	for (size_t i = 1000000; i < 1000000+N; i++)
	{
		string size = sizes[R() % sizes.size()];

		cout << size << " ";

	  	     if (size == "s1") gen_int<unsigned char>();
		else if (size == "s2") gen_int<unsigned short int>();
		else if (size == "s4") gen_int<unsigned int>();
		else if (size == "s8") gen_int<unsigned long int>();
		else if (size == "u1") gen_int<unsigned char>();
		else if (size == "u2") gen_int<unsigned short int>();
		else if (size == "u4") gen_int<unsigned int>();
		else if (size == "u8") gen_int<unsigned long int>();
		else if (size == "f4") gen_floating<float, 8, 23>();
		else if (size == "f8") gen_floating<double, 11, 52>();
		else throw logic_error("unexpected size");

		cout << endl;
	}
}

