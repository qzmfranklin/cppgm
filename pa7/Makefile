all: nsdecl

# build nsdecl application
nsdecl: nsdecl.cpp
	g++ -g -std=gnu++11 -Wall -o nsdecl nsdecl.cpp

# test nsdecl application
test: all
	scripts/run_all_tests.pl nsdecl my
	scripts/compare_results.pl ref my

# regenerate reference test output
ref-test:
	scripts/run_all_tests.pl nsdecl-ref ref

