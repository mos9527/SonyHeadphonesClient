#include "Codegen.hpp"
// Q: Why are these globals?
// A: We don't need to capture these in lambdas.
std::string gSrc = "libmdr/ProtocolV2T1Enums.hpp";
std::string gNamespaceName = "mdr::v2::t1";

std::string gPrevDeclName;
CXCursorVisitor gVisitIn, gVisit, gVisitOut;
CXChildVisitResult structVisitor(CXCursor cursor, CXCursor parent, CXClientData)
{
    CXCursorKind kind = clang_getCursorKind(cursor);
    switch (kind)
    {
    case CXCursor_Namespace:
        return CXChildVisit_Recurse;
    case CXCursor_EnumDecl:
        {
            std::string parentStr = getFullParentName(parent);
            if (parentStr != gNamespaceName)
                return CXChildVisit_Continue;
            CXString name = clang_getCursorSpelling(cursor);
            if (!gPrevDeclName.empty() && gPrevDeclName != clang_getCString(name))
                gVisitOut(cursor, parent, nullptr);
            gVisitIn(cursor, parent, nullptr);
            clang_visitChildren(cursor, gVisit, nullptr);
            gPrevDeclName = clang_getCString(name);
            clang_disposeString(name);
            return CXChildVisit_Continue;
        }
    default:
        return CXChildVisit_Continue;
    }
}
void doVisit(CXCursor cursor, CXCursorVisitor in, CXCursorVisitor visit, CXCursorVisitor out)
{
    gVisit = visit, gVisitIn = in, gVisitOut = out;
    gPrevDeclName.clear();
    clang_visitChildren(cursor, structVisitor, nullptr);
    out(cursor, cursor, nullptr);
}

CXChildVisitResult enumNameIn(CXCursor c, CXCursor, CXClientData)
{
    CXString name = clang_getCursorSpelling(c);
    println("    static const char* format_as({} value) {{", clang_getCString(name));
    println("        using enum {};", clang_getCString(name));
    println("        switch (value) {{");
    clang_disposeString(name);
    return CXChildVisit_Continue;
}
CXChildVisitResult enumNameVisit(CXCursor c, CXCursor, CXClientData)
{
    CXString enumName = clang_getCursorDisplayName(c);
    println("            case {}: return \"{}\";", clang_getCString(enumName), clang_getCString(enumName));
    clang_disposeString(enumName);
    return CXChildVisit_Continue;
}
CXChildVisitResult enumNameOut(CXCursor, CXCursor, CXClientData)
{
    println("            default: return \"Unknown\";");
    println("        }}");
    println("    }}");
    return CXChildVisit_Continue;
}
// XXX: This would probably never be used.
#ifdef CODEGEN_ENUM_BITMASK
uint8_t gEnumRangeBitmask = 0u;
bool gEnumIsBitmask = true;
uint32_t gEnumCount = 0;
std::string gEnumCursorName;
#endif
CXChildVisitResult enumRangeIn(CXCursor c, CXCursor, CXClientData)
{
    CXString name = clang_getCursorSpelling(c);
    println("    static bool is_valid({} value) {{", clang_getCString(name));
    println("        using enum {};", clang_getCString(name));
    println("        switch (value) {{");
#ifdef CODEGEN_ENUM_BITMASK
    gEnumRangeBitmask = 0u;
    gEnumIsBitmask = true;
    gEnumCursorName = clang_getCString(name);
    gEnumCount = 0u;
#endif
    clang_disposeString(name);
    return CXChildVisit_Continue;
}
CXChildVisitResult enumRangeVisit(CXCursor c, CXCursor, CXClientData)
{
    CXString enumName = clang_getCursorDisplayName(c);
    uint8_t enumVal = static_cast<uint8_t>(clang_getEnumConstantDeclUnsignedValue(c));
    println("            case {}:", clang_getCString(enumName), clang_getCString(enumName));
#ifdef CODEGEN_ENUM_BITMASK
    gEnumIsBitmask &= (enumVal == 0 || (enumVal & (enumVal - 1)) == 0); // power of 2
    gEnumRangeBitmask |= enumVal;
    gEnumCount++;
    clang_disposeString(enumName);
#endif
    return CXChildVisit_Continue;
}
CXChildVisitResult enumRangeOut(CXCursor, CXCursor, CXClientData)
{
    println("            return true;");
    println("        default:");
    println("           return false;");
    println("        }}");
    println("    }}");
#ifdef CODEGEN_ENUM_BITMASK
    // Bitmask version
    // Not always generated. Only when all enum values are powers of 2 and we have more than 1 value
    if (gEnumIsBitmask && gEnumCount > 1)
    {
        println("    static bool is_valid_bitmask({} value) {{", gEnumCursorName);
        println("           return !(static_cast<uint8_t>(value) & 0x{:02x});",
                static_cast<uint8_t>(~gEnumRangeBitmask) /* flipped for subset test */);
        println("    }}");
    }
#endif
    return CXChildVisit_Continue;
}
int main(int argc, char** argv)
{
    if (argc != 3)
    {
        println("usage: {} <source-file> <namespace-name>", argv[0]);
        println("\tGenerate enum helper functions from the given source file and namespace name.");
        println("\tOutput is printed to stdout.");
        return 1;
    }
    gSrc = argv[1];
    gNamespaceName = argv[2];
    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit unit =
        clang_parseTranslationUnit(index, gSrc.c_str(), nullptr, 0, nullptr, 0, CXTranslationUnit_None);
    CXCursor cursor = clang_getTranslationUnitCursor(unit);
    println("/* This file is auto-generated by tooling/EnumCodegen.cpp */");
    println("#pragma once");
    println("");
    println("namespace {} {{", gNamespaceName);
    doVisit(cursor, enumNameIn, enumNameVisit, enumNameOut);
    doVisit(cursor, enumRangeIn, enumRangeVisit, enumRangeOut);
    println("}}");
    std::fflush(stdout);

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    return 0;
}
