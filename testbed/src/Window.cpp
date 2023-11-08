#include "Window.h"

#include <vector>

#include "Logging.h"
#include "SDL_syswm.h"

using namespace testbed;

const int testbed::WINDOW_WIDTH = 1280;
const int testbed::WINDOW_HEIGHT = 720;

static SDL_Window *window = nullptr;
static std::vector<EventInterceptor> eventInterceptors;

SDL_Window *testbed::GetTestbedWindow() { return window; }

void testbed::AddEventInterceptor(EventInterceptor interceptor) {
	eventInterceptors.push_back(interceptor);
}

void testbed::InitializeSDL() {
	GetLogger()->Info("Initializing SDL");
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		GetLogger()->Error("Failed to initialize SDL");
		exit(1);
	}
}

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
		for (auto interceptor : eventInterceptors) {
			interceptor(&event);
		}

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