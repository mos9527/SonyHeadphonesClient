set(FETCHCONTENT_QUIET FALSE)
include(FetchContent)
find_package(Git REQUIRED)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 12.1.0
        GIT_SHALLOW TRUE
)

if (MDR_PLATFORM_OS STREQUAL "OHOS")
    FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL
            GIT_TAG a24429c192cfee5ae92697f12f219cad27635fb4
    )
else ()
    FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL
            GIT_TAG release-3.2.26
            GIT_SHALLOW TRUE
            PATCH_COMMAND
                "${GIT_EXECUTABLE}" reset --hard
                COMMAND "${GIT_EXECUTABLE}" apply "${CMAKE_CURRENT_SOURCE_DIR}/SDL-disable-dynapi.patch"
    )
endif ()

FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.92.7
        GIT_SHALLOW TRUE
)

