#include "raw-lua-access.h"

#include <stdexcept>

#include "logging/global-macros.h"

luajit::LuaShared::LuaShared() {
	luaSharedHandle = GetModuleHandleA("lua_shared.dll");
	if (luaSharedHandle == nullptr) {
		LOG_ERROR("Failed to get handle to lua_shared.dll");
		throw std::runtime_error("Failed to get handle to lua_shared.dll");
	}
}