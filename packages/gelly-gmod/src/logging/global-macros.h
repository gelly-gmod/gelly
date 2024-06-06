#ifndef LOGGINGMACROS_H
#define LOGGINGMACROS_H

#include "helpers/save-log-to-file.h"
#include "log-entry.h"
#include "log.h"

extern logging::Log g_macroLog;

void AddLogEntryFromMacro(
	logging::Severity severity, const char *function, const char *format, ...
);

#undef INFO
#undef WARNING
#undef ERROR

#define LOG_INFO(...)                                                         \
	AddLogEntryFromMacro(logging::Severity::INFO, __FUNCTION__, __VA_ARGS__); \
	logging::SaveLogToFile(g_macroLog);

#define LOG_WARNING(...)                                      \
	AddLogEntryFromMacro(                                     \
		logging::Severity::WARNING, __FUNCTION__, __VA_ARGS__ \
	);                                                        \
	logging::SaveLogToFile(g_macroLog);

#define LOG_ERROR(...)                                        \
	AddLogEntryFromMacro(                                     \
		logging::Severity::ER##ROR, __FUNCTION__, __VA_ARGS__ \
	);                                                        \
	logging::SaveLogToFile(g_macroLog);

#define LOG_DX_CALL(info, call)                       \
	if (const auto status = call; status != D3D_OK) { \
		LOG_ERROR("%s: 0x%08lX", info, status);       \
	}

#define LOG_SAVE_TO_FILE() logging::SaveLogToFile(g_macroLog);

#endif	// LOGGINGMACROS_H
