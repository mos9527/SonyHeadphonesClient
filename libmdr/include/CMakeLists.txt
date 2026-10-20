cmake_minimum_required(VERSION 3.31)
add_library(mdr-c INTERFACE
    mdr-c/Base.h
    mdr-c/Connection.h
    mdr-c/Headphones.h
)

target_include_directories(
    mdr-c
    INTERFACE
    .
)

add_library(mdr_Includes INTERFACE
    mdr/Command.hpp
    mdr/Protocol.hpp
    mdr/ProtocolV1.hpp
    mdr/ProtocolV1T1.hpp
    mdr/ProtocolV1T2.hpp
    mdr/ProtocolV2.hpp
    mdr/ProtocolV2T1.hpp
    mdr/ProtocolV2T2.hpp
    mdr/Result.hpp
    mdr/Generated/Command.hpp
    mdr/Generated/ProtocolV1Enum.hpp
    mdr/Generated/ProtocolV1T1Enum.hpp
    mdr/Generated/ProtocolV1T1Traits.hpp
    mdr/Generated/ProtocolV1T2Enum.hpp
    mdr/Generated/ProtocolV1T2Traits.hpp
    mdr/Generated/ProtocolV2Enum.hpp
    mdr/Generated/ProtocolV2T1Enum.hpp
    mdr/Generated/ProtocolV2T1Traits.hpp
    mdr/Generated/ProtocolV2T2Enum.hpp
    mdr/Generated/ProtocolV2T2Traits.hpp
)
target_link_libraries(mdr_Includes INTERFACE mdr-c fmt::fmt)
target_include_directories(
        mdr_Includes
        INTERFACE
        .
)