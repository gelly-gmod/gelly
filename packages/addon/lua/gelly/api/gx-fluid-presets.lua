---@module "gelly.logging"
local logging = include("gelly/logging.lua")
local presetRadiusScaleConVar = CreateClientConVar("gelly_preset_radius_scale", "1", true, false,
	"The scale of the preset radius. This variable is useful for scaling presets to match the scale of the scene.")
gellyx = gellyx or {}
gellyx.presets = gellyx.presets or {}
GELLY_PRESETS = GELLY_PRESETS or {}
GELLY_CUSTOM_PRESETS = GELLY_CUSTOM_PRESETS or {}
GELLY_ACTIVE_PRESET = GELLY_ACTIVE_PRESET or nil

local function loadPreset(path)
	return include(path)
end

--- Loads or reloads all presets from the filesystem.
---
--- Not particularly useful for production, but useful for development of presets.
---
--- To expand on that, it's probably not a good idea to use this function each frame or something similar as it
--- will cause a lot of unnecessary disk reads.
function gellyx.presets.loadPresetFiles()
	local files, _ = file.Find("gelly/api/presets/immutable/*.lua", "LUA")

	for _, fileName in ipairs(files) do
		local path = "gelly/api/presets/immutable/" .. fileName
		local preset = loadPreset(path)

		logging.info("Loading immutable preset %s", path)
		if preset then
			GELLY_PRESETS[preset.Name] = preset
			logging.info("Loaded immutable preset %s", path)
		end
	end
end

local function selectPreset(preset)
	gelly.ChangeParticleRadius(preset.Radius * presetRadiusScaleConVar:GetFloat())
	gelly.SetFluidProperties(preset.SolverParams)
	gelly.SetFluidMaterial(preset.Material)
	gelly.SetDiffuseScale(preset.DiffuseScale)
	GELLY_ACTIVE_PRESET = preset
	logging.info("Selected preset %s", preset.Name)
end

--- Selects a loaded preset to be used for all future particles.
--- # Warning
--- This function will affect all the current particles beha
---@param name string The name of the preset to select.
function gellyx.presets.select(name)
	local preset = GELLY_PRESETS[name]
	if not preset then
		logging.error("Preset %s does not exist", name)
	end

	selectPreset(preset)
end

function gellyx.presets.selectEphemeralPreset(preset)
	if type(preset) ~= "table" then
		logging.error("Ephemeral preset must be a table")
	end

	selectPreset(preset)
end

function gellyx.presets.getActivePreset()
	return table.Copy(GELLY_ACTIVE_PRESET)
end

function gellyx.presets.getAllPresets()
	return table.Copy(GELLY_PRESETS)
end

function gellyx.presets.getCustomPresets()
	return table.Copy(GELLY_CUSTOM_PRESETS)
end

--- Copies a preset's material.
---@param name string The name of the preset to copy the material from.
---@return table The copied material, this is an actual copy and not a reference.
function gellyx.presets.copyPresetMaterial(name)
	local preset = GELLY_PRESETS[name]
	if not preset then
		logging.error("Preset %s does not exist", name)
	end

	local material = table.Copy(preset.Material)
	return material
end

cvars.AddChangeCallback("gelly_preset_radius_scale", function(_, _, newValue)
	if not GELLY_ACTIVE_PRESET then
		return
	end

	gelly.ChangeParticleRadius(GELLY_ACTIVE_PRESET.Radius * newValue)
end)
