#include "log-events.h"

namespace logging {
LogEvents::LogEvents() : onLogEntryAdded() {}

void LogEvents::TriggerOnLogEntryAdded(const LogEntry &entry) const {
	if (onLogEntryAdded) {
		onLogEntryAdded(entry);
	}
}

void LogEvents::SetEntryAddedCallback(OnLogEntryAdded callback) {
	onLogEntryAdded = std::move(callback);
}

}  // namespace logging