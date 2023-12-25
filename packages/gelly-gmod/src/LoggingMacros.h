#ifndef LOGGINGMACROS_H
#define LOGGINGMACROS_H

// Our binary module really only needs have primitive logging since the addon
// will convey information to the user.

#define LOG_INFO(...) printf("[gelly-gmod == INFO] " __VA_ARGS__); printf("\n");
#define LOG_WARNING(...) printf("[gelly-gmod == WARNING] " __VA_ARGS__); printf("\n");
#define LOG_ERROR(...) printf("[gelly-gmod == ERROR] " __VA_ARGS__); printf("\n");

#endif //LOGGINGMACROS_H
