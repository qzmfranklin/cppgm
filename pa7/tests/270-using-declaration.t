namespace A
{
	typedef int T;
}

using A::T;

typedef const T CT;

namespace X
{
	using ::CT;
	using A::T;
}

X::CT* ci;
X::T i;

