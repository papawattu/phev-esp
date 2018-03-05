#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
PROJECT_NAME := outlander-connect

local: setup test

ifeq ($(MAKECMDGOALS),setup)
include components/Makefile
endif
ifeq ($(MAKECMDGOALS),local)
include components/Makefile
endif

-include $(IDF_PATH)/make/project.mk
