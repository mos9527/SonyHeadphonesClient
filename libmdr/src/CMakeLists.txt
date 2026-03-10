cmake_minimum_required(VERSION 3.31)

set(_INC "${CMAKE_SOURCE_DIR}/libmdr/include/mdr")
set(_GEN_INC "${_INC}/Generated")
set(_GEN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/Generated")

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

include(FetchContent)
FetchContent_MakeAvailable(fmt)

# Generated .cpp files are listed explicitly so CMake tracks them as outputs
# of the codegen rules below, rather than relying on GLOB at configure time.
set(GENERATED_SRCS
    "${_GEN_SRC}/ProtocolV2T1Serialization.cpp"
    "${_GEN_SRC}/ProtocolV2T1Validation.cpp"
    "${_GEN_SRC}/ProtocolV2T2Serialization.cpp"
    "${_GEN_SRC}/ProtocolV2T2Validation.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${INCLUDES} ${GENERATED_SRCS})
add_library(mdr ${SOURCES} ${INCLUDES} ${GENERATED_SRCS})

add_subdirectory(Platform)

target_link_libraries(mdr PRIVATE mdr_Platform)
target_link_libraries(mdr PUBLIC mdr_Includes fmt::fmt)
target_include_directories(mdr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Codegen rules — only active when LLVM was found at configure time.
# mdr_add_codegen is defined in tooling/Codegen.cmake; it is absent (with a warning) otherwise.
if (COMMAND mdr_add_codegen)
    # --- Generated headers (included by public headers in libmdr/include/mdr/) ---
    mdr_add_codegen(OUTPUT "${_GEN_INC}/Command.hpp"
        TOOL      tooling_EnumCodegen
        INPUT     "${_INC}/Command.hpp"
        NAMESPACE mdr)

    mdr_add_codegen(OUTPUT "${_GEN_INC}/ProtocolV2Enum.hpp"
        TOOL      tooling_EnumCodegen
        INPUT     "${_INC}/ProtocolV2.hpp"
        NAMESPACE "mdr::v2")

    mdr_add_codegen(OUTPUT "${_GEN_INC}/ProtocolV2T1Enum.hpp"
        TOOL      tooling_EnumCodegen
        INPUT     "${_INC}/ProtocolV2T1.hpp"
        NAMESPACE "mdr::v2::t1")

    mdr_add_codegen(OUTPUT "${_GEN_INC}/ProtocolV2T2Enum.hpp"
        TOOL      tooling_EnumCodegen
        INPUT     "${_INC}/ProtocolV2T2.hpp"
        NAMESPACE "mdr::v2::t2")

    mdr_add_codegen(OUTPUT "${_GEN_INC}/ProtocolV2T1Traits.hpp"
        TOOL      tooling_TraitsCodegen
        INPUT     "${_INC}/ProtocolV2T1.hpp"
        NAMESPACE "mdr::v2::t1")

    mdr_add_codegen(OUTPUT "${_GEN_INC}/ProtocolV2T2Traits.hpp"
        TOOL      tooling_TraitsCodegen
        INPUT     "${_INC}/ProtocolV2T2.hpp"
        NAMESPACE "mdr::v2::t2")

    # Generated headers aren't compiled directly, but must exist before mdr
    # compiles any .cpp that transitively includes them.
    add_custom_target(mdr_codegen_headers DEPENDS
        "${_GEN_INC}/Command.hpp"
        "${_GEN_INC}/ProtocolV2Enum.hpp"
        "${_GEN_INC}/ProtocolV2T1Enum.hpp"
        "${_GEN_INC}/ProtocolV2T2Enum.hpp"
        "${_GEN_INC}/ProtocolV2T1Traits.hpp"
        "${_GEN_INC}/ProtocolV2T2Traits.hpp"
    )
    add_dependencies(mdr mdr_codegen_headers)

    # --- Generated .cpp sources ---
    mdr_add_codegen(OUTPUT "${_GEN_SRC}/ProtocolV2T1Serialization.cpp"
        TOOL      tooling_SerializationCodegen
        INPUT     "${_INC}/ProtocolV2T1.hpp"
        NAMESPACE "mdr::v2::t1"
        INCLUDE   "mdr/ProtocolV2T1.hpp")

    mdr_add_codegen(OUTPUT "${_GEN_SRC}/ProtocolV2T2Serialization.cpp"
        TOOL      tooling_SerializationCodegen
        INPUT     "${_INC}/ProtocolV2T2.hpp"
        NAMESPACE "mdr::v2::t2"
        INCLUDE   "mdr/ProtocolV2T2.hpp")

    mdr_add_codegen(OUTPUT "${_GEN_SRC}/ProtocolV2T1Validation.cpp"
        TOOL      tooling_ValidationCodegen
        INPUT     "${_INC}/ProtocolV2T1.hpp"
        NAMESPACE "mdr::v2::t1"
        INCLUDE   "mdr/ProtocolV2T1.hpp")

    mdr_add_codegen(OUTPUT "${_GEN_SRC}/ProtocolV2T2Validation.cpp"
        TOOL      tooling_ValidationCodegen
        INPUT     "${_INC}/ProtocolV2T2.hpp"
        NAMESPACE "mdr::v2::t2"
        INCLUDE   "mdr/ProtocolV2T2.hpp")
endif ()
