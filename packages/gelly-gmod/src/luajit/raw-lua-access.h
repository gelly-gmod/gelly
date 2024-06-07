#ifndef RAW_LUA_ACCESS_H
#define RAW_LUA_ACCESS_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <functional>

#include "GarrysMod/Lua/Interface.h"

namespace luajit {
class LuaShared {
public:
	LuaShared();
	~LuaShared() = default;

	template <typename ReturnType, typename... Args>
	ReturnType CallRawFunction(const char *function, Args &&...args) {
		using LuaFunctionType = ReturnType (*)(Args...);
		const auto functionPointer = reinterpret_cast<LuaFunctionType>(
			GetProcAddress(luaSharedHandle, function)
		);

		return functionPointer(std::forward<Args>(args)...);
	}

private:
	HMODULE luaSharedHandle;
};
}  // namespace luajit

#endif	// RAW_LUA_ACCESS_H
