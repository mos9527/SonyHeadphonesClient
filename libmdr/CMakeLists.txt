cmake_minimum_required(VERSION 3.26)
file(
    GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(
    GLOB_RECURSE INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
include(FetchContent)
FetchContent_MakeAvailable(fmt)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${INCLUDES})
add_library(libmdr ${SOURCES} ${INCLUDES})
target_link_libraries(libmdr PRIVATE fmt::fmt)
