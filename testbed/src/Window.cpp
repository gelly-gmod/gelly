#include "Window.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "Logging.h"
#include "SDL_syswm.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
SDL_Window *window = nullptr;

void MakeTestbedWindow() {
	GetLogger()->Info("Creating the window");
	window = SDL_CreateWindow(
		"Testbed",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_SHOWN
	);
}

HWND GetTestbedWindowHandle() {
	if (!window) {
		GetLogger()->Error("Window is not created");
		return nullptr;
	}
	
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(window, &info);
	return info.info.win.window;
}