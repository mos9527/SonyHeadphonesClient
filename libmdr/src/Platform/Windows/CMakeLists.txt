cmake_minimum_required(VERSION 3.31)

set(_system_libs ws2_32.lib rpcrt4.lib Bthprops.lib windowsapp.lib)

add_library(mdr_PlatformWindows PlatformWindows.cpp)
target_link_libraries(mdr_PlatformWindows PRIVATE mdr-c mdr ${_system_libs})

add_library(mdr_PlatformWindowsBLE PlatformWindowsBLE.cpp)
target_link_libraries(mdr_PlatformWindowsBLE PRIVATE mdr-c mdr ${_system_libs})

set(_shared_sources "${CMAKE_CURRENT_SOURCE_DIR}/PlatformWindows.cpp")
if (MDR_BLE)
    list(APPEND _shared_sources "${CMAKE_CURRENT_SOURCE_DIR}/PlatformWindowsBLE.cpp")
endif ()
set(MDR_PLATFORM_SHARED_SOURCES ${_shared_sources} PARENT_SCOPE)
set(MDR_PLATFORM_SHARED_LIBS ${_system_libs} PARENT_SCOPE)
