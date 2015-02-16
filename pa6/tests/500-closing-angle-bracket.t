int main()
{
	TC1<(1>2)> x2;       // OK - not at same nested level
	TC1<TC2<1>> x3;      // OK
    	TC1<TC2<(6>>1)>> x5; // OK
}

