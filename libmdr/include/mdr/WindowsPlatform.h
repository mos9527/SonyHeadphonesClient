#pragma once
#include "Headphones.h"

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
