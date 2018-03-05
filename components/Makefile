#SHELL := /bin/bash

CLEANUP = rm -Rf

#UNITY_ROOT = ../Unity
#CMOCK_DIR = ../cmock

SRC_DIR := ./components/**/src
INCLUDE_DIR := ./components/api/include
#CJSON_DIR := C:\msys32\home\Jamie\esp-idf\components\json\cJSON 
BUILD_DIR ?= ./build
TEST_BUILD_DIR = $(BUILD_DIR)/test
TEST_MAKEFILE = $(TEST_BUILD_DIR)/MakefileTestSupport

export

all: setup test
clean:
	$(CLEANUP) $(TEST_BUILD_DIR)
setup:
	@mkdir -p $(BUILD_DIR)
	@echo Building Makefile
	ruby ./scripts/create_makefile.rb

-include $(TEST_MAKEFILE)