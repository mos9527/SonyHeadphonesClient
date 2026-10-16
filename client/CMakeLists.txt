set(CLIENT_VERSION "2.0.0")

set(CLIENT_DEBUGGER_GENERATED_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV1T1Debugger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV1T2Debugger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV2T1Debugger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV2T2Debugger.cpp"
)

if (COMMAND mdr_add_codegen)
    mdr_add_codegen(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV1T1Debugger.cpp"
        TOOL      tooling_DebuggerCodegen
        INPUT     "${CMAKE_CURRENT_SOURCE_DIR}/../libmdr/include/mdr/ProtocolV1T1.hpp"
        NAMESPACE "mdr::v1::t1"
        INCLUDE   "mdr/ProtocolV1T1.hpp")
    mdr_add_codegen(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV1T2Debugger.cpp"
        TOOL      tooling_DebuggerCodegen
        INPUT     "${CMAKE_CURRENT_SOURCE_DIR}/../libmdr/include/mdr/ProtocolV1T2.hpp"
        NAMESPACE "mdr::v1::t2"
        INCLUDE   "mdr/ProtocolV1T2.hpp")
    mdr_add_codegen(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV2T1Debugger.cpp"
        TOOL      tooling_DebuggerCodegen
        INPUT     "${CMAKE_CURRENT_SOURCE_DIR}/../libmdr/include/mdr/ProtocolV2T1.hpp"
        NAMESPACE "mdr::v2::t1"
        INCLUDE   "mdr/ProtocolV2T1.hpp")
    mdr_add_codegen(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV2T2Debugger.cpp"
        TOOL      tooling_DebuggerCodegen
        INPUT     "${CMAKE_CURRENT_SOURCE_DIR}/../libmdr/include/mdr/ProtocolV2T2.hpp"
        NAMESPACE "mdr::v2::t2"
        INCLUDE   "mdr/ProtocolV2T2.hpp")
    add_custom_target(mdr_codegen_debugger DEPENDS ${CLIENT_DEBUGGER_GENERATED_SOURCES})
endif ()

if (NOT MDR_BUILD_CLIENT)
    return()
endif ()

if (MDR_PLATFORM_OS STREQUAL "WIN32")
    add_executable(SonyHeadphonesClient WIN32)
    target_sources(SonyHeadphonesClient PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Windows/app.manifest"
        "${CMAKE_CURRENT_SOURCE_DIR}/Platform/Windows/app.rc"
    )
elseif (MDR_PLATFORM_OS STREQUAL "MACOS")
    set(CLIENT_ICON "${CMAKE_CURRENT_SOURCE_DIR}/Platform/MacOS/AppIcon.icns")
    add_executable(SonyHeadphonesClient MACOSX_BUNDLE "${CLIENT_ICON}")
    set_source_files_properties("${CLIENT_ICON}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(SonyHeadphonesClient PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Platform/MacOS/Info.plist"
    )
elseif (MDR_PLATFORM_OS STREQUAL "IOS")
    set(CLIENT_ASSETS "${CMAKE_CURRENT_SOURCE_DIR}/Platform/iOS/Assets.xcassets")
    add_executable(SonyHeadphonesClient MACOSX_BUNDLE "${CLIENT_ASSETS}")
    set_source_files_properties("${CLIENT_ASSETS}" PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
    set_target_properties(SonyHeadphonesClient PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Platform/iOS/Info.plist"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${CLIENT_VERSION}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${CLIENT_VERSION}"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.mos9527.SonyHeadphonesClient"
        XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
        XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon"
    )
else ()
    add_executable(SonyHeadphonesClient)
endif ()

target_sources(
        SonyHeadphonesClient
        PRIVATE
        SDLMain.cpp
        Client.cpp
        Recorder.cpp
        Fonts/PlexSansIcon.c
)
target_link_libraries(SonyHeadphonesClient PRIVATE Client_I18N)

set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_CAMERA OFF CACHE BOOL "" FORCE)
set(SDL_DIALOG ON CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
set(SDL_GAMEPAD OFF CACHE BOOL "" FORCE)
set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
set(SDL_AUDIO OFF CACHE BOOL "" FORCE)
set(SDL_POWER OFF CACHE BOOL "" FORCE)
set(SDL_HIDAPI OFF CACHE BOOL "" FORCE)
set(SDL_LOCALE ON CACHE BOOL "" FORCE)
set(SDL_MISC OFF CACHE BOOL "" FORCE)
set(SDL_GPU OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_GPU OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_D3D OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_D3D12 OFF CACHE BOOL "" FORCE)
set(SDL_VULKAN OFF CACHE BOOL "" FORCE)
set(SDL_RENDER_VULKAN OFF CACHE BOOL "" FORCE)

set(CLIENT_SDL_D3D11 OFF)
set(CLIENT_SDL_METAL OFF)
set(CLIENT_SDL_OPENGL OFF)
set(CLIENT_SDL_OPENGLES OFF)
if (MDR_PLATFORM_OS STREQUAL "WIN32")
    set(CLIENT_SDL_D3D11 ON)
elseif (APPLE)
    set(CLIENT_SDL_METAL ON)
elseif (MDR_PLATFORM_OS STREQUAL "LINUX")
    set(CLIENT_SDL_OPENGL ON)
elseif (MDR_PLATFORM_OS STREQUAL "EMSCRIPTEN" OR MDR_PLATFORM_OS STREQUAL "ANDROID")
    set(CLIENT_SDL_OPENGLES ON)
endif ()
set(SDL_RENDER_D3D11 ${CLIENT_SDL_D3D11} CACHE BOOL "" FORCE)
set(SDL_METAL ${CLIENT_SDL_METAL} CACHE BOOL "" FORCE)
set(SDL_RENDER_METAL ${CLIENT_SDL_METAL} CACHE BOOL "" FORCE)
set(SDL_OPENGL ${CLIENT_SDL_OPENGL} CACHE BOOL "" FORCE)
set(SDL_OPENGLES ${CLIENT_SDL_OPENGLES} CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(imgui)
FetchContent_MakeAvailable(SDL3)
target_compile_definitions(SDL3-static PRIVATE SDL_LEAN_AND_MEAN SDL_VIDEO_RENDER_SW=1)

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

if (NOT MDR_PLATFORM_OS STREQUAL "IOS")
    add_executable(ImGuiSDL3Example)
    target_sources(ImGuiSDL3Example PRIVATE
            ${imgui_SOURCE_DIR}/imgui_demo.cpp
            ${imgui_SOURCE_DIR}/examples/example_sdl3_sdlrenderer3/main.cpp
    )
    target_link_libraries(ImGuiSDL3Example PUBLIC ImGuiSDL3)
endif ()

target_include_directories(ImGuiSDL3 PUBLIC
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        .
)

if (MDR_CLIENT_DEBUGGER_TARGET)
    add_library(Client_Debugger STATIC
        Debugger.cpp
        Debugger.hpp
        DebuggerDetails.hpp
        ${CLIENT_DEBUGGER_GENERATED_SOURCES}
    )
    target_link_libraries(Client_Debugger PUBLIC mdr ImGuiSDL3 Client_I18N)
    target_include_directories(Client_Debugger PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
endif ()

if (MDR_CLIENT_DEBUGGER_TARGET)
    set(CLIENT_DEBUGGER_ENABLED
        $<OR:$<BOOL:${MDR_CLIENT_DEBUGGER}>,$<CONFIG:Debug>>
    )
    target_link_libraries(SonyHeadphonesClient PRIVATE
        $<${CLIENT_DEBUGGER_ENABLED}:Client_Debugger>
    )
    target_compile_definitions(SonyHeadphonesClient PRIVATE
        $<${CLIENT_DEBUGGER_ENABLED}:MDR_CLIENT_DEBUGGER>
    )
endif ()

set(CLIENT_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}")
add_subdirectory(Platform)

target_link_libraries(ImGuiSDL3 PUBLIC SDL3::SDL3-static)
if (MDR_CLIENT_DEBUGGER_TARGET)
    target_link_libraries(Client_Debugger PUBLIC Client_Platform)
endif ()
target_link_libraries(SonyHeadphonesClient PRIVATE mdr ImGuiSDL3 Client_Platform mdr::bt)
if (NOT MDR_PLATFORM_OS STREQUAL "EMSCRIPTEN")
    find_package(Threads REQUIRED)
    target_link_libraries(SonyHeadphonesClient PRIVATE Threads::Threads)
endif ()

if (SDL_DIALOG AND (MDR_PLATFORM_OS STREQUAL "WIN32" OR
                   MDR_PLATFORM_OS STREQUAL "LINUX" OR
                   MDR_PLATFORM_OS STREQUAL "MACOS"))
    target_compile_definitions(SonyHeadphonesClient PRIVATE SDL_DIALOG)
endif ()

target_compile_definitions(SonyHeadphonesClient PRIVATE
    -DCLIENT_VERSION="${CLIENT_VERSION}"
    -DMDR_GIT_COMMIT_HASH="${MDR_GIT_COMMIT_HASH}"
    -DMDR_GIT_BRANCH_NAME="${MDR_GIT_BRANCH_NAME}"
)
