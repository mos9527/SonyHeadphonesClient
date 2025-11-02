cmake_minimum_required(VERSION 3.26)

add_library(Client_PlatformWindows STATIC)

target_sources(
        Client_PlatformWindows
        PUBLIC
        PlatformWindows.cpp
)
target_link_libraries(
        Client_PlatformWindows
        PUBLIC
        mdr
)
