#pragma once
#include "../Connection.h"

typedef struct MDRConnectionMac MDRConnectionMac;

#ifdef __cplusplus
extern "C" {
#endif
MDRConnectionMac* mdrConnectionMacCreate();
MDRConnection* mdrConnectionMacGet(MDRConnectionMac*);
void mdrConnectionMacDestroy(MDRConnectionMac*);
#ifdef __cplusplus
}
#endif
