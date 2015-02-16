all: \
	300-binary-calculator-test-data-generator \
	300-binary-calculator-cpp-version \
	400-integer-calculator-test-data-generator \
	400-integer-calculator-cpp-version \
	500-to-float80-test-data-generator \
	500-to-float80-cpp-version \
	600-float-calculator-test-data-generator \
	600-float-calculator-cpp-version

300-binary-calculator-test-data-generator: 300-binary-calculator-test-data-generator.cpp
	g++ -g -std=gnu++11 -o300-binary-calculator-test-data-generator 300-binary-calculator-test-data-generator.cpp

300-binary-calculator-cpp-version: 300-binary-calculator-cpp-version.cpp
	g++ -O3 -std=gnu++11 -o300-binary-calculator-cpp-version 300-binary-calculator-cpp-version.cpp

400-integer-calculator-test-data-generator: 400-integer-calculator-test-data-generator.cpp
	g++ -g -std=gnu++11 -o400-integer-calculator-test-data-generator 400-integer-calculator-test-data-generator.cpp

400-integer-calculator-cpp-version: 400-integer-calculator-cpp-version.cpp
	g++ -O3 -std=gnu++11 -o400-integer-calculator-cpp-version 400-integer-calculator-cpp-version.cpp

500-to-float80-test-data-generator: 500-to-float80-test-data-generator.cpp
	g++ -O3 -std=gnu++11 -o500-to-float80-test-data-generator 500-to-float80-test-data-generator.cpp

500-to-float80-cpp-version: 500-to-float80-cpp-version.cpp
	g++ -O3 -std=gnu++11 -o500-to-float80-cpp-version 500-to-float80-cpp-version.cpp

600-float-calculator-test-data-generator: 600-float-calculator-test-data-generator.cpp float-functions.h
	g++ -O3 -std=gnu++11 -o600-float-calculator-test-data-generator 600-float-calculator-test-data-generator.cpp

600-float-calculator-cpp-version: 600-float-calculator-cpp-version.cpp float-functions.h
	g++ -O3 -std=gnu++11 -o600-float-calculator-cpp-version 600-float-calculator-cpp-version.cpp
