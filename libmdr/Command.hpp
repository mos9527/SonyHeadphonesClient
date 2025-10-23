#pragma once
#include "Protocol.hpp"
namespace mdr
{
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
    using MDRCommandSeqNumber = UInt8;
    using MDRBuffer = Vector<UInt8>;
    /**
     * @brief Packs serialized data into an MDR packet with the given type and sequence number
     * @code
     * <START_MARKER>
     *      ESCAPE_SPECIALS
     *          <DATA_TYPE><SEQ_NUMBER><DATA SIZE INT32BE><DATA><1 BYTE CHECKSUM>
     * <END_MARKER>
     * @endcode
     **/
    MDRBuffer Pack(MDRDataType type, MDRCommandSeqNumber seq, Span<const UInt8> serializedData);
    /**
     * @brief Unpacks an MDR packet, returning the contained data and outputting the type and sequence number
     **/
    MDRBuffer Unpack(Span<const UInt8> packedData, MDRDataType& outType, MDRCommandSeqNumber& outSeq);
} // namespace mdr

#include "Generated/Command.hpp"

