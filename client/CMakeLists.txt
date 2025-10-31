cmake_minimum_required(VERSION 3.26)

option(CLIENT_BUILD_WITH_ASAN "Build with Address Sanitizer enabled" ON)
if (CLIENT_BUILD_WITH_ASAN)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options("/fsanitize=address")
    else ()
        add_compile_options("-fsanitize=address")
        add_link_options("-fsanitize=address")
    endif ()
endif ()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${INCLUDES})

if (MDR_PLATFORM_OS STREQUAL "WIN32")
    add_executable(SonyHeadphonesClient WIN32 "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Windows/app.manifest")
else ()
    add_executable(SonyHeadphonesClient)
endif ()

target_sources(
        SonyHeadphonesClient
        PRIVATE
        SDLMain.cpp
        Client.cpp
        Fonts/CousineRegular.c
)
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(imgui)
FetchContent_MakeAvailable(SDL2)

add_library(ImGuiSDL2 STATIC)
target_sources(ImGuiSDL2 PRIVATE
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp

)
target_include_directories(ImGuiSDL2 PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        .
)

set(CLIENT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
add_subdirectory(Platform)

target_link_libraries(ImGuiSDL2 PUBLIC SDL2::SDL2-static SDL2::SDL2main)
target_link_libraries(SonyHeadphonesClient PRIVATE mdr ImGuiSDL2 Client_Platform)
