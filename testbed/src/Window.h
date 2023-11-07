#ifndef GELLY_WINDOW_H
#define GELLY_WINDOW_H

#include <SDL.h>
#include <windows.h>

namespace testbed {
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

void MakeTestbedWindow();
bool HandleWindowMessages();
HWND GetTestbedWindowHandle();
SDL_Window *GetTestbedWindow();
}  // namespace testbed

#endif	// GELLY_WINDOW_H
