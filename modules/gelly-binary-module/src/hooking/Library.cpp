#include "Library.h"

#include <array>
#include <cassert>
#include <vector>

struct PatternByte {
	bool wildcard;
	uint8_t byte;
};

void GetModuleAddrAndSize(
	const char *moduleName, uintptr_t *addr, size_t *size
) {
	// Take a snapshot of the GMod process. We want to see what modules are
	// loaded into the process.

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);

	if (snapshot == INVALID_HANDLE_VALUE) {
		return;
	}

	MODULEENTRY32 module_entry;
	module_entry.dwSize = sizeof(MODULEENTRY32);

	if (!Module32First(snapshot, &module_entry)) {
		CloseHandle(snapshot);
		return;
	}

	do {
		if (strcmp(module_entry.szModule, moduleName) == 0) {
			CloseHandle(snapshot);
			(*addr) = (uintptr_t)module_entry.modBaseAddr;
			(*size) = module_entry.modBaseSize;
		}
	} while (Module32Next(snapshot, &module_entry));

	CloseHandle(snapshot);
}

Library::Library(const char *name) : base_address(0), size(0) {
	GetModuleAddrAndSize(name, &base_address, &size);
#ifdef _DEBUG
	assert(base_address != 0);
	assert(size != 0);
#endif
}

uintptr_t Library::Scan(const char *pattern) const {
	// Basic scan algorithm, start at byte zero, check pattern, if match, return
	// address. If not, increment byte pointer. Rinse and repeat.

	std::vector<PatternByte> patternBytes;
	patternBytes.reserve(32);  // Arbitrary, but most patterns are usually less
							   // than or equal to 32 bytes.

	for (int charPtr = 0; charPtr <= strlen(pattern); charPtr += 3) {
		char nibble1 = pattern[charPtr];
		char nibble2 = pattern[charPtr + 1];

		if (nibble1 == '?' && nibble2 == '?') {
			patternBytes.push_back({true, 0});
		} else {
			char byteString[3] = {nibble1, nibble2, '\0'};
			auto byte = static_cast<uint8_t>(strtol(byteString, nullptr, 16));

			patternBytes.push_back({false, byte});
		}
	}

	uintptr_t address = base_address;
	uintptr_t endAddress = base_address + size;

	while (address < endAddress) {
		bool match = false;

		for (int i = 0; i < patternBytes.size(); i++) {
			PatternByte patternByte = patternBytes[i];
			uint8_t byte = *(uint8_t *)(address + i);

			if (patternByte.wildcard) {
				continue;
			}

			if (patternByte.byte == byte) {
				match = true;
				break;
			}
		}

		if (match) {
			return address;
		}

		address++;
	}
}