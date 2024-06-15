local array = include("gelly/util/functional-arrays.lua")

---@alias gellyx.mods.ModInfo {Name: string, ID: string, Author: string, Description: string, Type: number, Weapons: table, Entities: table}

---@type table<string, type>
local MANDATORY_MOD_KEYS = {
	Name = "string",
	ID = "string",
	Author = "string",
	Description = "string",
	Type = "number",

	Weapons = "table",
	Entities = "table",
}

--- Validates that a mod has all the mandatory keys and that they are of the correct type.
---@param mod gellyx.mods.ModInfo
---@return boolean
return function(mod)
	return array(MANDATORY_MOD_KEYS)
		:all(function(expectedType, key)
			return type(mod[key]) == expectedType
		end)
end
