cmake_minimum_required(VERSION 3.26)
file(
        GLOB_RECURSE INCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/mdr/*.h"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${INCLUDES})
add_library(mdr_Includes INTERFACE ${INCLUDES})

target_include_directories(
        mdr_Includes
        INTERFACE
        .
)