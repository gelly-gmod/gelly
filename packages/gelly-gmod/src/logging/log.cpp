#include "log.h"

#include <ctime>

namespace logging {
Log::Log() : entries(), creationTime(time(nullptr)), events() {
	entries.reserve(RESERVE_ENTRIES_AMOUNT);
}

void Log::AddEntry(const LogEntry &entry) {
	entries.push_back(entry);
	events.TriggerOnLogEntryAdded(entry);
}

auto Log::GetEntries() const -> Entries { return {entries}; }

auto Log::GetCreationTime() const -> time_t { return creationTime; }

auto Log::GetEvents() -> LogEvents & { return events; }

}  // namespace logging