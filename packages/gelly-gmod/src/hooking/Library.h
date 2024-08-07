#ifndef LIBRARY_H
#define LIBRARY_H

// clang-format off
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <TlHelp32.h>
#include <unordered_map>
#include <string>
// clang-format on

class HookedFunction {
private:
	/**
	 * The original address of the function.
	 */
	void *originalAddress;
	void *hookAddress;

public:
	HookedFunction(void *originalAddress, void *hookAddress, void **originalFn);
	HookedFunction();
	~HookedFunction();

	void Init(void *originalAddress, void *hookAddress, void **originalFn);

	[[nodiscard]] void *GetOriginalAddress() const;
	[[nodiscard]] void *GetHookAddress() const;

	void Enable() const;
	void Disable() const;
	void Remove() const;
};

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

	bool HookFunction(
		const char *pattern,
		void *hook,
		void **original,
		HookedFunction &hookedFunction
	) const;
};

#endif //LIBRARY_H
