set(CLIENT_VERSION "2.0.0")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${INCLUDES})

if (MDR_PLATFORM_OS STREQUAL "WIN32")
    add_executable(SonyHeadphonesClient)
    # Console for Debug / MDR_DEBUG; WIN32 (GUI) otherwise — works with multi-config generators
    set_target_properties(SonyHeadphonesClient PROPERTIES
        WIN32_EXECUTABLE $<AND:$<NOT:$<BOOL:${MDR_DEBUG}>>,$<NOT:$<CONFIG:Debug>>>
    )
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
        PayloadRecorder.cpp
        Fonts/PlexSansIcon.c
)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
set(SDL_GPU OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_GPU OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_VULKAN OFF CACHE BOOL "" FORCE)
set(SDL_VULKAN OFF CACHE BOOL "" FORCE)
set(SDL_DIALOG OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_GAMEPAD OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_HIDAPI OFF CACHE BOOL "" FORCE)
set(SDL_LOCALE OFF CACHE BOOL "" FORCE)
set(SDL_MISC OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(imgui)
FetchContent_MakeAvailable(SDL3)

add_library(ImGuiSDL3 STATIC)
target_sources(ImGuiSDL3 PRIVATE
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp

)
target_compile_definitions(ImGuiSDL3 PUBLIC
    IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    $<$<NOT:$<CONFIG:Debug>>:IMGUI_DISABLE_DEMO_WINDOWS>
    $<$<NOT:$<CONFIG:Debug>>:IMGUI_DISABLE_DEBUG_TOOLS>
)

add_executable(ImGuiSDL3Example)
target_sources(ImGuiSDL3Example PRIVATE
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/examples/example_sdl3_sdlrenderer3/main.cpp
)
target_link_libraries(ImGuiSDL3Example PUBLIC ImGuiSDL3)

target_include_directories(ImGuiSDL3 PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        .
)

set(CLIENT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
add_subdirectory(Platform)

target_link_libraries(ImGuiSDL3 PUBLIC SDL3::SDL3-static)
target_link_libraries(SonyHeadphonesClient PRIVATE mdr ImGuiSDL3 Client_Platform)

target_compile_definitions(SonyHeadphonesClient PRIVATE
    -DCLIENT_VERSION="${CLIENT_VERSION}"
    -DMDR_GIT_COMMIT_HASH="${MDR_GIT_COMMIT_HASH}"
    -DMDR_GIT_BRANCH_NAME="${MDR_GIT_BRANCH_NAME}"
    -DMDR_PLATFORM_OS="${MDR_PLATFORM_OS}"
)
