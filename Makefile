EXEC := app
PROJECT_NAME := phev-esp
BUILD_NUMBER ?= 1
SHELL := /bin/bash
BUILD_DIR ?= ./build
COMP_DIR := ./components/**
SRC_DIR := $(COMP_DIR)/src
TEST_DIR ?= $(COMP_DIR)/test
CJSON_DIR ?= /usr/local/include/cjson/
INC_DIRS := $(shell find $(COMP_DIR) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))
CPPFLAGS ?= $(INC_FLAGS) -DBUILD_NUMBER=$(BUILD_NUMBER) -MMD -Wall -Wextra -g
CFLAGS += -g
TEST_BUILD_DIR ?= $(BUILD_DIR)/test
TEST_MAKEFILE = $(TEST_BUILD_DIR)/MakefileTestSupport
#INCLUDE_PATH += -I$(SRC_DIR)/include 
INCLUDE_PATH += -lcjson -I$(INC_FLAGS) -I$(CJSON_DIR) -std=c99 
CMOCK_DIR ?= ../cmock
RM := rm
TEST_CFLAGS += -lcjson
LDFLAGS +=-lcjson
LD_LIBRARY_PATH +=/usr/local/lib
LD_RUN_PATH +=/usr/local/lib

export

ifdef IDF_PATH
undefine LDFLAGS
unexport
include $(IDF_PATH)/make/project.mk
endif

MKDIR_P ?= mkdir -p

setup:
	$(MKDIR_P) $(dir $@)
	ruby $(CMOCK_DIR)/scripts/create_makefile.rb

test: setup
.PHONY: clean

.PHONY: test_clean
test_clean:
	$(RM) -r $(BUILD_DIR)/test

CSRC = $(wildcard main/src/*.c) \
       $(wildcard components/msg_core/src/*.c) \
       $(wildcard components/phev_core/src/*.c) \
	   $(wildcard components/ota/src/*.c)
OBJ = $(CSRC:.c=.o)
DEP = $(OBJ:.o=.d) 

linux: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

.PHONY: clean
clean:
	rm -f $(OBJ) app

.PHONY: cleandep
cleandep:
	rm -f $(DEP)

-include $(TEST_MAKEFILE)