cmake_minimum_required(VERSION 3.5)
project(runner)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD 11)

file(GLOB SRC_FILES "main.cpp")
add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "../include")

get_filename_component(CNMFE_LIB ${CMAKE_CURRENT_SOURCE_DIR}/../build/libcnmfe.a ABSOLUTE)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE ${CNMFE_LIB})
