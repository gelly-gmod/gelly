#ifndef LUA_TABLE_H
#define LUA_TABLE_H
#include <optional>

#include "GarrysMod/Lua/LuaBase.h"

namespace gelly {
namespace gmod {
namespace helpers {

using GModType = decltype(GarrysMod::Lua::Type::None);

class LuaTable {
public:
	explicit LuaTable(GarrysMod::Lua::ILuaBase *lua) : lua(lua) {}

	[[nodiscard]] auto GetLength() const noexcept -> int {
		return lua->ObjLen(-1);
	}

	template <typename T>
	auto Get(const char *key, T defaultValue) const -> T {
		lua->GetField(-1, key);
		return FetchFromStack<T>(-1).value_or(defaultValue);
	}

	template <typename T>
	auto Get(const char *key) const -> std::optional<T> {
		lua->GetField(-1, key);
		return FetchFromStack<T>(-1);
	}

private:
	GarrysMod::Lua::ILuaBase *lua = nullptr;

	template <typename T>
	auto FetchFromStack(const int index) const noexcept -> std::optional<T> {
		const auto stackValueType = lua->GetType(index);
		if (const auto expectedType = InferType<T>();
			stackValueType != expectedType) {
			return std::nullopt;
		}

		if constexpr (std::is_same_v<T, bool>) {
			return static_cast<T>(lua->GetBool(index));
		} else if constexpr (std::is_same_v<T, double>) {
			return static_cast<T>(lua->GetNumber(index));
		} else if constexpr (std::is_same_v<T, const char *>) {
			return static_cast<T>(lua->GetString(index));
		} else if constexpr (std::is_same_v<T, Vector>) {
			return lua->GetVector(index);
		} else {
			return std::nullopt;
		}
	}

	template <typename T>
	[[nodiscard]] auto InferType() const noexcept -> GModType {
		if constexpr (std::is_same_v<T, bool>) {
			return GarrysMod::Lua::Type::Bool;
		} else if constexpr (std::is_same_v<T, double>) {
			return GarrysMod::Lua::Type::Number;
		} else if constexpr (std::is_same_v<T, const char *>) {
			return GarrysMod::Lua::Type::String;
		} else if constexpr (std::is_same_v<T, Vector>) {
			return GarrysMod::Lua::Type::Vector;
		} else {
			return GarrysMod::Lua::Type::None;
		}
	}
};

}  // namespace helpers
}  // namespace gmod
}  // namespace gelly

#endif	// LUA_TABLE_H
