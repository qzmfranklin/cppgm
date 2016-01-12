# This is a hand crafted file to integrate the Google Test Framework into this
# Makefile system. This is significantly different from the rules.mk generated
# by the zmake script. Please do not change this file unless you really know
# what you are doing.
#
# gtest source files ($(DIR_GTEST)/src/*.cc) are built in-place. gtest samples
# and its static libraries are left $(DIR_GTEST) to be easy to use by other
# modules.
#
# Do not use $(DIR_GTEST) anywhere outside of this file

TMP:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))
$(TMP)DIR:=$(TMP)

$($(TMP)DIR)CC:=$(wildcard $(TMP)/src/*.cc) $(wildcard $(TMP)/samples/*.cc)

$($(TMP)DIR)CFLAGS:=$(CFLAGS)
$($(TMP)DIR)CXXFLAGS:=$(CXXFLAGS)
$($(TMP)DIR)INCS:=$(INCS)
$($(TMP)DIR)LIBS:=$(LIBS)

DEP:=$(DEP) $($(TMP)CC:%.cc=%.d)
#OBJ:=$(OBJ) $($(TMP)CC:%.cc=%.d)
ASM:=$(ASM) $($(TMP)CC:%.cc=%.s)

$($(TMP)DIR)/%.o: $($(TMP)DIR)/%.cc
	$(QUIET)echo "make $(GREEN)$@ $(NONE)"
	$(QUIET)$(CXX) -o $@ -c $< $(DEPFLAGS) ${$($(TMP)DIR)CXXFLAGS} ${$($(TMP)DIR)INCS}
$($(TMP)DIR)/%.s: $($(TMP)DIR)/%.cc
	$(QUIET)echo "make $(CYAN)$@ $(NONE)"
	$(QUIET)$(CXX) -o $@ $< $(ASMFLAGS) ${$($(TMP)DIR)CXXFLAGS} ${$($(TMP)DIR)INCS}

%.exe: $($(TMP)DIR)/samples/%.o
	$(QUIET)echo "make $(MAGENTA)$@ $(NONE)"
	$(QUIET)$(CXX) -o $@ $^ ${$($(TMP)DIR)LIBS}

$($(TMP)DIR)/gtest.a : $($(TMP)DIR)/src/gtest-all.o
	$(QUIET)echo "make $(MAGENTA)$@ $(NONE)"
	$(QUIET)$(AR) $(ARFLAGS) $@ $^

$($(TMP)DIR)/gtest_main.a : $($(TMP)DIR)/src/gtest-all.o $($(TMP)DIR)/src/gtest_main.o
	$(QUIET)echo "make $(MAGENTA)$@ $(NONE)"
	$(QUIET)$(AR) $(ARFLAGS) $@ $^
