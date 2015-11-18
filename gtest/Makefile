ROOT    :=$(realpath ..)
d       :=$(ROOT)/gtest#this directory
-include $(ROOT)/root.mk
-include $(ROOT)/gtest/rules.mk
-include $(DEP)

TESTS :=sample1_unittest.exe \
	sample2_unittest.exe \
	sample3_unittest.exe \
	sample4_unittest.exe \
	sample5_unittest.exe \
	sample6_unittest.exe \
	sample7_unittest.exe \
	sample8_unittest.exe \
	sample9_unittest.exe \
	sample10_unittest.exe

.PHONY: all asm clean test
all: $(OBJ)
asm: $(ASM)
clean:
	$(QUIET)rm -f $(OBJ) $(DEP) $(ASM) $(TESTS:%.exe=$(d)/%.exe)
	$(QUIET)rm -f $(d)/samples/*.o $(d)/src/gtest-all.o \
		$(d)/src/gtest_main.o $(d)/gtest_main.a $(d)/gtest.a

test: $(TESTS)
	$(QUIET)for t in $^ ; do ./"$$t" ; done

sample1_unittest.exe:  $(d)/gtest_main.a $(d)/samples/sample1.o $(d)/samples/sample1_unittest.o
sample2_unittest.exe:  $(d)/gtest_main.a $(d)/samples/sample2.o $(d)/samples/sample2_unittest.o
sample3_unittest.exe:  $(d)/gtest_main.a $(d)/samples/sample3_unittest.o
sample4_unittest.exe:  $(d)/gtest_main.a $(d)/samples/sample4.o $(d)/samples/sample4_unittest.o
sample5_unittest.exe:  $(d)/gtest_main.a $(d)/samples/sample1.o $(d)/samples/sample5_unittest.o
sample6_unittest.exe:  $(d)/gtest_main.a $(d)/samples/sample6_unittest.o
sample7_unittest.exe:  $(d)/gtest_main.a $(d)/samples/sample7_unittest.o
sample8_unittest.exe:  $(d)/gtest_main.a $(d)/samples/sample8_unittest.o
sample9_unittest.exe:  $(d)/gtest_main.a $(d)/samples/sample9_unittest.o
sample10_unittest.exe: $(d)/gtest_main.a $(d)/samples/sample10_unittest.o
