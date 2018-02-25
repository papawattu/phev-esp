#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
PROJECT_NAME := outlander-connect

local: setup test
	
-include $(IDF_PATH)/make/project.mk

-include components/Makefile
