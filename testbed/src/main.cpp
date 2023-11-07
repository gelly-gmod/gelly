#include <cstdio>

#include "Logging.h"
#include "Window.h"

using namespace testbed;

int main() {
	InitializeLogger(LoggerType::Console);
	GetLogger()->Info("Hello, world!");

	MakeTestbedWindow();

	while (true) {
	}
	return 0;
}
