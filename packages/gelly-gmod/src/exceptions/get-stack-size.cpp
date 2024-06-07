#include "get-stack-size.h"

namespace logging::stack {
auto GetCurrentStackSize() -> uintptr_t {
	// prepare..
	auto *ntTib = reinterpret_cast<NT_TIB *>(NtCurrentTeb());
	auto stackBase = reinterpret_cast<uintptr_t>(ntTib->StackBase);
	auto stackLimit = reinterpret_cast<uintptr_t>(ntTib->StackLimit);

	return stackBase -
		   stackLimit;	// it's reversed because the stack grows downwards
}
}  // namespace logging::stack