cmake_minimum_required(VERSION 3.31)

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
    if (MDR_BLE)
        target_link_libraries(mdr_Platform INTERFACE mdr_PlatformWindowsBLE)
    endif()
elseif (MDR_PLATFORM_OS STREQUAL "MACOS")
    message(STATUS "Configuring for MacOS platform")
    add_subdirectory(MacOS)
    target_link_libraries(mdr_Platform INTERFACE mdr_PlatformMacOS)
elseif (MDR_PLATFORM_OS STREQUAL "EMSCRIPTEN")
    message(STATUS "Configuring for Emscripten platform")
    add_subdirectory(Emscripten)
    target_link_libraries(mdr_Platform INTERFACE mdr_PlatformEmscripten)    
else()
    message(FATAL_ERROR "Unsupported platform ${MDR_PLATFORM_OS}")
endif ()

if (MDR_PLATFORM_SHARED_SOURCES)
    add_library(mdr_Platform_shared SHARED ${MDR_PLATFORM_SHARED_SOURCES})
    set_target_properties(mdr_Platform_shared PROPERTIES OUTPUT_NAME "mdr-platform-shared")
    target_compile_definitions(mdr_Platform_shared
        PRIVATE MDR_PLATFORM_BUILDING_SHARED
        INTERFACE MDR_PLATFORM_USING_SHARED
    )
    target_link_libraries(mdr_Platform_shared
        PUBLIC mdr-c
        PRIVATE mdr_shared mdr_Includes fmt::fmt ${MDR_PLATFORM_SHARED_LIBS}
    )
    target_include_directories(mdr_Platform_shared
        PRIVATE "${CMAKE_SOURCE_DIR}/libmdr/src" ${MDR_PLATFORM_SHARED_INCLUDES}
    )
endif ()
