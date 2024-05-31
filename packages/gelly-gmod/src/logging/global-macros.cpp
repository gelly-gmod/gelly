#include "global-macros.h"

#include <cstdarg>
#include <ctime>

logging::Log g_macroLog;

void AddLogEntryFromMacro(
	logging::Severity severity, const char *function, const char *format, ...
) {
	using namespace logging;
	LogEntry entry = {};
	va_list args;
	va_start(args, format);

	char formatBuffer[1024];
	vsnprintf(formatBuffer, sizeof(formatBuffer), format, args);

	va_end(args);

	entry.severity = severity;
	entry.time = time(nullptr);
	entry.function = std::wstring(function, function + strlen(function));
	entry.message =
		std::wstring(formatBuffer, formatBuffer + strlen(formatBuffer));

	g_macroLog.AddEntry(entry);
}
