set(TEST_CNMFE_NAME cnmfeTests)

get_filename_component(CATCH_HEADER_SEARCH_PATH "../lib/catch" ABSOLUTE)
include_directories(${CATCH_HEADER_SEARCH_PATH})

file(GLOB TEST_SRC_FILES "*.cpp")
add_executable(${TEST_CNMFE_NAME} ${TEST_SRC_FILES})
target_link_libraries(${TEST_CNMFE_NAME} PRIVATE cnmfe)
