#pragma once
#include "../Connection.h"

typedef struct MDRConnectionWindowsBLE MDRConnectionWindowsBLE;

#ifdef __cplusplus
extern "C" {
#endif
MDR_PLATFORM_API MDRConnectionWindowsBLE* mdrConnectionWindowsBLECreate(void);
MDR_PLATFORM_API MDRConnection* mdrConnectionWindowsBLEGet(MDRConnectionWindowsBLE* connection);
MDR_PLATFORM_API void mdrConnectionWindowsBLEDestroy(MDRConnectionWindowsBLE* connection);
#ifdef __cplusplus
}
#endif
