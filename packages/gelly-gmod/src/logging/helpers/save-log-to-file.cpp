#include "save-log-to-file.h"

#include <algorithm>
#include <ctime>
#include <format>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "logging/format-log-entry.h"

auto GetLogFileName() -> std::string {
	const auto currentTime = time(nullptr);
	const auto timeInfo = std::localtime(&currentTime);

	std::string fileName = {};
	fileName.resize(128);

	const auto size = strftime(
		fileName.data(),
		fileName.size(),
		logging::LOG_FILE_NAME_TEMPLATE,
		timeInfo
	);

	if (size == 0) {
		throw std::runtime_error("Failed to format log file name.");
	}

	return fileName;
}

auto FormatLogHeader(const logging::Log &log) -> std::string {
	std::stringstream formattedDate;
	const auto logCreationTime = log.GetCreationTime();
	formattedDate << std::put_time(std::localtime(&logCreationTime), "%c");

	return std::format(logging::LOG_HEADER, formattedDate.str());
}

namespace logging {
void SaveLogToFile(const Log &log) {
	std::wofstream logFile(GetLogFileName(), std::ios::out | std::ios::trunc);
	logFile.exceptions(std::ios::failbit | std::ios::badbit);

	const auto header = FormatLogHeader(log);
	logFile << std::wstring(header.begin(), header.end());

	std::for_each(
		log.GetEntries().begin(),
		log.GetEntries().end(),
		[&logFile](const auto &entry) { logFile << FormatLogEntry(entry); }
	);

	// ensure that the log file is closed
	logFile.flush();
	logFile.close();
}

}  // namespace logging