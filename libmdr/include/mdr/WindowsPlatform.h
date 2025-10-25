#pragma once
#include "Headphones.h"

typedef struct MDRConnectionWindows MDRConnectionWindows;

#ifdef __cplusplus
extern "C" {
#endif
MDRConnectionWindows* mdrConnectionWindowsCreate();
void mdrConnectionWindowsDestroy(MDRConnectionWindows*);

MDRConnection* mdrConnectionWindowsGet(MDRConnectionWindows*);
#ifdef __cplusplus
}
#endif
