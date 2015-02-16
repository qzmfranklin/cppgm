all: ctrlexpr

# build posttoken application
ctrlexpr: ctrlexpr.cpp
	g++ -g -std=gnu++11 -Wall -o ctrlexpr ctrlexpr.cpp

# test posttoken application
test: all
	scripts/run_all_tests.pl ctrlexpr my
	scripts/compare_results.pl ref my

# regenerate reference test output
ref-test:
	scripts/run_all_tests.pl ctrlexpr-ref ref

