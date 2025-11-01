# Sets MDR_GIT_COMMIT_HASH and MDR_GIT_BRANCH_NAME with information from Git
# Written by Gemini 2.5 Pro
function(MDR_DetectGitInfo)
    # Find the Git executable
    find_package(Git QUIET)

    if(GIT_FOUND)
        # Get the short commit hash
        execute_process(
                COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE MDR_GIT_COMMIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE
                RESULT_VARIABLE GIT_HASH_RESULT
        )

        # Get the current branch name
        execute_process(
                COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE MDR_GIT_BRANCH_NAME
                OUTPUT_STRIP_TRAILING_WHITESPACE
                RESULT_VARIABLE GIT_BRANCH_RESULT
        )

        # Check if the commands succeeded
        if(NOT GIT_HASH_RESULT EQUAL 0)
            set(MDR_GIT_COMMIT_HASH "unknown-hash")
        endif()
        if(NOT GIT_BRANCH_RESULT EQUAL 0)
            set(MDR_GIT_BRANCH_NAME "unknown-branch")
        endif()

    else()
        # Fallback if Git isn't found
        message(STATUS "Git not found. Using 'unknown' for version info.")
        set(MDR_GIT_COMMIT_HASH "unknown")
        set(MDR_GIT_BRANCH_NAME "unknown")
    endif()
    set(MDR_GIT_COMMIT_HASH "${MDR_GIT_COMMIT_HASH}" PARENT_SCOPE)
    set(MDR_GIT_BRANCH_NAME "${MDR_GIT_BRANCH_NAME}" PARENT_SCOPE)
endfunction()