#if 0
#if foo bar baz
#else @@@@@ // <--- else out of order
#elif foo bar baz ... @
#endif @@@@@
#else
ok
#endif


