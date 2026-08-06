cmake_minimum_required(VERSION 3.31)

set(_system_libs ws2_32.lib rpcrt4.lib Bthprops.lib windowsapp.lib)

add_library(Client_PlatformWindows STATIC
        ClientPlatform.cpp
        ConnectionWindows.cpp
)

if (MDR_BLE)
    target_sources(Client_PlatformWindows PRIVATE ConnectionWindowsBLE.cpp)
endif ()

target_link_libraries(Client_PlatformWindows
        PRIVATE
        mdr
        ${_system_libs}
)
