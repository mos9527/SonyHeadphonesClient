set(FETCHCONTENT_QUIET FALSE)
include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 12.1.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)

FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG release-3.2.26
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.92.4
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)

FetchContent_Declare(
        material_color_utilities
        GIT_REPOSITORY https://github.com/material-foundation/material-color-utilities.git
        GIT_TAG 1a34bd2d696a5dd669beca679546ad72209d0937
        GIT_PROGRESS TRUE
        PATCH_COMMAND ${CMAKE_COMMAND} -E touch <SOURCE_DIR>/CMakeLists.txt
)
