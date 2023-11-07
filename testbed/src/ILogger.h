#ifndef GELLY_ILOGGER_H
#define GELLY_ILOGGER_H

/*
 * Interface for logging messages.
 * This is a simple interface but makes it easier to log to different places
 * when the necessary platforms are ready (such as ImGui, or a file).
 */
class __declspec(novtable) ILogger {
public:
	virtual ~ILogger() = default;

	virtual void Info(const char *message) = 0;
	virtual void Debug(const char *message) = 0;
	virtual void Warning(const char *message) = 0;
	/*
	 * For completely unrecoverable errors. Use warnings for recoverable issues.
	 * Depending on the logger, this can terminate the program.
	 */
	virtual void Error(const char *message) = 0;
};

#endif	// GELLY_ILOGGER_H
