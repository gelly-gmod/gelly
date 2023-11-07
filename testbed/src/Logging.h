#ifndef GELLY_LOGGING_H
#define GELLY_LOGGING_H

#include "ILogger.h"

enum class LoggerType { Console };

ILogger *GetLogger();

void InitializeLogger(LoggerType type);

#endif	// GELLY_LOGGING_H
