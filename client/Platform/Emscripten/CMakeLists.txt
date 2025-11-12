file(
        GLOB
        STATIC_FILES
        "${CMAKE_CURRENT_LIST_DIR}/*.html"
        "${CMAKE_CURRENT_LIST_DIR}/*.css"
        "${CMAKE_CURRENT_LIST_DIR}/*.json"
        "${CMAKE_CURRENT_LIST_DIR}/*.js"
        "${CMAKE_CURRENT_LIST_DIR}/*.png"
        "${CMAKE_CURRENT_LIST_DIR}/*.ttf"
)

add_library(Client_PlatformEmscripten)
foreach(FILE IN LISTS STATIC_FILES)
    get_filename_component(FILENAME "${FILE}" NAME_WE)
    set(OUTPUT_FILE "${CLIENT_BINARY_DIR}/${FILENAME}")
    add_custom_command(
            OUTPUT "${OUTPUT_FILE}"
            COMMAND ${CMAKE_COMMAND} -E copy
            "${FILE}"
            "${CLIENT_BINARY_DIR}"
            COMMENT "Copy static content ${FILE} -> ${CLIENT_BINARY_DIR}"
            DEPENDS "${FILE}"
    )
    add_custom_target("Static_${FILENAME}" ALL DEPENDS ${OUTPUT_FILE})
    add_dependencies(Client_PlatformEmscripten "Static_${FILENAME}")
endforeach()

target_sources(
        Client_PlatformEmscripten PRIVATE
        PlatformEmscripten.cpp
)
target_link_libraries(
        Client_PlatformEmscripten
        PUBLIC
        mdr
)