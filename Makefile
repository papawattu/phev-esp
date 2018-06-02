PROJECT_NAME := phev-esp
CFLAGS=-std=c99
BUILD_NUMBER := 1234
SHELL := /bin/bash
BUILD_DIR ?= ./build
COMP_DIR := ./components/**
SRC_DIR := $(COMP_DIR)/src
TEST_DIR ?= $(COMP_DIR)/test
CJSON_DIR ?= ${CJSON_DIR}
INC_DIRS := $(shell find $(COMP_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS ?= $(INC_FLAGS) -std=c99 -MMD -MP -D_TEST_ -DBUILD_NUMBER=$(BUILD_NUMBER)
TEST_BUILD_DIR ?= $(BUILD_DIR)/test
TEST_MAKEFILE = $(TEST_BUILD_DIR)/MakefileTestSupport
#INCLUDE_PATH += -I$(SRC_DIR)/include 
INCLUDE_PATH += -I$(INC_FLAGS) -std=c99
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