cmake_minimum_required(VERSION 3.5)
project(cnmfe)

set(CMAKE_CXX_FLAGS "-Wall -fPIC")
set(CMAKE_CXX_STANDARD 11)

# identify operating system
set(OS_MAC FALSE)
set(OS_LINUX FALSE)
set(OS_WINDOWS FALSE)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(OS_MAC TRUE)
elseif(UNIX AND NOT APPLE)
    set(OS_LINUX TRUE)
    set(CMAKE_C_COMPILER "/usr/bin/gcc-4.8")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++-4.8")
elseif(WIN32)
    set(OS_WINDOWS TRUE)
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

# python bindings
if (${BUILD_PYTHON_API})
    set(PYTHON_MODULE_NAME inscopix_cnmfe)
    add_subdirectory(lib/pybind11)
    pybind11_add_module(${PYTHON_MODULE_NAME} src/api/pythonAPI.cpp)
    target_link_libraries(${PYTHON_MODULE_NAME} PRIVATE ${CMAKE_PROJECT_NAME})
endif()

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
    set(HDF5_LIBRARIES
            ${HDF5_LIB_DIR_PATH}/libhdf5_cpp.a
            ${HDF5_LIB_DIR_PATH}/libhdf5.a)
elseif (${OS_WINDOWS})
    set(HDF5_LIBRARIES
            ${HDF5_LIB_DIR_PATH}/libhdf5_cpp.lib
            ${HDF5_LIB_DIR_PATH}/libhdf5.lib)
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
elseif (${OS_WINDOWS})
    set(ARMADILLO_LIBRARIES
            ${INTELMKL_LIB_DIR}/mkl_intel_ilp64.lib
            ${INTELMKL_LIB_DIR}/mkl_tbb_thread.lib
            ${INTELMKL_LIB_DIR}/mkl_core.lib
            ${INTELMKL_LIB_DIR}/tbb.lib)
endif()
# opencv
if (${OS_WINDOWS})
    set(OpenCV_STATIC ON)
    list(APPEND CMAKE_PREFIX_PATH ${LIB_DIR}/OpenCV)
else()
    list(APPEND CMAKE_PREFIX_PATH ${LIB_DIR}/OpenCV/share/OpenCV)
endif()
find_package(OpenCV 3.2.0 REQUIRED core imgproc video features2d)
set(OPENCV_HEADER_PATHS ${OpenCV_INCLUDE_DIRS})
set(OPENCV_LIBRARIES ${OpenCV_LIBS})
# libtiff
get_filename_component(LIBTIFF_HEADER_PATHS ${LIB_DIR}/libtiff/include ABSOLUTE)
get_filename_component(LIBTIFF_LIB_DIR_PATH ${LIB_DIR}/libtiff/lib ABSOLUTE)
if (${OS_WINDOWS})
    set(LIBTIFF_LIBRARIES ${LIBTIFF_LIB_DIR_PATH}/tiff.lib)
else()
    set(LIBTIFF_LIBRARIES ${LIBTIFF_LIB_DIR_PATH}/libtiff.a)
endif()
# Qt
set(QT_VERSION "5.8")
find_package(Qt5Core ${QT_VERSION} REQUIRED PATHS ${LIB_DIR}/Qt/lib/cmake/Qt5Core NO_DEFAULT_PATH)
set(QT_CORE_HEADER_PATHS ${Qt5Core_INCLUDE_DIRS})
set(QT_CORE_LIBRARIES ${Qt5Core_LIBRARIES})

# add headers and libs to project target
list(APPEND LIB_INCLUDE_PATHS
        ${ARMADILLO_HEADER_PATHS}
        ${OPENCV_HEADER_PATHS}
        ${LIBTIFF_HEADER_PATHS}
        ${QT_CORE_HEADER_PATHS}
        ${HDF5_HEADER_PATHS})
list(APPEND LIB_FILE_PATHS
        ${ARMADILLO_LIBRARIES}
        ${OPENCV_LIBRARIES}
        ${LIBTIFF_LIBRARIES}
        ${QT_CORE_LIBRARIES}
        ${HDF5_LIBRARIES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${HEADER_ONLY_LIB_PATHS} ${LIB_INCLUDE_PATHS} ${SRC_DIR})
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${LIB_FILE_PATHS})
target_compile_definitions(${CMAKE_PROJECT_NAME} PUBLIC ${ARMADILLO_DEFINITIONS})
