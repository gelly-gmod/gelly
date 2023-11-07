#include "Logging.h"

#include "CConsoleLogger.h"

using namespace testbed;

ILogger *activeLogger = nullptr;

void InitializeLogger(LoggerType type) {
	switch (type) {
		case LoggerType::Console:
			activeLogger = new CConsoleLogger();
			break;
		default:
			activeLogger = nullptr;
			break;
	}
}

ILogger *GetLogger() { return activeLogger; }