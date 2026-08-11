#include <cstdio>
#include <cstdlib>
#include <memory>

void* operator new(std::size_t n) { return malloc(n); }
void* operator new(size_t const n, std::nothrow_t const&) noexcept { return malloc(n); }
void operator delete(void* p) { free(p); }
