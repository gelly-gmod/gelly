#include "setup-atpanic-handler.h"

#include "logging/global-macros.h"
#include "raw-lua-access.h"

// no clue who is polluting the global namespace with ERROR
#undef ERROR

static luajit::LuaShared *luaShared;

struct lua_Debug {
	int event;
	const char *name;	  /* (n) */
	const char *namewhat; /* (n) `global', `local', `field', `method' */
	const char *what;	  /* (S) `Lua', `C', `main', `tail' */
	const char *source;	  /* (S) */
	int currentline;	  /* (l) */
	int nups;			  /* (u) number of upvalues */
	int linedefined;	  /* (S) */
	int lastlinedefined;  /* (S) */
	char short_src[60];	  /* (S) */
	/* private part */
	int i_ci; /* active function */
};

using lua_CFunction = int (*)(lua_State *);
using lua_Hook = void (*)(lua_State *, lua_Debug *);

int atpanic_handler(lua_State *L) {
	if (luaShared == nullptr) {
		return 0;
	}

	// Apparently we get 1 error message on the top of the stack, so we'll fetch
	// that and let it terminate the program
	const auto *error =
		luaShared->CallRawFunction<const char *>("lua_tostring", L, -1);

	MessageBoxA(
		nullptr, error, "Gelly caught a LuaJIT panic", MB_OK | MB_ICONERROR
	);

	LOG_ERROR("LuaJIT panic: %s", error);
	LOG_ERROR("Terminating!");

	return 0;
}

void debug_hook(lua_State *L, lua_Debug *debug) {
	luaShared->CallRawFunction<int>("lua_getinfo", L, "nSl", debug);

	if (debug->event == 0 && debug->name != nullptr && debug->what != nullptr) {
		if (strcmp(debug->what, "C") == 0) {
			LOG_INFO("Called C function %s", debug->name);
		}
	} else if (debug->event == 1 && debug->name != nullptr && debug->what != nullptr) {
		if (strcmp(debug->what, "C") == 0) {
			LOG_INFO("Returned from C function %s", debug->name);
		}
	}
}

#define LUA_HOOKCALL 0
#define LUA_HOOKRET 1
#define LUA_HOOKLINE 2
#define LUA_HOOKCOUNT 3
#define LUA_HOOKTAILCALL 4

/*
** Event masks
*/
#define LUA_MASKCALL (1 << LUA_HOOKCALL)
#define LUA_MASKRET (1 << LUA_HOOKRET)
#define LUA_MASKLINE (1 << LUA_HOOKLINE)
#define LUA_MASKCOUNT (1 << LUA_HOOKCOUNT)

namespace luajit {
void SetupAtPanicHandler(lua_State *L, LuaShared *luaSharedPtr) {
	luaShared = luaSharedPtr;
	LOG_INFO("Setting up atpanic handler");
	luaShared->CallRawFunction<lua_CFunction>(
		"lua_atpanic", L, &atpanic_handler
	);
	LOG_INFO("Set atpanic handler: %p", &atpanic_handler);

	// turn off jit
	// idx 0 = entire state
	luaShared->CallRawFunction<void>("luaJIT_setmode", L, 0, 0x000);
	LOG_INFO("Turned off JIT compilation");
}

}  // namespace luajit