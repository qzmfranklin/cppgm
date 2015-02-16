C c;
asm( "foo" );
using foo = int;

namespace N
{
	C c;
}

namespace N3 = ::N1::N2;

template<typename C>
class TC
{
	C c;
};


