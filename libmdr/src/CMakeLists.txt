cmake_minimum_required(VERSION 3.31)

set(_INC "${CMAKE_CURRENT_SOURCE_DIR}/../include/mdr")
set(_GEN_INC "${_INC}/Generated")
set(_GEN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/Generated")

set(MDR_CORE_SOURCES
    Command.cpp
    HeadphonesV1.cpp
    HeadphonesV1T1.cpp
    HeadphonesV1T2.cpp
    HeadphonesV2.cpp
    HeadphonesV2T1.cpp
    HeadphonesV2T2.cpp
    ProtocolV1T1CustomSerialization.cpp
    ProtocolV1T2CustomSerialization.cpp
    ProtocolV2T1CustomSerialization.cpp
    ProtocolV2T2CustomSerialization.cpp
    Details.hpp
    DetailsV1.hpp
    DetailsV2.hpp
    Property.hpp
)

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

add_library(mdr_core OBJECT ${MDR_CORE_SOURCES} ${GENERATED_SRCS})
set_target_properties(mdr_core PROPERTIES
    POSITION_INDEPENDENT_CODE ${MDR_BUILD_SHARED}
    CXX_SCAN_FOR_MODULES OFF
)
target_link_libraries(mdr_core PRIVATE mdr_Includes)

add_library(mdr STATIC Headphones.cpp $<TARGET_OBJECTS:mdr_core>)
target_link_libraries(mdr PUBLIC mdr_Includes)
target_include_directories(mdr PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

if (MDR_BUILD_SHARED)
    add_library(mdr_shared SHARED Headphones.cpp $<TARGET_OBJECTS:mdr_core>)
    set_target_properties(mdr_shared PROPERTIES
        OUTPUT_NAME "mdr-shared"
    )
    target_compile_definitions(mdr_shared
        PRIVATE MDR_BUILDING_SHARED
        INTERFACE MDR_USE_SHARED
    )

    # The ABI tests provide their own MDRConnection and intentionally exercise the
    # public C surface through a DLL/shared object, without platform backends.
    target_link_libraries(mdr_shared
        PUBLIC mdr-c
        PRIVATE mdr_Includes
    )
    target_include_directories(mdr_shared PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
endif ()

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
    add_dependencies(mdr_core mdr_codegen_headers)    

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

    add_custom_target(mdr_codegen_sources DEPENDS ${GENERATED_SRCS})
    add_dependencies(mdr_core mdr_codegen_sources)
endif ()
