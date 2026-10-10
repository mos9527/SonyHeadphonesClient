cmake_minimum_required(VERSION 3.31)

add_library(Client_PlatformMacOS STATIC
        ClientPlatform.cpp
)

find_library(CORETEXT_LIB CoreText REQUIRED)
find_library(COREFOUNDATION_LIB CoreFoundation REQUIRED)

target_link_libraries(Client_PlatformMacOS
        PUBLIC
        mdr::bt
        PRIVATE
        mdr
        ${CORETEXT_LIB}
        ${COREFOUNDATION_LIB}
)
