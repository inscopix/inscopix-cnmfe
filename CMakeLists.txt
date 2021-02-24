cmake_minimum_required(VERSION 3.5)
project(cnmfe)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD 11)

set(LIB_DIR lib)

file(GLOB SRC_FILES "src/*.cpp")
include_directories(include src)

# static library
add_library(${CMAKE_PROJECT_NAME} STATIC ${SRC_FILES})

# header-only libs
set(HEADER_ONLY_LIB_SEARCH_PATHS)
get_filename_component(t ${LIB_DIR}/ThreadPool ABSOLUTE)
list(APPEND HEADER_ONLY_LIB_SEARCH_PATHS ${t})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${HEADER_ONLY_LIB_SEARCH_PATHS})

# unit tests
add_subdirectory(test)
