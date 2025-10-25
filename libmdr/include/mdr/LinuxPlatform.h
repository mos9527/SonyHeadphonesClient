#pragma once
#include "Headphones.h"

typedef struct MDRConnectionLinux MDRConnectionLinux;

#ifdef __cplusplus
extern "C" {
#endif
MDRConnectionLinux* mdrConnectionLinuxCreate();
void mdrConnectionLinuxDestroy(MDRConnectionLinux*);

MDRConnection* mdrConnectionLinuxGet(MDRConnectionLinux*);
#ifdef __cplusplus
}
#endif
