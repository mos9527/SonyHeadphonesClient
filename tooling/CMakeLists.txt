cmake_minimum_required(VERSION 3.31)
# On macOS, Homebrew's LLVM isn't on the default search path.
# Prepend its cmake dir so find_package(Clang) can locate ClangConfig.cmake.
if (APPLE)
    execute_process(
        COMMAND brew --prefix llvm
        OUTPUT_VARIABLE HOMEBREW_LLVM_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if (HOMEBREW_LLVM_PREFIX)
        list(PREPEND CMAKE_PREFIX_PATH "${HOMEBREW_LLVM_PREFIX}")
        message(STATUS "Homebrew LLVM prefix: ${HOMEBREW_LLVM_PREFIX}")
    endif ()
endif ()

set(_MDR_LLVM_PREFIXES)
foreach(_dir IN ITEMS "${Clang_DIR}" "${LLVM_DIR}")
    if (IS_DIRECTORY "${_dir}")
        if (EXISTS "${_dir}/clang-c/Index.h")
            get_filename_component(_prefix "${_dir}/.." ABSOLUTE)
        else ()
            get_filename_component(_prefix "${_dir}/../../.." ABSOLUTE)
        endif ()
        list(APPEND _MDR_LLVM_PREFIXES "${_prefix}")
    endif ()
endforeach ()
file(GLOB _MDR_LLVM_VERSIONED_PREFIXES LIST_DIRECTORIES true
    "/usr/lib/llvm-*"
    "/usr/local/lib/llvm-*"
    "/opt/llvm-*"
    "/opt/homebrew/opt/llvm*"
    "/usr/local/opt/llvm*"
)
list(SORT _MDR_LLVM_VERSIONED_PREFIXES COMPARE NATURAL ORDER DESCENDING)
list(APPEND _MDR_LLVM_PREFIXES
    ${_MDR_LLVM_VERSIONED_PREFIXES}
    "C:/Program Files/LLVM"
    "C:/Program Files (x86)/LLVM"
)

find_package(Clang QUIET CONFIG
    HINTS ${_MDR_LLVM_PREFIXES}
    PATH_SUFFIXES lib/cmake/clang lib64/cmake/clang share/clang/cmake
)
if (TARGET libclang)
    set(_MDR_LIBCLANG_TARGET libclang)
else ()
    find_path(MDR_CLANG_INCLUDE_DIR clang-c/Index.h
        HINTS ${CLANG_INCLUDE_DIRS} "${LLVM_DIR}" ${_MDR_LLVM_PREFIXES}
        PATH_SUFFIXES include
    )
    if (MDR_CLANG_INCLUDE_DIR)
        find_library(MDR_CLANG_LIBRARY NAMES clang libclang
            HINTS "${MDR_CLANG_INCLUDE_DIR}/../lib" "${MDR_CLANG_INCLUDE_DIR}/../lib64"
            NO_DEFAULT_PATH
        )
        find_library(MDR_CLANG_LIBRARY NAMES clang libclang
            HINTS ${CLANG_LIBRARY_DIRS} ${LLVM_LIBRARY_DIRS} ${_MDR_LLVM_PREFIXES}
            PATH_SUFFIXES lib lib64
        )
        if (MDR_CLANG_LIBRARY)
            add_library(mdr_libclang UNKNOWN IMPORTED)
            set_target_properties(mdr_libclang PROPERTIES
                IMPORTED_LOCATION "${MDR_CLANG_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${MDR_CLANG_INCLUDE_DIR}"
            )
            set(_MDR_LIBCLANG_TARGET mdr_libclang)
            message(STATUS "libclang: ${MDR_CLANG_LIBRARY} (include: ${MDR_CLANG_INCLUDE_DIR})")
        endif ()
    endif ()
endif ()

# Expose detection result to parent scope via a cache variable.
if (_MDR_LIBCLANG_TARGET)
    set(MDR_LLVM_FOUND TRUE CACHE BOOL "LLVM/libclang was found; codegen tools are available" FORCE)
else ()
    set(MDR_LLVM_FOUND FALSE CACHE BOOL "LLVM/libclang was found; codegen tools are available" FORCE)
endif ()

include(FetchContent)
FetchContent_MakeAvailable(fmt)

# Codegen tools — only built when LLVM is available.
if (MDR_LLVM_FOUND)
    add_library(tooling_CodegenCommon INTERFACE)
    target_link_libraries(tooling_CodegenCommon INTERFACE fmt::fmt ${_MDR_LIBCLANG_TARGET})
    if (Clang_FOUND)
        target_include_directories(tooling_CodegenCommon INTERFACE ${CLANG_INCLUDE_DIRS})
    endif ()

    add_executable(tooling_EnumCodegen EnumCodegen.cpp)
    target_link_libraries(tooling_EnumCodegen PRIVATE tooling_CodegenCommon)

    add_executable(tooling_SerializationCodegen SerializationCodegen.cpp)
    target_link_libraries(tooling_SerializationCodegen PRIVATE tooling_CodegenCommon)

    add_executable(tooling_ValidationCodegen ValidationCodegen.cpp)
    target_link_libraries(tooling_ValidationCodegen PRIVATE tooling_CodegenCommon)

    add_executable(tooling_TraitsCodegen TraitsCodegen.cpp)
    target_link_libraries(tooling_TraitsCodegen PRIVATE tooling_CodegenCommon)

    add_executable(tooling_DebuggerCodegen DebuggerCodegen.cpp)
    target_link_libraries(tooling_DebuggerCodegen PRIVATE tooling_CodegenCommon)
endif ()

add_executable(tooling_BinaryEmbed BinaryEmbed.cpp)
