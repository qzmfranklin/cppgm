#define foo 2

#if foo + 2 == 4

true1

#   if foo == 3

false1

#   elif foo == 2

true2

#   else

false2

#   endif

#else

false3

#endif

