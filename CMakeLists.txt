cmake_minimum_required(VERSION 3.5)
project(cnmfe)

set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_STANDARD 11)

# identify operating system
set(OS_MAC FALSE)
set(OS_LINUX FALSE)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(OS_MAC TRUE)
elseif(UNIX AND NOT APPLE)
    set(OS_LINUX TRUE)
else()
    message(FATAL_ERROR "cmake is not configured for this OS")
endif()

set(SRC_DIR
        include
        src
        src/exposed
        src/core
        src/oasis
        src/io
        src/utilities
        src/log)
file(GLOB SRC_FILES
        "src/*.cpp"
        "src/exposed/*.cpp"
        "src/core/*.cpp"
        "src/oasis/*.cpp"
        "src/io/*.cpp"
        "src/utilities/*.cpp"
        "src/log/*.cpp")

# cnmfe static library
add_library(${CMAKE_PROJECT_NAME} STATIC ${SRC_FILES})

# cnmfe unit tests
add_subdirectory(test)

# create executable
set(EXECUTABLE_NAME runCnmfe)
add_executable(${EXECUTABLE_NAME} example.cpp)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${CMAKE_PROJECT_NAME})

# header-only dependencies
set(LIB_DIR lib)
set(HEADER_ONLY_LIB_PATHS)
get_filename_component(THREADPOOL_HEADER_PATHS ${LIB_DIR}/ThreadPool ABSOLUTE)
get_filename_component(JSON_HEADER_PATHS ${LIB_DIR}/json ABSOLUTE)
list(APPEND HEADER_ONLY_LIB_PATHS ${THREADPOOL_HEADER_PATHS} ${JSON_HEADER_PATHS})

# static lib dependencies
set(LIB_INCLUDE_PATHS)
set(LIB_FILE_PATHS)
# hdf5
get_filename_component(HDF5_HEADER_PATHS ${LIB_DIR}/hdf5/include ABSOLUTE)
get_filename_component(HDF5_LIB_DIR_PATH ${LIB_DIR}/hdf5/lib ABSOLUTE)
if (${OS_MAC})
    set(HDF5_LIBRARIES
            ${HDF5_LIB_DIR_PATH}/libhdf5_cpp-static.a
            ${HDF5_LIB_DIR_PATH}/libhdf5-static.a)
elseif (${OS_LINUX})
    find_package(HDF5 COMPONENTS C CXX HL REQUIRED)
    link_directories(${HDF5_LIBRARY_DIRS})
    include_directories(${HDF5_INCLUDE_DIRS})
endif()
# armadillo
set(ARMADILLO_DEFINITIONS ARMA_DONT_USE_WRAPPER ARMA_64BIT_WORD ARMA_BLAS_LONG_LONG ARMA_USE_HDF5 ARMA_NO_DEBUG)
get_filename_component(ARMADILLO_HEADER_PATHS ${LIB_DIR}/armadillo ABSOLUTE)
get_filename_component(INTELMKL_LIB_DIR ${LIB_DIR}/Intel_MKL/lib ABSOLUTE)
if (${OS_MAC})
    set(ARMADILLO_LIBRARIES
            ${INTELMKL_LIB_DIR}/libmkl_intel_ilp64.a
            ${INTELMKL_LIB_DIR}/libmkl_tbb_thread.a
            ${INTELMKL_LIB_DIR}/libmkl_core.a
            ${INTELMKL_LIB_DIR}/libtbb.a)
elseif (${OS_LINUX})
    set(ARMADILLO_LIBRARIES
            -Wl,--start-group
            ${INTELMKL_LIB_DIR}/libmkl_intel_ilp64.a
            ${INTELMKL_LIB_DIR}/libmkl_tbb_thread.a
            ${INTELMKL_LIB_DIR}/libmkl_core.a
            -Wl,--end-group
            ${INTELMKL_LIB_DIR}/libtbb.a
            libpthread.so)
endif()
# opencv
list(APPEND CMAKE_PREFIX_PATH ${LIB_DIR}/OpenCV/share/OpenCV)
find_package(OpenCV 3.2.0 REQUIRED core imgproc video features2d)
set(OPENCV_HEADER_PATHS ${OpenCV_INCLUDE_DIRS})
set(OPENCV_LIBRARIES ${OpenCV_LIBS})
# libtiff
get_filename_component(LIBTIFF_HEADER_PATHS ${LIB_DIR}/libtiff/include ABSOLUTE)
get_filename_component(LIBTIFF_LIB_DIR_PATH ${LIB_DIR}/libtiff/lib ABSOLUTE)
set(LIBTIFF_LIBRARIES ${LIBTIFF_LIB_DIR_PATH}/libtiff.a)
set(QT_VERSION "5.8")
find_package(Qt5Core ${QT_VERSION} REQUIRED PATHS ${LIB_DIR}/Qt/lib/cmake/Qt5Core NO_DEFAULT_PATH)
set(QT_CORE_HEADER_PATHS ${Qt5Core_INCLUDE_DIRS})
set(QT_CORE_LIBRARIES ${Qt5Core_LIBRARIES})

# add headers and libs to project target
list(APPEND LIB_INCLUDE_PATHS
        ${ARMADILLO_HEADER_PATHS}
        ${OPENCV_HEADER_PATHS}
        ${LIBTIFF_HEADER_PATHS}
        ${QT_CORE_HEADER_PATHS})
list(APPEND LIB_FILE_PATHS
        ${ARMADILLO_LIBRARIES}
        ${OPENCV_LIBRARIES}
        ${LIBTIFF_LIBRARIES}
        ${QT_CORE_LIBRARIES})
if (${OS_MAC})
    list(APPEND LIB_INCLUDE_PATHS ${HDF5_HEADER_PATHS})
    list(APPEND LIB_FILE_PATHS ${HDF5_LIBRARIES})
elseif (${OS_LINUX})
    list(APPEND LIB_INCLUDE_PATHS ${HDF5_INCLUDE_DIRS})
    list(APPEND LIB_FILE_PATHS ${HDF5_CXX_LIBRARIES})
endif()
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${HEADER_ONLY_LIB_PATHS} ${LIB_INCLUDE_PATHS} ${SRC_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${LIB_FILE_PATHS})
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC ${ARMADILLO_DEFINITIONS})
