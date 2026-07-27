cmake_minimum_required(VERSION 3.31 FATAL_ERROR)
project(SonyHeadphonesClient C CXX)

set(CMAKE_CXX_STANDARD 20)

if (MSVC)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # https://developercommunity.visualstudio.com/t/__VA_OPT__-should-be-enabled-by-std:c/10565399
        add_compile_options("/Zc:preprocessor")
    endif ()
    add_compile_options("/utf-8")
else()
    add_compile_options("-Wall" "-Wno-unused-function" "-Wno-unused-variable" "-Wno-unused-but-set-global")
endif()

# No exceptions!!!
# Needed since MSVC STL containers use this all the time.
# Running with scissors here... though quite actively maintained by STL
# - https://github.com/microsoft/STL/pull/5406
# - https://github.com/microsoft/STL/issues/2216
if (MSVC) # True for both MSVC and Clang-CL
    # Begone. /EHsc is everywhere even with Clang-CL configurations.
    set(target_vars
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
    )
    foreach(var IN LISTS target_vars)
        if(${var} MATCHES "/EHsc")
            string(REPLACE "/EHsc" "" ${var} "${${var}}")
        endif()
    endforeach()
endif()

if (MSVC)
    add_compile_options(/EHs-c-)
    add_compile_definitions(_HAS_EXCEPTIONS=0)
    # You might be surprised that even with enabling these flags, the final PE
    # would *still* contain Exception Unwind tables. And no - there's no way to remove them
    # from MSVC by itself.
    # Of course, you can always...
else()
    # ...Use Clang/LLVM instead. Or Cl-Clang if you must stick with MS toolchains.
    add_compile_options(
        -fno-exceptions
        -fno-unwind-tables
        -fno-asynchronous-unwind-tables
    )
endif()
# On FMT_USE_LOCALE - these usually allocates with operator new(), which we do not use.
# We don't even use chrono either, hence the disablement.
add_compile_definitions(FMT_EXCEPTIONS=0 FMT_USE_LOCALE=0)

# Dead-code strip and no RTTI
if (MSVC)
    add_compile_options(/GR- /Gy /Gw)
    add_link_options(/OPT:REF /OPT:ICF)
else()
    add_compile_options(-fno-rtti -ffunction-sections -fdata-sections)
    if (APPLE)
        add_link_options(-Wl,-dead_strip)
    else()
        add_link_options(-Wl,--gc-sections -s)
    endif()
endif()

# LTO
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_ok OUTPUT ipo_msg)
if (ipo_ok)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON)
else()
    message(STATUS "LTO not supported: ${ipo_msg}")
endif()

option(MDR_BUILD_WITH_ASAN "Build with Address Sanitizer enabled" OFF)
if (MDR_BUILD_WITH_ASAN)
    if (MSVC)
        message(WARNING "ASAN not enabled on MSVC by *us*. You're on your own.")
    else ()
        add_compile_options("-fsanitize=address")
        add_link_options("-fsanitize=address")
    endif ()
endif ()

option(MDR_DEBUG "Build with debug flags enabled" OFF)
if (MDR_DEBUG)
    message(STATUS "Debugging features *ON*. Enjoy the logs.")
   add_compile_definitions(MDR_DEBUG)
   add_compile_definitions(_DEBUG)
endif ()

option(MDR_BLE "Build with BLE Backend support" ON)
if (MDR_BLE)
	message(STATUS "BLE Backend support is ON.")
    add_compile_definitions(MDR_BLE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

include(DetectPlatform.cmake)
MDR_DetectPlatform() # Sets MDR_PLATFORM_OS
include(DetectGitInfo.cmake)
MDR_DetectGitInfo() # Sets MDR_GIT_COMMIT_HASH and MDR_GIT_BRANCH_NAME
if(MDR_PLATFORM_OS STREQUAL "EMSCRIPTEN")
    add_link_options(
            -sALLOW_MEMORY_GROWTH
            -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap','HEAPU8']
            -sGROWABLE_ARRAYBUFFERS=0
    )
    add_compile_options(-O1)
endif()

add_compile_definitions("MDR_PLATFORM_${MDR_PLATFORM_OS}")

cmake_policy(SET CMP0171 NEW) # CODEGEN keyword for add_custom_command

add_subdirectory(contrib)
# tooling comes before libmdr so codegen targets exist when libmdr declares its generated files.
# mdr_add_codegen() is only defined when LLVM was detected and codegen is not disabled;
# libmdr guards on (COMMAND mdr_add_codegen).
option(MDR_ENABLE_CODEGEN "Enable codegen targets (requires LLVM)" ON)
if (MDR_ENABLE_CODEGEN)
    add_subdirectory(tooling)
endif()

include(tooling/Codegen.cmake)
add_subdirectory(libmdr)
add_subdirectory(client)
