all: macro

# build posttoken application
macro: macro.cpp
	g++ -g -std=gnu++11 -Wall -o macro macro.cpp

# test posttoken application
test: all
	scripts/run_all_tests.pl macro my
	scripts/compare_results.pl ref my

# regenerate reference test output
ref-test:
	scripts/run_all_tests.pl macro-ref ref

