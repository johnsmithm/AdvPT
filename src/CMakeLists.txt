cmake_minimum_required (VERSION 2.6)
project (SC2BuildOptimize)

# set compiler flags
set(CMAKE_CXX_FLAGS "--std=c++11 -O0 -g -Wall -pedantic")

# The version number
set (VERSION_MAJOR 0)
set (VERSION_MINOR 1)
set (BINARY_NAME SC2BuildOptimize)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/Config.h.in"
  "${PROJECT_BINARY_DIR}/Config.h"
)

# add the binary tree to the search path for include files
# so that we will find Config.h
include_directories("${PROJECT_BINARY_DIR}")

file(GLOB SOURCE_FILES
    "*.cpp"
    "json/*.cpp"
)

#resolve dependencies
find_package( Boost REQUIRED COMPONENTS program_options )

include_directories(${Boost_INCLUDE_DIRS})

add_executable(${BINARY_NAME} ${SOURCE_FILES})
target_link_libraries(${BINARY_NAME} ${Boost_LIBRARIES})

