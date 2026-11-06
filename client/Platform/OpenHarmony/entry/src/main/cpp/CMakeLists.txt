cmake_minimum_required(VERSION 3.28)
project(SHCNative LANGUAGES C CXX)

get_filename_component(SHC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../.." ABSOLUTE)
include("${SHC_ROOT}/DetectGitInfo.cmake")
MDR_DetectGitInfo()

option(MDR_CLIENT_DEBUGGER "Build the ImGui packet debugger in all configurations" OFF)

set(MDR_HOST_CMAKE "" CACHE FILEPATH "Host CMake 3.31+ for the protocol core")
set(MDR_FMT_SOURCE_DIR "" CACHE PATH "Existing fmt 12.1.0 source directory")
if (NOT MDR_HOST_CMAKE)
    find_program(SHC_HOST_CMAKE NAMES cmake
        HINTS "$ENV{ProgramFiles}/Microsoft Visual Studio/18/Community/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin"
        NO_CMAKE_FIND_ROOT_PATH)
    set(MDR_HOST_CMAKE "${SHC_HOST_CMAKE}")
endif ()
execute_process(COMMAND "${MDR_HOST_CMAKE}" --version
    OUTPUT_VARIABLE SHC_CMAKE_VERSION RESULT_VARIABLE SHC_CMAKE_RESULT)
string(REGEX MATCH "cmake version ([0-9]+\\.[0-9]+\\.[0-9]+)" SHC_CMAKE_MATCH "${SHC_CMAKE_VERSION}")
if (NOT SHC_CMAKE_RESULT EQUAL 0 OR NOT SHC_CMAKE_MATCH OR CMAKE_MATCH_1 VERSION_LESS 3.31)
    message(FATAL_ERROR "Set MDR_HOST_CMAKE to CMake 3.31+; the SDK's CMake 3.28 cannot configure libmdr")
endif ()
if (NOT MDR_FMT_SOURCE_DIR)
    file(GLOB SHC_FMT_CANDIDATES "${SHC_ROOT}/out/build/*/_deps/fmt-src" "${SHC_ROOT}/build/_deps/fmt-src")
    if (SHC_FMT_CANDIDATES)
        list(GET SHC_FMT_CANDIDATES 0 MDR_FMT_SOURCE_DIR)
    endif ()
endif ()
if (NOT EXISTS "${MDR_FMT_SOURCE_DIR}/include/fmt/format.h")
    message(FATAL_ERROR "Set MDR_FMT_SOURCE_DIR to existing fmt 12.1.0 sources; this bridge does not download dependencies")
endif ()

set(SHC_CORE_DIR "${CMAKE_CURRENT_BINARY_DIR}/protocol-core")
set(SHC_COMPATIBILITY_ARGS -UOHOS_COMPATIBLE_SDK_VERSION)
if (OHOS_COMPATIBLE_SDK_VERSION)
    set(SHC_COMPATIBILITY_ARGS "-DOHOS_COMPATIBLE_SDK_VERSION=${OHOS_COMPATIBLE_SDK_VERSION}")
endif ()
execute_process(
    COMMAND "${MDR_HOST_CMAKE}" -S "${SHC_ROOT}" -B "${SHC_CORE_DIR}" -G Ninja
        "-DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
        "-DOHOS_SDK_NATIVE=${OHOS_SDK_NATIVE}"
        "-DHMOS_SDK_NATIVE=${HMOS_SDK_NATIVE}"
        "-DOHOS_ARCH=${OHOS_ARCH}"
        "-DOHOS_STL=${OHOS_STL}"
        ${SHC_COMPATIBILITY_ARGS}
        "-DCMAKE_CXX_FLAGS=-D_LIBCPP_ENABLE_EXPERIMENTAL"
        "-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${SHC_CORE_DIR}/lib"
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_CXX_VISIBILITY_PRESET=hidden
        -DCMAKE_VISIBILITY_INLINES_HIDDEN=ON
        -DMDR_BUILD_CLIENT=OFF -DMDR_BUILD_BT=OFF -DMDR_BUILD_CODEGEN=OFF -DMDR_BUILD_TESTING=OFF
        -DMDR_BUILD_SHARED=ON
        "-DFETCHCONTENT_SOURCE_DIR_FMT=${MDR_FMT_SOURCE_DIR}"
        -DFETCHCONTENT_FULLY_DISCONNECTED=ON -Wno-deprecated
    RESULT_VARIABLE SHC_CORE_CONFIGURE_RESULT)
if (NOT SHC_CORE_CONFIGURE_RESULT EQUAL 0)
    message(FATAL_ERROR "OHOS libmdr configuration failed")
endif ()
add_custom_target(shc_protocol_core
    COMMAND "${MDR_HOST_CMAKE}" --build "${SHC_CORE_DIR}" --target mdr --parallel 4
    BYPRODUCTS "${SHC_CORE_DIR}/lib/libmdr.a" "${SHC_CORE_DIR}/lib/libfmt.a"
    VERBATIM)
add_library(shc_mdr STATIC IMPORTED)
set_target_properties(shc_mdr PROPERTIES IMPORTED_LOCATION "${SHC_CORE_DIR}/lib/libmdr.a")
target_include_directories(shc_mdr INTERFACE "${SHC_ROOT}/libmdr/include")
add_dependencies(shc_mdr shc_protocol_core)
add_library(shc_fmt STATIC IMPORTED)
set_target_properties(shc_fmt PROPERTIES IMPORTED_LOCATION "${SHC_CORE_DIR}/lib/libfmt.a")
target_include_directories(shc_fmt INTERFACE "${MDR_FMT_SOURCE_DIR}/include")
target_compile_definitions(shc_fmt INTERFACE FMT_EXCEPTIONS=0 FMT_USE_LOCALE=0)
add_dependencies(shc_fmt shc_protocol_core)

set(MDR_IMGUI_SOURCE_DIR "" CACHE PATH "Existing Dear ImGui v1.92.7 source directory")
if (NOT MDR_IMGUI_SOURCE_DIR)
    file(GLOB SHC_IMGUI_CANDIDATES "${SHC_ROOT}/out/build/*/_deps/imgui-src" "${SHC_ROOT}/build/_deps/imgui-src")
    foreach (SHC_IMGUI_CANDIDATE IN LISTS SHC_IMGUI_CANDIDATES)
        if (EXISTS "${SHC_IMGUI_CANDIDATE}/imgui.h")
            file(STRINGS "${SHC_IMGUI_CANDIDATE}/imgui.h" SHC_IMGUI_VERSION REGEX "^#define IMGUI_VERSION_NUM")
            if (SHC_IMGUI_VERSION MATCHES "19270")
                set(MDR_IMGUI_SOURCE_DIR "${SHC_IMGUI_CANDIDATE}")
                break()
            endif ()
        endif ()
    endforeach ()
endif ()
if (NOT EXISTS "${MDR_IMGUI_SOURCE_DIR}/imgui.h" OR
    NOT EXISTS "${MDR_IMGUI_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp" OR
    NOT EXISTS "${MDR_IMGUI_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp")
    message(FATAL_ERROR "Set MDR_IMGUI_SOURCE_DIR to existing Dear ImGui v1.92.7 sources; this client does not download dependencies")
endif ()
file(STRINGS "${MDR_IMGUI_SOURCE_DIR}/imgui.h" SHC_IMGUI_VERSION REGEX "^#define IMGUI_VERSION_NUM")
if (NOT SHC_IMGUI_VERSION MATCHES "19270")
    message(FATAL_ERROR "The OHOS client requires Dear ImGui v1.92.7")
endif ()

add_subdirectory("${SHC_ROOT}/client/I18N" "${CMAKE_CURRENT_BINARY_DIR}/client-i18n")
set_target_properties(Client_I18N PROPERTIES POSITION_INDEPENDENT_CODE ON CXX_VISIBILITY_PRESET hidden)

set(MDR_PLATFORM_OS "OHOS")
include("${SHC_ROOT}/contrib/CMakeLists.txt")

# !! TODO SDL_DIALOG !!
# 3.2.26 doesn't support these in OHOS yet.
set(SDL_DIALOG OFF CACHE BOOL "" FORCE)

set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_GAMEPAD OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_HIDAPI OFF CACHE BOOL "" FORCE)
set(SDL_LOCALE ON CACHE BOOL "" FORCE)
set(SDL_MISC OFF CACHE BOOL "" FORCE)
set(SDL_GPU OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_GPU OFF CACHE BOOL "" FORCE)
set(SDL_OPENGL OFF CACHE BOOL "" FORCE)
set(SDL_OPENGLES ON CACHE BOOL "" FORCE)
set(SDL_VULKAN OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_VULKAN OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL3)
set_property(TARGET SDL3-shared PROPERTY VERSION)
set_property(TARGET SDL3-shared PROPERTY SOVERSION)
if (NOT EXISTS "${sdl3_SOURCE_DIR}/src/core/openharmony/SDL_openharmony.c")
    message(FATAL_ERROR "SDL sources must include OpenHarmony support from SDL PR #16281; do not reuse SDL 3.2 sources")
endif ()

find_package(Threads REQUIRED)
add_library(main SHARED
    ClientPlatform.cpp
    "${SHC_ROOT}/client/SDLMain.cpp"
    "${SHC_ROOT}/client/Client.cpp"
    "${SHC_ROOT}/client/Recorder.cpp"
    "${SHC_ROOT}/client/Fonts/PlexSansIcon.c"
    "${MDR_IMGUI_SOURCE_DIR}/imgui.cpp"
    "${MDR_IMGUI_SOURCE_DIR}/imgui_draw.cpp"
    "${MDR_IMGUI_SOURCE_DIR}/imgui_tables.cpp"
    "${MDR_IMGUI_SOURCE_DIR}/imgui_widgets.cpp"
    "${MDR_IMGUI_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
    "${MDR_IMGUI_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp"
)
set(SHC_DEBUGGER_ENABLED $<OR:$<BOOL:${MDR_CLIENT_DEBUGGER}>,$<CONFIG:Debug>>)
target_sources(main PRIVATE
    "$<${SHC_DEBUGGER_ENABLED}:${SHC_ROOT}/client/Debugger.cpp>"
    "$<${SHC_DEBUGGER_ENABLED}:${SHC_ROOT}/client/Generated/ProtocolV1T1Debugger.cpp>"
    "$<${SHC_DEBUGGER_ENABLED}:${SHC_ROOT}/client/Generated/ProtocolV1T2Debugger.cpp>"
    "$<${SHC_DEBUGGER_ENABLED}:${SHC_ROOT}/client/Generated/ProtocolV2T1Debugger.cpp>"
    "$<${SHC_DEBUGGER_ENABLED}:${SHC_ROOT}/client/Generated/ProtocolV2T2Debugger.cpp>"
)
target_compile_features(main PRIVATE cxx_std_20)
target_compile_options(main PRIVATE -fno-exceptions -fno-rtti -Wall -Wextra)
target_compile_definitions(main PRIVATE OHOS_PLATFORM IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    $<${SHC_DEBUGGER_ENABLED}:MDR_CLIENT_DEBUGGER>
    MDR_PLATFORM_OHOS CLIENT_VERSION="2.0.0" MDR_GIT_COMMIT_HASH="${MDR_GIT_COMMIT_HASH}"
    MDR_GIT_BRANCH_NAME="${MDR_GIT_BRANCH_NAME}" MDR_PLATFORM_OS="OHOS" MDR_PLATFORM_PROCESSOR="${OHOS_ARCH}")
target_include_directories(main PRIVATE "${MDR_IMGUI_SOURCE_DIR}" "${MDR_IMGUI_SOURCE_DIR}/backends" "${SHC_ROOT}/client")
set_target_properties(main PROPERTIES CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN ON)
target_link_libraries(main PRIVATE shc_mdr shc_fmt Client_I18N SDL3::SDL3 Threads::Threads
    libace_napi.z.so libhilog_ndk.z.so)
