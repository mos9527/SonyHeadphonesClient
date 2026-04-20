#pragma once
#include "../Connection.h"

typedef struct MDRConnectionWindowsBLE MDRConnectionWindowsBLE;

#ifdef __cplusplus
extern "C" {
#endif
MDRConnectionWindowsBLE* mdrConnectionWindowsBLECreate();
MDRConnection* mdrConnectionWindowsBLEGet(MDRConnectionWindowsBLE*);
void mdrConnectionWindowsBLEDestroy(MDRConnectionWindowsBLE*);
#ifdef __cplusplus
}
#endif
