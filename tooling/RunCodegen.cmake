# Portable runner for codegen tools.
# Invoked via: cmake -DTOOL=... -DARG1=... [-DARG2=...] [-DARG3=...] -DOUTPUT_FILE=... -P RunCodegen.cmake
#
# Required:
#   TOOL        - absolute path to the codegen executable
#   OUTPUT_FILE - file to write (stdout of TOOL is captured here)
# Optional:
#   ARG1..ARG3  - positional arguments forwarded to TOOL

set(_args)
foreach(_i RANGE 1 3)
    if(DEFINED ARG${_i})
        list(APPEND _args "${ARG${_i}}")
    endif()
endforeach()

execute_process(
    COMMAND "${TOOL}" ${_args}
    OUTPUT_FILE "${OUTPUT_FILE}"
    RESULT_VARIABLE _result
    ERROR_VARIABLE _error
)

if(_result)
    file(REMOVE "${OUTPUT_FILE}")
    message(FATAL_ERROR "Codegen failed (exit ${_result}):\n${_error}")
endif()
