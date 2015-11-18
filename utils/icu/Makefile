ROOT    :=$(realpath ../..)
D       :=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))# this directory
-include $(ROOT)/root.mk
-include $(ROOT)/gtest/rules.mk
-include $(D)/rules.mk
# Inlcude more rules.mk here if you this directory depends on them.
-include $(DEP)

# List all the executables you want to run when you type `make test` in $(TESTS)
# Note that you need to -include $(ROOT)/gtest/rules.mk to actually build those
# executables
TESTS:=$(wildcard $(D)/test_*.cpp) $(wildcard $(D)/gtest_*.cpp)
TESTS:=$(TESTS:$(D)/%.cpp=%.exe)

.PHONY: all asm clean test
all: $(OBJ)
asm: $(ASM)
clean:
	$(QUIET)rm -f *.exe $(OBJ) $(DEP) $(ASM)
test: $(TESTS)
	$(QUIET)for t in $^ ; do ./"$$t" || exit 1 ; done

# Sample linking rules for building executables:
test_icu.exe: $(D)/test_icu.o
gtest_UnicodeString.exe: $(ROOT)/gtest/gtest_main.a \
	$(D)/gtest_UnicodeString.o
