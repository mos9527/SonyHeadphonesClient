#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <map>
#include "Codegen.hpp"

enum class ValidationVerb
{
    CODEGEN,
    EnumRange,
    Range,
    Field
};

std::map<std::string, ValidationVerb> kCodegenTokens = {
    {"CODEGEN", ValidationVerb::CODEGEN},
    {"EnumRange", ValidationVerb::EnumRange},
    {"Range", ValidationVerb::Range},
    {"Field", ValidationVerb::Field},
};
const char* kCODEGEN = "CODEGEN";

std::string gSrc = "libmdr/ProtocolV2T1Enums.hpp";
std::string gNamespaceName = "mdr::v2::t1";
std::vector<std::string> gSource; // Source lines

#define CHECK(EXPR, MSG) \
    do { \
        if (!(EXPR)) { \
            fmt::print(stderr, "Error: {} at {}:{}\n", MSG, __FILE__, __LINE__); \
            std::exit(1); \
        } \
    } while (0)

void trimCommentString(std::string& s)
{
    s.erase(s.begin(), s.begin() + s.find_last_of("/") + 1);
    s.erase(s.begin(), s.begin() + s.find_first_of(" ") + 1);
}

std::string emitThrowRuntimeError(std::string_view name, std::string_view msg)
{
    return format("throw std::runtime_error(\"{}: {}\")", name, msg);
}

void emitLineEnumIsValid(std::string_view parentName, std::string_view fieldName)
{
    println("        if (!is_valid({}.{})) {};", parentName, fieldName,
            emitThrowRuntimeError(format("{}.{}", parentName, fieldName), "Bad enum value"));
}

void emitCodegenCheck(CXCursor cursor, std::string const& check)
{
    CXString name = clang_getCursorSpelling(cursor);
    std::string fieldName = format("data.{}", clang_getCString(name));
    clang_disposeString(name);

    std::stringstream cin(check);
    std::string tok;
    cin >> tok; // CODEGEN
    CHECK(kCodegenTokens[tok] == ValidationVerb::CODEGEN, "Expected CODEGEN token");
    while (cin)
    {
        cin >> tok; // Verb
        ValidationVerb verb = kCodegenTokens[tok];
        switch (verb)
        {
        case ValidationVerb::EnumRange:
        {
            std::ostringstream cout, diagOut;
            while (cin >> tok)
            {
                diagOut << tok;
                cout << format("{} != {}", fieldName, tok);
                if (cin >> tok)
                    cout << " || ", diagOut << " ";
            }
            println("        if ({}) {};", cout.str(),
                    emitThrowRuntimeError(fieldName, format("EnumRange check fail, must be one of {}", diagOut.str())));
            break;
        }
        case ValidationVerb::Range:
        {
            int mn, mx;
            cin >> mn >> mx;
        }
        case ValidationVerb::Field:
        {
            cin >> tok;
            fieldName = format("{}.{}", fieldName, tok);
            break;
        }
        default:
            CHECK(false, format("Unexpected token {}", tok));
        }
    }
}

CXChildVisitResult fieldValidateNestedVisitor(CXCursor cursor, CXCursor parent, CXClientData pData)
{
    auto* pParentName = static_cast<std::string*>(pData);
    CXString name = clang_getCursorSpelling(cursor);
    CXType type = clang_getCursorType(cursor);
    CXCursor typeDecl = clang_getTypeDeclaration(type);
    CXCursorKind typeKind = clang_getCursorKind(typeDecl);
    CXString typeName = clang_getTypeSpelling(type);
    // if (kind == CXCursor_FieldDecl)
    //     println("        // {}.{}", *pParentName, clang_getCString(name));
    switch (typeKind)
    {
    case CXCursor_EnumDecl:
        emitLineEnumIsValid(*pParentName, clang_getCString(name));
        break;
    case CXCursor_StructDecl:
    {
        std::string newParentName = format("{}.{}", *pParentName, clang_getCString(name));
        clang_visitChildren(typeDecl, fieldValidateNestedVisitor, &newParentName);
        break;
    }
    }
    clang_disposeString(name);
    clang_disposeString(typeName);
    return CXChildVisit_Continue;
}

void emitValidationForField(CXCursor cursor, const std::vector<std::string>& comments)
{
    CXString name = clang_getCursorSpelling(cursor);
    CXType type = clang_getCursorType(cursor);
    CXCursor typeDecl = clang_getTypeDeclaration(type);
    CXCursorKind typeKind = clang_getCursorKind(typeDecl);
    CXString typeName = clang_getTypeSpelling(type);
    // println("        // data.{}", clang_getCString(name));
    switch (typeKind)
    {
    case CXCursor_EnumDecl:
    {
        // Enums should always be in range
        emitLineEnumIsValid("data", clang_getCString(name));
        break;
    }
    case CXCursor_StructDecl:
    {
        // Nested struct. Enums should apply as well.
        std::string parentName = format("data.{}", clang_getCString(name));
        clang_visitChildren(typeDecl, fieldValidateNestedVisitor, &parentName);
    }
    default:
        break;
    }
    // Parse CODEGEN checks
    // These are not necessarily stricter than default checks above
    for (auto& comment : comments)
        emitCodegenCheck(cursor, comment);
    clang_disposeString(typeName);
    clang_disposeString(name);
}

CXChildVisitResult fieldValidateVisitor(CXCursor cursor, CXCursor parent, CXClientData)
{
    using enum ValidationVerb;
    CXCursorKind kind = clang_getCursorKind(cursor);
    if (kind == CXCursor_FieldDecl)
    {
        std::vector<std::string> comments;
        CXSourceLocation loc = clang_getCursorLocation(cursor);
        auto [lineMin, lineMax] = getCursorExtents(parent);
        CXFile file;
        unsigned line, col, offset;
        clang_getSpellingLocation(loc, &file, &line, &col, &offset);
        for (line = line - 1; line >= lineMin; line--)
        {
            auto& ln = gSource[line];
            trimCommentString(ln);
            if (!ln.starts_with(kCODEGEN))
                break;
            comments.emplace_back(ln);
        }
        std::ranges::reverse(comments);
        emitValidationForField(cursor, comments);
    }
    return CXChildVisit_Continue;
}

struct MethodVisitorResult
{
    bool hasValidate = false;
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
        if (methodName == "Validate")
            p->hasValidate = true;
        clang_disposeString(name);
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
        MethodVisitorResult methods;
        clang_visitChildren(cursor, methodVisitor, &methods);
        // Emit Validate bodies
        if (methods.hasValidate)
        {
            println("    bool {}::Validate(const {}& data) {{", structName,structName);
            clang_visitChildren(cursor, fieldValidateVisitor, nullptr);
            println("        return true;");
            println("    }};");
        }
        clang_disposeString(name);
        return CXChildVisit_Continue;
    }
    default:
        return CXChildVisit_Continue;
    }
}

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        println("usage: {} <source-file> <namespace-name>", argv[0]);
        println("\tGenerate Validation(const T&) bodies for the given source file and namespace name.");
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
        CXTranslationUnit_IncludeBriefCommentsInCodeCompletion);
    CXCursor cursor = clang_getTranslationUnitCursor(unit);
    // Read into gSource
    {
        gSource.emplace_back(); // 1-based line numbers
        std::ifstream infile(gSrc);
        std::string line;
        while (std::getline(infile, line))
            gSource.push_back(line);
    }
    println("/* This file is auto-generated by tooling/SerializationCodegen.cpp */");
    println("#include \"{}\"", gSrc);
    println("");
    println("namespace {} {{", gNamespaceName);
    clang_visitChildren(cursor, structVisitor, nullptr);
    println("}}");

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    return 0;
}
