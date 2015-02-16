ROOT    :=/home/zhongming/git/cppgm
d       :=${ROOT}/pa1# this directory
-include ${ROOT}/root.mk
-include ${ROOT}/pa1/rules.mk
-include ${DEP}

# List all the executables you want to run when you type `make test` in ${TESTS}
# Note that you need to -include ${ROOT}/gtest/rules.mk to actually build those
# executables
TESTS:=

.PHONY: all asm clean test
all: ${OBJ}
asm: ${ASM}
clean:
	rm -f *.exe ${OBJ} ${DEP} ${ASM}
test: ${TESTS}
	for t in $^ ; do ./"$$t" ; done

# Sample linking rules for building executables:
#test_heapsort.exe: ${d}/heapsort.o ${d}/test_heapsort.o ${ROOT}/utils/utils.o
#gtest_dag.exe: ${d}/gtest_dag.o ${d}/dag.o ${ROOT}/gtest/gtest_main.a
