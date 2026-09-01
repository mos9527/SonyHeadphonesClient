#pragma once

#include <cstddef>

#include <mdr-c/Headphones.h>

struct ImFont;
struct SDL_Window;

void clientDebuggerSetMonospaceFont(ImFont* font);
void clientDebuggerSetWindow(SDL_Window* window);
void clientDebuggerAttach(MDRHeadphones* headphones);
void clientDebuggerDetach();
void clientDebuggerObservePacket(MDRPacketDirection direction, const unsigned char* frame, int frameSize);
bool clientDebuggerReplayPath(const char* path, size_t* packetCount = nullptr);
bool clientDebuggerReplayDirectory(const char* directory, size_t* packetCount = nullptr);
bool clientDebuggerWritePacketFile(const char* path, const unsigned char* frame, size_t frameSize);
bool clientDebuggerHasPackets();
bool clientDebuggerExportInProgress();
bool clientDebuggerExportLatestPacket();
const char* clientDebuggerGetExportStatus();
void clientDebuggerDraw(bool* open, bool replayMode = false);
