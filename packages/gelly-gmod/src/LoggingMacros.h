#ifndef LOGGINGMACROS_H
#define LOGGINGMACROS_H

#define LOG_INFO(...)                                               \
	printf("[gelly-gmod == INFO @ " __FUNCTION__ "] " __VA_ARGS__); \
	printf("\n");

#define LOG_WARNING(...)                            \
	printf("[gelly-gmod == WARNING] " __VA_ARGS__); \
	printf("\n");
#define LOG_ERROR(...)                            \
	printf("[gelly-gmod == ERROR] " __VA_ARGS__); \
	printf("\n");

#define LOG_DX_CALL(info, call)                       \
	if (const auto status = call; status != D3D_OK) { \
		LOG_ERROR("%s: 0x%08lX", info, status);       \
	}

#endif	// LOGGINGMACROS_H
