cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
project(SonyHeadphonesClient CXX)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)

if (WIN32)
    add_executable(SonyHeadphonesClient WIN32)
endif()
if (LINUX)
    add_executable(SonyHeadphonesClient)
endif()
if (APPLE)
    add_executable(SonyHeadphonesClient MACOSX_BUNDLE)
    set_target_properties(SonyHeadphonesClient PROPERTIES
            MACOSX_BUNDLE TRUE
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/macos/Info.plist
    )
endif()

add_definitions(-D__HEADPHONES_APP_VERSION__="1.4.0")

target_sources(SonyHeadphonesClient
        PRIVATE src/BluetoothWrapper.cpp
        src/CommandSerializer.cpp
        src/App.cpp
        src/Headphones.cpp
        contrib/tomlplusplus/src/toml.cpp
)

target_include_directories (SonyHeadphonesClient
        PRIVATE contrib/tomlplusplus/include
)

target_include_directories (SonyHeadphonesClient
        PRIVATE src contrib
)
set (DEAR_IMGUI TRUE)
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if (DEAR_IMGUI)
    target_sources(SonyHeadphonesClient
            PRIVATE src/App.cpp
            contrib/imgui/imgui.cpp
            contrib/imgui/imgui_draw.cpp
            contrib/imgui/imgui_tables.cpp
            contrib/imgui/imgui_widgets.cpp
            contrib/imgui/misc/cpp/imgui_stdlib.cpp
    )

    target_include_directories (SonyHeadphonesClient
            PRIVATE contrib/imgui
    )
endif ()

if (LINUX)
    add_subdirectory(contrib/glfw)
    target_link_libraries(SonyHeadphonesClient PRIVATE glfw)
    target_sources(SonyHeadphonesClient
            PRIVATE PRIVATE src/Main.cpp
            src/platform/linux/DBusHelper.cpp
            src/platform/linux/LinuxBluetoothConnector.cpp
            contrib/imgui/backends/imgui_impl_opengl3.cpp
            contrib/imgui/backends/imgui_impl_glfw.cpp
    )

    set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/src/platform/linux)

    set (OpenGL_GL_PREFERENCE GLVND)

    find_package(DBus REQUIRED)

    find_package(OpenGL REQUIRED)

    if ((NOT DBUS_FOUND) OR (NOT OPENGL_FOUND))
        message ( FATAL_ERROR
                "Didn't find one of the packages. Refer to the README for a list of dependencies that you need to build this application."
        )
    endif ()

    target_include_directories(SonyHeadphonesClient
            PRIVATE ${DBUS_INCLUDE_DIRS}
            ${OPENGL_INCLUDE_DIRS}
    )

    target_link_libraries(SonyHeadphonesClient
            PRIVATE ${DBUS_LIBRARIES}
            glfw
            bluetooth
            OpenGL::GL
    )

endif ()

if (WIN32)
    ADD_DEFINITIONS(-DUNICODE)
    add_subdirectory(contrib/glfw)
    target_link_libraries(SonyHeadphonesClient PRIVATE glfw)

    target_sources(SonyHeadphonesClient
            PRIVATE src/Main.cpp
            src/platform/windows/WindowsBluetoothConnector.cpp
            contrib/imgui/backends/imgui_impl_opengl3.cpp
            contrib/imgui/backends/imgui_impl_glfw.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/windows/app.manifest
    )
    # https://devblogs.microsoft.com/oldnewthing/20240816-00/?p=110136
    target_link_libraries(SonyHeadphonesClient
            PRIVATE opengl32 comctl32
    )
endif ()

if (APPLE)
    enable_language(OBJCXX)
    # Homebrew does not offer universal binaries for GLFW, so we need to build it ourselves
    add_subdirectory(contrib/glfw)
    target_link_libraries(SonyHeadphonesClient PRIVATE glfw)

    target_sources(SonyHeadphonesClient
            PRIVATE src/Main.cpp
            src/platform/macos/MacOSBluetoothConnector.mm            
            contrib/imgui/backends/imgui_impl_metal.mm
            contrib/imgui/backends/imgui_impl_glfw.cpp
    )
    set_source_files_properties(src/Main.cpp
            PROPERTIES LANGUAGE OBJCXX
    )
    target_link_libraries(SonyHeadphonesClient
            PRIVATE "-framework Foundation -framework IOBluetooth -framework IOKit -framework Metal -framework MetalKit -framework Cocoa -framework CoreVideo -framework QuartzCore"
    )
endif ()

target_link_libraries(SonyHeadphonesClient
        PRIVATE Threads::Threads
)