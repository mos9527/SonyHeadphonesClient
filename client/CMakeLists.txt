set(CLIENT_VERSION "2.0.0")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${INCLUDES})

if (MDR_PLATFORM_OS STREQUAL "WIN32")
    if (MDR_DEBUG)        
        add_executable(SonyHeadphonesClient) # In MSVC this would build with CONSOLE subsystem
    else()
        add_executable(SonyHeadphonesClient WIN32)
    endif()
    target_sources(SonyHeadphonesClient PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Windows/app.manifest")
elseif (MDR_PLATFORM_OS STREQUAL "MACOS")
    add_executable(SonyHeadphonesClient MACOSX_BUNDLE)
    set_target_properties(SonyHeadphonesClient PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Platform/MacOS/Info.plist"
    )
else ()
    add_executable(SonyHeadphonesClient)
endif ()

target_sources(
        SonyHeadphonesClient
        PRIVATE
        SDLMain.cpp
        Client.cpp
        Fonts/PlexSansIcon.c
)
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(imgui)
FetchContent_MakeAvailable(SDL3)
FetchContent_MakeAvailable(material_color_utilities)
# Patch material-color-utilities for MSVC compatibility
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    file(READ "${material_color_utilities_SOURCE_DIR}/cpp/cam/hct_solver.cc" _hct_solver)
    # Replace C-style functional casts: int(expr) -> static_cast<int>(expr)
    string(REGEX REPLACE "([^a-zA-Z_])int\\(" "\\1static_cast<int>(" _hct_solver "${_hct_solver}")
    file(WRITE "${material_color_utilities_SOURCE_DIR}/cpp/cam/hct_solver.cc" "${_hct_solver}")

    file(READ "${material_color_utilities_SOURCE_DIR}/cpp/cam/viewing_conditions.h" _vc)
    string(REGEX REPLACE "([^a-zA-Z_])int\\(" "\\1static_cast<int>(" _vc "${_vc}")
    # Replace C99 compound literal (Type){...} with C++ brace init Type{...}
    string(REPLACE "(ViewingConditions){" "ViewingConditions{" _vc "${_vc}")
    file(WRITE "${material_color_utilities_SOURCE_DIR}/cpp/cam/viewing_conditions.h" "${_vc}")

    file(READ "${material_color_utilities_SOURCE_DIR}/cpp/cam/cam.cc" _cam)
    string(REGEX REPLACE "([^a-zA-Z_])int\\(" "\\1static_cast<int>(" _cam "${_cam}")
    string(REPLACE "(Vec3){" "Vec3{" _cam "${_cam}")
    file(WRITE "${material_color_utilities_SOURCE_DIR}/cpp/cam/cam.cc" "${_cam}")

    # Patch all C99 compound literals (Type){...} -> Type{...}
    foreach(_f hct_solver.cc hct.cc)
        file(READ "${material_color_utilities_SOURCE_DIR}/cpp/cam/${_f}" _src)
        string(REGEX REPLACE "([^a-zA-Z_])int\\(" "\\1static_cast<int>(" _src "${_src}")
        string(REPLACE "(Vec3){" "Vec3{" _src "${_src}")
        string(REPLACE "(Cam){" "Cam{" _src "${_src}")
        file(WRITE "${material_color_utilities_SOURCE_DIR}/cpp/cam/${_f}" "${_src}")
    endforeach()
endif()

# Build material-color-utilities as a static library
add_library(MaterialColorUtilities STATIC
        MaterialYouPatches.cpp
        ${material_color_utilities_SOURCE_DIR}/cpp/cam/cam.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/cam/hct.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/cam/hct_solver.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/cam/viewing_conditions.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/palettes/tones.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/contrast/contrast.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/dislike/dislike.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/blend/blend.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/dynamiccolor/dynamic_color.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/dynamiccolor/dynamic_scheme.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/dynamiccolor/material_dynamic_colors.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/scheme/scheme_content.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/scheme/scheme_expressive.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/scheme/scheme_fidelity.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/scheme/scheme_fruit_salad.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/scheme/scheme_monochrome.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/scheme/scheme_neutral.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/scheme/scheme_rainbow.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/scheme/scheme_tonal_spot.cc
        ${material_color_utilities_SOURCE_DIR}/cpp/scheme/scheme_vibrant.cc
)
# Headers use #include "cpp/..." so we include the repo root
target_include_directories(MaterialColorUtilities PUBLIC ${material_color_utilities_SOURCE_DIR})
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # Suppress MSVC warnings in third-party code:
    # C4576: parenthesized type with initializer (Clang/GCC cast style)
    # C2397: narrowing conversion
    # /permissive- is default; override to /permissive to allow C-style casts int(x)
    # used throughout the upstream code (C4576)
    target_compile_options(MaterialColorUtilities PRIVATE /permissive /w)
    target_compile_definitions(MaterialColorUtilities PRIVATE _USE_MATH_DEFINES)
endif()

add_library(ImGuiSDL3 STATIC)
target_sources(ImGuiSDL3 PRIVATE
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp

)
target_include_directories(ImGuiSDL3 PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        .
)

set(CLIENT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
add_subdirectory(Platform)

target_link_libraries(ImGuiSDL3 PUBLIC SDL3::SDL3-static)
target_link_libraries(SonyHeadphonesClient PRIVATE mdr ImGuiSDL3 Client_Platform MaterialColorUtilities)

target_compile_definitions(SonyHeadphonesClient PRIVATE
    -DCLIENT_VERSION="${CLIENT_VERSION}"
    -DMDR_GIT_COMMIT_HASH="${MDR_GIT_COMMIT_HASH}"
    -DMDR_GIT_BRANCH_NAME="${MDR_GIT_BRANCH_NAME}"
    -DMDR_PLATFORM_OS="${MDR_PLATFORM_OS}"
)