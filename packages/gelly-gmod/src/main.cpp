#include <GarrysMod/Lua/Interface.h>
#include "LoggingMacros.h"
#include <cstdio>
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Gelly.h"

static GellyIntegration *gelly = nullptr;

void InjectConsoleWindow() {
	AllocConsole();
	freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
}

GMOD_MODULE_OPEN() {
	InjectConsoleWindow();
	LOG_INFO("Hello, world!");
	gelly = new GellyIntegration();
	return 0;
}

GMOD_MODULE_CLOSE() {
	LOG_INFO("Goodbye, world!");
	delete gelly;
	return 0;
}
