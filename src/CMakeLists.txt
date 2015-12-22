cmake_minimum_required (VERSION 2.6)
project (SC2BuildOptimize)

# set compiler flags
set(CMAKE_CXX_FLAGS "--std=c++11 -g -Wall -pedantic")

# The version number
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/Config.h.in"
  "${PROJECT_BINARY_DIR}/Config.h"
)

# add the binary tree to the search path for include files
# so that we will find Config.h
include_directories("${PROJECT_BINARY_DIR}")

file(GLOB source_files
    "*.h"
    "*.cpp"
)

# add the executable
add_executable(SC2BuildOptimize ${source_files})
