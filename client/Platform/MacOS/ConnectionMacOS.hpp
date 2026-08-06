#pragma once

#include <mdr-c/Connection.h>

struct MDRConnectionMacOS;

extern "C" {
MDRConnectionMacOS* clientPlatformMacOSConnectionCreate();
MDRConnection* clientPlatformMacOSConnectionGet(MDRConnectionMacOS* connection);
void clientPlatformMacOSConnectionDestroy(MDRConnectionMacOS* connection);
}
