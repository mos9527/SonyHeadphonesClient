cmake_minimum_required(VERSION 3.26)

add_library(Client_Platform INTERFACE)
target_link_libraries(
        Client_Platform
        INTERFACE
        mdr
        fmt::fmt
)
if (MDR_PLATFORM_OS STREQUAL "LINUX")
    message(STATUS "Configuring for Linux platform")
    add_subdirectory(Linux)
    target_link_libraries(Client_Platform INTERFACE Client_PlatformLinux)
elseif (MDR_PLATFORM_OS STREQUAL "WIN32")
    message(STATUS "Configuring for Windows platform")
    add_subdirectory(Windows)
    target_link_libraries(Client_Platform INTERFACE Client_PlatformWindows)
elseif (MDR_PLATFORM_OS STREQUAL "EMSCRIPTEN")
    message(STATUS "Configuring for Emscripten platform")
    add_subdirectory(Emscripten)
    target_link_libraries(Client_Platform INTERFACE Client_PlatformEmscripten)
else()
    message(FATAL_ERROR "Unsupported platform")
endif ()
