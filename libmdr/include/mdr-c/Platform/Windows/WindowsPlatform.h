#pragma once
#include "../../Connection.h"

typedef struct MDRConnectionWindows MDRConnectionWindows;

#ifdef __cplusplus
extern "C" {
#endif
MDRConnectionWindows* mdrConnectionWindowsCreate();
MDRConnection* mdrConnectionWindowsGet(MDRConnectionWindows*);
void mdrConnectionWindowsDestroy(MDRConnectionWindows*);
#ifdef __cplusplus
}
#endif
