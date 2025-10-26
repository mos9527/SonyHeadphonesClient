#pragma once
#include "Headphones.h"

typedef struct MDRConnectionLinux MDRConnectionLinux;

#ifdef __cplusplus
extern "C" {
#endif
MDRConnectionLinux* mdrConnectionLinuxCreate();
MDRConnection* mdrConnectionLinuxGet(MDRConnectionLinux*);
void mdrConnectionLinuxDestroy(MDRConnectionLinux*);
#ifdef __cplusplus
}
#endif
