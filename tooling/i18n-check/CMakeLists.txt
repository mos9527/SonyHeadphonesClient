cmake_minimum_required(VERSION 3.20)
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(I18NCheck LANGUAGES CXX)
endif ()

set(I18N_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../client/I18N")
if (NOT TARGET Client_I18N)
    add_subdirectory("${I18N_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/Client_I18N")
endif ()

# Auto-gen I18NTextNames.inc for enum TextId lookup
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${I18N_DIR}/Default.cpp")
file(STRINGS "${I18N_DIR}/Default.cpp" default_definitions
    REGEX "^[ \t]*_Def\\([A-Za-z_][A-Za-z0-9_]*,")
set(text_names "")
foreach (definition IN LISTS default_definitions)
    string(REGEX REPLACE "^[ \t]*_Def\\(([A-Za-z_][A-Za-z0-9_]*),.*" "\\1" text_id "${definition}")
    string(APPEND text_names "    case i18n::TextId::${text_id}: return \"${text_id}\";\n")
endforeach ()
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/I18NTextNames.inc" "${text_names}")

add_executable(tooling_I18NCheck I18NCheck.cpp)
target_link_libraries(tooling_I18NCheck PRIVATE Client_I18N)
target_include_directories(tooling_I18NCheck PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
if (MSVC)
    target_compile_options(tooling_I18NCheck PRIVATE /utf-8)
endif ()

add_custom_target(check_i18n
    COMMAND tooling_I18NCheck
    DEPENDS tooling_I18NCheck
    USES_TERMINAL
    VERBATIM
)
