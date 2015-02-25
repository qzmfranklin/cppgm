ROOT    :=/home/zhongming/git/cppgm
d       :=${ROOT}/pa1# this directory
-include ${ROOT}/root.mk
-include ${ROOT}/pa1/rules.mk
-include ${DEP}

.PHONY: all asm clean test
all: ${OBJ}
asm: ${ASM}
clean:
	rm -f *.exe ${OBJ} ${DEP} ${ASM}
test: pptoken.exe
	scripts/run_all_tests.pl $< my
	scripts/compare_results.pl ref my

pptoken.exe: ${d}/pptoken.o ${d}/pp_tokenizer.o
