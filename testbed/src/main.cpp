#include <cstdio>

#include "Logging.h"
#include "Rendering.h"
#include "Window.h"

using namespace testbed;

int main() {
	InitializeLogger(LoggerType::Console);
	GetLogger()->Info("Hello, world!");

	MakeTestbedWindow();
	InitializeRenderer();
	bool isRunning = true;
	while (isRunning) {
		isRunning = HandleWindowMessages();

		StartFrame();
		EndFrame();
	}

	return 0;
}
