gellyx = gellyx or {}
gellyx.mods = gellyx.mods or {}
gellyx.mods.loadedMods = gellyx.mods.loadedMods or {}

local INVALID_PARSE = { Valid = false }

local CRUCIAL_FILES = {
	"info.lua",
	"init.lua",
}

local function parseMod(path)
	for _, crucialFile in ipairs(CRUCIAL_FILES) do
		if not file.Exists(path .. "/" .. crucialFile, "LUA") then
			return INVALID_PARSE
		end
	end

	local info = include(path .. "/info.lua")

	if not info then
		return INVALID_PARSE
	end

	if
		info.Name == nil
		or info.Author == nil
		or info.Description == nil
		or info.Type == nil
	then
		return INVALID_PARSE
	end

	print("[gellyx mods] - Loaded mod " .. info.Name)
	print("\tAuthor: " .. info.Author)
	print("\tDescription: " .. info.Description)
	print("\tType: " .. info.Type)
	print("\tPath: " .. path)

	return {
		Valid = true,
		Enabled = true,
		Info = info,
		Path = path,
		InitPath = path .. "/init.lua",
	}
end

--- Loads a mod into the manager.
---@param path string The path to the mod.
---@return boolean If the mod was loaded successfully.
function gellyx.mods.loadMod(path)
	local mod = parseMod(path)

	if not mod.Valid then
		ErrorNoHalt("[gellyx mods] - Failed to load mod at " .. path)
		return false
	end

	gellyx.mods.loadedMods[mod.Info.Name] = mod

	return true
end

--- Loads all mods from the filesystem.
function gellyx.mods.loadMods()
	local _, dirs = file.Find("gelly/mods/*", "LUA")

	for _, mod in ipairs(dirs) do
		gellyx.mods.loadMod("gelly/mods/" .. mod)
	end
end

--- Initializes all loaded mods.
function gellyx.mods.initializeMods()
	for _, mod in pairs(gellyx.mods.loadedMods) do
		if mod.Enabled then
			include(mod.InitPath)
		end
	end
end

hook.Add("GellyLoaded", "gellyx.mods", function()
	gellyx.mods.loadMods()
	gellyx.mods.initializeMods()
end)
