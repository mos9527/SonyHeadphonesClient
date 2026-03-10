cmake_minimum_required(VERSION 3.31)
file(
    GLOB_RECURSE INCLUDES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

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

find_package(Clang)
if (NOT Clang_FOUND)
    # Windows LLVM installer doesn't come with FindClang.cmake.
    # As per tooling README, fall back to finding the include dir manually.
    find_path(LLVM_DIR clang-c)
    message(STATUS "LLVM_DIR=${LLVM_DIR}")
    if (NOT LLVM_DIR STREQUAL "LLVM_DIR-NOTFOUND")
        link_directories("${LLVM_DIR}/../lib")
    endif ()
endif ()

# Expose detection result to parent scope via a cache variable.
if (Clang_FOUND OR (LLVM_DIR AND NOT LLVM_DIR STREQUAL "LLVM_DIR-NOTFOUND"))
    set(MDR_LLVM_FOUND TRUE CACHE BOOL "LLVM/libclang was found; codegen tools are available" FORCE)
else ()
    set(MDR_LLVM_FOUND FALSE CACHE BOOL "LLVM/libclang was found; codegen tools are available" FORCE)
endif ()

include(FetchContent)
FetchContent_MakeAvailable(fmt)

# Codegen tools — only built when LLVM is available.
if (MDR_LLVM_FOUND)
    add_library(tooling_CodegenCommon INTERFACE)
    target_link_libraries(tooling_CodegenCommon INTERFACE fmt::fmt libclang)
    if (Clang_FOUND)
        target_include_directories(tooling_CodegenCommon INTERFACE ${CLANG_INCLUDE_DIRS})
    else ()
        target_include_directories(tooling_CodegenCommon INTERFACE ${LLVM_DIR})
    endif ()

    add_executable(tooling_EnumCodegen EnumCodegen.cpp)
    target_link_libraries(tooling_EnumCodegen PRIVATE tooling_CodegenCommon)

    add_executable(tooling_SerializationCodegen SerializationCodegen.cpp)
    target_link_libraries(tooling_SerializationCodegen PRIVATE tooling_CodegenCommon)

    add_executable(tooling_ValidationCodegen ValidationCodegen.cpp)
    target_link_libraries(tooling_ValidationCodegen PRIVATE tooling_CodegenCommon)

    add_executable(tooling_TraitsCodegen TraitsCodegen.cpp)
    target_link_libraries(tooling_TraitsCodegen PRIVATE tooling_CodegenCommon)
endif ()

add_executable(tooling_BinaryEmbed BinaryEmbed.cpp)
