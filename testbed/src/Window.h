#ifndef GELLY_WINDOW_H
#define GELLY_WINDOW_H

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <windows.h>

namespace testbed {
extern const int WINDOW_WIDTH;
extern const int WINDOW_HEIGHT;

using EventInterceptor = void (*)(SDL_Event *event);

void MakeTestbedWindow();
bool HandleWindowMessages();
HWND GetTestbedWindowHandle();
SDL_Window *GetTestbedWindow();
void InitializeSDL();
void SetEventInterceptor(EventInterceptor interceptor);
}  // namespace testbed

#endif	// GELLY_WINDOW_H