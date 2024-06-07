#ifndef GET_STACK_SIZE_H
#define GET_STACK_SIZE_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace logging::stack {
/**
 * Useful debugging function that returns the size of the stack region.
 * @return The size of the stack region.
 */
auto GetCurrentStackSize() -> uintptr_t;
}  // namespace logging::stack

#endif	// GET_STACK_SIZE_H
