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
    if (MDR_BLE)
        list(APPEND SRCS
            Windows/ConnectionWindowsBLE.cpp)
    endif ()
elseif (APPLE)
    list(APPEND SRCS
        MacOS/ConnectionMacOS.mm)
    if (MDR_BLE)
        find_library(FOUNDATION_LIB Foundation REQUIRED)
        find_library(IOBLUETOOTH_LIB IOBluetooth REQUIRED)
    endif ()
elseif (MDR_PLATFORM_OS STREQUAL "EMSCRIPTEN")
    list(APPEND SRCS
        Emscripten/ConnectionEmscripten.cpp)
endif ()

add_library(mdr-bt STATIC ${SRCS})
add_library(mdr::bt ALIAS mdr-bt)

target_include_directories(mdr-bt
    PUBLIC
        ${CMAKE_SOURCE_DIR}/libmdr-bt/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(mdr-bt
    PUBLIC  mdr-c
    PRIVATE mdr
)

add_library(mdr-bt_shared SHARED ${SRCS})
add_library(mdr::bt_shared ALIAS mdr-bt_shared)

target_include_directories(mdr-bt_shared
    PUBLIC
        ${CMAKE_SOURCE_DIR}/libmdr-bt/include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(mdr-bt_shared
    PUBLIC  mdr-c
    PRIVATE mdr
)

target_compile_definitions(mdr-bt_shared
    PRIVATE MDR_BUILDING_SHARED
    INTERFACE MDR_USING_SHARED
)

if (WIN32)
    target_link_libraries(mdr-bt PRIVATE ws2_32 rpcrt4)
    target_link_libraries(mdr-bt_shared PRIVATE ws2_32 rpcrt4)
    if (MDR_BLE)
        target_link_libraries(mdr-bt PRIVATE Bthprops windowsapp)
        target_link_libraries(mdr-bt_shared PRIVATE Bthprops windowsapp)
    endif ()
elseif (MDR_PLATFORM_OS STREQUAL "LINUX")
    target_include_directories(mdr-bt PRIVATE ${DBus_INCLUDE_DIRS})
    target_include_directories(mdr-bt_shared PRIVATE ${DBus_INCLUDE_DIRS})
    target_link_libraries(mdr-bt PRIVATE  ${DBus_LIBRARIES} bluetooth)
    target_link_libraries(mdr-bt_shared PRIVATE  ${DBus_LIBRARIES} bluetooth)
elseif (APPLE)
    if (MDR_BLE)
        target_link_libraries(mdr-bt PRIVATE ${FOUNDATION_LIB} ${IOBLUETOOTH_LIB})
        target_link_libraries(mdr-bt_shared PRIVATE ${FOUNDATION_LIB} ${IOBLUETOOTH_LIB})
    endif ()
elseif (MDR_PLATFORM_OS STREQUAL "EMSCRIPTEN")
    target_link_options(mdr-bt PRIVATE -sALLOW_MEMORY_GROWTH)
    target_link_options(mdr-bt_shared PRIVATE -sALLOW_MEMORY_GROWTH)
endif ()
