#ifndef GELLY_LIBRARY_H
#define GELLY_LIBRARY_H

// clang-format off
#include <windows.h>
#include <TlHelp32.h>
// clang-format on

class Library {
private:
	uintptr_t base_address;
	/**
	 * Size in bytes of the library.
	 */
	size_t size;

public:
	explicit Library(const char *name);
	Library();
	~Library() = default;

	void Init(const char *name);
	[[nodiscard]] bool IsInitialized() const;

	/**
	 * Scans for a pattern in the entire library's data.
	 * Patterns are bytes (hexadecimal pairs) separated by spaces. If a byte
	 * should be ignored, use ?? in place of it.
	 * @param pattern
	 * @return
	 */
	uintptr_t Scan(const char *pattern) const;

	template <typename T>
	T GetObjectAt(uintptr_t offset) {
		return reinterpret_cast<T>(base_address + offset);
	}

	template <typename Fn>
	Fn FindFunction(const char *pattern) {
		return reinterpret_cast<Fn>(Scan(pattern));
	}
};

#endif	// GELLY_LIBRARY_H
