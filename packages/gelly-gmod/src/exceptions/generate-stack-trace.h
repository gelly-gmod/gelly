#ifndef GENERATE_STACK_TRACE_H
#define GENERATE_STACK_TRACE_H
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace logging::exceptions {
auto GetFormattedStackTrace(PCONTEXT context) -> std::string;
}

#endif	// GENERATE_STACK_TRACE_H
