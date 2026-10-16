set(SRCS
    Utils.hpp
)

if (MDR_PLATFORM_OS STREQUAL "LINUX")
    list(APPEND SRCS
        Linux/ConnectionLinux.cpp
        Linux/DBusHelper.cpp)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Linux")
    # Classic Linux connection uses DBus (always, not just for BLE).
    find_package(DBus REQUIRED)
elseif (WIN32)
    list(APPEND SRCS
        Windows/ConnectionWindows.cpp)
    if (MDR_BT_BLE_SUPPORT)
        list(APPEND SRCS
            Windows/ConnectionWindowsBLE.cpp)
    endif ()
elseif (MDR_PLATFORM_OS STREQUAL "IOS")
    set(CMAKE_OBJCXX_STANDARD 20)
    list(APPEND SRCS
        iOS/ConnectionIOS.mm)
    set_source_files_properties(iOS/ConnectionIOS.mm PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
elseif (APPLE)
    set(CMAKE_OBJCXX_STANDARD 20)
    list(APPEND SRCS
        MacOS/ConnectionMacOS.mm)
    set_source_files_properties(MacOS/ConnectionMacOS.mm PROPERTIES COMPILE_OPTIONS "-fobjc-arc")
    find_library(FOUNDATION_LIB Foundation REQUIRED)
    find_library(IOBLUETOOTH_LIB IOBluetooth REQUIRED)
elseif (MDR_PLATFORM_OS STREQUAL "EMSCRIPTEN")
    list(APPEND SRCS
        Emscripten/ConnectionEmscripten.cpp)
endif ()

add_library(mdr-bt STATIC ${SRCS})
add_library(mdr::bt ALIAS mdr-bt)

target_include_directories(mdr-bt
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(mdr-bt
    PUBLIC  mdr-c
    PRIVATE mdr_Includes
)

if (MDR_BUILD_SHARED)
    add_library(mdr-bt_shared SHARED ${SRCS})
    add_library(mdr::bt_shared ALIAS mdr-bt_shared)

    target_include_directories(mdr-bt_shared
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/../include
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(mdr-bt_shared
        PUBLIC  mdr-c
        PRIVATE mdr_Includes
    )

    target_compile_definitions(mdr-bt_shared
        PRIVATE MDR_BUILDING_SHARED
        INTERFACE MDR_USE_SHARED
    )

    set_target_properties(mdr-bt_shared PROPERTIES OUTPUT_NAME "mdr-bt-shared")
endif ()

set(MDR_BT_TARGETS mdr-bt)
if (TARGET mdr-bt_shared)
    list(APPEND MDR_BT_TARGETS mdr-bt_shared)
endif ()
foreach(MDR_BT_TARGET IN LISTS MDR_BT_TARGETS)
    if (WIN32)
        target_link_libraries(${MDR_BT_TARGET} PRIVATE ws2_32 rpcrt4 Bthprops)
        if (MDR_BT_BLE_SUPPORT)
            target_link_libraries(${MDR_BT_TARGET} PRIVATE windowsapp)
        endif ()
    elseif (MDR_PLATFORM_OS STREQUAL "LINUX")
        target_include_directories(${MDR_BT_TARGET} PRIVATE ${DBus_INCLUDE_DIRS})
        target_link_libraries(${MDR_BT_TARGET} PRIVATE ${DBus_LIBRARIES} bluetooth)
    elseif (MDR_PLATFORM_OS STREQUAL "IOS")
        target_link_libraries(${MDR_BT_TARGET} PRIVATE "-framework Foundation" "-framework ExternalAccessory")
    elseif (APPLE)
        target_link_libraries(${MDR_BT_TARGET} PRIVATE ${FOUNDATION_LIB} ${IOBLUETOOTH_LIB})
    elseif (MDR_PLATFORM_OS STREQUAL "EMSCRIPTEN")
        target_link_options(${MDR_BT_TARGET} PRIVATE -sALLOW_MEMORY_GROWTH)
    endif ()
endforeach()
