#ifndef GELLY_CCONSOLELOGGER_H
#define GELLY_CCONSOLELOGGER_H

#include "ILogger.h"

namespace testbed {
class CConsoleLogger : public ILogger {
public:
	CConsoleLogger() = default;
	~CConsoleLogger() override = default;

	void Info(const char *message, ...) override;
	void Debug(const char *message, ...) override;
	void Warning(const char *message, ...) override;
	void Error(const char *message, ...) override;
};
}  // namespace testbed

#endif	// GELLY_CCONSOLELOGGER_H
