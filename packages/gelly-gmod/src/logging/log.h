#ifndef LOG_H
#define LOG_H
#include <span>
#include <vector>

#include "log-entry.h"
#include "log-events.h"

namespace logging {
constexpr auto RESERVE_ENTRIES_AMOUNT = 100;

class Log {
	using Entries = std::span<const LogEntry>;

public:
	Log();
	~Log() = default;

	void AddEntry(const LogEntry &entry);
	[[nodiscard]] auto GetEntries() const -> Entries;
	[[nodiscard]] auto GetCreationTime() const -> time_t;
	[[nodiscard]] auto GetEvents() -> LogEvents &;

private:
	std::vector<LogEntry> entries;
	time_t creationTime;
	LogEvents events;
};
};	// namespace logging

#endif	// LOG_H
