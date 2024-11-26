cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(SonyHeadphonesClient)

set(CMAKE_CXX_STANDARD 20)

find_package(Threads REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(SonyHeadphonesClient)

add_definitions(-D__HEADPHONES_APP_VERSION__="1.3.6")

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
    set(src/platform/linux TRUE)
endif()

if (DEAR_IMGUI)
    target_sources(SonyHeadphonesClient
        PRIVATE src/App.cpp
            contrib/imgui/imgui.cpp
            contrib/imgui/imgui_draw.cpp
            contrib/imgui/imgui_tables.cpp
            contrib/imgui/imgui_widgets.cpp
            contrib/imgui/misc/cpp/imgui_stdlib.cpp
            contrib/imgui/backends/imgui_impl_glfw.cpp
    )

    target_include_directories (SonyHeadphonesClient
        PRIVATE contrib/imgui
    )
endif ()

if (src/platform/linux)
	target_sources(SonyHeadphonesClient
		PRIVATE src/platform/linux/DBusHelper.cpp
            src/platform/linux/LinuxBluetoothConnector.cpp
            src/platform/linux/LinuxGUI.cpp
            src/platform/linux/main.cpp
            contrib/imgui/backends/imgui_impl_opengl3.cpp
    )

    set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/linux)

    set (OpenGL_GL_PREFERENCE GLVND)
	
    find_package(DBus REQUIRED)

    find_package(GLEW REQUIRED)

    find_package(OpenGL REQUIRED)

    if ((NOT DBUS_FOUND) OR (NOT GLEW_FOUND) OR (NOT glfw3_FOUND) OR (NOT OPENGL_FOUND))
        message ( FATAL_ERROR 
        "Didn't find one of the packages. For Debian based systems, use:"
        "sudo apt install libbluetooth-dev libglew-dev libglfw3-dev libdbus-1-dev" )
    endif ()

    target_include_directories(SonyHeadphonesClient
        PRIVATE ${DBUS_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIRS}
    )

    target_link_libraries(SonyHeadphonesClient
        PRIVATE ${DBUS_LIBRARIES}
        GLEW::GLEW 
        glfw
        bluetooth
        OpenGL::GL
    )

endif ()

if (WIN32)
    ADD_DEFINITIONS(-DUNICODE)

    target_sources(SonyHeadphonesClient
        PRIVATE src/platform/windows/WindowsGUI.cpp
            src/platform/windows/main.cpp
            src/platform/windows/WindowsBluetoothConnector.cpp
            contrib/imgui/backends/imgui_impl_dx11.cpp
            contrib/imgui/backends/imgui_impl_win32.cpp
    )

    target_link_libraries(SonyHeadphonesClient
        PRIVATE d3d11
    )
endif ()

if (APPLE)
    enable_language(OBJCXX)
    target_sources(SonyHeadphonesClient
        PRIVATE src/platform/macos/MacOSBluetoothConnector.mm
            src/platform/macos/main.mm
            contrib/imgui/backends/imgui_impl_metal.mm
    )
    target_link_libraries(SonyHeadphonesClient
        PRIVATE "-framework Foundation -framework IOBluetooth -framework IOKit -framework Metal -framework MetalKit -framework Cocoa -framework CoreVideo -framework QuartzCore"
    )
endif ()

target_link_libraries(SonyHeadphonesClient
    PRIVATE Threads::Threads glfw
)
