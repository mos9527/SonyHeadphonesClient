#!/bin/bash
# Run this script from libmdr/Generated directory to regenerate all generated files
# Requires tooling to be built first
../../cmake-build-debug/tooling/tooling_EnumCodegen ../Command.hpp mdr > Command.hpp
../../cmake-build-debug/tooling/tooling_EnumCodegen ../ProtocolV2.hpp mdr::v2 > ProtocolV2.hpp
../../cmake-build-debug/tooling/tooling_EnumCodegen ../ProtocolV2T1.hpp mdr::v2::t1 > ProtocolV2T1.hpp
../../cmake-build-debug/tooling/tooling_EnumCodegen ../ProtocolV2T2.hpp mdr::v2::t2 > ProtocolV2T2.hpp
../../cmake-build-debug/tooling/tooling_SerializationCodegen ../ProtocolV2T1.hpp mdr::v2::t1 > ProtocolV2T1Serialization.cpp
../../cmake-build-debug/tooling/tooling_SerializationCodegen ../ProtocolV2T2.hpp mdr::v2::t2 > ProtocolV2T2Serialization.cpp
../../cmake-build-debug/tooling/tooling_ValidationCodegen ../ProtocolV2T1.hpp mdr::v2::t1 > ProtocolV2T1Validation.cpp
../../cmake-build-debug/tooling/tooling_ValidationCodegen ../ProtocolV2T2.hpp mdr::v2::t2 > ProtocolV2T2Validation.cpp
