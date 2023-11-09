#include "Logging.h"

#include "CConsoleLogger.h"

using namespace testbed;

ILogger *testbed::InitializeLogger(LoggerType type) {
	ILogger *activeLogger;

	switch (type) {
		case LoggerType::Console:
			activeLogger = new CConsoleLogger();
			break;
		default:
			activeLogger = nullptr;
			break;
	}

	return activeLogger;
}