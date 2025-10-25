cmake_minimum_required(VERSION 3.26)

file(
        GLOB INCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(mdr_Platform INTERFACE ${INCLUDES})

if (UNIX AND NOT APPLE)
    message(STATUS "Configuring for Linux platform")
    add_subdirectory(Linux)
    target_link_libraries(mdr_Platform INTERFACE mdr_PlatformLinux)
else()
    message(FATAL_ERROR "Unsupported platform")
endif ()
