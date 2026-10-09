configure_file(
        "${CMAKE_CURRENT_LIST_DIR}/config.js.in"
        "${CLIENT_BINARY_DIR}/config.js"
        @ONLY
)

file(
        GLOB
        STATIC_FILES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_LIST_DIR}/*.html"
        "${CMAKE_CURRENT_LIST_DIR}/*.css"
        "${CMAKE_CURRENT_LIST_DIR}/*.json"
        "${CMAKE_CURRENT_LIST_DIR}/*.js"
        "${CMAKE_CURRENT_LIST_DIR}/*.png"
)

add_library(Client_PlatformEmscripten)
foreach(FILE IN LISTS STATIC_FILES)
    get_filename_component(FILENAME "${FILE}" NAME)
    string(MAKE_C_IDENTIFIER "${FILENAME}" STATIC_TARGET)
    set(OUTPUT_FILE "${CLIENT_BINARY_DIR}/${FILENAME}")
    add_custom_command(
            OUTPUT "${OUTPUT_FILE}"
            COMMAND ${CMAKE_COMMAND} -E copy
            "${FILE}"
            "${CLIENT_BINARY_DIR}"
            COMMENT "Copy static content ${FILE} -> ${CLIENT_BINARY_DIR}"
            DEPENDS "${FILE}"
    )
    add_custom_target("Static_${STATIC_TARGET}" ALL DEPENDS "${OUTPUT_FILE}")
    add_dependencies(Client_PlatformEmscripten "Static_${STATIC_TARGET}")
endforeach()

target_sources(
        Client_PlatformEmscripten PRIVATE
        ClientPlatform.cpp
)
target_link_libraries(
        Client_PlatformEmscripten
        PUBLIC
        mdr::bt
        PRIVATE
        mdr
)
if (MDR_CLIENT_DEBUGGER_TARGET)
    target_compile_definitions(Client_PlatformEmscripten PRIVATE
        $<${CLIENT_DEBUGGER_ENABLED}:MDR_CLIENT_DEBUGGER>
    )
    target_link_libraries(Client_PlatformEmscripten PRIVATE
        $<${CLIENT_DEBUGGER_ENABLED}:SDL3::SDL3-static>
    )
endif ()
