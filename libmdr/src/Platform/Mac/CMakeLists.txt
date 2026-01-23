cmake_minimum_required(VERSION 3.26)

file(
    GLOB SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/*.mm"
)
file(
    GLOB INCLUDES
    "${CMAKE_CURRENT_LIST_DIR}/*.h"
)

add_library(mdr_PlatformMac ${SOURCES} ${INCLUDES})
target_link_libraries(mdr_PlatformMac PRIVATE mdr-c fmt::fmt)
