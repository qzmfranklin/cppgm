#  THIS DIRECTORY
TMP:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))
$(TMP)DIR:=$(TMP)

#  ALL C/C++ FILES IN THIS DIRECTORY (WITHOUT PATHNAME)
$($(TMP)DIR)C  :=$(wildcard $(TMP)/*.c)
$($(TMP)DIR)CC :=$(wildcard $(TMP)/*.cc)
$($(TMP)DIR)CPP:=$(wildcard $(TMP)/*.cpp)
#  DIRECTORY-SPECIFIC COMPILING FLAGS AND INCLUDE DIRECTORIES
$($(TMP)DIR)CFLAGS:=$(CFLAGS)
$($(TMP)DIR)CXXFLAGS:=$(CXXFLAGS)
$($(TMP)DIR)INCS:=$(INCS)
$($(TMP)DIR)LIBS:=$(LIBS)

DEP:=$(DEP) $($(TMP)C:%.c=%.d) $($(TMP)CC:%.cc=%.d) $($(TMP)CPP:%.cpp=%.d)
OBJ:=$(OBJ) $($(TMP)C:%.c=%.o) $($(TMP)CC:%.cc=%.o) $($(TMP)CPP:%.cpp=%.o)
ASM:=$(ASM) $($(TMP)C:%.c=%.s) $($(TMP)CC:%.cc=%.s) $($(TMP)CPP:%.cpp=%.s)

$($(TMP)DIR)/%.o: $($(TMP)DIR)/%.c
	$(QUIET)$(CC) -o $@ -c $< $(DEPFLAGS) $($($(TMP)DIR)CFLAGS) $($($(TMP)DIR)INCS)
	$(QUIET)echo "Compiling $(GREEN)$(notdir $<) $(NONE)..."
$($(TMP)DIR)/%.s: $($(TMP)DIR)/%.c
	$(QUIET)$(CC) -o $@ $< $(ASMFLAGS) $($($(TMP)DIR)CFLAGS) $($($(TMP)DIR)INCS)
	$(QUIET)echo "Assembly listing $(CYAN)$(notdir $<) $(NONE)..."

$($(TMP)DIR)/%.o: $($(TMP)DIR)/%.cc
	$(QUIET)$(CXX) -o $@ -c $< $(DEPFLAGS) ${$($(TMP)DIR)CXXFLAGS} ${$($(TMP)DIR)INCS}
	$(QUIET)echo "Compiling $(GREEN)$(notdir $<) $(NONE)..."
$($(TMP)DIR)/%.s: $($(TMP)DIR)/%.cc
	$(QUIET)$(CXX) -o $@ $< $(ASMFLAGS) ${$($(TMP)DIR)CXXFLAGS} ${$($(TMP)DIR)INCS}
	$(QUIET)echo "Assembly listing $(CYAN)$(notdir $<) $(NONE)..."

$($(TMP)DIR)/%.o: $($(TMP)DIR)/%.cpp
	$(QUIET)$(CXX) -o $@ -c $< $(DEPFLAGS) $($($(TMP)DIR)CXXFLAGS) $($($(TMP)DIR)INCS)
	$(QUIET)echo "Compiling $(GREEN)$(notdir $<) $(NONE)..."
$($(TMP)DIR)/%.s: $($(TMP)DIR)/%.cpp
	$(QUIET)$(CXX) -o $@ $< $(ASMFLAGS) $($($(TMP)DIR)CXXFLAGS) $($($(TMP)DIR)INCS)
	$(QUIET)echo "Assembly listing $(CYAN)$(notdir $<) $(NONE)..."

# Linking pattern rule for this directory
%.exe: $($(TMP)DIR)/%.o
	$(QUIET)$(CXX) -o $@ $^ $($($(TMP)DIR)LIBS)
	$(QUIET)echo "Linking $(MAGENTA)$(notdir $@) $(NONE)..."

# Recursive inclusion
-include $(wildcard $(TMP)/*/$(notdir $(lastword $(MAKEFILE_LIST))))
