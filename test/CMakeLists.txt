set(TEST_CNMFE_NAME cnmfeTests)

file(GLOB TEST_SRC_FILES "*.cpp")
add_executable(${TEST_CNMFE_NAME} ${TEST_SRC_FILES})

get_filename_component(CATCH_HEADER_SEARCH_PATH "../lib/catch" ABSOLUTE)
include_directories(${CATCH_HEADER_SEARCH_PATH})

target_link_libraries(${TEST_CNMFE_NAME} PUBLIC cnmfe)
target_link_libraries(${TEST_CNMFE_NAME} PUBLIC ${LIB_FILE_PATHS})
target_include_directories(${TEST_CNMFE_NAME} PUBLIC ${HEADER_ONLY_LIB_PATHS} ${LIB_INCLUDE_PATHS})
target_compile_definitions(${TEST_CNMFE_NAME} PUBLIC ${ARMADILLO_DEFINITIONS})
