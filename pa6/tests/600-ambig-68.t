// see N3485 6.8
int main()
{
	C(a)->m = 7;     // expression-statement
	C(a)++;          // expression-statement
	C(a,5)<<c;       // expression-statement

	C(*d)(int);      // declaration
	C(e)[5];         // declaration
	C(f) = { 1, 2 }; // declaration
	C(*g)(double(3)); // declaration

	C(a);		// declaration
	C(*b)();        // declaration
	C(c)=7;		// declaration
	C(d), e, f=3;   // declaration
	C(g)(h,2);	// declaration
}

