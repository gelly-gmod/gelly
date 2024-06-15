local array = include("gelly/util/functional-arrays.lua")
local logging = include("gelly/logging.lua")
local isModValid = include("gelly/api/mods/validate-mod.lua")

local GELLY_MODS_DIR = "lua/gelly/mods"
local GELLY_MODS_INFO_FILE = "info.lua"

local function loadModInfo(path)
	local info = include(GELLY_MODS_DIR .. "/" .. mod .. "/" .. GELLY_MODS_INFO_FILE)

	if not info then
		logging.warn("Skipping mod " .. mod .. " because it is missing info.")
		return nil
	end

	return info
end

local function getModDirectories()
	return array(select(2, file.Find(GELLY_MODS_DIR .. "/*", "LUA")))
end

--- Fetches all Gelly mods, returning their parsed info. If a mod is missing information or is invalid, it is skipped and a warning is printed.
---@return table<string, table>
return function()
	array(getModDirectories())
		:map(loadModInfo)
		:filter(isModValid)
		:toArray()
end
