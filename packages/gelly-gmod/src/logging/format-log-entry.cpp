#include "format-log-entry.h"

#include <chrono>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "severity-strings.h"

auto FormatDateInLocale(const auto &timeInfo) -> std::wstring {
	const auto dateInformation =
		std::put_time(std::localtime(&timeInfo), L"%c");

	std::wostringstream formattedDate;
	formattedDate << dateInformation;
	return formattedDate.str();
}

namespace logging {
auto FormatLogEntry(const LogEntry &entry) -> std::wstring {
	const auto formattedEntry = std::format(
		L"{} - [{} @ {}]: {}\n",
		FormatDateInLocale(entry.time),
		SeverityToString(entry.severity),
		entry.function,
		entry.message
	);

	return formattedEntry;
}
}  // namespace logging
