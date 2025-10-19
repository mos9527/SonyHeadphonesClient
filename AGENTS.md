# Payload Struct Implementation Details (libmdr)
A MDR Payload struct 
- Always has a `Command` enum field named `command` as the first field, with a default value set to the command it represents.
    - May have a `static constexpr Command kResponseCommand` field if the command has a well-defined response command.
- Always implements the `MDRIsSerializable` concept, which means it can be serialized and deserialized to/from a byte buffer,
  either through trivial serialization macro (`MDR_DEFINE_TRIVIAL_SERIALIZATION`) or through custom serialization functions.
- Always has a static assertion to verify it implements the `MDRIsSerializable` concept.
- May contain dynamic array types (vectors or strings), otherwise it should use the trivial serialization macro (see the next section).
- _ALWAYS_ trivially copyable (no user-defined constructors, destructors, or copy/move operators) UNLESS contains dynamic array types (vectors or strings).
- _ALWAYS_ in standard layout (no virtual functions, no multiple inheritance, all non-static data members have the same access control). 
## Trivially Serializable Payloads (PODs)
These are payload structs that can be trivially serialized and deserialized in memory. This implies:
- No dynamic array types (vectors or strings)
- In memory representation is the same as packet representation (no packing, no endianness issues)
- Enums can be casted in well-defined ways into integers

### Implementation Details
```c++
#pragma pack(push, 1)
... <extra code>
struct ConnectRetProtocolInfo
{
    Command command{Command::CONNECT_RET_PROTOCOL_INFO}; // 0x0
    ConnectInquiredType inquiredType{ConnectInquiredType::FIXED_VALUE}; // 0x1
    Int32BE protocolVersion; // 0x2-0x5
    MessageMdrV2EnableDisable supportTable1Value; // 0x6
    MessageMdrV2EnableDisable supportTable2Value; // 0x7

    MDR_DEFINE_TRIVIAL_SERIALIZATION(ConnectRetProtocolInfo);
};
static_assert(MDRIsSerializable<ConnectRetProtocolInfo>);
... <extra code>
#pragma pack(pop)
```
- The struct is tightly packed.
- The fields are defined in the order they appear in the packet, with appropriate types.
- The `MDR_DEFINE_TRIVIAL_SERIALIZATION` macro is used to automatically generate the serialization and deserialization functions.

## Non-Trivially Serializable Payloads
These are payload structs that cannot be trivially serialized and deserialized, usually because they contain dynamic array types (vectors or strings).
```c++
struct ConnectRetSupportFunction
{
    Command command{Command::CONNECT_RET_SUPPORT_FUNCTION}; // 0x0
    ConnectInquiredType inquiredType{ConnectInquiredType::FIXED_VALUE}; // 0x1
    MDRPodArray<MessageMdrV2SupportFunction> supportFunctions; // 0x2-

    MDR_DEFINE_EXTERN_SERIALIZATION(ConnectRetSupportFunction);
};
static_assert(MDRIsSerializable<ConnectRetSupportFunction>);
... <extra code>
// ProtocolV2T1.cpp
size_t ConnectRetSupportFunction::Serialize(const ConnectRetSupportFunction& data, UInt8* out)
{
    UInt8* ptr = out;
    MDRPod::Write(data.inquiredType, &ptr);
    MDRPodArray<MessageMdrV2SupportFunction>::Write(data.supportFunctions, &ptr);
    return ptr - out;
}

MDRSerializationResult ConnectRetSupportFunction::Deserialize(UInt8* data, ConnectRetSupportFunction& out)
{
    using enum DeviceInfoType;
    MDRPod::Read(&data, out.command);
    MDRPod::Read(&data, out.inquiredType);
    if (out.command != Command::CONNECT_RET_SUPPORT_FUNCTION || out.inquiredType != ConnectInquiredType::FIXED_VALUE)
        return MDRSerializationResult::MalformedPayload;
    MDRPodArray<MessageMdrV2SupportFunction>::Read(&data,out.supportFunctions);
    return MDRSerializationResult::OK;
}
```
- The struct has no alignment requirements.
- This field declaration has no ordering requirements, though  recommended and has been followed in practice.
- The struct implements static `Serialize` and `Deserialize` functions to handle the serialization and deserialization logic.
- Helper functions are provided to perform RWs
    - All `Read` has the signature of     
        `void Read(UInt8** ppSrcBuffer, MDRPrefixedString& str, size_t maxSize = ~0LL)`
    
    And will advance the `*ppSrcBuffer` pointer by the number of bytes read.            
        The optional `maxSize` parameter can be used to limit the maximum size of the string to read.
    - All `Write` has the signature of        
        `size_t Write(MDRPrefixedString const& str, UInt8** ppDstBuffer)`        
        returning the number of bytes written, or 0 on failure.     

    And will advance the `*ppDstBuffer` pointer by the number of bytes written.
    - Use `MDRPod::Read` and `MDRPod::Write` for basic types and PODs
    - Use `MDRPodArray::Read` and `MDRPodArray::Write` for dynamic arrays of PODs
    - Use `MDRPrefixedString::Read` and `MDRPrefixedString::Write` for dynamic strings
## Snippets
### Read/Write signature
- On failure, exceptions are always thrown.
- Write _always_ returns non-zero amount of bytes written, or throws.
```c++
    static void Read(UInt8** ppSrcBuffer, Type &out, size_t maxSize = ~0LL);
    static size_t Write(const Type &data, UInt8** ppDstBuffer);
```
### Non-trivial Serialization/Deserialization signature
- On failure, exceptions are always thrown.
- Serialize _always_ returns non-zero amount of bytes written, or throws.
- Declaration in Headers always uses `MDR_DEFINE_EXTERN_SERIALIZATION(Type)` macro.
- Implementation in translation units always uses the following signatures:
```c++
    static size_t Serialize(const Type &data, UInt8* out);
    static void Deserialize(UInt8* data, Type &out);
```
- To have the codegen _exclude_ a struct, include `MDR_CODEGEN_IGNORE_SERIALIZATION` within the context
  of the said struct.