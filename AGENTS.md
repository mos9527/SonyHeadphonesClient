# Payload Struct Implementation Details (libmdr)
A MDR Payload struct 
- ALWAYS has a `Command` enum field named `command` as the first field, with a default value set to the command it represents.
    - May have a `static constexpr Command kResponseCommand` field if the command has a well-defined response command.
- ALWAYS implements the `MDRIsSerializable` concept, which means it can be serialized and deserialized to/from a byte buffer,
  either through trivial serialization macro (`MDR_DEFINE_TRIVIAL_SERIALIZATION`) or through custom serialization functions.
- ALWAYS has a static assertion to verify it implements the `MDRIsSerializable` concept.
- MAY contain dynamic array types (vectors or strings), otherwise it should use the trivial serialization macro (see the next section).
- ALWAYS trivially copyable (no user-defined constructors, destructors, or copy/move operators) UNLESS contains dynamic array types (vectors or strings).
- ALWAYS in standard layout (no virtual functions, no multiple inheritance, all non-static data members have the same access control). 
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
- The struct has NO alignment requirements (any packing).
- This field declaration has NO ordering requirements, though  recommended and has been followed in practice.
- The struct ALWAYS implements external static `Serialize` and `Deserialize` functions to handle the serialization and deserialization logic.
- Helper functions are provided to perform RWs on subtypes for fields
    - ALL `Read` has the signature of     
        `void Read(UInt8** ppSrcBuffer, MDRPrefixedString& str, size_t maxSize = ~0LL)`
    
    And will advance the `*ppSrcBuffer` pointer by the number of bytes read.            
        The optional `maxSize` parameter can be used to limit the maximum size of the string to read.
    - ALL `Write` has the signature of        
        `size_t Write(MDRPrefixedString const& str, UInt8** ppDstBuffer)`        
        returning the number of bytes written, or 0 on failure.     

    And will advance the `*ppDstBuffer` pointer by the number of bytes written.
    - Use `MDRPod::Read` and `MDRPod::Write` for basic types and PODs
    - Use `MDRPodArray::Read` and `MDRPodArray::Write` for dynamic arrays of PODs
    - Use `MDRPrefixedString::Read` and `MDRPrefixedString::Write` for dynamic strings
## Snippets
### Read/Write signature
- This is meant for non-trivial Read/Write functions, for **fields** that require special handling.
- On failure, exceptions are ALWAYS thrown.
- Write ALWAYS returns non-zero amount of bytes written, or throws.
- Declaration in Headers ALWAYS uses `MDR_DEFINE_EXTERN_READ_WRITE(SubType)` macro.
- Implementation in translation units always uses the following signatures:
```c++
    static void Read(UInt8** ppSrcBuffer, Type &out, size_t maxSize = ~0LL);
    static size_t Write(const Type &data, UInt8** ppDstBuffer);
```
### Non-trivial Serialization/Deserialization signature
- This is meant for non-trivial Serialize/Deserialize functions, for **structs** that require special handling.
- On failure, exceptions are ALWAYS thrown.
- Serialize ALWAYS returns non-zero amount of bytes written, or throws.
- Declaration in Headers ALWAYS uses `MDR_DEFINE_EXTERN_SERIALIZATION(Type)` macro.
- Implementation in translation units ALWAYS uses the following signatures:
```c++
    static size_t Serialize(const Type &data, UInt8* out);
    static void Deserialize(UInt8* data, Type &out);
```
- To have the codegen *exclude* a struct, include `MDR_CODEGEN_IGNORE_SERIALIZATION` within the context
  of the said struct.

## Validation
- ALL structs MUST implement static assertions to verify they implement the `MDRIsSerializable` concept.
- ALL structs MUST implement data validation
  - `CODEGEN` comments hints the codegen tool to generate validation code for the struct.
  - For details, see the next section
### `CODEGEN` validation comments
`CODEGEN` comments are one-line markups that provide hints to the codegen tool to generate validation code for the fields.
- Placed above the field they apply to.
- Multiple `CODEGEN` comments can be placed above a field, each on its own line.
- General syntax: `CODEGEN [Verb] [Arguments]`
#### Verbs
- `CODEGEN EnumRange [Values...]`
Declare that the field is an enum, and its value must be one of the specified values.
Multiple values are seperated by spaces.
- `CODEGEN Range [Min] [Max]`
Declare that the field is a numeric type, and its value must be within the specified range (inclusive).
- `CODEGEN Field [Field Name] [Verb] [Arguments]`
Declare that the field is a struct, and the specified verb and arguments apply to the specified field within the struct.
Nested validation can be achieved by declaring `Field base.field1.field2 ...` to reach deeper levels.
## Summary
### Macros to use in Headers
- `MDR_DEFINE_TRIVIAL_SERIALIZATION(Type)` for trivially serializable structs.
- `MDR_DEFINE_EXTERN_SERIALIZATION(Type)` for non-trivially serializable structs, where codegen will generate implementations.
  - `MDR_CODEGEN_IGNORE_SERIALIZATION` to exclude a struct from codegen serialization impl generation, and implement manually.
- `MDR_DEFINE_EXTERN_READ_WRITE(SubType)` for non-trivially serializable fields structs declaration, where codegen will generate implementations.
  - `MDR_CODEGEN_IGNORE_SERIALIZATION` to exclude a field struct from codegen serialization impl generation, and implement manually.
  - `MDR_CODEGEN_IGNORE_VALIDATION` to exclude a struct from codegen validation.
