#ifndef GELLY_LOGGING_H
#define GELLY_LOGGING_H

#include "ILogger.h"
#include "Memory.h"

namespace testbed {
enum class LoggerType { Console };

ILogger *InitializeLogger(LoggerType type);
}  // namespace testbed

#endif	// GELLY_LOGGING_H
