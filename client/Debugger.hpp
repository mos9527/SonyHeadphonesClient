#pragma once

#include <mdr-c/Headphones.h>

struct ImFont;

void clientDebuggerSetMonospaceFont(ImFont* font);
void clientDebuggerAttach(MDRHeadphones* headphones);
void clientDebuggerDetach();
void clientDebuggerObservePacket(MDRPacketDirection direction, const unsigned char* frame, int frameSize);
void clientDebuggerDraw(bool* open);
