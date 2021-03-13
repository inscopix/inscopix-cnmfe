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
get_filename_component(HDF5_HEADER_PATHS ${LIB_DIR}/hdf5/include ABSOLUTE)
get_filename_component(HDF5_LIB_DIR_PATH ${LIB_DIR}/hdf5/lib ABSOLUTE)
set(HDF5_LIBRARIES
        ${HDF5_LIB_DIR_PATH}/libhdf5_cpp-static.a
        ${HDF5_LIB_DIR_PATH}/libhdf5-static.a)
# armadillo
set(ARMADILLO_DEFINITIONS ARMA_DONT_USE_WRAPPER ARMA_64BIT_WORD ARMA_BLAS_LONG_LONG ARMA_USE_HDF5 ARMA_NO_DEBUG)
get_filename_component(ARMADILLO_HEADER_PATHS ${LIB_DIR}/armadillo ABSOLUTE)
get_filename_component(INTELMKL_LIB_DIR ${LIB_DIR}/Intel_MKL/lib ABSOLUTE)
set(ARMADILLO_LIBRARIES
        ${INTELMKL_LIB_DIR}/libmkl_intel_ilp64.a
        ${INTELMKL_LIB_DIR}/libmkl_tbb_thread.a
        ${INTELMKL_LIB_DIR}/libmkl_core.a
        ${INTELMKL_LIB_DIR}/libtbb.a)
# openCV
list(APPEND CMAKE_PREFIX_PATH ${LIB_DIR}/OpenCV/share/OpenCV)
find_package(OpenCV 3.2.0 REQUIRED core imgproc video features2d)
set(OPENCV_HEADER_PATHS ${OpenCV_INCLUDE_DIRS})
set(OPENCV_LIBRARIES ${OpenCV_LIBS})

# add headers and libs to project target
list(APPEND LIB_INCLUDE_PATHS
        ${HDF5_HEADER_PATHS}
        ${ARMADILLO_HEADER_PATHS}
        ${OPENCV_HEADER_PATHS})
list(APPEND LIB_FILE_PATHS
        ${HDF5_LIBRARIES}
        ${ARMADILLO_LIBRARIES}
        ${OPENCV_LIBRARIES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${HEADER_ONLY_LIB_PATHS} ${LIB_INCLUDE_PATHS})
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${LIB_FILE_PATHS})
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC ${ARMADILLO_DEFINITIONS})
