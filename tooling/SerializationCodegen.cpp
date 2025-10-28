#include <vector>
#include <algorithm>
#include "Codegen.hpp"
const char* kMDRExternMacro = "MDR_DEFINE_EXTERN_SERIALIZATION";
const char* kMDRIgnoredMacro = "MDR_CODEGEN_IGNORE_SERIALIZATION";
const char* kMDRRWMacro = "MDR_DEFINE_EXTERN_READ_WRITE";
const char* kMDRReservedRWStructs[] = {
    "MDRPodArray",
    "MDRPrefixedString",
    "MDRArray",
    "MDRFixedArray"
};
std::string gSrc = "libmdr/ProtocolV2T1Enums.hpp";
std::string gNamespaceName = "mdr::v2::t1";
using MacroPair = std::pair<unsigned, std::string>; // Line, Name
std::vector<MacroPair> gMacros;
CXChildVisitResult macroVisitor(CXCursor cursor, CXCursor parent, CXClientData)
{
    CXCursorKind kind = clang_getCursorKind(cursor);
    if (kind == CXCursor_MacroExpansion)
    {
        CXString name = clang_getCursorSpelling(cursor);
        CXSourceLocation loc = clang_getCursorLocation(cursor);
        // Identify macros with line numbers. getLexicalParent doesn't work since we're still
        // in the preprocessor
        CXFile file; unsigned line, col, offset;
        clang_getSpellingLocation(loc, &file, &line, &col, &offset);
        CXString fileName = clang_getFileName(file);
        std::string fileStr = clang_getCString(fileName);
        if (fileStr == gSrc)
            gMacros.emplace_back(line, clang_getCString(name));
        clang_disposeString(name);
        clang_disposeString(fileName);
    }
    return CXChildVisit_Continue;
}
struct MethodVisitorResult
{
    bool hasRead = false;
    bool hasWrite = false;
};
CXChildVisitResult methodVisitor(CXCursor cursor, CXCursor parent, CXClientData pResult)
{
    CXString parentName = clang_getCursorSpelling(parent);
    clang_disposeString(parentName);
    CXCursorKind kind = clang_getCursorKind(cursor);
    auto* p = static_cast<MethodVisitorResult*>(pResult);
    if (kind == CXCursor_CXXMethod)
    {
        CXString name = clang_getCursorSpelling(cursor);
        std::string methodName = clang_getCString(name);
        if (methodName == "Read")
            p->hasRead = true;
        if (methodName == "Write")
            p->hasWrite = true;
        clang_disposeString(name);
    }
    return CXChildVisit_Continue;
}
struct FieldSerializeVisitorParams
{
    bool isSerialize = false;
    bool isDeserialize = false;
    bool isRead = false;
    bool isWrite = false;
};
CXChildVisitResult fieldValidateVisitor(CXCursor cursor, CXCursor parent, CXClientData pParams)
{
    const auto* params = static_cast<const FieldSerializeVisitorParams*>(pParams);
    CXCursorKind kind = clang_getCursorKind(cursor);
    if (kind == CXCursor_FieldDecl)
    {
        CXString name = clang_getCursorSpelling(cursor);
        CXType type = clang_getCursorType(cursor);
        CXString typeName = clang_getTypeSpelling(type);
        CXCursor typeDecl = clang_getTypeDeclaration(type);
        // Custom RW? Or POD
        MethodVisitorResult result{};
        // Reserved ones always do custom RW
        std::string_view typeNameStr = clang_getCString(typeName);
        for (const char* reserved : kMDRReservedRWStructs)
            if (typeNameStr.starts_with(reserved))
                result.hasRead = result.hasWrite = true;
        clang_visitChildren(typeDecl, methodVisitor, &result);
        if (result.hasWrite && result.hasRead)
        {
            if (params->isSerialize)
                println("        maxSize -= {}::Write(data.{}, &ptr, maxSize);", clang_getCString(typeName), clang_getCString(name));
            if (params->isWrite)
                println("        maxSize -= {}::Write(data.{}, ppDstBuffer, maxSize);", clang_getCString(typeName), clang_getCString(name));
            if (params->isDeserialize)
                println("        maxSize -= {}::Read(&data, out.{}, maxSize);", clang_getCString(typeName), clang_getCString(name));
            if (params->isRead)
                println("        maxSize -= {}::Read(ppSrcBuffer, out.{}, maxSize);", clang_getCString(typeName), clang_getCString(name));
        }
        else
        {
            if (params->isSerialize)
                println("        maxSize -= MDRPod::Write(data.{}, &ptr, maxSize);", clang_getCString(name));
            if (params->isWrite)
                println("        maxSize -= MDRPod::Write(data.{}, ppDstBuffer, maxSize);", clang_getCString(name));
            if (params->isDeserialize)
                println("        maxSize -= MDRPod::Read(&data, out.{}, maxSize);", clang_getCString(name));
            if (params->isRead)
                println("        maxSize -= MDRPod::Read(ppSrcBuffer, out.{}, maxSize);", clang_getCString(name));
        }
        clang_disposeString(name);
        clang_disposeString(typeName);
    }
    return CXChildVisit_Continue;
}
CXChildVisitResult structVisitor(CXCursor cursor, CXCursor parent, CXClientData)
{
    CXCursorKind kind = clang_getCursorKind(cursor);
    switch (kind)
    {
    case CXCursor_Namespace:
        return CXChildVisit_Recurse;
    case CXCursor_StructDecl:
    {
        std::string parentStr = getFullParentName(parent);
        if (parentStr != gNamespaceName)
            return CXChildVisit_Continue;
        CXString name = clang_getCursorSpelling(cursor);
        std::string structName = clang_getCString(name);
        /* Collect macros */
        auto [startLine, endLine] = getCursorExtents(cursor);
        auto start = std::ranges::lower_bound(gMacros, MacroPair{startLine,""});
        std::vector<std::string> macros;
        bool isExtern = false, isIgnored = false, isRWFieldStruct = false;
        for (auto it = start; it != gMacros.end() && it->first < endLine; ++it)
        {
            if (it->second == kMDRIgnoredMacro) isIgnored = true;
            if (it->second == kMDRExternMacro) isExtern = true;
            if (it->second == kMDRRWMacro) isRWFieldStruct = true;
        }
        FieldSerializeVisitorParams params{};
        if (isRWFieldStruct)
        {
            // Emit field RW bodies
            if (!isIgnored)
            {
                // Write
                // static size_t Write(const Type &data, UInt8** ppDstBuffer);
                println("    size_t {}::Write(const {}& data, UInt8** ppDstBuffer, size_t maxSize)", structName,structName);
                println("    {{");
                println("        UInt8* ptr = *ppDstBuffer;");
                params.isRead = false, params.isWrite = true;
                clang_visitChildren(cursor, fieldValidateVisitor, &params);
                println("        return *ppDstBuffer - ptr;");
                println("    }}");
                // Read
                // static void Read(UInt8** ppSrcBuffer, Type &out, size_t maxSize = ~0LL);
                println("    size_t {}::Read(const UInt8** ppSrcBuffer, {}& out, size_t maxSize)", structName,structName);
                println("    {{");
                println("        const UInt8* ptr = *ppSrcBuffer;");
                params.isRead = true, params.isWrite = false;
                clang_visitChildren(cursor, fieldValidateVisitor, &params);
                println("        return *ppSrcBuffer - ptr;");
                println("    }}");
            }
        } else
        {
            // Emit serialization bodies
            if (isExtern && !isIgnored)
            {
                // Serialize
                println("    size_t {}::Serialize(const {}& data, UInt8* out, size_t maxSize)", structName,structName);
                println("    {{");
                println("        UInt8* ptr = out;");
                params.isSerialize = true, params.isDeserialize = false;
                clang_visitChildren(cursor, fieldValidateVisitor, &params);
                println("        return ptr - out;");
                println("    }}");
                // Deserialize
                println("    void {}::Deserialize(const UInt8* data, {}& out, size_t maxSize)", structName,structName);
                println("    {{");
                params.isSerialize = false, params.isDeserialize = true;
                clang_visitChildren(cursor, fieldValidateVisitor, &params);
                println("    }}");
            }
            clang_disposeString(name);
        }
        return CXChildVisit_Continue;
    }
    default:
        return CXChildVisit_Continue;
    }
}

int main( int argc, char** argv )
{
    if (argc != 4)
    {
        println("usage: {} <source-file> <namespace-name> <include-name>", argv[0]);
        println("\tGenerate MDR_DEFINE_EXTERN_SERIALIZATION bodies for the given source file and namespace name.");
        println("\tOutput is printed to stdout.");
        return 1;
    }
    gSrc = argv[1];
    gNamespaceName = argv[2];
    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit unit = clang_parseTranslationUnit(
        index,
        gSrc.c_str(),
        nullptr, 0,
        nullptr, 0,
        CXTranslationUnit_DetailedPreprocessingRecord);
    CXCursor cursor = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(cursor, macroVisitor, nullptr);
    std::ranges::sort(gMacros);
    println("/* This file is auto-generated by tooling/SerializationCodegen.cpp */");
    println("#include <{}>", argv[3]);
    println("");
    println("namespace {} {{", gNamespaceName);
    clang_visitChildren(cursor, structVisitor, nullptr);
    println("}}");

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    return 0;
}
