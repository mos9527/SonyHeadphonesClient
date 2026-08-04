cmake_minimum_required(VERSION 3.31)

set(_INC "${CMAKE_SOURCE_DIR}/libmdr/include/mdr")
set(_GEN_INC "${_INC}/Generated")
set(_GEN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/Generated")

file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

include(FetchContent)
FetchContent_MakeAvailable(fmt)

set(GENERATED_SRCS
    "${_GEN_SRC}/ProtocolV1T1Serialization.cpp"
    "${_GEN_SRC}/ProtocolV1T1Validation.cpp"
    "${_GEN_SRC}/ProtocolV1T2Serialization.cpp"
    "${_GEN_SRC}/ProtocolV1T2Validation.cpp"
    "${_GEN_SRC}/ProtocolV2T1Serialization.cpp"
    "${_GEN_SRC}/ProtocolV2T1Validation.cpp"
    "${_GEN_SRC}/ProtocolV2T2Serialization.cpp"
    "${_GEN_SRC}/ProtocolV2T2Validation.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${INCLUDES} ${GENERATED_SRCS})
add_library(mdr ${SOURCES} ${INCLUDES} ${GENERATED_SRCS})
add_library(mdr_shared SHARED ${SOURCES} ${INCLUDES} ${GENERATED_SRCS})
set_target_properties(mdr_shared PROPERTIES
    OUTPUT_NAME "mdr-shared"
    WINDOWS_EXPORT_ALL_SYMBOLS ON
)
add_subdirectory(Platform)

target_link_libraries(mdr PRIVATE mdr_Platform)
target_link_libraries(mdr PUBLIC mdr_Includes fmt::fmt)
target_include_directories(mdr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# The ABI tests provide their own MDRConnection and intentionally exercise the
# public C surface through a DLL/shared object, without platform backends.
target_link_libraries(mdr_shared
    PUBLIC mdr-c
    PRIVATE mdr_Includes fmt::fmt
)
target_include_directories(mdr_shared PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

if (COMMAND mdr_add_codegen)
    mdr_add_codegen(OUTPUT "${_GEN_INC}/Command.hpp"
        TOOL      tooling_EnumCodegen
        INPUT     "${_INC}/Command.hpp"
        NAMESPACE mdr)

    mdr_add_codegen(OUTPUT "${_GEN_INC}/ProtocolV2Enum.hpp"
        TOOL      tooling_EnumCodegen
        INPUT     "${_INC}/ProtocolV2.hpp"
        NAMESPACE "mdr::v2")

    mdr_add_codegen(OUTPUT "${_GEN_INC}/ProtocolV1Enum.hpp"
        TOOL      tooling_EnumCodegen
        INPUT     "${_INC}/ProtocolV1.hpp"
        NAMESPACE "mdr::v1")

    mdr_add_codegen(OUTPUT "${_GEN_INC}/ProtocolV1T1Enum.hpp"
        TOOL      tooling_EnumCodegen
        INPUT     "${_INC}/ProtocolV1T1.hpp"
        NAMESPACE "mdr::v1::t1")

    mdr_add_codegen(OUTPUT "${_GEN_INC}/ProtocolV1T2Enum.hpp"
        TOOL      tooling_EnumCodegen
        INPUT     "${_INC}/ProtocolV1T2.hpp"
        NAMESPACE "mdr::v1::t2")

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

    mdr_add_codegen(OUTPUT "${_GEN_INC}/ProtocolV1T1Traits.hpp"
        TOOL      tooling_TraitsCodegen
        INPUT     "${_INC}/ProtocolV1T1.hpp"
        NAMESPACE "mdr::v1::t1")

    mdr_add_codegen(OUTPUT "${_GEN_INC}/ProtocolV1T2Traits.hpp"
        TOOL      tooling_TraitsCodegen
        INPUT     "${_INC}/ProtocolV1T2.hpp"
        NAMESPACE "mdr::v1::t2")

    mdr_add_codegen(OUTPUT "${_GEN_INC}/ProtocolV2T2Traits.hpp"
        TOOL      tooling_TraitsCodegen
        INPUT     "${_INC}/ProtocolV2T2.hpp"
        NAMESPACE "mdr::v2::t2")
    
    add_custom_target(mdr_codegen_headers DEPENDS
        "${_GEN_INC}/Command.hpp"
        "${_GEN_INC}/ProtocolV1Enum.hpp"
        "${_GEN_INC}/ProtocolV1T1Enum.hpp"
        "${_GEN_INC}/ProtocolV1T2Enum.hpp"
        "${_GEN_INC}/ProtocolV1T1Traits.hpp"
        "${_GEN_INC}/ProtocolV1T2Traits.hpp"
        "${_GEN_INC}/ProtocolV2Enum.hpp"
        "${_GEN_INC}/ProtocolV2T1Enum.hpp"
        "${_GEN_INC}/ProtocolV2T2Enum.hpp"
        "${_GEN_INC}/ProtocolV2T1Traits.hpp"
        "${_GEN_INC}/ProtocolV2T2Traits.hpp"
    )
    add_dependencies(mdr mdr_codegen_headers)
    add_dependencies(mdr_shared mdr_codegen_headers)

    mdr_add_codegen(OUTPUT "${_GEN_SRC}/ProtocolV1T1Serialization.cpp"
        TOOL      tooling_SerializationCodegen
        INPUT     "${_INC}/ProtocolV1T1.hpp"
        NAMESPACE "mdr::v1::t1"
        INCLUDE   "mdr/ProtocolV1T1.hpp")

    mdr_add_codegen(OUTPUT "${_GEN_SRC}/ProtocolV1T2Serialization.cpp"
        TOOL      tooling_SerializationCodegen
        INPUT     "${_INC}/ProtocolV1T2.hpp"
        NAMESPACE "mdr::v1::t2"
        INCLUDE   "mdr/ProtocolV1T2.hpp")

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

    mdr_add_codegen(OUTPUT "${_GEN_SRC}/ProtocolV1T1Validation.cpp"
        TOOL      tooling_ValidationCodegen
        INPUT     "${_INC}/ProtocolV1T1.hpp"
        NAMESPACE "mdr::v1::t1"
        INCLUDE   "mdr/ProtocolV1T1.hpp")

    mdr_add_codegen(OUTPUT "${_GEN_SRC}/ProtocolV1T2Validation.cpp"
        TOOL      tooling_ValidationCodegen
        INPUT     "${_INC}/ProtocolV1T2.hpp"
        NAMESPACE "mdr::v1::t2"
        INCLUDE   "mdr/ProtocolV1T2.hpp")

    mdr_add_codegen(OUTPUT "${_GEN_SRC}/ProtocolV2T2Validation.cpp"
        TOOL      tooling_ValidationCodegen
        INPUT     "${_INC}/ProtocolV2T2.hpp"
        NAMESPACE "mdr::v2::t2"
        INCLUDE   "mdr/ProtocolV2T2.hpp")
endif ()
