#define foo() bar

foo()

#undef foo

#define foo(x) bar

foo()

foo(qux)

#undef foo

#define foo(a,b) b bar a

foo(baz,qux)

#undef foo

