add_library(Client_I18N STATIC
    Strings.hpp
    Details.hpp
    Strings.cpp
    Default.cpp
    SimplifiedChinese.cpp
    TraditionalChinese.cpp
    Japanese.cpp
    Korean.cpp
)
target_compile_features(Client_I18N PUBLIC cxx_std_20)
target_include_directories(Client_I18N PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
if (MSVC)
    target_compile_options(Client_I18N PRIVATE /utf-8)
endif ()
