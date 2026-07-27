cmake_minimum_required(VERSION 3.31)

add_library(mdr_PlatformLinux DBusHelper.cpp PlatformLinux.cpp)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(DBus REQUIRED)
message(STATUS "Found DBus: ${DBus_INCLUDE_DIRS} ${DBus_LIBRARIES}")

target_link_libraries(mdr_PlatformLinux PRIVATE mdr-c mdr ${DBus_LIBRARIES} bluetooth)
target_include_directories(mdr_PlatformLinux PRIVATE ${DBus_INCLUDE_DIRS})