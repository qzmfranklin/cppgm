template<int n, class C1>
class C2
{
	template<int m, class C3>
	void f(C3 c) { return C2(c); }
};

template<>
class TC2<n,C1>
{
	typedef C4 c;
};

template<typename Y>
Y foo()
{
	return Y();
}


