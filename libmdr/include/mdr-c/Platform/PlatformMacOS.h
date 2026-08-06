#pragma once
#include "../Connection.h"

typedef struct MDRConnectionMacOS MDRConnectionMacOS;

#ifdef __cplusplus
extern "C" {
#endif
MDR_PLATFORM_API MDRConnectionMacOS* mdrConnectionMacOSCreate(void);
MDR_PLATFORM_API MDRConnection* mdrConnectionMacOSGet(MDRConnectionMacOS* connection);
MDR_PLATFORM_API void mdrConnectionMacOSDestroy(MDRConnectionMacOS* connection);
#ifdef __cplusplus
}
#endif
