# Codegen.cmake
# Provides mdr_add_codegen() when LLVM is available.
# If codegen is explicitly enabled (MDR_ENABLE_CODEGEN=ON) but LLVM was not
# found at configure time, a fatal error is raised. If codegen is disabled,
# the function is simply not defined — callers guard on (COMMAND mdr_add_codegen).
#
# Usage:
#   mdr_add_codegen(
#       OUTPUT    <output-file>    # absolute path to the file to generate
#       TOOL      <cmake-target>   # tooling target (e.g. tooling_EnumCodegen)
#       INPUT     <source-header>  # absolute path to input header
#       NAMESPACE <ns>             # namespace argument
#       [INCLUDE  <include-name>]  # optional include argument (for Serialization/Validation)
#   )

set(MDR_CODEGEN_RUNNER "${CMAKE_CURRENT_LIST_DIR}/RunCodegen.cmake")

if (NOT MDR_ENABLE_CODEGEN)
    message(STATUS "Codegen disabled (MDR_ENABLE_CODEGEN=OFF).")
    return()
endif ()

if (NOT MDR_LLVM_FOUND)
    message(FATAL_ERROR
        "LLVM/libclang not found — codegen was explicitly enabled "
        "(MDR_ENABLE_CODEGEN=ON) but required dependencies are missing. "
        "Install LLVM/libclang or set MDR_ENABLE_CODEGEN=OFF. "
        "See tooling/README.md for setup instructions.")
endif ()

function(mdr_add_codegen)
    cmake_parse_arguments(ARG "" "OUTPUT;TOOL;INPUT;NAMESPACE;INCLUDE" "" ${ARGN})

    if (NOT ARG_OUTPUT OR NOT ARG_TOOL OR NOT ARG_INPUT OR NOT ARG_NAMESPACE)
        message(FATAL_ERROR "mdr_add_codegen: OUTPUT, TOOL, INPUT, and NAMESPACE are required")
    endif()

    set(_cmake_args
        -DTOOL=$<TARGET_FILE:${ARG_TOOL}>
        -DARG1=${ARG_INPUT}
        -DARG2=${ARG_NAMESPACE}
        -DOUTPUT_FILE=${ARG_OUTPUT}
    )
    if(ARG_INCLUDE)
        list(APPEND _cmake_args -DARG3=${ARG_INCLUDE})
    endif()

    add_custom_command(
        OUTPUT  "${ARG_OUTPUT}"
        COMMAND "${CMAKE_COMMAND}" ${_cmake_args} -P "${MDR_CODEGEN_RUNNER}"
        DEPENDS "${ARG_TOOL}" "${ARG_INPUT}" "${MDR_CODEGEN_RUNNER}"
        COMMENT "Codegen: ${ARG_TOOL} -> ${ARG_OUTPUT} (${_cmake_args})"
        CODEGEN
        VERBATIM
    )
endfunction()
