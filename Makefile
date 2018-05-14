# PROJECT_NAME := phev-esp
# BUILD_DIR ?= ./build
# SRC_DIR ?= ./**/**/src

# SRCS := $(shell find $(SRC_DIR) -name *.c)
# TEST_DIR ?= ./**/**/test
# OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
# DEPS := $(OBJS:.o=.d)

# INC_DIRS := $(shell find $(SRC_DIR) -type d)
# LIB_DIRS := $(shell find $(IDF_PATH)/components -type d)
# INC_FLAGS := $(addprefix -I,$(INC_DIRS))
# #INC_FLAGS += $(addprefix -I,$(LIB_DIRS))
# CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
# TEST_BUILD_DIR ?= $(BUILD_DIR)/test
# TEST_MAKEFILE = $(TEST_BUILD_DIR)/MakefileTestSupport
# INCLUDE_PATH = $(INC_FLAGS) 
# CMOCK_DIR := ${CMOCK_DIR}
# RM := rm

# ifeq ($(MAKECMDGOALS),test)
# export

# setup:
# 	$(MKDIR_P) $(dir $@)
# 	ruby $(CMOCK_DIR)/scripts/create_makefile.rb

# test: setup

# else 
# include $(IDF_PATH)/make/project.mk
# endif 
# ifeq ($(MAKECMDGOALS),setup)
# export

# setup:
# 	$(MKDIR_P) $(dir $@)
# 	ruby $(CMOCK_DIR)/scripts/create_makefile.rb

# test: setup

# else 
include $(IDF_PATH)/make/project.mk
# endif 

# .PHONY: clean

# -include $(TEST_MAKEFILE)

# clean:
# 	$(RM) -r $(BUILD_DIR)

# #-include $(DEPS)

# #MKDIR_P ?= mkdir -p
