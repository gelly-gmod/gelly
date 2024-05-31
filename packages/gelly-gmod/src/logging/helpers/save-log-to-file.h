#ifndef SAVE_LOG_TO_FILE_H
#define SAVE_LOG_TO_FILE_H
#include "logging/log.h"

namespace logging {
constexpr auto LOG_FILE_NAME_TEMPLATE = "gelly-log-%F.log";
/**
 * Only one format placeholder is used, {}, and it is replaced with the date and
 * time of the log creation.
 */
constexpr auto LOG_HEADER = "[gelly log - {}]\n";
constexpr auto LOG_FOOTER = "[end of log]\n";

void SaveLogToFile(const Log &log);
}  // namespace logging

#endif	// SAVE_LOG_TO_FILE_H
