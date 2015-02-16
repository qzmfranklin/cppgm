all: cy86

# build cy86 application
cy86: cy86.cpp
	g++ -g -std=gnu++11 -Wall -o cy86 cy86.cpp

# test cy86 application
test: all
	scripts/run_all_tests.pl cy86 my
	scripts/compare_results.pl ref my

# regenerate reference test output
ref-test:
	scripts/run_all_tests.pl cy86-ref ref

