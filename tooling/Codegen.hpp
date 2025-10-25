#pragma once

#include <clang-c/Index.h>
#include <fmt/base.h>
#include <fmt/format.h>
#include <string>
#include <string_view>

using namespace fmt;

// Full parent name up to cursor joined by '::'
inline std::string getFullParentName(CXCursor c)
{
    std::string s;
    while (true)
    {
        CXString name = clang_getCursorSpelling(c);
        std::string nameStr = clang_getCString(name);
        clang_disposeString(name);
        if (nameStr.empty())
            break;
        if (clang_getCursorKind(c) != CXCursor_TranslationUnit)
            s = nameStr + (s.empty() ? "" : "::") + s;
        c = clang_getCursorLexicalParent(c);
        if (clang_equalCursors(c, clang_getNullCursor()))
            break;
    }
    return s;
}

inline std::pair<unsigned,unsigned> getCursorExtents(CXCursor cursor)
{
    CXSourceRange range = clang_getCursorExtent(cursor);
    CXSourceLocation startLoc = clang_getRangeStart(range);
    CXSourceLocation endLoc = clang_getRangeEnd(range);
    CXFile file; unsigned startLine, endLine, col, offset;
    clang_getSpellingLocation(startLoc, &file, &startLine, &col, &offset);
    clang_getSpellingLocation(endLoc, &file, &endLine, &col, &offset);
    return {startLine, endLine};
}