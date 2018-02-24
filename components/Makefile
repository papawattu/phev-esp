
CLEANUP = rm -Rf
MKDIR = mkdir -p
TARGET_EXTENSION=.exe
C_COMPILER=gcc
CC=$(C_COMPILER)
UNITY_ROOT = ../../Unity
CMOCK_DIR = ../../cmock

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

TARGET_BASE1=test1
TARGET_BASE2=test2

TARGET1 = $(TARGET_BASE1)$(TARGET_EXTENSION)
TARGET2 = $(TARGET_BASE2)$(TARGET_EXTENSION)

#CAR_CONNECT = ./car_connect
MESSAGING = ./messaging

INC_DIRS=-I$(MESSAGING)/include -I$(MESSAGING)/test -I$(UNITY_ROOT)/src
SYMBOLS=

BUILD_DIR ?= ./build
SRC_DIR ?= $(MESSAGING)/src
TEST_DIR ?= $(MESSAGING)/test
TEST_BUILD_DIR = $(BUILD_DIR)/test
TEST_MAKEFILE = $(TEST_BUILD_DIR)/MakefileTestSupport
OBJ = $(BUILD_DIR)/obj
OBJ_DIR = $(OBJ)
SRC_FILES1 = $(UNITY_ROOT)/src/unity.c $(TEST_DIR)/test_messaging.c $(SRC_DIR)/messaging.c $(TEST_DIR)/test_message_runner.c
SRC_FILES2 = $(UNITY_ROOT)/src/unity.c $(TEST_DIR)/test_socket_client.c $(SRC_DIR)/socket_client.c $(SRC_DIR)/messaging.c $(TEST_DIR)/test_message_client_runner.c


all: setup test

clean:
	$(CLEANUP) $(TARGET1) $(TARGET2) $(BUILD_DIR) $(OBJ)
setup:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ)
	ruby $(CMOCK_DIR)/scripts/create_makefile.rb

test: setup

-include $(TEST_MAKEFILE)