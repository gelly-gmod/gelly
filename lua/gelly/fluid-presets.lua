---@module "gelly.logging"
local logging = include("gelly/logging.lua")
GELLY_PRESETS = GELLY_PRESETS or {}
GELLY_ACTIVE_PRESET = GELLY_ACTIVE_PRESET or nil

local function loadPreset(path)
	return include(path)
end

local function loadPresets()
	local files, _ = file.Find("gelly/presets/immutable/*.lua", "LUA")

	for _, file in ipairs(files) do
		local path = "gelly/presets/immutable/" .. file
		local preset = loadPreset(path)

		logging.info("Loading immutable preset %s", path)
		if preset then
			GELLY_PRESETS[preset.Name] = preset
			logging.info("Loaded immutable preset %s", path)
		end
	end
end

local function selectPreset(name)
	local preset = GELLY_PRESETS[name]
	if not preset then
		logging.error("Preset %s does not exist", name)
	end

	gelly.SetFluidProperties(preset.SolverParams)
	gelly.SetFluidVisualParams(preset.VisualParams)
	GELLY_ACTIVE_PRESET = preset
	logging.info("Selected preset %s", name)
end

local function getActivePreset()
	return GELLY_ACTIVE_PRESET
end

return {
	loadPresets = loadPresets,
	selectPreset = selectPreset,
	getActivePreset = getActivePreset,
}
