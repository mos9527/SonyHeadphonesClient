cmake_minimum_required(VERSION 3.26)
file(
    GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
add_executable(SonyHeadphonesClient ${SOURCES} ${INCLUDES})

target_link_libraries(SonyHeadphonesClient PRIVATE mdr mdr_Includes)

option(CLIENT_BUILD_WITH_ASAN "Build with Address Sanitizer enabled" ON)
if (CLIENT_BUILD_WITH_ASAN)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options("/fsanitize=address")
        add_link_options("/fsanitize=address")
    else ()
        add_compile_options("-fsanitize=address")
        add_link_options("-fsanitize=address")
    endif ()
endif()

