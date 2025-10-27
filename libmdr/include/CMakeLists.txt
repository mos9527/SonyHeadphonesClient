cmake_minimum_required(VERSION 3.26)
file(
        GLOB_RECURSE INCLUDES_C
        "${CMAKE_CURRENT_SOURCE_DIR}/mdr-c/*.h"
)
add_library(mdr-c INTERFACE ${INCLUDES_C})

target_include_directories(
    mdr-c
    INTERFACE
    .
)

file(
        GLOB_RECURSE INCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/mdr/*.hpp"
)
add_library(mdr_Includes INTERFACE ${INCLUDES})
target_link_libraries(mdr_Includes INTERFACE mdr-c)
target_include_directories(
        mdr_Includes
        INTERFACE
        .
)