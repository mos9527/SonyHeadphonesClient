cmake_minimum_required(VERSION 3.31)

add_library(mdr_PlatformWindows PlatformWindows.cpp)
target_link_libraries(mdr_PlatformWindows PRIVATE mdr-c mdr ws2_32.lib rpcrt4.lib Bthprops.lib windowsapp.lib)

add_library(mdr_PlatformWindowsBLE PlatformWindowsBLE.cpp)
target_link_libraries(mdr_PlatformWindowsBLE PRIVATE mdr-c mdr ws2_32.lib rpcrt4.lib Bthprops.lib windowsapp.lib)
