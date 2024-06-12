#include "Library.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <vector>

#include "MinHook.h"

#define DEBUG_ASSERT_MH(status)                    \
	OutputDebugStringA(MH_StatusToString(status)); \
	assert(status == MH_OK);

HookedFunction::HookedFunction(
	void *originalAddress, void *hookAddress, void **originalFn
)
	: originalAddress(originalAddress), hookAddress(hookAddress) {
	Init(originalAddress, hookAddress, originalFn);
}

HookedFunction::HookedFunction()
	: originalAddress(nullptr), hookAddress(nullptr) {}

HookedFunction::~HookedFunction() { Remove(); }

void HookedFunction::Init(
	void *originalAddress, void *hookAddress, void **originalFn
) {
#ifdef _DEBUG
	assert(originalAddress != nullptr);
	assert(hookAddress != nullptr);
	assert(originalFn != nullptr);
#endif

	this->originalAddress = originalAddress;
	this->hookAddress = hookAddress;

	MH_STATUS status = MH_CreateHook(originalAddress, hookAddress, originalFn);
#ifdef _DEBUG
	DEBUG_ASSERT_MH(status);
#endif
}

void *HookedFunction::GetOriginalAddress() const { return originalAddress; }
void *HookedFunction::GetHookAddress() const { return hookAddress; }

void HookedFunction::Enable() const {
	MH_STATUS status = MH_EnableHook(originalAddress);
#ifdef _DEBUG
	DEBUG_ASSERT_MH(status);
#endif
}

void HookedFunction::Disable() const {
	MH_STATUS status = MH_DisableHook(originalAddress);
#ifdef _DEBUG
	DEBUG_ASSERT_MH(status);
#endif
}

void HookedFunction::Remove() const {
	MH_STATUS status = MH_RemoveHook(originalAddress);
#ifdef _DEBUG
	DEBUG_ASSERT_MH(status);
#endif
}

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
			*addr = reinterpret_cast<uintptr_t>(module_entry.modBaseAddr);
			*size = module_entry.modBaseSize;
		}
	} while (Module32Next(snapshot, &module_entry));

	CloseHandle(snapshot);
}

Library::Library(const char *name) : base_address(0), size(0) {
	Init(name);
#ifdef _DEBUG
	assert(base_address != 0);
	assert(size != 0);
#endif
}

Library::Library() : base_address(0), size(0) {}

void Library::Init(const char *name) {
	GetModuleAddrAndSize(name, &base_address, &size);
}

bool Library::IsInitialized() const { return base_address != 0 && size != 0; }

uintptr_t Library::Scan(const char *pattern) const {
	// Basic scan algorithm, start at byte zero, check pattern, if match, return
	// address. If not, increment byte pointer. Rinse and repeat.

	std::vector<PatternByte> patternBytes;
	patternBytes.reserve(32);  // Arbitrary, but most patterns are usually less
							   // than or equal to 32 bytes.

	for (int charPtr = 0; charPtr <= strlen(pattern); charPtr += 3) {
		const char nibble1 = pattern[charPtr];
		const char nibble2 = pattern[charPtr + 1];

		if (nibble1 == '?' && nibble2 == '?') {
			patternBytes.push_back({true, 0});
		} else {
			const char byteString[3] = {nibble1, nibble2, '\0'};
			const auto byte =
				static_cast<uint8_t>(strtol(byteString, nullptr, 16));

			patternBytes.push_back({false, byte});
		}
	}

	uintptr_t address = base_address;
	uintptr_t endAddress = base_address + size;

	while (address < endAddress) {
		bool match = true;

		for (int i = 0; i < patternBytes.size(); i++) {
			PatternByte patternByte = patternBytes[i];
			uint8_t byte = *(uint8_t *)(address + i);

			if (patternByte.wildcard) {
				continue;
			}

			if (patternByte.byte != byte) {
				match = false;
				break;
			}
		}

		if (match) {
			return address;
		}

		address++;
	}

	return 0;
}

bool Library::HookFunction(
	const char *pattern,
	void *hook,
	void **original,
	HookedFunction &hookedFunction
) const {
	const auto address = Scan(pattern);

	if (address == 0) {
		return false;
	}

	hookedFunction.Init(reinterpret_cast<void *>(address), hook, original);

	return true;
}
