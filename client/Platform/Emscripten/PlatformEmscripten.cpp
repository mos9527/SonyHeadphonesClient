#include "../Platform.hpp"

#include <cstdio>
#include <emscripten.h>
#include <emscripten/html5.h>

void clientPlatformInit(){}
void clientPlatformDestroy() {}
MDRConnection* clientPlatformConnectionGet()
{
    return nullptr;
}