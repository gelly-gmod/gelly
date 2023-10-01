#ifndef GELLY_VTABLE_H
#define GELLY_VTABLE_H

#include <cstdint>

/**
 * Helper metaprogramming macros for hooking virtual methods.
 */

#define DEFINE_VMT_HOOK(name, index, fnTypedef) \
	fnTypedef name##original = nullptr;         \
	static const uintptr_t name##_index = index;

#define VMT_HOOK_BODY(name, convention, returnType, ...) \
	returnType convention name##Hook(__VA_ARGS__)
#define VMT_HOOK_END_ORIGINAL(name, ...) return name##original(__VA_ARGS__);

#define VMT_HOOK_CALL_ORIG(name, ...) name##original(__VA_ARGS__);

#define APPLY_VMT_HOOK(vtable, name) \
	vtable.Hook(name##_index, (void *)&name##Hook, (void **)&name##original);

#define ENABLE_VMT_HOOK(vtable, name) vtable.EnableHook(name##_index);
#define DISABLE_VMT_HOOK(vtable, name) vtable.DisableHook(name##_index);
#define REMOVE_VMT_HOOK(vtable, name) vtable.Unhook(name##_index);

/**
 * @brief A helper class to contain a VTable so that it can be extended with
 * hooking methods.
 */
class VTable {
private:
	void **vtableCopy;
	int size;

public:
	VTable(void **vtable, int size);
	VTable();
	~VTable();

	void Init(void **vtable, int vtableSize);

	[[nodiscard]] void **GetVTable() const;
	void Hook(int index, void *hook, void **original);
	void EnableHook(int index) const;
	void DisableHook(int index) const;
	void Unhook(int index) const;

	template <typename ReturnType, typename... Args>
	ReturnType CallOriginal(int index, void *classInstance, Args... args) {
		return reinterpret_cast<
			ReturnType(__fastcall *)(void *, Args...)>(vtableCopy[index])(
			classInstance, args...
		);
	}

	template <typename ReturnType, typename... Args>
	ReturnType CallOriginalCdecl(int index, void *classInstance, Args... args) {
		return reinterpret_cast<
			ReturnType(__cdecl *)(void *, Args...)>(vtableCopy[index])(
			classInstance, args...
		);
	}
};

#endif	// GELLY_VTABLE_H
