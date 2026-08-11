cmake_minimum_required(VERSION 3.31)

add_executable(mdr_replay_tests Replay.cpp)
target_link_libraries(mdr_replay_tests PRIVATE mdr_shared)

add_executable(mdr_abi_tests ABI.c)
set_target_properties(mdr_abi_tests PROPERTIES C_STANDARD 11 C_STANDARD_REQUIRED ON)
target_link_libraries(mdr_abi_tests PRIVATE mdr_shared)

add_executable(mdr_framing_tests Framing.cpp)
target_link_libraries(mdr_framing_tests PRIVATE mdr_shared)
add_test(NAME mdr-framing COMMAND mdr_framing_tests)

set(mdr_test_targets mdr_replay_tests mdr_abi_tests mdr_framing_tests)

if (MDR_CLIENT_DEBUGGER_TARGET)
    add_executable(mdr_debugger_tests Debugger.cpp)
    target_link_libraries(mdr_debugger_tests PRIVATE Client_Debugger)
    list(APPEND mdr_test_targets mdr_debugger_tests)
    add_test(NAME mdr-debugger COMMAND mdr_debugger_tests)
endif ()

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
