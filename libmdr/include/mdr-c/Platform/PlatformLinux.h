#pragma once
#include "../Connection.h"

typedef struct MDRConnectionLinux MDRConnectionLinux;

#ifdef __cplusplus
extern "C" {
#endif
MDR_PLATFORM_API MDRConnectionLinux* mdrConnectionLinuxCreate(void);
MDR_PLATFORM_API MDRConnection* mdrConnectionLinuxGet(MDRConnectionLinux* connection);
MDR_PLATFORM_API void mdrConnectionLinuxDestroy(MDRConnectionLinux* connection);
#ifdef __cplusplus
}
#endif
