#include "generate-stack-trace.h"

#include <DbgHelp.h>

#include <sstream>

#include "logging/global-macros.h"

auto GetIndentation(int frame) {
	std::string indentation;
	for (int i = 0; i < frame; i++) {
		indentation += "    ";
	}
	return indentation;
}

auto StackWalk64GMod(PCONTEXT context, LPSTACKFRAME64 frame) {
	const auto process = GetCurrentProcess();
	const auto thread = GetCurrentThread();

	return StackWalk64(
		IMAGE_FILE_MACHINE_AMD64,
		process,
		thread,
		frame,
		context,
		nullptr,
		SymFunctionTableAccess64,
		SymGetModuleBase64,
		nullptr
	);
}

constexpr const char *DEFAULT_MODULE_NAME = "Unknown Module";

auto GetModuleNameFromAddress(DWORD64 address) -> std::string {
	HMODULE module;
	if (GetModuleHandleEx(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			reinterpret_cast<LPCTSTR>(address),
			&module
		)) {
		char moduleName[MAX_PATH];
		GetModuleFileNameA(module, moduleName, MAX_PATH);
		return moduleName;
	}

	return DEFAULT_MODULE_NAME;
}

auto GetFormattedAddress(DWORD64 address) {
	std::stringstream addressStream;
	addressStream << "0x" << std::hex << address;

	auto formattedAddress = addressStream.str();
	return formattedAddress;
}

auto GetSymbolName(DWORD64 address) -> std::string {
	// you need to actually dynamically allocate for the name
	// variable--otherwise it just runs over the stack pointer and absolutely
	// destroys everything... 0 documentation on this!
	auto *symbol = static_cast<IMAGEHLP_SYMBOL64 *>(
		calloc(sizeof(IMAGEHLP_SYMBOL64) + 256 * sizeof(char), 1)
	);

	memset(symbol, 0, sizeof(IMAGEHLP_SYMBOL64));
	symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
	symbol->MaxNameLength = 255;

	DWORD64 displacement;

	if (!SymGetSymFromAddr64(
			GetCurrentProcess(), address, &displacement, symbol
		)) {
		return "Unknown Symbol";
	}

	return symbol->Name;
}

auto GetSymbolLine(DWORD64 address) -> std::string {
	IMAGEHLP_LINE64 line = {};
	memset(&line, 0, sizeof(IMAGEHLP_LINE64));
	line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

	DWORD displacement;
	if (!SymGetLineFromAddr64(
			GetCurrentProcess(), address, &displacement, &line
		)) {
		return "Unknown Line";
	}

	std::stringstream lineStream;
	lineStream << line.FileName << "(" << line.LineNumber << ")";
	return lineStream.str();
}

auto GetFrameSummary(LPSTACKFRAME64 frame) {
	std::string frameSummary;

	// Our summary is pretty much just address and module name then symbol name
	frameSummary += "> ";
	frameSummary += GetFormattedAddress(frame->AddrPC.Offset);
	// translation (e.g 32:64 or 16:32)
	frameSummary += " in ";
	frameSummary += GetModuleNameFromAddress(frame->AddrPC.Offset);
	frameSummary += " | ";
	frameSummary += GetSymbolName(frame->AddrPC.Offset);
	frameSummary += "@";
	frameSummary += GetSymbolLine(frame->AddrPC.Offset);

	return frameSummary;
}

namespace logging::exceptions {
auto GetFormattedStackTrace(PCONTEXT context) -> std::string {
	std::string stackTrace;
	int frame = 0;
	STACKFRAME64 stackFrame = {};
	memset(&stackFrame, 0, sizeof(STACKFRAME64));

	do {
		StackWalk64GMod(context, &stackFrame);
		if (stackFrame.AddrPC.Offset == 0) {
			break;
		}

		stackTrace += GetIndentation(frame);
		stackTrace += GetFrameSummary(&stackFrame);
		stackTrace += "\n";
	} while (stackFrame.AddrPC.Offset != 0 && frame++ < 64);

	return stackTrace;
}
}  // namespace logging::exceptions
