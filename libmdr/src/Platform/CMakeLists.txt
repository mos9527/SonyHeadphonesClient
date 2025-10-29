cmake_minimum_required(VERSION 3.26)

file(
        GLOB INCLUDES
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

add_library(mdr_Platform INTERFACE ${INCLUDES})

if (MDR_PLATFORM_OS STREQUAL "LINUX")
    message(STATUS "Configuring for Linux platform")
    add_subdirectory(Linux)
    target_link_libraries(mdr_Platform INTERFACE mdr_PlatformLinux)
elseif (MDR_PLATFORM_OS STREQUAL "WIN32")
    message(STATUS "Configuring for Windows platform")
    add_subdirectory(Windows)
    target_link_libraries(mdr_Platform INTERFACE mdr_PlatformWindows)
elseif (MDR_PLATFORM_OS STREQUAL "EMSCRIPTEN")
    message(STATUS "Configuring for Emscripten platform")
    add_subdirectory(Emscripten)
    target_link_libraries(mdr_Platform INTERFACE mdr_PlatformEmscripten)
else()
    message(FATAL_ERROR "Unsupported platform")
endif ()
