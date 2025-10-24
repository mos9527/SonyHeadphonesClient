#pragma once
#include "Headphones.h"

struct MDRConnectionLinux;

#ifdef __cplusplus
extern "C" {
#endif
MDRConnectionLinux* mdrConnectionLinuxCreate();
void mdrConnectionLinuxDestroy(MDRConnectionLinux*);

MDRConnection* mdrConnectionLinuxGet(MDRConnectionLinux*);
#ifdef __cplusplus
}
#endif
