#include "CConsoleLogger.h"

#include <windows.h>

#include <cstdio>
#include <tracy/Tracy.hpp>

using namespace testbed;

// It's as simple as it gets...

#define CREATE_FORMATTED_BUFFER(buffer, message) \
	char buffer[1024];                           \
	va_list args;                                \
	va_start(args, message);                     \
	vsprintf_s(buffer, message, args);           \
	va_end(args);

void testbed::CConsoleLogger::Info(const char *message, ...) {
	CREATE_FORMATTED_BUFFER(buffer, message);
	TracyMessage(buffer, strlen(buffer));
	printf("[testbed] [info]: %s\n", buffer);
}

void testbed::CConsoleLogger::Debug(const char *message, ...) {
	CREATE_FORMATTED_BUFFER(buffer, message);
	TracyMessage(buffer, strlen(buffer));
	printf("[testbed] [debug]: %s\n", buffer);
}

void testbed::CConsoleLogger::Warning(const char *message, ...) {
	CREATE_FORMATTED_BUFFER(buffer, message);
	TracyMessage(buffer, strlen(buffer));
	printf("[testbed] [warn]: %s\n", buffer);
}

void testbed::CConsoleLogger::Error(const char *message, ...) {
	CREATE_FORMATTED_BUFFER(buffer, message);
	TracyMessage(buffer, strlen(buffer));
	printf("[testbed] [error]: %s\n", buffer);

	MessageBoxA(nullptr, buffer, "Error", MB_OK | MB_ICONERROR);
	exit(1);
}