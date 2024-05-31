#ifndef FORMAT_LOG_ENTRY_H
#define FORMAT_LOG_ENTRY_H
#include "log-entry.h"

namespace logging {
// date and time, locale dependent - [severity @ function]: message
constexpr char logEntryFormat[] = "%c - [%s @ %s]: %s\n";

auto FormatLogEntry(const LogEntry &entry) -> std::wstring;

}  // namespace logging

#endif	// FORMAT_LOG_ENTRY_H
