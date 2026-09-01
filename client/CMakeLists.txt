set(CLIENT_VERSION "2.0.0")

set(CLIENT_DEBUGGER_GENERATED_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV1T1Debugger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV1T2Debugger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV2T1Debugger.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV2T2Debugger.cpp"
)

if (MDR_CLIENT_DEBUGGER_TARGET AND COMMAND mdr_add_codegen)
    mdr_add_codegen(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV1T1Debugger.cpp"
        TOOL      tooling_DebuggerCodegen
        INPUT     "${CMAKE_SOURCE_DIR}/libmdr/include/mdr/ProtocolV1T1.hpp"
        NAMESPACE "mdr::v1::t1"
        INCLUDE   "mdr/ProtocolV1T1.hpp")
    mdr_add_codegen(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV1T2Debugger.cpp"
        TOOL      tooling_DebuggerCodegen
        INPUT     "${CMAKE_SOURCE_DIR}/libmdr/include/mdr/ProtocolV1T2.hpp"
        NAMESPACE "mdr::v1::t2"
        INCLUDE   "mdr/ProtocolV1T2.hpp")
    mdr_add_codegen(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV2T1Debugger.cpp"
        TOOL      tooling_DebuggerCodegen
        INPUT     "${CMAKE_SOURCE_DIR}/libmdr/include/mdr/ProtocolV2T1.hpp"
        NAMESPACE "mdr::v2::t1"
        INCLUDE   "mdr/ProtocolV2T1.hpp")
    mdr_add_codegen(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/Generated/ProtocolV2T2Debugger.cpp"
        TOOL      tooling_DebuggerCodegen
        INPUT     "${CMAKE_SOURCE_DIR}/libmdr/include/mdr/ProtocolV2T2.hpp"
        NAMESPACE "mdr::v2::t2"
        INCLUDE   "mdr/ProtocolV2T2.hpp")
endif ()

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
        PacketObserver.cpp
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

if (MDR_CLIENT_DEBUGGER_TARGET)
    add_library(Client_Debugger STATIC
        Debugger.cpp
        Debugger.hpp
        DebuggerDetails.hpp
        ${CLIENT_DEBUGGER_GENERATED_SOURCES}
    )
    target_link_libraries(Client_Debugger PUBLIC mdr ImGuiSDL3)
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
target_link_libraries(SonyHeadphonesClient PRIVATE mdr ImGuiSDL3 Client_Platform mdr::bt)

target_compile_definitions(SonyHeadphonesClient PRIVATE
    -DCLIENT_VERSION="${CLIENT_VERSION}"
    -DMDR_GIT_COMMIT_HASH="${MDR_GIT_COMMIT_HASH}"
    -DMDR_GIT_BRANCH_NAME="${MDR_GIT_BRANCH_NAME}"
    -DMDR_PLATFORM_OS="${MDR_PLATFORM_OS}"
    -DMDR_PLATFORM_PROCESSOR="${CMAKE_SYSTEM_PROCESSOR}"
)
