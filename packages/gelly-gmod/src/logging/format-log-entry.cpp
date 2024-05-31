#include "format-log-entry.h"

#include <chrono>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "severity-strings.h"

auto FormatDateInLocale(const auto &timeInfo) -> std::wstring {
	const auto dateInformation = std::put_time(std::localtime(&timeInfo), "%c");

	std::wostringstream formattedDate;
	formattedDate << dateInformation;
	return formattedDate.str();
}

namespace logging {
auto FormatLogEntry(const LogEntry &entry) -> std::wstring {
	const auto formattedEntry = std::format(
		"{} - [{} @ {}]: {}\n",
		FormatDateInLocale(entry.time),
		SeverityToWString(entry.severity),
		entry.function,
		entry.message
	);

	// The C++ standard actually discourages going from std::string ->
	// std::wstring using wstring_convert, instead it recommends using Windows'
	// Win32 conversion functions.

	std::wstring convertedEntry;
	convertedEntry.resize(MultiByteToWideChar(
		CP_UTF8, 0, formattedEntry.data(), formattedEntry.size(), nullptr, 0
	));

	MultiByteToWideChar(
		CP_UTF8,
		0,
		formattedEntry.data(),
		formattedEntry.size(),
		convertedEntry.data(),
		convertedEntry.size()
	);

	return convertedEntry;
}
}  // namespace logging
