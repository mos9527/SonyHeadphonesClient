#pragma once

#include <mdr-c/Connection.h>

struct MDRConnectionEmscripten;

extern "C" {
MDRConnectionEmscripten* clientPlatformEmscriptenConnectionCreate();
MDRConnection* clientPlatformEmscriptenConnectionGet(MDRConnectionEmscripten* connection);
void clientPlatformEmscriptenConnectionDestroy(MDRConnectionEmscripten* connection);
}
