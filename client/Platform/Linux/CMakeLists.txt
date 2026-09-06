cmake_minimum_required(VERSION 3.31)

add_library(Client_PlatformLinux STATIC
        ClientPlatform.cpp
)

target_link_libraries(Client_PlatformLinux
        PUBLIC
        mdr::bt
        PRIVATE
        mdr
)
