#include "log.h"

#include <ctime>

namespace logging {
Log::Log() : entries(RESERVE_ENTRIES_AMOUNT), creationTime(time(nullptr)){};

void Log::AddEntry(const LogEntry &entry) { entries.push_back(entry); }

auto Log::GetEntries() const -> Entries { return {entries}; }

auto Log::GetCreationTime() const -> time_t { return creationTime; }
}  // namespace logging