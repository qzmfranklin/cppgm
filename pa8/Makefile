all: nsinit

# build nsexpr application
nsinit: nsinit.cpp
	g++ -g -std=gnu++11 -Wall -o nsinit nsinit.cpp

# test nsexpr application
test: all
	scripts/run_all_tests.pl nsinit my
	scripts/compare_results.pl ref my

# regenerate reference test output
ref-test:
	scripts/run_all_tests.pl nsinit-ref ref
	scripts/run_all_tests.pl nsinit-other other
	-rm tests/*.other
	scripts/compare_exit_status.pl other ref

