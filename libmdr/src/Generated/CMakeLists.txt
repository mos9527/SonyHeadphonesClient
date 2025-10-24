cmake_minimum_required(VERSION 3.26)
file(
        GLOB SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(
        GLOB INCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${INCLUDES})

add_library(mdr_Generated ${SOURCES} ${INCLUDES})
target_link_libraries(mdr_Generated PRIVATE fmt::fmt)
target_include_directories(mdr_Generated PRIVATE ..)