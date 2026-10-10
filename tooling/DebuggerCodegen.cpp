#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <map>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "Codegen.hpp"

namespace
{
    constexpr const char* kExternMacro = "MDR_DEFINE_EXTERN_SERIALIZATION";
    constexpr const char* kTrivialMacro = "MDR_DEFINE_TRIVIAL_SERIALIZATION";
    constexpr const char* kReadWriteMacro = "MDR_DEFINE_EXTERN_READ_WRITE";

    using MacroLocation = std::pair<unsigned, std::string>;

    std::string gSource;
    std::string gNamespace;
    std::string gInclude;
    std::vector<MacroLocation> gMacros;
    std::unordered_set<std::string> gReadWriteTypes;
    int gIndent{};

    void Check(bool expression, std::string_view message)
    {
        if (!expression)
        {
            fmt::print(stderr, "Debugger codegen error: {}\n", message);
            std::exit(1);
        }
    }

    std::string Indent(int extra = 0) { return std::string(static_cast<size_t>(gIndent + extra) * 4, ' '); }

    std::string CursorSpelling(CXCursor cursor)
    {
        CXString spelling = clang_getCursorSpelling(cursor);
        const char* raw = clang_getCString(spelling);
        std::string result = raw ? raw : "";
        clang_disposeString(spelling);
        return result;
    }

    std::string TypeSpelling(CXType type)
    {
        CXString spelling = clang_getTypeSpelling(type);
        const char* raw = clang_getCString(spelling);
        std::string result = raw ? raw : "";
        clang_disposeString(spelling);
        return result;
    }

    std::string NormalizedTypeName(std::string typeName)
    {
        constexpr std::string_view prefixes[] = {"const ", "mdr::"};
        bool changed;
        do
        {
            changed = false;
            for (const std::string_view prefix : prefixes)
            {
                if (typeName.starts_with(prefix))
                {
                    typeName.erase(0, prefix.size());
                    changed = true;
                }
            }
        }
        while (changed);
        return typeName;
    }

    bool StartsWithType(std::string_view typeName, std::string_view prefix)
    {
        return NormalizedTypeName(std::string(typeName)).starts_with(prefix);
    }

    CXChildVisitResult MacroVisitor(CXCursor cursor, CXCursor, CXClientData)
    {
        if (clang_getCursorKind(cursor) != CXCursor_MacroExpansion)
            return CXChildVisit_Continue;

        CXSourceLocation location = clang_getCursorLocation(cursor);
        CXFile file;
        unsigned line, column, offset;
        clang_getSpellingLocation(location, &file, &line, &column, &offset);
        CXString filename = clang_getFileName(file);
        const char* rawFilename = clang_getCString(filename);
        if (rawFilename && gSource == rawFilename)
            gMacros.emplace_back(line, CursorSpelling(cursor));
        clang_disposeString(filename);
        return CXChildVisit_Continue;
    }

    std::vector<std::string> StructMacros(CXCursor cursor)
    {
        const auto [startLine, endLine] = getCursorExtents(cursor);
        auto current = std::ranges::lower_bound(gMacros, MacroLocation{startLine, ""});
        std::vector<std::string> result;
        while (current != gMacros.end() && current->first < endLine)
        {
            result.push_back(current->second);
            ++current;
        }
        return result;
    }

    bool Contains(const std::vector<std::string>& values, std::string_view value)
    {
        return std::ranges::find(values, value) != values.end();
    }

    CXChildVisitResult ReadWriteTypeVisitor(CXCursor cursor, CXCursor parent, CXClientData)
    {
        const CXCursorKind kind = clang_getCursorKind(cursor);
        if (kind == CXCursor_Namespace)
            return CXChildVisit_Recurse;
        if (kind != CXCursor_StructDecl || getFullParentName(parent) != gNamespace)
            return CXChildVisit_Continue;

        if (Contains(StructMacros(cursor), kReadWriteMacro))
            gReadWriteTypes.insert(CursorSpelling(cursor));
        return CXChildVisit_Continue;
    }

    bool UsesCustomReadWrite(std::string_view typeName)
    {
        constexpr std::string_view reserved[] = {"MDRPodArray", "MDRPrefixedString16BE", "MDRPrefixedString",
                                                 "MDRArray",    "MDRFixedArray",         "MDRMap"};
        const std::string normalized = NormalizedTypeName(std::string(typeName));
        for (const std::string_view prefix : reserved)
            if (normalized.starts_with(prefix))
                return true;
        return gReadWriteTypes.contains(normalized);
    }

    void EmitReadField(std::string_view resultType, std::string_view fieldType, std::string_view expression)
    {
        if (UsesCustomReadWrite(fieldType))
        {
            println("{}MDR_DEBUGGER_TRY_READ({}, ({}::Read)(&ptr, {}, remaining));", Indent(), resultType, fieldType,
                    expression);
        }
        else
        {
            println("{}MDR_DEBUGGER_TRY_READ({}, MDRPod::Read(&ptr, {}, remaining));", Indent(), resultType,
                    expression);
        }
    }

    void EmitWriteField(std::string_view resultType, std::string_view fieldType, std::string_view expression)
    {
        if (UsesCustomReadWrite(fieldType))
        {
            println("{}MDR_DEBUGGER_TRY_WRITE({}, ({}::Write)({}, &ptr, remaining));", Indent(), resultType, fieldType,
                    expression);
        }
        else
        {
            println("{}MDR_DEBUGGER_TRY_WRITE({}, MDRPod::Write({}, &ptr, remaining));", Indent(), resultType,
                    expression);
        }
    }

    struct EnumValue
    {
        std::string name;
        unsigned long long value;
    };

    struct CollectedEnum
    {
        std::string identifier;
        std::vector<EnumValue> values;
    };

    std::map<std::string, CollectedEnum> gEnums;
    std::unordered_set<std::string> gVisitedStructs;

    CXChildVisitResult EnumValueVisitor(CXCursor cursor, CXCursor, CXClientData data)
    {
        if (clang_getCursorKind(cursor) != CXCursor_EnumConstantDecl)
            return CXChildVisit_Continue;
        auto& values = *static_cast<std::vector<EnumValue>*>(data);
        values.push_back({CursorSpelling(cursor), clang_getEnumConstantDeclUnsignedValue(cursor)});
        return CXChildVisit_Continue;
    }

    std::string SafeIdentifier(std::string value)
    {
        for (char& character : value)
            if (!std::isalnum(static_cast<unsigned char>(character)))
                character = '_';
        return value;
    }

    std::string EnumKey(CXCursor declaration) { return getFullParentName(declaration); }

    void CollectEnumsFromType(CXType type);

    CXChildVisitResult CollectEnumsFieldVisitor(CXCursor cursor, CXCursor, CXClientData)
    {
        if (clang_getCursorKind(cursor) == CXCursor_FieldDecl && clang_Cursor_getStorageClass(cursor) != CX_SC_Static)
        {
            CollectEnumsFromType(clang_getCursorType(cursor));
        }
        return CXChildVisit_Continue;
    }

    void CollectEnumsFromType(CXType type)
    {
        const CXCursor declaration = clang_getTypeDeclaration(type);
        const CXCursorKind declarationKind = clang_getCursorKind(declaration);
        if (declarationKind == CXCursor_EnumDecl)
        {
            const std::string key = EnumKey(declaration);
            if (gEnums.contains(key))
                return;
            CollectedEnum info{SafeIdentifier(key), {}};
            clang_visitChildren(declaration, EnumValueVisitor, &info.values);
            gEnums.emplace(key, std::move(info));
            return;
        }

        const std::string typeName = TypeSpelling(type);
        if (StartsWithType(typeName, "MDRPodArray<") || StartsWithType(typeName, "MDRArray<") ||
            StartsWithType(typeName, "MDRFixedArray<"))
        {
            const CXType elementType = clang_Type_getTemplateArgumentAsType(type, 0);
            if (elementType.kind != CXType_Invalid)
                CollectEnumsFromType(elementType);
            return;
        }
        if (StartsWithType(typeName, "MDRMap<"))
        {
            const CXType keyType = clang_Type_getTemplateArgumentAsType(type, 0);
            const CXType valueType = clang_Type_getTemplateArgumentAsType(type, 1);
            if (keyType.kind != CXType_Invalid)
                CollectEnumsFromType(keyType);
            if (valueType.kind != CXType_Invalid)
                CollectEnumsFromType(valueType);
            return;
        }

        const CXType canonical = clang_getCanonicalType(type);
        if (canonical.kind == CXType_ConstantArray)
        {
            CollectEnumsFromType(clang_getArrayElementType(canonical));
            return;
        }

        if (declarationKind == CXCursor_StructDecl)
        {
            const std::string key = getFullParentName(declaration);
            if (!gVisitedStructs.insert(key).second)
                return;
            clang_visitChildren(declaration, CollectEnumsFieldVisitor, nullptr);
        }
    }

    CXChildVisitResult CollectPacketEnumsVisitor(CXCursor cursor, CXCursor parent, CXClientData)
    {
        const CXCursorKind kind = clang_getCursorKind(cursor);
        if (kind == CXCursor_Namespace)
            return CXChildVisit_Recurse;
        if (kind != CXCursor_StructDecl || getFullParentName(parent) != gNamespace)
            return CXChildVisit_Continue;
        const std::vector<std::string> macros = StructMacros(cursor);
        if (!Contains(macros, kExternMacro) && !Contains(macros, kTrivialMacro))
            return CXChildVisit_Continue;
        clang_visitChildren(cursor, CollectEnumsFieldVisitor, nullptr);
        return CXChildVisit_Continue;
    }

    bool IsScalar(CXType type)
    {
        switch (clang_getCanonicalType(type).kind)
        {
        case CXType_Bool:
        case CXType_Char_U:
        case CXType_UChar:
        case CXType_Char16:
        case CXType_Char32:
        case CXType_UShort:
        case CXType_UInt:
        case CXType_ULong:
        case CXType_ULongLong:
        case CXType_Char_S:
        case CXType_SChar:
        case CXType_WChar:
        case CXType_Short:
        case CXType_Int:
        case CXType_Long:
        case CXType_LongLong:
        case CXType_Float:
        case CXType_Double:
        case CXType_LongDouble:
            return true;
        default:
            return false;
        }
    }

    void EmitDrawValue(CXType type, std::string_view expression, std::string_view labelExpression);

    struct DrawFieldsData
    {
        std::string expression;
    };

    CXChildVisitResult DrawFieldsVisitor(CXCursor cursor, CXCursor, CXClientData data)
    {
        if (clang_getCursorKind(cursor) != CXCursor_FieldDecl)
            return CXChildVisit_Continue;
        if (clang_Cursor_getStorageClass(cursor) == CX_SC_Static)
            return CXChildVisit_Continue;

        const auto& parent = *static_cast<DrawFieldsData*>(data);
        const std::string name = CursorSpelling(cursor);
        const std::string expression = format("{}.{}", parent.expression, name);
        EmitDrawValue(clang_getCursorType(cursor), expression, format("\"{}\"", name));
        return CXChildVisit_Continue;
    }

    void EmitSequence(CXType elementType, std::string_view expression, std::string_view labelExpression, bool resizable)
    {
        println("{}changed |= DrawSequence({}, {}, {}, [&](const char* elementLabel, auto& element) {{", Indent(),
                labelExpression, expression, resizable ? "true" : "false");
        gIndent++;
        println("{}bool elementChanged = false;", Indent());
        println("{}{{", Indent());
        gIndent++;
        println("{}bool& changed = elementChanged;", Indent());
        EmitDrawValue(elementType, "element", "elementLabel");
        gIndent--;
        println("{}}}", Indent());
        println("{}return elementChanged;", Indent());
        gIndent--;
        println("{}}});", Indent());
    }

    void EmitDrawValue(CXType type, std::string_view expression, std::string_view labelExpression)
    {
        const CXCursor declaration = clang_getTypeDeclaration(type);
        const CXCursorKind declarationKind = clang_getCursorKind(declaration);
        const std::string typeName = TypeSpelling(type);
        const std::string normalized = NormalizedTypeName(typeName);

        if (declarationKind == CXCursor_EnumDecl)
        {
            const auto found = gEnums.find(EnumKey(declaration));
            Check(found != gEnums.end(), format("Enum metadata was not collected for {}", typeName));
            println("{}changed |= DrawEnum({}, {}, kEnum_{});", Indent(), labelExpression, expression,
                    found->second.identifier);
            return;
        }

        if (normalized == "UInt16BE" || normalized == "Int16BE" || normalized == "Int24BE" ||
            normalized == "Int32BE" || normalized == "UInt64BE")
        {
            println("{}changed |= DrawEndian({}, {});", Indent(), labelExpression, expression);
            return;
        }

        if (normalized == "MDRPrefixedString")
        {
            println("{}changed |= DrawPrefixedString({}, {});", Indent(), labelExpression, expression);
            return;
        }

        if (normalized == "MDRPrefixedString16BE")
        {
            println("{}changed |= DrawString({}, {}.value, UINT16_MAX);", Indent(), labelExpression, expression);
            return;
        }

        if (normalized == "String")
        {
            println("{}changed |= DrawString({}, {}, SIZE_MAX);", Indent(), labelExpression, expression);
            return;
        }

        if (StartsWithType(typeName, "MDRPodArray<") || StartsWithType(typeName, "MDRArray<"))
        {
            const CXType elementType = clang_Type_getTemplateArgumentAsType(type, 0);
            Check(elementType.kind != CXType_Invalid, format("Unable to get element type for {}", typeName));
            EmitSequence(elementType, format("{}.value", expression), labelExpression, true);
            return;
        }

        if (StartsWithType(typeName, "MDRFixedArray<"))
        {
            const CXType elementType = clang_Type_getTemplateArgumentAsType(type, 0);
            Check(elementType.kind != CXType_Invalid, format("Unable to get element type for {}", typeName));
            EmitSequence(elementType, format("{}.value", expression), labelExpression, false);
            return;
        }

        if (StartsWithType(typeName, "MDRMap<"))
        {
            const CXType keyType = clang_Type_getTemplateArgumentAsType(type, 0);
            const CXType valueType = clang_Type_getTemplateArgumentAsType(type, 1);
            Check(keyType.kind != CXType_Invalid && valueType.kind != CXType_Invalid,
                  format("Unable to get map types for {}", typeName));
            println("{}changed |= DrawMap({}, {}.entries,", Indent(), labelExpression, expression);
            gIndent++;
            println("{}[&](const char* entryLabel, auto& entryValue) {{", Indent());
            gIndent++;
            println("{}bool entryChanged = false;", Indent());
            println("{}{{", Indent());
            gIndent++;
            println("{}bool& changed = entryChanged;", Indent());
            EmitDrawValue(keyType, "entryValue", "entryLabel");
            gIndent--;
            println("{}}}", Indent());
            println("{}return entryChanged;", Indent());
            gIndent--;
            println("{}}},", Indent());
            println("{}[&](const char* entryLabel, auto& entryValue) {{", Indent());
            gIndent++;
            println("{}bool entryChanged = false;", Indent());
            println("{}{{", Indent());
            gIndent++;
            println("{}bool& changed = entryChanged;", Indent());
            EmitDrawValue(valueType, "entryValue", "entryLabel");
            gIndent--;
            println("{}}}", Indent());
            println("{}return entryChanged;", Indent());
            gIndent--;
            println("{}}});", Indent());
            gIndent--;
            return;
        }

        const CXType canonical = clang_getCanonicalType(type);
        if (canonical.kind == CXType_ConstantArray)
        {
            EmitSequence(clang_getArrayElementType(canonical), expression, labelExpression, false);
            return;
        }

        if (IsScalar(type))
        {
            println("{}changed |= DrawScalar({}, {});", Indent(), labelExpression, expression);
            return;
        }

        if (declarationKind == CXCursor_StructDecl)
        {
            println("{}changed |= DrawTree({}, [&]() {{", Indent(), labelExpression);
            gIndent++;
            println("{}bool changed = false;", Indent());
            DrawFieldsData nested{std::string(expression)};
            clang_visitChildren(declaration, DrawFieldsVisitor, &nested);
            println("{}return changed;", Indent());
            gIndent--;
            println("{}}});", Indent());
            return;
        }

        println("{}changed |= DrawOpaqueBytes({}, &{}, sizeof({}));", Indent(), labelExpression, expression,
                expression);
    }

    struct CodecFieldsData
    {
        std::string objectExpression;
        bool write;
        std::string resultType;
    };

    CXChildVisitResult CodecFieldsVisitor(CXCursor cursor, CXCursor, CXClientData data)
    {
        if (clang_getCursorKind(cursor) != CXCursor_FieldDecl)
            return CXChildVisit_Continue;
        if (clang_Cursor_getStorageClass(cursor) == CX_SC_Static)
            return CXChildVisit_Continue;

        const auto& codec = *static_cast<CodecFieldsData*>(data);
        const std::string fieldName = CursorSpelling(cursor);
        const std::string fieldType = TypeSpelling(clang_getCursorType(cursor));
        const std::string expression = format("{}.{}", codec.objectExpression, fieldName);
        if (codec.write)
            EmitWriteField(codec.resultType, fieldType, expression);
        else
            EmitReadField(codec.resultType, fieldType, expression);
        return CXChildVisit_Continue;
    }

    void EmitPacket(CXCursor cursor, std::string_view structName)
    {
        println("{}bool Draw_{}({}& value)", Indent(), structName, structName);
        println("{}{{", Indent());
        gIndent++;
        println("{}bool changed = false;", Indent());
        DrawFieldsData drawData{"value"};
        clang_visitChildren(cursor, DrawFieldsVisitor, &drawData);
        println("{}return changed;", Indent());
        gIndent--;
        println("{}}}", Indent());

        println("{}mdr::MDRResult<void> Decode_{}(void* storage, mdr::Span<const mdr::UInt8> bytes)", Indent(),
                structName);
        println("{}{{", Indent());
        gIndent++;
        println("{}auto& value = *static_cast<{}*>(storage);", Indent(), structName);
        println("{}const UInt8* ptr = bytes.data();", Indent());
        println("{}size_t remaining = bytes.size();", Indent());
        CodecFieldsData readData{"value", false, "void"};
        clang_visitChildren(cursor, CodecFieldsVisitor, &readData);
        println("{}if (remaining != 0) return mdr::MDRResult<void>::Failure(MDR_RESULT_ERROR_MALFORMED_PAYLOAD, "
                "\"Trailing packet bytes\");",
                Indent());
        println("{}return mdr::MDRResult<void>::Success();", Indent());
        gIndent--;
        println("{}}}", Indent());

        println("{}mdr::MDRResult<size_t> Encode_{}(const void* storage, mdr::UInt8* output, size_t maxSize)", Indent(),
                structName);
        println("{}{{", Indent());
        gIndent++;
        println("{}const auto& value = *static_cast<const {}*>(storage);", Indent(), structName);
        println("{}UInt8* ptr = output;", Indent());
        println("{}size_t remaining = maxSize;", Indent());
        CodecFieldsData writeData{"value", true, "size_t"};
        clang_visitChildren(cursor, CodecFieldsVisitor, &writeData);
        println("{}return mdr::MDRResult<size_t>::Success(ptr - output);", Indent());
        gIndent--;
        println("{}}}", Indent());

        println("{}void* Create_{}() {{ return Construct<{}>(); }}", Indent(), structName, structName);
        println("{}void Destroy_{}(void* value) {{ Destruct(static_cast<{}*>(value)); }}", Indent(), structName,
                structName);
        println("{}mdr::MDRResult<void> Validate_{}(const void* value)", Indent(), structName);
        println("{}{{", Indent());
        gIndent++;
        println("{}return {}::Validate(*static_cast<const {}*>(value));", Indent(), structName, structName);
        gIndent--;
        println("{}}}", Indent());
        println("{}bool DrawErased_{}(void* value) {{ return Draw_{}(*static_cast<{}*>(value)); }}", Indent(),
                structName, structName, structName);
        println("");
    }

    struct PacketInfo
    {
        std::string name;
    };
    std::vector<PacketInfo> gPackets;

    CXChildVisitResult EmitStructVisitor(CXCursor cursor, CXCursor parent, CXClientData)
    {
        const CXCursorKind kind = clang_getCursorKind(cursor);
        if (kind == CXCursor_Namespace)
            return CXChildVisit_Recurse;
        if (kind != CXCursor_StructDecl || getFullParentName(parent) != gNamespace)
            return CXChildVisit_Continue;

        const std::vector<std::string> macros = StructMacros(cursor);
        if (!Contains(macros, kExternMacro) && !Contains(macros, kTrivialMacro))
            return CXChildVisit_Continue;

        const std::string name = CursorSpelling(cursor);
        if (name.empty())
            return CXChildVisit_Continue;
        EmitPacket(cursor, name);
        gPackets.push_back({name});
        return CXChildVisit_Continue;
    }

    std::string RegistryFunctionName()
    {
        std::string name = gNamespace;
        name.erase(0, std::string("mdr::").size());
        for (char& character : name)
            if (character == ':')
                character = '_';
        name.erase(std::remove(name.begin(), name.end(), '_'), name.end());
        return format("Get{}Packets", name);
    }
} // namespace

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        println("usage: {} <source-file> <namespace-name> <include-name>", argv[0]);
        println("\tGenerate static ImGui packet layouts and unchecked codecs.");
        return 1;
    }

    gSource = argv[1];
    gNamespace = argv[2];
    gInclude = argv[3];

    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit unit = clang_parseTranslationUnit(index, gSource.c_str(), nullptr, 0, nullptr, 0,
                                                        CXTranslationUnit_DetailedPreprocessingRecord);
    Check(unit != nullptr, "Unable to parse translation unit");
    CXCursor root = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(root, MacroVisitor, nullptr);
    std::ranges::sort(gMacros);
    clang_visitChildren(root, ReadWriteTypeVisitor, nullptr);
    clang_visitChildren(root, CollectPacketEnumsVisitor, nullptr);

    println("/* This file is auto-generated by tooling/DebuggerCodegen.cpp */");
    println("#include <{}>", gInclude);
    println("#include \"../DebuggerDetails.hpp\"");
    println("");
    println("namespace client::debugger {{");
    println("    using namespace mdr;");
    println("    using namespace {};", gNamespace.substr(0, gNamespace.rfind("::")));
    println("    using namespace {};", gNamespace);
    println("    namespace {{");
    gIndent = 2;
    for (const auto& [key, enumInfo] : gEnums)
    {
        (void)key;
        println("{}static constexpr EnumOption kEnum_{}[] = {{", Indent(), enumInfo.identifier);
        gIndent++;
        for (const EnumValue& value : enumInfo.values)
        {
            println("{}{{\"{}\", UINT64_C({})}},", Indent(), value.name, value.value);
        }
        gIndent--;
        println("{}}};", Indent());
    }
    println("");
    clang_visitChildren(root, EmitStructVisitor, nullptr);
    println("{}PacketDescriptor kPackets[] = {{", Indent());
    gIndent++;
    for (const auto& packet : gPackets)
    {
        println("{}{{\"{}::{}\", ProtocolFamily::{}, MDRTraits<{}>::kDataType, "
                "static_cast<UInt8>({}{{}}.command), &Create_{}, &Destroy_{}, "
                "&Decode_{}, &Encode_{}, &Validate_{}, &DrawErased_{}}},",
                Indent(), gNamespace, packet.name, gNamespace.starts_with("mdr::v1::") ? "V1" : "V2", packet.name,
                packet.name, packet.name, packet.name, packet.name, packet.name, packet.name, packet.name);
    }
    gIndent--;
    println("{}}};", Indent());
    println("    }}");
    println("");
    println("    std::span<const PacketDescriptor> {}()", RegistryFunctionName());
    println("    {{");
    println("        return kPackets;");
    println("    }}");
    println("}}");

    std::fflush(stdout);
    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    return 0;
}
