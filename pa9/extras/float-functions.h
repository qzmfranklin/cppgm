#pragma once

#include <string>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

template<typename F> struct ftrait;

template<> struct ftrait<float>
{
	static constexpr size_t E = 8;
	static constexpr size_t I = 0;
	static constexpr size_t T = 23;

	static constexpr size_t nbytes = 4;
};

template<> struct ftrait<double>
{
	static constexpr size_t E = 11;
	static constexpr size_t I = 0;
	static constexpr size_t T = 52;

	static constexpr size_t nbytes = 8;
};

template<> struct ftrait<long double>
{
	static constexpr size_t E = 15;
	static constexpr size_t I = 1;
	static constexpr size_t T = 63;

	static constexpr size_t nbytes = 10;
};

unsigned long int R()
{
	static random_device rd;
	static mt19937 gen(rd());
	static uniform_int_distribution<unsigned long int>
		dis(0x0000000000000000UL, 0xFFFFFFFFFFFFFFFFUL);
	return dis(gen);
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

template<typename T>
string float_to_hex(T t)
{
	ostringstream oss;

	unsigned char* begin = (unsigned char*) &t;
	unsigned char* end = begin + ftrait<T>::nbytes;

	for (auto p = begin; p < end; p++)
	{
		int h0 = *p >> 4;
		int h1 = *p & 0xF;

		oss << char(h0 < 10 ? '0' + h0 : 'a' - 10 + h0)
			 << char(h1 < 10 ? '0' + h1 : 'a' - 10 + h1);
	}

	return oss.str();
}

template<typename T>
T float_from_hex(const string& h)
{
	T t;

	unsigned char* begin = (unsigned char*)&t;

	for (size_t i = 0; i < ftrait<T>::nbytes; i++)
	{
		char c0 = h[2*i + 0];
		char c1 = h[2*i + 1];

		begin[i] = (c0 < 'a' ? c0 - '0' : c0 + 10 - 'a') << 4
			       | (c1 < 'a' ? c1 - '0' : c1 + 10 - 'a');
	}

	return t;
}

template<typename F>
F gen_floating()
{
	constexpr size_t E = ftrait<F>::E;
	constexpr size_t I = ftrait<F>::I;
	constexpr size_t T = ftrait<F>::T;
	constexpr size_t N = ftrait<F>::nbytes;

	constexpr size_t bS = 0;
	constexpr size_t eS = 1;
	constexpr size_t bE = eS;
	constexpr size_t eE = bE + E;
	constexpr size_t bI = eE;
	constexpr size_t eI = bI + I;
	constexpr size_t bT = eI;
	constexpr size_t eT = bT + T;

	static_assert(eT == 8 * N, "eT ftraits mismatch");

	F t;

	unsigned char* begin = (unsigned char*)&t;
	unsigned char* end = begin + N;

	set_bits(begin, bS, eS, R());

	switch (R() % 3)
	{
	case 0:
		set_bits(begin, bE, eE, 0);
		if (R() % 2)
		{
			set_bits(begin, bT, eT, 0); // +/- zero
			set_bits(begin, bI, eI, 0);
		}
		else
		{
			set_bits(begin, bT, eT, 1 + R() % ((1UL<<T) -1)); // subnormal
			set_bits(begin, bI, eI, 0);
		}
		break;

	case 1:
		set_bits(begin, bE, eE, (1UL << E) - 1);
		set_bits(begin, bT, eT, 0); // +/- inf
		set_bits(begin, bI, eI, 1);

		break;

	case 2:
		set_bits(begin, bE, eE, 1 + R() % ((1UL << E) - 2));
		set_bits(begin, bT, eT, R() % (1UL << T)); // normal
		set_bits(begin, bI, eI, 1);

		break;
	}

	reverse(begin, end);

	string s = float_to_hex<F>(t);
	F t2 = float_from_hex<F>(s);

	if (t != t2)
	{
		cerr << "N = " << N << endl;
		cerr << "t = " << t << endl;
		cerr << "s = " << s << endl;
		cerr << "t2 = " << t2 << endl;
		cerr << "s2 = " << float_to_hex<F>(t2) << endl;

		throw logic_error("float_to/from_hex fail");
	}

	return t;
}


