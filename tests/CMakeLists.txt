cmake_minimum_required(VERSION 3.31)

add_executable(mdr_replay_tests Replay.cpp)
target_link_libraries(mdr_replay_tests PRIVATE mdr_shared)

add_executable(mdr_abi_tests ABI.c)
target_link_libraries(mdr_abi_tests PRIVATE mdr_shared)

set(mdr_test_targets mdr_replay_tests mdr_abi_tests)
add_custom_target(mdr_tests DEPENDS ${mdr_test_targets})

foreach(test_target IN LISTS mdr_test_targets)
    add_custom_command(
        TARGET ${test_target}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:mdr_shared>
                $<TARGET_FILE_DIR:${test_target}>
    )

    if (APPLE)
        set_target_properties(${test_target} PROPERTIES BUILD_RPATH "@loader_path")
    elseif (UNIX)
        set_target_properties(${test_target} PROPERTIES BUILD_RPATH "$ORIGIN")
    endif()
endforeach()
