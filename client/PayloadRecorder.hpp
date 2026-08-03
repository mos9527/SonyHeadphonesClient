#pragma once

#include <mdr-c/Headphones.h>

bool clientPayloadRecorderConfigure(const char* directory);
void clientPayloadRecorderAttach(MDRHeadphones* headphones);
