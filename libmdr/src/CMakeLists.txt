cmake_minimum_required(VERSION 3.26)
file(
    GLOB SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Generated/*.cpp"
)
file(
    GLOB INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

include(FetchContent)
FetchContent_MakeAvailable(fmt)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${INCLUDES})
add_library(mdr ${SOURCES} ${INCLUDES})

add_subdirectory(Platform)

target_link_libraries(mdr PRIVATE mdr_Platform)
target_link_libraries(mdr PUBLIC mdr_Includes fmt::fmt)
target_include_directories(mdr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
