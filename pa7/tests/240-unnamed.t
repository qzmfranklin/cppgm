namespace { typedef int I; }

I i;

namespace A {
	namespace {
		typedef double I;
		typedef char J;
	}

	I i;
}

using namespace A;

A::I x;
J j;

