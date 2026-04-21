#pragma once
#include "../Connection.h"

typedef struct MDRConnectionEmscriptenBLE MDRConnectionEmscriptenBLE;

#ifdef __cplusplus
extern "C" {
#endif
MDRConnectionEmscriptenBLE* mdrConnectionEmscriptenBLECreate();
MDRConnection* mdrConnectionEmscriptenBLEGet(MDRConnectionEmscriptenBLE*);
void mdrConnectionEmscriptenBLEDestroy(MDRConnectionEmscriptenBLE*);
#ifdef __cplusplus
}
#endif
