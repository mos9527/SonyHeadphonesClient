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
else ()
    add_executable(SonyHeadphonesClient)
endif ()

set(CLIENT_SOURCES "")
if (MDR_PLATFORM_OS STREQUAL "MACOS")
    list(APPEND CLIENT_SOURCES Platform/Mac/MacMain.mm)
else ()
    list(APPEND CLIENT_SOURCES SDLMain.cpp Client.cpp Fonts/PlexSansIcon.c)
endif ()

target_sources(
        SonyHeadphonesClient
        PRIVATE
        ${CLIENT_SOURCES}
)
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)

if (NOT MDR_PLATFORM_OS STREQUAL "MACOS")
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
    target_include_directories(ImGuiSDL3 PUBLIC
            ${imgui_SOURCE_DIR}
            ${imgui_SOURCE_DIR}/backends
            .
    )
endif ()

set(CLIENT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
add_subdirectory(Platform)

if (NOT MDR_PLATFORM_OS STREQUAL "MACOS")
    target_link_libraries(ImGuiSDL3 PUBLIC SDL3::SDL3-static)
    target_link_libraries(SonyHeadphonesClient PRIVATE mdr ImGuiSDL3 Client_Platform)
else ()
    target_link_libraries(SonyHeadphonesClient PRIVATE mdr Client_Platform)
endif ()

if (MDR_PLATFORM_OS STREQUAL "MACOS")
    set_target_properties(SonyHeadphonesClient PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Mac/Info.plist.in"
        MACOSX_BUNDLE_BUNDLE_NAME "SonyHeadphonesClient"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.sony.SonyHeadphonesClient"
        MACOSX_BUNDLE_BUNDLE_VERSION "${CLIENT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${CLIENT_VERSION}"
    )
endif ()

target_compile_definitions(SonyHeadphonesClient PRIVATE
    -DCLIENT_VERSION="${CLIENT_VERSION}"
    -DMDR_GIT_COMMIT_HASH="${MDR_GIT_COMMIT_HASH}"
    -DMDR_GIT_BRANCH_NAME="${MDR_GIT_BRANCH_NAME}"
    -DMDR_PLATFORM_OS="${MDR_PLATFORM_OS}"
)
