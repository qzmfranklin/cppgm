ROOT    :=$(realpath ../..)
D       :=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))# this directory
-include $(ROOT)/root.mk
-include $(ROOT)/gtest/rules.mk
-include $(D)/rules.mk
# Inlcude more rules.mk here if you this directory depends on them.
-include $(DEP)

TESTS:=gtest_path.exe

.PHONY: all asm clean test
all: $(OBJ)
asm: $(ASM)
clean:
	$(QUIET)rm -f *.exe $(OBJ) $(DEP) $(ASM)
test: $(TESTS)
	$(QUIET)for t in $^ ; do ./"$$t" ; done

gtest_path.exe: $(ROOT)/gtest/gtest_main.a $(D)/gtest_path.o $(D)/path.o
