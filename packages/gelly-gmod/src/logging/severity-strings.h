#ifndef SEVERITY_STRINGS_H
#define SEVERITY_STRINGS_H

#include <string>

#include "log-entry.h"

#undef ERROR

namespace logging {
constexpr auto SeverityToString(const Severity &severity) -> std::wstring {
	switch (severity) {
		case Severity::INFO:
			return L"INFO";
		case Severity::WARNING:
			return L"WARNING";
		case Severity::ERROR:
			return L"ERROR";
		default:
			return L"UNKNOWN";
	}
}
}  // namespace logging
#endif	// SEVERITY_STRINGS_H
