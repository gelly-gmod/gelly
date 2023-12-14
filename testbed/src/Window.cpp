#include "Window.h"

#include <vector>

#include "Logging.h"
#include "SDL_syswm.h"

using namespace testbed;

const int testbed::WINDOW_WIDTH = 1920;
const int testbed::WINDOW_HEIGHT = 1080;

static SDL_Window *window = nullptr;
static std::vector<EventInterceptor> eventInterceptors;
static ILogger *logger = nullptr;

SDL_Window *testbed::GetTestbedWindow() { return window; }

void testbed::AddEventInterceptor(EventInterceptor interceptor) {
	eventInterceptors.push_back(interceptor);
}

void InitializeSDL() {
	logger->Info("Initializing SDL");
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		logger->Error("Failed to initialize SDL");
		exit(1);
	}
}

void MakeTestbedWindow() {
	logger->Info("Creating the window");
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
		for (const auto &interceptor : eventInterceptors) {
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
		logger->Error("Window is not created");
		return nullptr;
	}

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWindowWMInfo(window, &info);
	return info.info.win.window;
}

void testbed::InitializeWindow(ILogger *newLogger) {
	logger = newLogger;

	InitializeSDL();
	MakeTestbedWindow();
}