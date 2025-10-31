cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(SonyHeadphonesClient C CXX)

set(CMAKE_CXX_STANDARD 20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # https://developercommunity.visualstudio.com/t/__VA_OPT__-should-be-enabled-by-std:c/10565399
    add_compile_options("/Zc:preprocessor")
    add_compile_options("/utf-8")
else()
    add_compile_options("-Wall" "-Wno-unused-function" "-Wno-unused-variable")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

include(DetectPlatform.cmake)
MDR_DetectPlatform() # Sets MDR_PLATFORM_OS

if(MDR_PLATFORM_OS STREQUAL "EMSCRIPTEN")
    # For Emscripten: We use exceptions mainly for Validation and MDR_CHECKs
    # Do note that the overhead from exceptions is not trivial _even if_ we don't
    # take the slow path.
    # TODO: Disable exceptions (and therefore _die_ on any) on exception disabled builds
    add_link_options("-fexceptions" "-sALLOW_MEMORY_GROWTH" "-sEXCEPTION_CATCHING_ALLOWED=[..]")
    add_compile_options("-fexceptions")
endif()

add_compile_definitions("MDR_PLATFORM_${MDR_PLATFORM_OS}")

add_subdirectory(contrib)
add_subdirectory(libmdr)
add_subdirectory(client)
add_subdirectory(tooling)
