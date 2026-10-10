cmake_minimum_required(VERSION 3.31 FATAL_ERROR)
project(SonyHeadphonesClient C CXX)

set(CMAKE_CXX_STANDARD 20)


if (MSVC)
    option(MDR_STATIC_CRT "Statically link the MSVC runtime (/MT)" ON)
    option(MDR_DYNAMIC_UCRT "With MDR_STATIC_CRT on, keep UCRT linked dynamically in non-Debug builds" ON)
    if (MDR_STATIC_CRT AND NOT DEFINED CMAKE_MSVC_RUNTIME_LIBRARY)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
        if (MDR_DYNAMIC_UCRT)
            add_link_options(
                $<$<NOT:$<CONFIG:Debug>>:/NODEFAULTLIB:libucrt.lib>
                $<$<NOT:$<CONFIG:Debug>>:/DEFAULTLIB:ucrt.lib>
            )
        endif ()
    endif ()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # https://developercommunity.visualstudio.com/t/__VA_OPT__-should-be-enabled-by-std:c/10565399
        add_compile_options("/Zc:preprocessor")
        # https://developercommunity.visualstudio.com/t/absolute-paths-for-associated-pdb-files-are-record/573023#T-N623156
        file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" MDR_SOURCE_PREFIX)
        add_compile_options("/d1trimfile:${MDR_SOURCE_PREFIX}")
    endif ()
    add_compile_options("/utf-8")
else()
    add_compile_options("-Wall" "-Wno-unused-function" "-Wno-unused-variable" "-Wno-unused-but-set-global")
    add_compile_options("-fmacro-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/=")
endif()

option(MDR_NO_EXCEPTIONS "Do not use C++ exceptions" ON)
if (MDR_NO_EXCEPTIONS)
    message(STATUS "C++ exceptions are disabled")
    # No exceptions.
    # =============================================================================
    # For MSVC/Clang-cl with STL:
    # - https://github.com/microsoft/STL/issues/202
    # - _HAS_EXCEPTIONS=0 barely works.
    # - With cases like these - in the end you'd be linking to MSVCP*.dll, VCRUNTIME*.dll 
    #   since _Xlen checks are always performed.
    # - MDR_STATIC_CRT (default ON) can be used to avoid VCRUNTIME entirely.
    if (MSVC) # True for both MSVC and Clang-cl
        string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        add_compile_options(/EHs-c-)
        add_compile_definitions(_HAS_EXCEPTIONS=0 _CRT_SECURE_NO_WARNINGS)  
    else()
        # Other platforms are easier.
        add_compile_options(
            -fno-exceptions
            -fno-unwind-tables
            -fno-asynchronous-unwind-tables
        )
    endif()

    # On FMT_USE_LOCALE - these allocate with operator new(), which we do not use.
    # We don't even use chrono either, hence the disablement.
    add_compile_definitions(FMT_EXCEPTIONS=0 FMT_USE_LOCALE=0)
endif()

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

option(MDR_BUILD_WITH_ASAN "Build with Address Sanitizer enabled" OFF)
if (MDR_BUILD_WITH_ASAN)
    if (MSVC)
        add_compile_options("/fsanitize=address")
    else ()
        add_compile_options("-fsanitize=address")
        add_link_options("-fsanitize=address")
    endif ()
endif ()

option(MDR_DEBUG "Build with debug flags enabled (also always on for Debug builds)" OFF)
option(MDR_DEBUG_TRAPS "Break into the debugger when MDR operations fail in Debug builds" OFF)
option(MDR_BUILD_CLIENT "Build the Sony Headphones Client and platform backends" ON)
option(MDR_CLIENT_DEBUGGER "Build the ImGui packet debugger in all configurations" OFF)
set(MDR_CLIENT_DEBUGGER_TARGET OFF)
if (MDR_BUILD_CLIENT AND
    (MDR_CLIENT_DEBUGGER OR CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_CONFIGURATION_TYPES))
    set(MDR_CLIENT_DEBUGGER_TARGET ON)
endif ()
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(MDR_DEBUG ON)
endif ()
add_compile_definitions(
    $<$<OR:$<BOOL:${MDR_DEBUG}>,$<CONFIG:Debug>>:MDR_DEBUG>
    $<$<AND:$<BOOL:${MDR_DEBUG_TRAPS}>,$<CONFIG:Debug>>:MDR_DEBUG_TRAPS>
)
if (MDR_DEBUG)
    message(STATUS "Debugging features enabled.")
endif ()

option(MDR_ENABLE_LOG "Enable MDR_LOG logging output in non-Debug builds" OFF)
set(MDR_LOG_NON_DEBUG OFF)
if (MDR_ENABLE_LOG OR MDR_DEBUG)
    set(MDR_LOG_NON_DEBUG ON)
endif ()
add_compile_definitions(MDR_ENABLE_LOG=$<IF:$<OR:$<CONFIG:Debug>,$<BOOL:${MDR_LOG_NON_DEBUG}>>,1,0>)

# LTCG/IPO
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_ok OUTPUT ipo_msg)
if (ipo_ok)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON)
else()
    message(STATUS "LTO not supported: ${ipo_msg}")
endif()

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
            -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap','HEAPU8','FS']
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
if (MDR_BUILD_CLIENT OR NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(client/I18N)
endif ()
if (NOT CMAKE_CROSSCOMPILING)
    add_subdirectory(tooling/i18n-check)
endif ()

get_property(MDR_SUPPORTS_SHARED_LIBS GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS)
option(MDR_BUILD_SHARED "Build the MDR and Bluetooth shared libraries" ${MDR_SUPPORTS_SHARED_LIBS})

include(tooling/Codegen.cmake)
add_subdirectory(libmdr)
add_subdirectory(libmdr-bt)
if (MDR_BUILD_CLIENT)
    add_subdirectory(client)
endif ()

include(CTest)
if (BUILD_TESTING)
    add_subdirectory(tests)
    add_test(
        NAME mdr-c-abi
        COMMAND mdr_abi_tests
    )

    file(
        GLOB replay_test_entries
        CONFIGURE_DEPENDS
        LIST_DIRECTORIES true
        "${CMAKE_CURRENT_SOURCE_DIR}/tests/*"
    )
    foreach(replay_test_entry IN LISTS replay_test_entries)
        if (IS_DIRECTORY "${replay_test_entry}")
            get_filename_component(
                replay_test_name
                "${replay_test_entry}"
                NAME
            )
            add_test(
                NAME "${replay_test_name}"
                COMMAND mdr_replay_tests "${replay_test_entry}"
            )
        endif()
    endforeach()
endif()
