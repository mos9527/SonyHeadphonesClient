cmake_minimum_required(VERSION 3.31)

add_executable(mdr_tests TestMain.cpp)
target_link_libraries(mdr_tests PRIVATE mdr)
target_compile_definitions(
    mdr_tests
    PRIVATE
        MDR_TEST_FIXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/fixtures"
)

add_test(NAME mdr_tests COMMAND mdr_tests)
