cmake_minimum_required(VERSION 3.31)

add_library(Client_PlatformMacOS
        ClientPlatform.cpp
        ConnectionMacOS.mm
)

find_library(IOBLUETOOTH_FRAMEWORK IOBluetooth REQUIRED)
find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)

target_link_libraries(Client_PlatformMacOS
        PRIVATE
        mdr
        ${IOBLUETOOTH_FRAMEWORK}
        ${FOUNDATION_FRAMEWORK}
)
