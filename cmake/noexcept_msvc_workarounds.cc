#include <cstdlib>

[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xbad_alloc() { std::abort(); }
[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xinvalid_argument(_In_z_ const char*) { std::abort(); }
[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xlength_error(_In_z_ const char*) { std::abort(); }
[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xout_of_range(_In_z_ const char*) { std::abort(); }
[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xoverflow_error(_In_z_ const char*) { std::abort(); }
[[noreturn]] void __CLRCALL_PURE_OR_CDECL _Xruntime_error(_In_z_ const char*) { std::abort(); }
[[noreturn]] void __CLRCALL_PURE_OR_CDECL _XGetLastError() { std::abort(); }
