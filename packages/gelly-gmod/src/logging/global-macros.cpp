#include "global-macros.h"

#include <cstdarg>
#include <ctime>
#include <memory>

logging::Log g_macroLog;

void AddLogEntryFromMacro(
	logging::Severity severity, const char *function, const char *format, ...
) {
	using namespace logging;
	LogEntry entry = {};
	va_list args;
	va_start(args, format);

	const auto memoryRequired = std::vsnprintf(nullptr, 0, format, args);
	const auto formatBuffer = std::make_unique<char[]>(memoryRequired + 1);
	const auto formatBufferPtr = formatBuffer.get();

	std::vsnprintf(formatBufferPtr, memoryRequired + 1, format, args);

	va_end(args);

	entry.severity = severity;
	entry.time = time(nullptr);
	entry.function = std::wstring(function, function + strlen(function));
	entry.message =
		std::wstring(formatBufferPtr, formatBufferPtr + memoryRequired);

	g_macroLog.AddEntry(entry);
}
