cmake_minimum_required(VERSION 3.26)

option(CLIENT_BUILD_WITH_ASAN "Build with Address Sanitizer enabled" ON)
if (CLIENT_BUILD_WITH_ASAN)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options("/fsanitize=address")
        add_link_options("/fsanitize=address")
    else ()
        add_compile_options("-fsanitize=address")
        add_link_options("-fsanitize=address")
    endif ()
endif ()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${INCLUDES})
add_executable(
        SonyHeadphonesClient
        SDLMain.cpp
        Client.cpp
)

set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK ON CACHE BOOL "" FORCE)

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
target_link_libraries(ImGuiSDL2 PUBLIC SDL2::SDL2-static)

target_link_libraries(SonyHeadphonesClient PRIVATE mdr ImGuiSDL2)



