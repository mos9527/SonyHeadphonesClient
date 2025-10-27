#!/bin/bash
# Run this script from libmdr/Generated directory to regenerate all generated files
# Requires tooling to be built first
# TODO: Use CMake to handle these instead
rm *.hpp
rm *.cpp
../../../cmake-build-debug/tooling/tooling_EnumCodegen ../Command.hpp mdr > Command.hpp
../../../cmake-build-debug/tooling/tooling_EnumCodegen ../ProtocolV2.hpp mdr::v2 > ProtocolV2Enum.hpp
../../../cmake-build-debug/tooling/tooling_EnumCodegen ../ProtocolV2T1.hpp mdr::v2::t1 > ProtocolV2T1Enum.hpp
../../../cmake-build-debug/tooling/tooling_EnumCodegen ../ProtocolV2T2.hpp mdr::v2::t2 > ProtocolV2T2Enum.hpp
../../../cmake-build-debug/tooling/tooling_SerializationCodegen ../ProtocolV2T1.hpp mdr::v2::t1 > ProtocolV2T1Serialization.cpp
../../../cmake-build-debug/tooling/tooling_SerializationCodegen ../ProtocolV2T2.hpp mdr::v2::t2 > ProtocolV2T2Serialization.cpp
../../../cmake-build-debug/tooling/tooling_ValidationCodegen ../ProtocolV2T1.hpp mdr::v2::t1 > ProtocolV2T1Validation.cpp
../../../cmake-build-debug/tooling/tooling_ValidationCodegen ../ProtocolV2T2.hpp mdr::v2::t2 > ProtocolV2T2Validation.cpp
../../../cmake-build-debug/tooling/tooling_TraitsCodegen ../ProtocolV2T1.hpp mdr::v2::t1 > ProtocolV2T1Traits.hpp
../../../cmake-build-debug/tooling/tooling_TraitsCodegen ../ProtocolV2T2.hpp mdr::v2::t2 > ProtocolV2T2Traits.hpp

