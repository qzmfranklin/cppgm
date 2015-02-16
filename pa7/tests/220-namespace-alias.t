namespace A { typedef int T; }
namespace B { typedef char T; }
namespace C { typedef double T; }

namespace D = B;

A::T x;
D::T y;

