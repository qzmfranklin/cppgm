ROOT    :=$(realpath ..)
D       :=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))# this directory
-include $(ROOT)/root.mk
-include $(ROOT)/gtest/rules.mk
-include $(D)/rules.mk
# Inlcude more rules.mk here if you this directory depends on them.
-include $(ROOT)/utils/rules.mk
-include $(DEP)

# List all the executables you want to run when you type `make test` in $(TESTS)
# Note that you need to -include $(ROOT)/gtest/rules.mk to actually build those
# executables
TESTS:=gtest_PPToken.exe gtest_PPCodePointCheck.exe gtest_PPCodeUnit.exe \
	gtest_PPUTF32Stream.exe gtest_PPCodeUnitStream.exe gtest_PPTokenizerDFA.exe

.PHONY: all asm clean test
all: $(OBJ)
asm: $(ASM)
clean:
	$(QUIET)rm -f *.exe $(OBJ) $(DEP) $(ASM)
test: $(TESTS)
	$(QUIET)for t in $^ ; do ./"$$t" || exit 1 ; done

# Sample linking rules for building executables:
gtest_PPToken.exe: $(ROOT)/gtest/gtest_main.a $(D)/gtest_PPToken.o $(D)/PPToken.o

gtest_PPCodePointCheck.exe: $(ROOT)/gtest/gtest_main.a $(D)/gtest_PPCodePointCheck.o $(D)/PPCodePointCheck.o

gtest_PPCodeUnit.exe: $(ROOT)/gtest/gtest_main.a $(ROOT)/utils/UStringTools.o \
	$(D)/gtest_PPCodeUnit.o $(D)/PPCodeUnit.o $(D)/PPCodePointCheck.o

gtest_PPUTF32Stream.exe: $(ROOT)/gtest/gtest_main.a $(ROOT)/utils/UStringTools.o \
	$(D)/gtest_PPUTF32Stream.o $(D)/PPUTF32Stream.o

gtest_PPCodeUnitStream.exe: $(ROOT)/gtest/gtest_main.a $(ROOT)/utils/UStringTools.o \
	$(D)/gtest_PPCodeUnitStream.o $(D)/PPCodeUnit.o $(D)/PPCodeUnitStream.o \
	$(D)/PPCodePointCheck.o $(D)/PPUTF32Stream.o $(D)/PPCodePointCheck.o

gtest_PPTokenizerDFA.exe: $(ROOT)/gtest/gtest_main.a $(ROOT)/utils/UStringTools.o \
	$(D)/gtest_PPTokenizerDFA.o $(D)/PPCodeUnit.o $(D)/PPCodeUnitStream.o \
	$(D)/PPCodePointCheck.o $(D)/PPUTF32Stream.o \
	$(D)/PPTokenizerDFA.o $(D)/PPToken.o $(D)/PPCodeUnitCheck.o

pptok.exe: $(D)/pptok.o $(ROOT)/utils/os/path.o $(ROOT)/utils/UStringTools.o \
	$(D)/PPCodeUnit.o $(D)/PPCodeUnitStream.o \
	$(D)/PPCodePointCheck.o $(D)/PPUTF32Stream.o \
	$(D)/PPTokenizerDFA.o $(D)/PPToken.o $(D)/PPCodeUnitCheck.o
