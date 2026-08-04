#include <algorithm>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>
#include "Codegen.hpp"

enum class ValidationVerb
{
    CODEGEN,
    EnumRange,
    Range,
    Field,
    Ignore,
#ifdef CODEGEN_ENUM_BITMASK
    Bitmask
#endif
};

constexpr uint32_t kValidationFlagNONE = 0;
constexpr uint32_t kValidationFlagIGNORE = 1u << 0;
#ifdef CODEGEN_ENUM_BITMASK
constexpr uint32_t kValidationFlagBITMASK = 1u << 1;
#endif

std::map<std::string, ValidationVerb> kCodegenTokens = {
    // emitCodegenCheck
    {"CODEGEN", ValidationVerb::CODEGEN},
    {"EnumRange", ValidationVerb::EnumRange},
    {"Range", ValidationVerb::Range},
    {"Field", ValidationVerb::Field},
    // collectCodegenFlags
    {"Ignore", ValidationVerb::Ignore},
#ifdef CODEGEN_ENUM_BITMASK
    {"Bitmask", ValidationVerb::Bitmask},
#endif
};

const char* kCODEGEN = "CODEGEN";
const char* kMDRReservedIterableStructs[] = {"MDRPodArray", "MDRArray", "MDRFixedArray"};
std::string gSrc = "libmdr/ProtocolV2T1Enums.hpp";
std::string gNamespaceName = "mdr::v2::t1";
std::vector<std::string> gSource; // Source lines

#define CHECK(EXPR, MSG)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        if (!(EXPR))                                                                                                   \
        {                                                                                                              \
            fmt::print(stderr, "Error: {} at {}:{}\n", MSG, __FILE__, __LINE__);                                       \
            std::exit(1);                                                                                              \
        }                                                                                                              \
    }                                                                                                                  \
    while (0)

void trimCommentString(std::string& s)
{
    if (s.find_last_of("/") != std::string::npos)
        s.erase(s.begin(), s.begin() + s.find_last_of("/") + 1);
    if (s.find_first_of(" ") != std::string::npos)
        s.erase(s.begin(), s.begin() + s.find_first_of(" ") + 1);
    while (!s.empty() && (s.back() == ' ' || s.back() == '\r' || s.back() == '\n'))
        s.pop_back();
}

int gDepth = 0;
std::string emitIndent() { return std::basic_string(gDepth * 4, ' '); }
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
        const char* rawName = clang_getCString(name);
        std::string methodName = rawName ? rawName : "";
        if (methodName == "Validate")
            p->hasValidate = true;
        clang_disposeString(name);
    }
    return CXChildVisit_Continue;
}
struct CodegenFlags
{
    uint32_t flags{kValidationFlagNONE};
    std::string ignoreReason;
};

CodegenFlags collectCodegenFlags(CXCursor, std::string const& check)
{
    std::stringstream cin(check);
    std::string tok;
    cin >> tok; // CODEGEN
    CHECK(kCodegenTokens[tok] == ValidationVerb::CODEGEN, "Expected CODEGEN token");
    CodegenFlags result{};
    while (cin >> tok)
    {
        auto it = kCodegenTokens.find(tok);
        if (it == kCodegenTokens.end())
            continue;
        switch (it->second)
        {
        case ValidationVerb::Ignore:
            {
                result.flags |= kValidationFlagIGNORE;
                std::string reason;
                std::getline(cin, reason);
                while (!reason.empty() && (reason.front() == ' ' || reason.front() == '\t'))
                    reason.erase(reason.begin());
                while (!reason.empty() && (reason.back() == ' ' || reason.back() == '\r' || reason.back() == '\n'))
                    reason.pop_back();
                CHECK(!reason.empty(), "CODEGEN Ignore requires a reason");
                result.ignoreReason = std::move(reason);
                return result;
            }
#ifdef CODEGEN_ENUM_BITMASK
        case ValidationVerb::Bitmask:
            {
                result.flags |= kValidationFlagBITMASK;
                break;
            }
#endif
        default:
            break;
        }
    }
    return result;
}
void emitCodegenCheck(CXCursor cursor, std::string const& fieldName, std::string const& check, uint32_t flags)
{
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
                std::ostringstream cout;
                cin >> tok;
                while (true)
                {
                    cout << format("{} == {}", scopeFiledName, tok);
                    if (cin >> tok)
                        cout << " || ";
                    else
                        break;
                }
                println("{}MDR_VALIDATE({});", emitIndent(), cout.str());
                break;
            }
        case ValidationVerb::Range:
            {
                int mn, mx;
                cin >> mn >> mx;
                println("{}MDR_VALIDATE({} >= {} && {} <= {});", emitIndent(), scopeFiledName, mn, scopeFiledName, mx);
                break;
            }
        case ValidationVerb::Field:
            {
                cin >> tok;
                scopeFiledName = format("{}.{}", scopeFiledName, tok);
                break;
            }
        default:
            break;
        }
    }
}
std::map<std::string, std::vector<std::string>> gCodegenComments;
int gVisitDepth = 0;
struct ValidateVisitorCD
{
    std::string* pParentName;
    uint32_t flags;
};
CXChildVisitResult fieldValidateNestedVisitor(CXCursor cursor, CXCursor, CXClientData pData)
{
    auto* pCD = static_cast<ValidateVisitorCD*>(pData);
    auto [pParentName, flags] = *pCD;
    if (clang_Cursor_getStorageClass(cursor) == CX_SC_Static)
        return CXChildVisit_Continue; // Ignore static members
    CXString name = clang_getCursorSpelling(cursor);
    const char* rawName = clang_getCString(name);
    const std::string fieldName = rawName ? rawName : "";
    CXType type = clang_getCursorType(cursor);
    CXCursor typeDecl = clang_getTypeDeclaration(type);
    CXCursorKind typeKind = clang_getCursorKind(typeDecl);
    CXString typeName = clang_getTypeSpelling(type);
    // Emit for-each loop for iterable types
    bool isIterable = false;
    const char* rawTypeName = clang_getCString(typeName);
    std::string typeNameStr = rawTypeName ? rawTypeName : "";
    for (const char* reserved : kMDRReservedIterableStructs)
        if (typeNameStr.starts_with(reserved))
            isIterable = true;
    std::string newParentName = format("{}.{}", *pParentName, fieldName);
    std::string forClauseName = format("{}_elem", fieldName);
    if (isIterable)
    {
        // Deduce element type
        type = clang_Type_getTemplateArgumentAsType(type, 0);
        typeDecl = clang_getTypeDeclaration(type);
        typeKind = clang_getCursorKind(typeDecl);
        clang_disposeString(typeName);
        typeName = clang_getTypeSpelling(type);
        // Enter for-each clause
        println("{}for (const auto& {} : {}.{}) {{", emitIndent(), forClauseName, *pParentName, fieldName);
        gDepth++;
        newParentName = forClauseName;
    }
    CodegenFlags fieldValidation{};
    for (auto& check : gCodegenComments[fieldName])
    {
        CodegenFlags parsed = collectCodegenFlags(cursor, check);
        fieldValidation.flags |= parsed.flags;
        if (!parsed.ignoreReason.empty())
            fieldValidation.ignoreReason = std::move(parsed.ignoreReason);
    }
    ValidateVisitorCD CD{&newParentName, fieldValidation.flags};
    if (!(fieldValidation.flags & kValidationFlagIGNORE))
    {
        switch (typeKind)
        {
        case CXCursor_EnumDecl:
        {
#ifdef CODEGEN_ENUM_BITMASK
            if ((fieldValidation.flags & kValidationFlagBITMASK))
                println("{}MDR_VALIDATE(is_valid_bitmask({}));", emitIndent(), newParentName);
            else
#endif
                println("{}MDR_VALIDATE(is_valid({}));", emitIndent(), newParentName);
            break;
        }
        case CXCursor_StructDecl:
            {
                gVisitDepth++;
                clang_visitChildren(typeDecl, fieldValidateNestedVisitor, &CD);
                gVisitDepth--;
                break;
            }
        default:
            break;
        }
        // Emit CODEGEN specific checks
        // We only do this at the top level to avoid duplicate field names
        if (gVisitDepth == 0)
            for (auto& check : gCodegenComments[fieldName])
                emitCodegenCheck(cursor, newParentName, check, fieldValidation.flags);
    }
    else
    {
        println("{}// {} ignored: {}", emitIndent(), newParentName, fieldValidation.ignoreReason);
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
    const char* rawName = clang_getCString(name);
    std::string fieldName = rawName ? rawName : "";
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
            const char* rawName = clang_getCString(name);
            std::string structName = rawName ? rawName : "";
            MethodVisitorResult methods;
            clang_visitChildren(cursor, methodVisitor, &methods);
            // Emit Validate bodies
            if (methods.hasValidate)
            {
                gCodegenComments.clear();
                // Collect comments
                clang_visitChildren(cursor, fieldValidateVisitor, nullptr);
                println("{}MDRResult<void> {}::Validate(const {}& data) {{", emitIndent(), structName, structName);
                gDepth++;
                std::string firstParent = "data";
                ValidateVisitorCD CD{&firstParent, kValidationFlagNONE};
                clang_visitChildren(cursor, fieldValidateNestedVisitor, &CD);
                println("{}return MDRResult<void>::Success();", emitIndent());
                gDepth--;
                println("{}}}", emitIndent());
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
    if (argc != 4)
    {
        println("usage: {} <source-file> <namespace-name> <include-name>", argv[0]);
        println("\tGenerate Validation(const T&) bodies for the given source file and namespace name.");
        println("\tOutput is printed to stdout.");
        return 1;
    }
    gSrc = argv[1];
    gNamespaceName = argv[2];
    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit unit = clang_parseTranslationUnit(index, gSrc.c_str(), nullptr, 0, nullptr, 0,
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
    println("/* This file is auto-generated by tooling/ValidationCodegen.cpp */");
    println("#include <{}>", argv[3]);
    println("#include \"../Details.hpp\"");
    println("");
    println("namespace {} {{", gNamespaceName);
    clang_visitChildren(cursor, structVisitor, nullptr);
    println("}}");
    std::fflush(stdout);

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);
    return 0;
}
