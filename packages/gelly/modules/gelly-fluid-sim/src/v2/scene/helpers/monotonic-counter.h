#ifndef MONOTONIC_COUNTER_H
#define MONOTONIC_COUNTER_H

#include <cstdint>

struct MonotonicCounter {
	uint32_t counter = 0;

	uint32_t Increment() { return counter++; }
};
#endif	// MONOTONIC_COUNTER_H
