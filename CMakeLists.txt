cmake_minimum_required(VERSION 3.5)
project(cnmfe)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD 11)

set(LIB_DIR lib)

file(GLOB SRC_FILES "src/*.cpp")
include_directories(include src)

# cnmfe static library
add_library(${CMAKE_PROJECT_NAME} STATIC ${SRC_FILES})

## cnmfe unit tests
add_subdirectory(test)

# header-only libs
set(HEADER_ONLY_LIB_PATHS)
get_filename_component(THREADPOOL_HEADER_PATHS ${LIB_DIR}/ThreadPool ABSOLUTE)
get_filename_component(MIO_HEADER_PATHS ${LIB_DIR}/mio ABSOLUTE)
list(APPEND HEADER_ONLY_LIB_PATHS ${THREADPOOL_HEADER_PATHS} ${MIO_HEADER_PATHS})

# static libs
set(LIB_INCLUDE_PATHS)
set(LIB_FILE_PATHS)
# hdf5
set(HDF5_LIBRARIES)
get_filename_component(HDF5_LIB_DIR_PATH ${LIB_DIR}/hdf5/lib ABSOLUTE)
list(APPEND HDF5_LIBRARIES "${HDF5_LIB_DIR_PATH}/libhdf5_cpp-static.a" "${HDF5_LIB_DIR_PATH}/libhdf5-static.a")
get_filename_component(HDF5_HEADER_PATHS ${LIB_DIR}/hdf5/include ABSOLUTE)

# add headers and libs to project target
list(APPEND LIB_INCLUDE_PATHS ${HDF5_HEADER_PATHS})
list(APPEND LIB_FILE_PATHS ${HDF5_LIBRARIES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${HEADER_ONLY_LIB_PATHS} ${LIB_INCLUDE_PATHS})
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${LIB_FILE_PATHS})
