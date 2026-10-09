#include "Strings.hpp"

namespace i18n
{
namespace
{
constexpr std::array<const StringTable*, static_cast<std::size_t>(AppLocale::NUM_LOCALES)> kStringTables = []
{
    std::array<const StringTable*, static_cast<std::size_t>(AppLocale::NUM_LOCALES)> tables{};
    tables[static_cast<std::size_t>(AppLocale::UNDEFINED)] = &kEnglish;
    tables[static_cast<std::size_t>(AppLocale::SIMPLIFIED_CHINESE)] = &kSimplifiedChinese;
    tables[static_cast<std::size_t>(AppLocale::TRADITIONAL_CHINESE)] = &kTraditionalChinese;
    tables[static_cast<std::size_t>(AppLocale::JAPANESE)] = &kJapanese;
    tables[static_cast<std::size_t>(AppLocale::KOREAN)] = &kKorean;
    return tables;
}();
}

const char* Translate(TextId id, AppLocale locale)
{
    const std::size_t index = Index(id);
    if (index >= kEnglish.size())
        return "";

    const std::size_t localeIndex = static_cast<std::size_t>(locale);
    if (localeIndex < kStringTables.size())
    {
        const StringTable* table = kStringTables[localeIndex];
        if (table)
        {
            const char* text = (*table)[index];
            if (text && *text)
                return text;
        }
    }
    return kEnglish[index];
}
}
