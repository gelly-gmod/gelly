#include "Logging.h"

#include "CConsoleLogger.h"

using namespace testbed;

ILogger *activeLogger = nullptr;

void testbed::InitializeLogger(LoggerType type) {
	switch (type) {
		case LoggerType::Console:
			activeLogger = new CConsoleLogger();
			break;
		default:
			activeLogger = nullptr;
			break;
	}
}

ILogger *testbed::GetLogger() { return activeLogger; }