
foo

#define CATPRAG(a, b) PRAGMA(a ## b)
#define PRAGMA(x) _Pragma(#x)

CATPRAG( on, ce )

bar

