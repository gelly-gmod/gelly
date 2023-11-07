#ifndef GELLY_LOGGING_H
#define GELLY_LOGGING_H

#include "ILogger.h"

namespace testbed {
enum class LoggerType { Console };

ILogger *GetLogger();
void InitializeLogger(LoggerType type);
}  // namespace testbed

#endif	// GELLY_LOGGING_H
