#pragma once
#include <mdr-c/Connection.h>

void clientPlatformInit();
void clientPlatformDestroy();

MDRConnection* clientPlatformConnectionGet();