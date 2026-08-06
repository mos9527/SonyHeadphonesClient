#pragma once

#include <mdr-c/Connection.h>

struct MDRConnectionWindowsBLE;

extern "C" {
MDRConnectionWindowsBLE* clientPlatformWindowsBLEConnectionCreate();
MDRConnection* clientPlatformWindowsBLEConnectionGet(MDRConnectionWindowsBLE* connection);
void clientPlatformWindowsBLEConnectionDestroy(MDRConnectionWindowsBLE* connection);
}
