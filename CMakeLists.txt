# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
if(DEFINED ENV{BUILD_NUMBER})
    add_definitions(-DBUILD_NUMBER=${BUILD_NUMBER})
else()
    message("Build number not defined defaulting to 1 and turning off OTA")
    add_definitions(-DBUILD_NUMBER=1 -DNO_OTA)
endif()
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(phev-esp)