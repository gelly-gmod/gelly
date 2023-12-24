#include "Memory.h"

#include <tracy/Tracy.hpp>

void *operator new(size_t size) {
	auto ptr = malloc(size);
	TracyAlloc(ptr, size);
	return ptr;
}

void operator delete(void *ptr) noexcept {
	TracyFree(ptr);
	free(ptr);
}