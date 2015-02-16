int main()
{
	try
	{
		f();
		throw exCeption("oh no");
	}
	catch (exCeption& e)
	{
		cout << e.what();
		cin >> e.what();
	}
	catch (...)
	{
	}
}

void f()
try
{
	throw C();
}
catch (const C& c)
{
	throw;
}

void g() throw(int);
void (*fp)() throw (int);
void h(void pfa() throw(int));

void i() noexcept(2+3);

