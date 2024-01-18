#ifndef LOGGING_H
#define LOGGING_H

#include <cstdio>

#define GCR_LOG_INFO(...)                              \
	printf("[INFO - %s:%d] ", __FUNCTION__, __LINE__); \
	printf(__VA_ARGS__);                               \
	printf("\n")

#define GCR_LOG_ERROR(...)                              \
	printf("[ERROR - %s:%d] ", __FUNCTION__, __LINE__); \
	printf(__VA_ARGS__);                                \
	printf("\n")

#define GCR_LOG_WARN(...)                              \
	printf("[WARN - %s:%d] ", __FUNCTION__, __LINE__); \
	printf(__VA_ARGS__);                               \
	printf("\n")

#endif	// LOGGING_H
