#pragma once
#include "Protocol.hpp"

// Generated from Sound Connect iOS J2ObjC metadata. Do not edit by hand.
namespace mdr::v1
{
    enum class CommonStatus : UInt8
    {
        ENABLE = 0x00,
        DISABLE = 0x01,
        OUT_OF_RANGE = 0xFF,
    };

    enum class ModelColor : UInt8
    {
        DEFAULT = 0x00,
        BLACK = 0x01,
        WHITE = 0x02,
        SILVER = 0x03,
        RED = 0x04,
        BLUE = 0x05,
        PINK = 0x06,
        YELLOW = 0x07,
        GREEN = 0x08,
        GRAY = 0x09,
        GOLD = 0x0A,
        CREAM = 0x0B,
        ORANGE = 0x0C,
        BROWN = 0x0D,
        VIOLET = 0x0E,
        BLACK_I = 0x11,
        WHITE_I = 0x12,
        SILVER_I = 0x13,
        RED_I = 0x14,
        BLUE_I = 0x15,
        PINK_I = 0x16,
        YELLOW_I = 0x17,
        GREEN_I = 0x18,
        GRAY_I = 0x19,
        GOLD_I = 0x1A,
        CREAM_I = 0x1B,
        ORANGE_I = 0x1C,
        BROWN_I = 0x1D,
        VIOLET_I = 0x1E,
    };

    enum class UpdateMethod : UInt8
    {
        TANDEM_METHOD = 0x00,
        CSR_METHOD = 0x10,
        CSR_RESUMABLE_METHOD = 0x11,
        CSR_TWS_METHOD = 0x12,
        CSR_TWS_RESUMABLE_METHOD = 0x13,
        MTK_METHOD = 0x20,
        MTK_RESUMABLE_METHOD = 0x21,
        MTK_TWS_RESUMABLE_METHOD = 0x23,
        MTK_BACKGROUND_RESUMABLE_METHOD = 0x25,
        MTK_TWS_BACKGROUND_RESUMABLE_METHOD = 0x27,
    };
} // namespace mdr::v1

#include "Generated/ProtocolV1Enum.hpp"
