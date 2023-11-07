#include "CConsoleLogger.h"

#include <windows.h>

#include <cstdio>

using namespace testbed;

// It's as simple as it gets...

void testbed::CConsoleLogger::Info(const char *message) {
	printf("[testbed] [info]: %s\n", message);
}

void testbed::CConsoleLogger::Debug(const char *message) {
	printf("[testbed] [debug]: %s\n", message);
}

void testbed::CConsoleLogger::Warning(const char *message) {
	printf("[testbed] [warning]: %s\n", message);
}

void testbed::CConsoleLogger::Error(const char *message) {
	printf("[testbed] [error]: %s\n", message);

	MessageBoxA(nullptr, message, "Error", MB_OK | MB_ICONERROR);
	exit(1);
}