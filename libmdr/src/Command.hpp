#pragma once
#include "Protocol.hpp"

namespace mdr
{
    inline constexpr uint8_t kEscapedByteSentry = 0x3D; // '='
    inline constexpr uint8_t kEscaped60 = 44; // 0x3C -> 0x3D 0x2C
    inline constexpr uint8_t kEscaped61 = 45; // 0x3D -> 0x3D 0x2D
    inline constexpr uint8_t kEscaped62 = 46; // 0x3E -> 0x3D 0x2E
    inline constexpr char kStartMarker{62}; // >
    inline constexpr char kEndMarker{60}; // <

    enum class MDRDataType : UInt8
    {
        DATA = 0,
        ACK = 1,
        DATA_MC_NO1 = 2,
        DATA_ICD = 9,
        DATA_EV = 10,
        DATA_MDR = 12,
        DATA_COMMON = 13,
        DATA_MDR_NO2 = 14,
        SHOT = 16,
        SHOT_MC_NO1 = 18,
        SHOT_ICD = 25,
        SHOT_EV = 26,
        SHOT_MDR = 28,
        SHOT_COMMON = 29,
        SHOT_MDR_NO2 = 30,
        LARGE_DATA_COMMON = 45,
        UNKNOWN = 0xff
    };

    // Upstream mentioned chunking w/ sizes above 2048
    // https://github.com/Plutoberth/SonyHeadphonesClient/blob/5620e8ed5deccb957338b54e371b215146080819/Client/CommandSerializer.cpp#L128
    constexpr size_t kMDRMaxPacketSize = 2048;
    using MDRCommandSeqNumber = UInt8;
    using MDRBuffer = Vector<UInt8>;
    /**
     * @brief Packs serialized data into an MDR packet with the given type and sequence number
     * @code
     * <START_MARKER>
     *      ESCAPE_SPECIALS
     *          <DATA_TYPE><SEQ_NUMBER><UNESCAPED DATA SIZE INT32BE><DATA><1 BYTE CHECKSUM>
     * <END_MARKER>
     * @endcode
     **/
    MDRBuffer MDRPackCommand(MDRDataType type, MDRCommandSeqNumber seq, Span<const UInt8> serializedData) noexcept;

    enum class MDRUnpackResult
    {
        OK = 0,
        INCOMPLETE = 1,
        BAD_MARKER = 2,
        BAD_CHECKSUM = 3,
    };

    /**
     * @brief Unpacks an MDR packet, returning the contained data and outputting the type and sequence number
     **/
    MDRUnpackResult MDRUnpackCommand(Span<const UInt8> packedCommand, MDRBuffer& outData, MDRDataType& outType,
                           MDRCommandSeqNumber& outSeq) noexcept;
} // namespace mdr

#include "Generated/Command.hpp"
