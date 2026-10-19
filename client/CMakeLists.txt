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

# Here's some arcane magic that makes us build on Windows that goes as far back as XP RTM...
# ---
# 1. The first thing being thread static initialization during CRT init:
# https://learn.microsoft.com/en-us/cpp/build/reference/zc-threadsafeinit-thread-safe-local-static-initialization?view=msvc-170
# Removes static variable initialization thread safety guarantees when we *know* we don't need one
# ---
# Generally speaking you should NOT need this, and very possibly a footgun that you might not even know exists if you turned it off.
# `client` did this only for codepath that explictly is not threaded (MDR_CLIENT_THREADED OFF).
# - This only affects `client/` targets (client and co., ImGui)
# - Client no longer polls on a worker thread either (so static init order across *threads* is not a problem, but still a fiasco across TUs)
# ...Though in hindsight, you *really* should not have *depended* on static order to begin with.
# As to why besides smaller binary size:
# - We never relied on well-defined cocurrent static initialization order (across threads or else).
# - On new Windows SDKs (possibly anything past 7 at this point) this is implemented via SRW locks.
# - There's plans to support XP which does not support that. (possibly DOA already , if you know what standard MSVC CRT is like) 
if (MSVC AND NOT MDR_CLIENT_THREADED)
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/Zc:threadSafeInit->)
endif ()

# Which brings us to https://github.com/Chuyu-Team/YY-Thunks...
# ---
option(MDR_MSVC_CLIENT_YY_THUNKS "Link the client with YY-Thunks for backwards compatibility" OFF)
if (MDR_MSVC_CLIENT_YY_THUNKS AND NOT MSVC)
    message(FATAL_ERROR "MDR_MSVC_CLIENT_YY_THUNKS requires an MSVC-compatible toolchain")
endif ()

# 2. With YY-Thunks.
# ---
# This neat library implements, internally or through older APIs, new WinAPIs that's absent in legacy Windows builds
# As we're linking statically this made it possible to build binaries backwards compat with even *XP* with the latest toolchains!
# See https://github.com/Chuyu-Team/YY-Thunks/blob/master/ThunksList.md for a list.
if (MDR_MSVC_CLIENT_YY_THUNKS)
    if (NOT CMAKE_CXX_COMPILER_LINKER_ID STREQUAL "MSVC")
        message(FATAL_ERROR "MDR_MSVC_CLIENT_YY_THUNKS requires MSVC linker.")
    endif ()
    string(TOLOWER "${MSVC_CXX_ARCHITECTURE_ID}" MDR_YY_THUNKS_ARCH)

    if (MDR_YY_THUNKS_ARCH STREQUAL "x86")
        # XP x86
        set(MDR_YY_THUNKS_SUBSYSTEM_VERSION "5.01")
    elseif (MDR_YY_THUNKS_ARCH STREQUAL "x64")
        # XP x64, Server 2003
        set(MDR_YY_THUNKS_SUBSYSTEM_VERSION "5.02")
    else ()
        message(FATAL_ERROR "MDR_MSVC_CLIENT_YY_THUNKS supports only x86 and x64 targets")
    endif ()

    FetchContent_MakeAvailable(yy_thunks)
    set(MDR_YY_THUNKS_OBJECT "${yy_thunks_SOURCE_DIR}/objs/${MDR_YY_THUNKS_ARCH}/YY_Thunks_for_WinXP.obj")
    if (NOT EXISTS "${MDR_YY_THUNKS_OBJECT}")
        message(FATAL_ERROR "YY-Thunks object not found: ${MDR_YY_THUNKS_OBJECT}")
    endif ()

    set_source_files_properties("${MDR_YY_THUNKS_OBJECT}" PROPERTIES EXTERNAL_OBJECT TRUE)
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

if (MDR_MSVC_CLIENT_YY_THUNKS)
    target_sources(SonyHeadphonesClient PRIVATE "${MDR_YY_THUNKS_OBJECT}")

    # Mind you, that the PE subsystem version should always be overriden for older Windows loaders to work
    # By default it's 6.0, which XP does not support.
    target_link_options(SonyHeadphonesClient PRIVATE "/SUBSYSTEM:WINDOWS,${MDR_YY_THUNKS_SUBSYSTEM_VERSION}")
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
target_compile_definitions(SonyHeadphonesClient PRIVATE MDR_CLIENT_THREADED=$<BOOL:${MDR_CLIENT_THREADED}>)
if (MDR_CLIENT_THREADED)
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
