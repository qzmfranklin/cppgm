typedef int& LRI;
typedef int&& RRI;

void f(LRI&, const LRI&, const LRI&&, RRI&, RRI&&);

