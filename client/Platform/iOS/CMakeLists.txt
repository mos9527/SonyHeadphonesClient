cmake_minimum_required(VERSION 3.31)

add_library(Client_PlatformIOS STATIC
        ../MacOS/ClientPlatform.cpp
        ClientConnection.cpp
)

target_link_libraries(Client_PlatformIOS
        PUBLIC
        mdr::bt
        PRIVATE
        mdr
        SDL3::SDL3-static
        "-framework CoreText"
        "-framework CoreFoundation"
        "-framework GameController"
)
