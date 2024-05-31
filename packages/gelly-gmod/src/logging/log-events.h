#ifndef LOG_EVENTS_H
#define LOG_EVENTS_H
#include <functional>

#include "log-entry.h"

namespace logging {
class LogEvents {
public:
	using OnLogEntryAdded = std::function<void(const LogEntry &)>;

	LogEvents();
	~LogEvents() = default;

	void TriggerOnLogEntryAdded(const LogEntry &entry) const;
	void SetEntryAddedCallback(OnLogEntryAdded callback);

private:
	OnLogEntryAdded onLogEntryAdded;
};
}  // namespace logging

#endif	// LOG_EVENTS_H
