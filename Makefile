#----------------------------------------------------------------#
# Universal C/C++/Assembly makefile								 |
# Created by Job Vranish										 |
# (https://spin.atomicobject.com/2016/08/26/makefile-c-projects) |
#																 |
# Adapted by Leon Schumacher (options and comments)				 |
#----------------------------------------------------------------#


# For TARGET_EXEC: specify name of final binary
# For COMP: use $(CC) for default C or $(CXX) for default C++ compiler
TARGET_EXEC ?= EBCG2
COMP ?= $(CXX)

# Specify source, include and build directories here
# The build dir will be created to store object and dependency files during the build process.
# You can safely delete it afterwards with make clean
SRC_DIRS ?= ./src
INC_DIRS ?= ./include
BUILD_DIR ?= ./build

# Set flags here (C, C++, Assembly, Linker)
CFLAGS ?=
CXXFLAGS ?= --std=c++17
ASFLAGS ?=
LDFLAGS ?= -lmss --std=c++17


# Everything below this line does not need to be changed normally!
# Only proceed if you know what you are doing!
#-----------------------------------------------------------------


# Find sources and convert them to objects and dependency files
SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c -or -name *.s)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

# Prepare include statements from all include directories. Also create dependency files from objects with -MMD -MD.
INC_LIST := $(addprefix -I,$(INC_DIRS))
INCFLAGS ?= $(INC_LIST) -MMD -MP


# Main target, use user-defined compiler (to select C or C++)
$(TARGET_EXEC): $(OBJS)
	$(COMP) $(OBJS) -o $@ $(LDFLAGS)


# Compile sources with respective compiler
# Assembly
$(BUILD_DIR)/%.s.o: %.s
	$(MKDIR_P) $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# C source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(INCFLAGS) $(CFLAGS) -c $< -o $@

# C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(INCFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
