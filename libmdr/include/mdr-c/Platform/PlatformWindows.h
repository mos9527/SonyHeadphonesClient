#pragma once
#include "../Connection.h"

typedef struct MDRConnectionWindows MDRConnectionWindows;

#ifdef __cplusplus
extern "C" {
#endif
MDR_PLATFORM_API MDRConnectionWindows* mdrConnectionWindowsCreate(void);
MDR_PLATFORM_API MDRConnection* mdrConnectionWindowsGet(MDRConnectionWindows* connection);
MDR_PLATFORM_API void mdrConnectionWindowsDestroy(MDRConnectionWindows* connection);
#ifdef __cplusplus
}
#endif
