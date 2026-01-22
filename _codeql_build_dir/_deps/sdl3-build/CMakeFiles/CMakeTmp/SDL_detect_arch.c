
#if defined(__EMSCRIPTEN__)
#define ARCH_EMSCRIPTEN "1"
#else
#define ARCH_EMSCRIPTEN "0"
#endif
const char *arch_EMSCRIPTEN = "INFO<EMSCRIPTEN=" ARCH_EMSCRIPTEN ">";

#if defined(__arm__) || defined(_M_ARM)
#define ARCH_ARM32 "1"
#else
#define ARCH_ARM32 "0"
#endif
const char *arch_ARM32 = "INFO<ARM32=" ARCH_ARM32 ">";

#if defined(__aarch64__) || defined(_M_ARM64)
#define ARCH_ARM64 "1"
#else
#define ARCH_ARM64 "0"
#endif
const char *arch_ARM64 = "INFO<ARM64=" ARCH_ARM64 ">";

#if defined(_M_ARM64EC)
#define ARCH_ARM64EC "1"
#else
#define ARCH_ARM64EC "0"
#endif
const char *arch_ARM64EC = "INFO<ARM64EC=" ARCH_ARM64EC ">";

#if defined(__loongarch64)
#define ARCH_LOONGARCH64 "1"
#else
#define ARCH_LOONGARCH64 "0"
#endif
const char *arch_LOONGARCH64 = "INFO<LOONGARCH64=" ARCH_LOONGARCH64 ">";

#if (defined(__PPC__) || defined(__powerpc__)) && !defined(__powerpc64__)
#define ARCH_POWERPC32 "1"
#else
#define ARCH_POWERPC32 "0"
#endif
const char *arch_POWERPC32 = "INFO<POWERPC32=" ARCH_POWERPC32 ">";

#if defined(__PPC64__) || defined(__powerpc64__)
#define ARCH_POWERPC64 "1"
#else
#define ARCH_POWERPC64 "0"
#endif
const char *arch_POWERPC64 = "INFO<POWERPC64=" ARCH_POWERPC64 ">";

#if defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) ||defined( __i386) || defined(_M_IX86)
#define ARCH_X86 "1"
#else
#define ARCH_X86 "0"
#endif
const char *arch_X86 = "INFO<X86=" ARCH_X86 ">";

#if (defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64) || defined(_M_AMD64)) && !defined(_M_ARM64EC)
#define ARCH_X64 "1"
#else
#define ARCH_X64 "0"
#endif
const char *arch_X64 = "INFO<X64=" ARCH_X64 ">";

int main(int argc, char *argv[]) {
  int result = 0;
  (void)argv;

  result += arch_EMSCRIPTEN[argc];
  result += arch_ARM32[argc];
  result += arch_ARM64[argc];
  result += arch_ARM64EC[argc];
  result += arch_LOONGARCH64[argc];
  result += arch_POWERPC32[argc];
  result += arch_POWERPC64[argc];
  result += arch_X86[argc];
  result += arch_X64[argc];
  return result;
}