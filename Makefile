PROJECT_NAME := phev-esp
SHELL := /bin/bash
BUILD_DIR ?= ./build
COMP_DIR := ./components/msg_core
SRC_DIR := $(COMP_DIR)/src
TEST_DIR ?= $(COMP_DIR)/test

INC_DIRS := $(shell find $(COMP_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
TEST_BUILD_DIR ?= $(BUILD_DIR)/test
TEST_MAKEFILE = $(TEST_BUILD_DIR)/MakefileTestSupport
INCLUDE_PATH += -I$(SRC_DIR)/include 
CMOCK_DIR := ${CMOCK_DIR}
RM := rm

export

ifdef IDF_PATH
unexport
include $(IDF_PATH)/make/project.mk
endif

MKDIR_P ?= mkdir -p

setup:
	$(MKDIR_P) $(dir $@)
	ruby $(CMOCK_DIR)/scripts/create_makefile.rb

test: setup
.PHONY: clean

test_clean:
	$(RM) -r $(BUILD_DIR)/test

-include $(TEST_MAKEFILE)