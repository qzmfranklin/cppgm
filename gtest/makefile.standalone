# Standalone Makefile that works just out-of-box
# Not used or processed bythe zmake script

GTEST_DIR:=.#no trailing spaces!

CXXFLAGS:=-isystem $(GTEST_DIR)/include -MMD -MP
LDFLAGS :=-lpthread

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

all : $(TESTS)

test: $(TESTS)
	for t in $^ ; do ./"$$t" ; done

clean :
	rm -f $(TESTS) gtest.a gtest_main.a *.o *.d

# build sample unit tests
%.o : $(GTEST_DIR)/samples/%.cc
	$(CXX) -c -o $@ $< $(CXXFLAGS)

%.exe: %.o
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

sample1_unittest.exe: sample1.o sample1_unittest.o gtest_main.a
sample2_unittest.exe: sample2.o sample2_unittest.o gtest_main.a
sample3_unittest.exe: sample3_unittest.o gtest_main.a
sample4_unittest.exe: sample4.o sample4_unittest.o gtest_main.a
sample5_unittest.exe: sample1.o sample5_unittest.o gtest_main.a
sample6_unittest.exe: sample6_unittest.o gtest_main.a
sample7_unittest.exe: sample7_unittest.o gtest_main.a
sample8_unittest.exe: sample8_unittest.o gtest_main.a
sample9_unittest.exe: sample9_unittest.o gtest_main.a
sample10_unittest.exe: sample10_unittest.o gtest_main.a

# build gtest.a and gtest_main.a
%.o : $(GTEST_DIR)/src/%.cc
	$(CXX) -c -o $@ $< $(CXXFLAGS) -I$(GTEST_DIR)

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# include dependency files
-include $($(wildcard $(GTEST_DIR)/src/*.cc):$(GTEST_DIR)/src/%.cc=%.d) \
	$($(wildcard $(GTEST_DIR)/samples/*.cc):$(GTEST_DIR)/samples/%.cc=%.d)
