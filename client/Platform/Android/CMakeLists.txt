cmake_minimum_required(VERSION 3.31)
project(SHCAndroid LANGUAGES C CXX)

if (NOT ANDROID)
    message(FATAL_ERROR "Use the Android NDK toolchain to build this client")
endif ()
get_filename_component(SHC_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../.." ABSOLUTE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

include(FetchContent)
FetchContent_Declare(SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-3.2.26
    GIT_SHALLOW TRUE)
set(MDR_BUILD_CLIENT OFF CACHE BOOL "" FORCE)
set(MDR_BUILD_BT OFF CACHE BOOL "" FORCE)
set(MDR_BUILD_CODEGEN OFF CACHE BOOL "" FORCE)
set(MDR_BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(MDR_BUILD_SHARED ON CACHE BOOL "" FORCE)
option(MDR_CLIENT_DEBUGGER "Build the ImGui packet debugger" OFF)
add_subdirectory("${SHC_ROOT}" "${CMAKE_CURRENT_BINARY_DIR}/protocol-core")
include("${SHC_ROOT}/DetectGitInfo.cmake")
MDR_DetectGitInfo()
add_subdirectory("${SHC_ROOT}/client/I18N" "${CMAKE_CURRENT_BINARY_DIR}/client-i18n")

set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_SHARED ON CACHE BOOL "" FORCE)
foreach (feature TEST_LIBRARY TESTS EXAMPLES AUDIO CAMERA DIALOG JOYSTICK HAPTIC GAMEPAD SENSOR POWER HIDAPI MISC GPU RENDER_GPU OPENGL VULKAN RENDER_VULKAN)
    set(SDL_${feature} OFF CACHE BOOL "" FORCE)
endforeach ()
set(SDL_LOCALE ON CACHE BOOL "" FORCE)
set(SDL_OPENGLES ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(SDL3 imgui)
set_property(TARGET SDL3-shared PROPERTY VERSION)
set_property(TARGET SDL3-shared PROPERTY SOVERSION)

add_library(main SHARED
    ClientPlatform.cpp
    "${SHC_ROOT}/client/SDLMain.cpp"
    "${SHC_ROOT}/client/Client.cpp"
    "${SHC_ROOT}/client/Recorder.cpp"
    "${SHC_ROOT}/client/Fonts/PlexSansIcon.c"
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
    "${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp")
if (MDR_CLIENT_DEBUGGER)
    target_sources(main PRIVATE
        "${SHC_ROOT}/client/Debugger.cpp"
        "${SHC_ROOT}/client/Generated/ProtocolV1T1Debugger.cpp"
        "${SHC_ROOT}/client/Generated/ProtocolV1T2Debugger.cpp"
        "${SHC_ROOT}/client/Generated/ProtocolV2T1Debugger.cpp"
        "${SHC_ROOT}/client/Generated/ProtocolV2T2Debugger.cpp")
    target_compile_definitions(main PRIVATE MDR_CLIENT_DEBUGGER)
endif ()
target_compile_features(main PRIVATE cxx_std_20)
target_compile_options(main PRIVATE -fno-exceptions -fno-rtti -Wall -Wextra)
target_compile_definitions(main PRIVATE
    IMGUI_DISABLE_OBSOLETE_FUNCTIONS SDL_MAIN_USE_CALLBACKS=1
    FMT_EXCEPTIONS=0 FMT_USE_LOCALE=0 MDR_PLATFORM_ANDROID
    CLIENT_VERSION="2.0.0" MDR_GIT_COMMIT_HASH="${MDR_GIT_COMMIT_HASH}"
    MDR_GIT_BRANCH_NAME="${MDR_GIT_BRANCH_NAME}" MDR_PLATFORM_OS="ANDROID"
    MDR_PLATFORM_PROCESSOR="${CMAKE_ANDROID_ARCH_ABI}")
target_include_directories(main PRIVATE
    "${SHC_ROOT}/client" "${imgui_SOURCE_DIR}" "${imgui_SOURCE_DIR}/backends")
target_link_libraries(main PRIVATE mdr Client_I18N SDL3::SDL3 log)
