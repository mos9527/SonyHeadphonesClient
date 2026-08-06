#pragma once

#include <mdr-c/Connection.h>

struct MDRConnectionWindows;

extern "C" {
MDRConnectionWindows* clientPlatformWindowsConnectionCreate();
MDRConnection* clientPlatformWindowsConnectionGet(MDRConnectionWindows* connection);
void clientPlatformWindowsConnectionDestroy(MDRConnectionWindows* connection);
}
