cmake_minimum_required(VERSION 3.31)

add_library(Client_PlatformIOS STATIC
        ../MacOS/ClientPlatform.mm
        ClientConnection.cpp
        ClientPlatform.mm
)
set_target_properties(Client_PlatformIOS PROPERTIES OBJCXX_STANDARD 20 OBJCXX_STANDARD_REQUIRED ON)
set_source_files_properties(ClientPlatform.mm ../MacOS/ClientPlatform.mm PROPERTIES COMPILE_OPTIONS "-fobjc-arc")

target_link_libraries(Client_PlatformIOS
        PUBLIC
        mdr::bt
        PRIVATE
        mdr
        SDL3::SDL3-static
        "-framework CoreText"
        "-framework CoreFoundation"
        "-framework Foundation"
        "-framework QuartzCore"
        "-framework UIKit"
        "-framework GameController"
)
