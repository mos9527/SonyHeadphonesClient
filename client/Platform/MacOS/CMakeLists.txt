cmake_minimum_required(VERSION 3.31)

add_library(Client_PlatformMacOS STATIC
        ClientPlatform.cpp
)

target_link_libraries(Client_PlatformMacOS
        PUBLIC
        mdr::bt
        PRIVATE
        mdr
)
