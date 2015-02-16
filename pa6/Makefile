all: recog

# build posttoken application
recog: recog.cpp
	g++ -g -std=gnu++11 -Wall -o recog recog.cpp

# test posttoken application
test: all
	scripts/run_all_tests.pl recog my
	scripts/compare_results.pl ref my

# regenerate reference test output
ref-test:
	scripts/run_all_tests.pl recog-ref ref

