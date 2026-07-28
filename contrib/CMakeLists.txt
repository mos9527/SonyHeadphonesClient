set(FETCHCONTENT_QUIET FALSE)
include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 12.1.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        PATCH_COMMAND  
            ${GIT_EXECUTABLE} reset --hard
            COMMAND ${GIT_EXECUTABLE} clean -fdx
            COMMAND ${GIT_EXECUTABLE} apply ${CMAKE_CURRENT_LIST_DIR}/fmt.patch
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
        GIT_TAG v1.92.7
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)

