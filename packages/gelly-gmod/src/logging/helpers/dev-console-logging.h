#ifndef DEV_CONSOLE_LOGGING_H
#define DEV_CONSOLE_LOGGING_H

namespace logging {
/**
 * Begins logging to the developer console. This function should be called once
 * on initialization.
 */
void StartDevConsoleLogging();
/**
 * Stops logging to the developer console. This function should be called once,
 * and it will stop logging to the developer console and close the window.
 */
void StopDevConsoleLogging();
}  // namespace logging

#endif	// DEV_CONSOLE_LOGGING_H
