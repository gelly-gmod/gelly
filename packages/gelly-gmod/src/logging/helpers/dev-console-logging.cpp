#include "dev-console-logging.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "logging/format-log-entry.h"
#include "logging/global-macros.h"
#include "logging/log-entry.h"

using logging::LogEntry;

void LogToConsole(const LogEntry &entry) {
	const auto &formattedEntry = FormatLogEntry(entry);
	printf("%ls", formattedEntry.c_str());
}

namespace logging {
/**
 * Begins logging to the developer console. This function should be called once
 * on initialization.
 */
void StartDevConsoleLogging() {
	AllocConsole();
	freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);

	LogEvents::OnLogEntryAdded callback = LogToConsole;
	g_macroLog.GetEvents().SetEntryAddedCallback(std::move(callback));
}

void StopDevConsoleLogging() { FreeConsole(); }

}  // namespace logging