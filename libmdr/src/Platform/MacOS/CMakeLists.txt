cmake_minimum_required(VERSION 3.31)

file(
    GLOB SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/*.mm"
)

add_library(mdr_PlatformMacOS ${SOURCES})

find_library(IOBLUETOOTH_FRAMEWORK IOBluetooth REQUIRED)
find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)

target_link_libraries(mdr_PlatformMacOS PRIVATE mdr-c fmt::fmt ${IOBLUETOOTH_FRAMEWORK} ${FOUNDATION_FRAMEWORK})
