#pragma once

#include <cstddef>

#include <mdr-c/Headphones.h>

struct ImFont;

void clientDebuggerSetMonospaceFont(ImFont* font);
void clientDebuggerAttach(MDRHeadphones* headphones);
void clientDebuggerDetach();
void clientDebuggerObservePacket(MDRPacketDirection direction, const unsigned char* frame, int frameSize);
bool clientDebuggerReplayPath(const char* path, size_t* packetCount = nullptr);
bool clientDebuggerReplayDirectory(const char* directory, size_t* packetCount = nullptr);
void clientDebuggerDraw(bool* open);
