cmake_minimum_required(VERSION 3.5)
project(cnmfe)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD 11)

file(GLOB SRC_FILES "src/*.cpp")
include_directories(include src)

# static library
add_library(${CMAKE_PROJECT_NAME} STATIC ${SRC_FILES})

# unit tests
add_subdirectory(test)