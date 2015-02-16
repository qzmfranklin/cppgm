int main()
{
	--x;
	++x;
	*x;
	&x;
	+x;
	-x;
	!x;
	~x;
	sizeof x;
	sizeof !x;
	sizeof (x);
	sizeof...(x);
	alignof(int);
	noexcept(3);
	new C;
	::new (3,2,4) C {foo};
	delete x;
	::delete [] x;

}

