// see N3485 8.2

int main()
{
	C w(int(a)); // function declaration
	C x(int());  // function declaration
	C y((int)a); // object declaration
	C z = int(a); // object declaration
}

