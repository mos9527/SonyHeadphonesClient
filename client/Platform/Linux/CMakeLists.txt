cmake_minimum_required(VERSION 3.31)

add_library(Client_PlatformLinux STATIC
        ClientPlatform.cpp
        MediaControl.cpp
)

# MPRIS media control talks to the session bus. FindDBus.cmake is a copy of the
# one in libmdr-bt/src/Linux, kept local so this directory stands on its own.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(DBus REQUIRED)

target_include_directories(Client_PlatformLinux PRIVATE ${DBus_INCLUDE_DIRS})

target_link_libraries(Client_PlatformLinux
        PUBLIC
        mdr::bt
        PRIVATE
        mdr
        ${DBus_LIBRARIES}
)
