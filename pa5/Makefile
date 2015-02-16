all: preproc

# build posttoken application
preproc: preproc.cpp
	g++ -g -std=gnu++11 -Wall -o preproc preproc.cpp

# test posttoken application
test: all
	scripts/run_all_tests.pl preproc my
	scripts/compare_results.pl ref my

# regenerate reference test output
ref-test:
	scripts/run_all_tests.pl preproc-ref ref

