#pragma once

#include <mdr-c/Headphones.h>

bool clientPayloadRecorderConfigure(const char* directory);
void clientPayloadRecorderObserve(
    MDRPacketDirection direction,
    uint64_t packetId,
    const unsigned char* frame,
    int frameSize
);
