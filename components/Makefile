SHELL := /bin/bash
SHELL := /bin/bash
CLEANUP = rm -Rf
MKDIR = mkdir -p
TARGET_EXTENSION=.exe
C_COMPILER=gcc
CC=$(C_COMPILER)

UNITY_ROOT = ../Unity
CMOCK_DIR = ../cmock

CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -Wpointer-arith
CFLAGS += -Wcast-align
CFLAGS += -Wwrite-strings
CFLAGS += -Wswitch-default
CFLAGS += -Wunreachable-code
CFLAGS += -Winit-self
CFLAGS += -Wmissing-field-initializers
CFLAGS += -Wno-unknown-pragmas
CFLAGS += -Wstrict-prototypes
CFLAGS += -Wundef
CFLAGS += -Wold-style-definition
CFLAGS += -D_TEST

SRC_DIR := ./components/**/src
INCLUDE_PATH := ./components/api/include
BUILD_DIR ?= ./build
TEST_BUILD_DIR = $(BUILD_DIR)/test
TEST_MAKEFILE = $(TEST_BUILD_DIR)/MakefileTestSupport

clean:
	$(CLEANUP) $(TEST_BUILD_DIR)
setup:
	mkdir -p $(BUILD_DIR)
	INCLUDE_PATH=$(INCLUDE_PATH) ruby $(CMOCK_DIR)/scripts/create_makefile.rb

-include $(TEST_MAKEFILE)