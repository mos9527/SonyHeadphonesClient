cmake_minimum_required(VERSION 3.26)

file(
    GLOB SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
)
file(
    GLOB INCLUDES
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp"
)

add_library(mdr_PlatformEmscripten ${SOURCES} ${INCLUDES})
target_link_libraries(mdr_PlatformEmscripten PRIVATE mdr-c fmt::fmt)

