#include "Window.h"

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "Logging.h"
#include "SDL_syswm.h"

using namespace testbed;

const int testbed::WINDOW_WIDTH = 1280;
const int testbed::WINDOW_HEIGHT = 720;

SDL_Window *window = nullptr;

void testbed::MakeTestbedWindow() {
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

bool testbed::HandleWindowMessages() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				return false;
				break;
			default:
				break;
		}
	}

	return true;
}

HWND testbed::GetTestbedWindowHandle() {
	if (!window) {
		GetLogger()->Error("Window is not created");
		return nullptr;
	}

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(window, &info);
	return info.info.win.window;
}