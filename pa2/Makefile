all: posttoken

# build posttoken application
posttoken: posttoken.cpp
	g++ -g -std=gnu++11 -Wall -o posttoken posttoken.cpp

# test posttoken application
test: all
	scripts/run_all_tests.pl posttoken my
	scripts/compare_results.pl ref my

# regenerate reference test output
ref-test:
	scripts/run_all_tests.pl posttoken-ref ref

