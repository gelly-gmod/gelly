#ifndef GELLY_INTERFACE_H
#define GELLY_INTERFACE_H

#include <windows.h>

template <typename T>
T *GetInterface(const char *module, const char *name) {
	auto createInterface = reinterpret_cast<T *(*)(const char *, int *)>(
		GetProcAddress(GetModuleHandle(module), "CreateInterface")
	);

	int returnCode;
	T *system = createInterface(name, &returnCode);

	// IFACE_OK = 0
	if (returnCode != 0) {
		return nullptr;
	}

	return system;
}

#endif	// GELLY_INTERFACE_H
