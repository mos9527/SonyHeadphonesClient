cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(SonyHeadphonesClient C CXX)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # https://developercommunity.visualstudio.com/t/__VA_OPT__-should-be-enabled-by-std:c/10565399
    add_compile_options("/Zc:preprocessor")
    add_compile_options("/utf-8")    
else()
    add_compile_options("-Wall")
endif()

set(MDR_PLATFORM_OS "")
if (UNIX AND NOT APPLE)
    set(MDR_PLATFORM_OS "LINUX")
elseif(WIN32)
    set(MDR_PLATFORM_OS "WIN32")
elseif(APPLE)
    set(MDR_PLATFORM_OS "MACOS")
endif()

add_compile_definitions("MDR_PLATFORM_${MDR_PLATFORM_OS}")

add_subdirectory(contrib)
add_subdirectory(libmdr)
add_subdirectory(client)
add_subdirectory(tooling)
