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
const char* kMDRReservedIterableStructs[] = {
    "MDRPodArray",
    "MDRArray",
};
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

int gDepth = 0;
std::string emitIndent(){ return std::basic_string(gDepth * 4, ' '); }
void emitLineEnumIsValid(std::string_view scopeFieldName)
{
    println("{}if (!is_valid({})) {};", emitIndent(), scopeFieldName,
            emitThrowRuntimeError(format("{}", scopeFieldName), "Bad enum value"));
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

void emitCodegenCheck(CXCursor cursor, std::string const& fieldName, std::string const& check)
{
    CXString name = clang_getCursorSpelling(cursor);
    clang_disposeString(name);
    std::string scopeFiledName = fieldName;
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
                cout << format("{} != {}", scopeFiledName, tok);
                if (cin >> tok)
                    cout << " || ", diagOut << " ";
            }
            println("{}if ({}) {};", emitIndent(), cout.str(),
                    emitThrowRuntimeError(scopeFiledName, format("EnumRange check fail, must be one of {}", diagOut.str())));
            break;
        }
        case ValidationVerb::Range:
        {
            int mn, mx;
            cin >> mn >> mx;
            println("{}if ({} < {} || {} > {}) {};", emitIndent(), scopeFiledName, mn, scopeFiledName, mx,
                    emitThrowRuntimeError(scopeFiledName, format("Range check fail, must be in [{}, {}]", mn, mx)));
        }
        case ValidationVerb::Field:
        {
            cin >> tok;
            scopeFiledName = format("{}.{}", scopeFiledName, tok);
            break;
        }
        default:
            CHECK(false, format("Unexpected token {}", tok));
        }
    }
}
std::map<std::string, std::vector<std::string>> gCodegenComments;
CXChildVisitResult fieldValidateNestedVisitor(CXCursor cursor, CXCursor, CXClientData pData)
{
    auto* pParentName = static_cast<std::string*>(pData);
    if (clang_Cursor_getStorageClass(cursor) == CX_SC_Static)
        return CXChildVisit_Continue; // Ignore static members
    CXString name = clang_getCursorSpelling(cursor);
    CXType type = clang_getCursorType(cursor);
    CXCursor typeDecl = clang_getTypeDeclaration(type);
    CXCursorKind typeKind = clang_getCursorKind(typeDecl);
    CXString typeName = clang_getTypeSpelling(type);
    // Emit for-each loop for iterable types
    bool isIterable = false;
    std::string typeNameStr = clang_getCString(typeName);
    for (const char* reserved : kMDRReservedIterableStructs)
        if (typeNameStr.starts_with(reserved))
            isIterable = true;
    std::string newParentName = format("{}.{}", *pParentName, clang_getCString(name));
    std::string forClauseName = format("{}_elem", clang_getCString(name));
    if (isIterable)
    {
        // Deduce element type
        type = clang_Type_getTemplateArgumentAsType(type, 0);
        typeDecl = clang_getTypeDeclaration(type);
        typeKind = clang_getCursorKind(typeDecl);
        clang_disposeString(typeName);
        typeName = clang_getTypeSpelling(type);
        // Enter for-each clause
        println("{}for (const auto& {} : {}.{}) {{", emitIndent(), forClauseName, *pParentName, clang_getCString(name));
        gDepth++;
        newParentName = forClauseName;
    }
    switch (typeKind)
    {
    case CXCursor_EnumDecl:
        emitLineEnumIsValid(newParentName);
        break;
    case CXCursor_StructDecl:
    {
        clang_visitChildren(typeDecl, fieldValidateNestedVisitor, &newParentName);
        break;
    }
    default:
        break;
    }
    // Emit CODEGEN specific checks
    for (auto& check : gCodegenComments[clang_getCString(name)])
    {
        emitCodegenCheck(cursor, newParentName, check);
    }
    clang_disposeString(name);
    clang_disposeString(typeName);
    if (isIterable)
    {
        gDepth--;
        println("{}}}", emitIndent());
    }
    return CXChildVisit_Continue;
}

CXChildVisitResult fieldValidateVisitor(CXCursor cursor, CXCursor parent, CXClientData)
{
    using enum ValidationVerb;
    CXCursorKind kind = clang_getCursorKind(cursor);
    CXString name = clang_getCursorSpelling(cursor);
    std::string fieldName = clang_getCString(name);
    clang_disposeString(name);
    if (kind == CXCursor_FieldDecl)
    {
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
            gCodegenComments[fieldName].emplace_back(ln);
        }
        std::ranges::reverse(gCodegenComments[fieldName]);
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
            gCodegenComments.clear();
            // Collect comments
            clang_visitChildren(cursor, fieldValidateVisitor, nullptr);
            println("{}bool {}::Validate(const {}& data) {{", emitIndent(), structName, structName);
            gDepth++;
            std::string firstParent = "data";
            clang_visitChildren(cursor, fieldValidateNestedVisitor, &firstParent);
            println("{}return true;", emitIndent());
            gDepth--;
            println("{}}};", emitIndent());
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
