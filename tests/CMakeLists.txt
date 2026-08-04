cmake_minimum_required(VERSION 3.31)

add_executable(mdr_replay_tests Replay.cpp)
target_link_libraries(mdr_replay_tests PRIVATE mdr_shared)

add_executable(mdr_abi_tests ABI.c)
target_link_libraries(mdr_abi_tests PRIVATE mdr_shared)

if (WIN32)
    foreach(test_target IN ITEMS mdr_replay_tests mdr_abi_tests)
        add_custom_command(
            TARGET ${test_target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    $<TARGET_FILE:mdr_shared>
                    $<TARGET_FILE_DIR:${test_target}>
        )
    endforeach()
endif()
