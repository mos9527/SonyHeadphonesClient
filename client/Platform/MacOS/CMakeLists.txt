cmake_minimum_required(VERSION 3.31)

add_library(Client_PlatformMacOS STATIC
        ClientPlatform.mm
        ClientConnection.cpp
)
set_target_properties(Client_PlatformMacOS PROPERTIES OBJCXX_STANDARD 20 OBJCXX_STANDARD_REQUIRED ON)
set_source_files_properties(ClientPlatform.mm PROPERTIES COMPILE_OPTIONS "-fobjc-arc")

find_library(CORETEXT_LIB CoreText REQUIRED)
find_library(COREFOUNDATION_LIB CoreFoundation REQUIRED)

target_link_libraries(Client_PlatformMacOS
        PUBLIC
        mdr::bt
        PRIVATE
        mdr
        SDL3::SDL3-static
        ${CORETEXT_LIB}
        ${COREFOUNDATION_LIB}
        "-framework QuartzCore"
)
