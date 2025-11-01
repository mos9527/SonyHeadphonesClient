#pragma once
#include "../Connection.h"

typedef struct MDRConnectionEmscripten MDRConnectionEmscripten;

#ifdef __cplusplus
extern "C" {
#endif
    MDRConnectionEmscripten* mdrConnectionEmscriptenCreate();
    MDRConnection* mdrConnectionEmscriptenGet(MDRConnectionEmscripten*);
    void mdrConnectionEmscriptenDestroy(MDRConnectionEmscripten*);
#ifdef __cplusplus
}
#endif
