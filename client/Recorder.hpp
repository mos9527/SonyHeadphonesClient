#pragma once

#include <mdr-c/Headphones.h>

bool clientPayloadRecorderConfigure(const char* directory);
void clientPayloadRecorderObserve(
    MDRPacketDirection direction,
    const unsigned char* frame,
    int frameSize
);
