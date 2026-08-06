cmake_minimum_required(VERSION 3.31)

add_library(Client_PlatformLinux STATIC
        ClientPlatform.cpp
        ConnectionLinux.cpp
        DBusHelper.cpp
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(DBus REQUIRED)

target_link_libraries(Client_PlatformLinux
        PRIVATE
        mdr
        ${DBus_LIBRARIES}
        bluetooth
)
target_include_directories(Client_PlatformLinux PRIVATE ${DBus_INCLUDE_DIRS})