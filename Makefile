PROJECT_NAME := phev-esp
BUILD_DIR ?= ./build
SRC_DIR ?= ./**/**/src

SRCS := $(shell find $(SRC_DIR) -name *.c)
TEST_DIR ?= ./**/**/test
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
TEST_BUILD_DIR ?= $(BUILD_DIR)/test
TEST_MAKEFILE = $(TEST_BUILD_DIR)/MakefileTestSupport
INCLUDE_PATH = $(INC_FLAGS)
ifneq ($(MAKECMDGOALS),flash)
export
endif 

setup:
	$(MKDIR_P) $(dir $@)
	ruby $(CMOCK_DIR)/scripts/create_makefile.rb

.PHONY: clean

test: setup

-include $(TEST_MAKEFILE)

clean:
	$(RM) -r $(BUILD_DIR)

-include $(DEPS)

MKDIR_P ?= mkdir -p
ifeq ($(MAKECMDGOALS),flash)
include $(IDF_PATH)/make/project.mk
endif