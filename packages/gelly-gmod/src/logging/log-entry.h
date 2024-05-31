#ifndef LOG_ENTRY_H
#define LOG_ENTRY_H

#include <string>

#undef INFO
#undef WARNING
#undef ERROR

namespace logging {
enum class Severity { INFO, WARNING, ERROR };

// given our predominantly windows environment--wide strings are exclusively
// used
struct LogEntry {
	std::wstring message;
	Severity severity;
	std::wstring function;
	time_t time;
};

}  // namespace logging

#endif	// LOG_ENTRY_H
