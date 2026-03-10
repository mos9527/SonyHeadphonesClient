cmake_minimum_required(VERSION 3.31)

file(GLOB SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")
add_library(Client_PlatformMacOS ${SOURCES})
target_link_libraries(Client_PlatformMacOS PRIVATE mdr)
