#!/bin/bash
# Run this script from libmdr/Generated directory to regenerate all generated files
# Requires tooling to be built first
# TODO: Use CMake to handle these instead

../cmake-build-debug/tooling/tooling_EnumCodegen include/mdr/Command.hpp mdr > include/mdr/Generated/Command.hpp
../cmake-build-debug/tooling/tooling_EnumCodegen include/mdr/ProtocolV2.hpp mdr::v2 > include/mdr/Generated/ProtocolV2Enum.hpp
../cmake-build-debug/tooling/tooling_EnumCodegen include/mdr/ProtocolV2T1.hpp mdr::v2::t1 > include/mdr/Generated/ProtocolV2T1Enum.hpp
../cmake-build-debug/tooling/tooling_EnumCodegen include/mdr/ProtocolV2T2.hpp mdr::v2::t2 > include/mdr/Generated/ProtocolV2T2Enum.hpp
../cmake-build-debug/tooling/tooling_TraitsCodegen include/mdr/ProtocolV2T1.hpp mdr::v2::t1 > include/mdr/Generated/ProtocolV2T1Traits.hpp
../cmake-build-debug/tooling/tooling_TraitsCodegen include/mdr/ProtocolV2T2.hpp mdr::v2::t2 > include/mdr/Generated/ProtocolV2T2Traits.hpp

../cmake-build-debug/tooling/tooling_SerializationCodegen include/mdr/ProtocolV2T1.hpp mdr::v2::t1 mdr/ProtocolV2T1.hpp > src/Generated/ProtocolV2T1Serialization.cpp
../cmake-build-debug/tooling/tooling_SerializationCodegen include/mdr/ProtocolV2T2.hpp mdr::v2::t2 mdr/ProtocolV2T2.hpp > src/Generated/ProtocolV2T2Serialization.cpp
../cmake-build-debug/tooling/tooling_ValidationCodegen include/mdr/ProtocolV2T1.hpp mdr::v2::t1 mdr/ProtocolV2T1.hpp > src/Generated/ProtocolV2T1Validation.cpp
../cmake-build-debug/tooling/tooling_ValidationCodegen include/mdr/ProtocolV2T2.hpp mdr::v2::t2 mdr/ProtocolV2T2.hpp > src/Generated/ProtocolV2T2Validation.cpp
