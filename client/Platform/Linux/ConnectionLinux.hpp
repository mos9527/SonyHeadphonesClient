#pragma once

#include <mdr-c/Connection.h>

struct MDRConnectionLinux;

extern "C" {
MDRConnectionLinux* clientPlatformLinuxConnectionCreate();
MDRConnection* clientPlatformLinuxConnectionGet(MDRConnectionLinux* connection);
void clientPlatformLinuxConnectionDestroy(MDRConnectionLinux* connection);
}
