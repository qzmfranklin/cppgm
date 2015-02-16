int main()
{
	{ 2 + 3; }

	if (x)
		y();
	
	if (x)
		y();
	else
		z();

	if (x)
		if (y)
			z();
		else
			w();

	switch (x)
	{
	case 1: f1(); break;
	case 2: f2(); break;
	case 3: f3(); break;
	default: g();
	}

	while (x)
		f();

	do f(); while (x);

	for (int i = 0; i < 10; i++)
		f();

	for (auto x : y)
		z();

	return x;
}

