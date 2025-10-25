cmake_minimum_required(VERSION 3.26)

file(
    GLOB SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/*.cpp"
)
file(
    GLOB INCLUDES
    "${CMAKE_CURRENT_LIST_DIR}/*.hpp"
)

add_library(mdr_PlatformLinux ${SOURCES} ${INCLUDES})
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
find_package(DBus REQUIRED)
message(STATUS "Found DBus: ${DBus_INCLUDE_DIRS}")

target_link_libraries(mdr_PlatformLinux PRIVATE mdr_Includes fmt::fmt ${DBUS_LIBRARIES} bluetooth)
target_include_directories(mdr_PlatformLinux PRIVATE ${DBus_INCLUDE_DIRS})