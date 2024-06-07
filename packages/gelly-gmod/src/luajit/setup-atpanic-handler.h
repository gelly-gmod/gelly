#ifndef SETUP_ATPANIC_HANDLER_H
#define SETUP_ATPANIC_HANDLER_H

#include "raw-lua-access.h"

namespace luajit {
void SetupAtPanicHandler(lua_State *L, LuaShared *luaSharedPtr);
}  // namespace luajit

#endif	// SETUP_ATPANIC_HANDLER_H
