cmake_minimum_required(VERSION 3.26)

add_library(Client_PlatformMac STATIC)

target_sources(
        Client_PlatformMac
        PUBLIC
        PlatformMac.mm
)

target_link_libraries(
        Client_PlatformMac
        PUBLIC
        mdr
        "-framework AppKit"
)
