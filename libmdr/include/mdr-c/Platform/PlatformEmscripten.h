#pragma once
#include "../Connection.h"

typedef struct MDRConnectionEmscripten MDRConnectionEmscripten;

#ifdef __cplusplus
extern "C" {
#endif
MDR_PLATFORM_API MDRConnectionEmscripten* mdrConnectionEmscriptenCreate(void);
MDR_PLATFORM_API MDRConnection* mdrConnectionEmscriptenGet(MDRConnectionEmscripten* connection);
MDR_PLATFORM_API void mdrConnectionEmscriptenDestroy(MDRConnectionEmscripten* connection);
#ifdef __cplusplus
}
#endif
