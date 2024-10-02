---@module "gelly.logging"
local logging = include("gelly/logging.lua")
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
	preset.SolverParams.RestDistanceRatio = preset.SolverParams.RestDistanceRatio or 0.73
	gelly.ChangeParticleRadius(preset.Radius * gellyx.settings.get("preset_radius_scale"):GetFloat())
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

	-- synchronize the fluid settings
	gellyx.settings.get("fluid_radius"):SetFloat(preset.Radius)
	gellyx.settings.get("fluid_viscosity"):SetFloat(preset.SolverParams.Viscosity)
	gellyx.settings.get("fluid_cohesion"):SetFloat(preset.SolverParams.Cohesion)
	gellyx.settings.get("fluid_adhesion"):SetFloat(preset.SolverParams.Adhesion)
	gellyx.settings.get("fluid_friction"):SetFloat(preset.SolverParams.DynamicFriction)
	gellyx.settings.get("fluid_rest_distance_ratio"):SetFloat(preset.SolverParams.RestDistanceRatio)

	if GELLY_CUSTOMIZATION then
		-- tell the UI that we have new settings
		GELLY_CUSTOMIZATION:ForceSettingUpdate()
	end
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

function gellyx.presets.getEffectiveRadius()
	return GELLY_ACTIVE_PRESET.Radius * gellyx.settings.get("preset_radius_scale"):GetFloat()
end

function gellyx.presets.getRadiusScale()
	return gellyx.settings.get("preset_radius_scale"):GetFloat()
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

gellyx.settings.registerOnChange("preset_radius_scale", function()
	if not GELLY_ACTIVE_PRESET then
		return
	end

	selectPreset(GELLY_ACTIVE_PRESET)
end)

local EPHEMERAL_FLUID_SETTING_NAMES = {
	"fluid_radius",
	"fluid_viscosity",
	"fluid_cohesion",
	"fluid_adhesion",
	"fluid_friction",
	"fluid_rest_distance_ratio",
}

gellyx.settings.registerMultipleOnChange(EPHEMERAL_FLUID_SETTING_NAMES, function()
	if not GELLY_ACTIVE_PRESET then
		return
	end

	local newPreset = gellyx.presets.getActivePreset()
	newPreset.Radius = gellyx.settings.get("fluid_radius"):GetFloat()
	newPreset.SolverParams.Viscosity = gellyx.settings.get("fluid_viscosity"):GetFloat()
	newPreset.SolverParams.Cohesion = gellyx.settings.get("fluid_cohesion"):GetFloat()
	newPreset.SolverParams.Adhesion = gellyx.settings.get("fluid_adhesion"):GetFloat()
	newPreset.SolverParams.DynamicFriction = gellyx.settings.get("fluid_friction"):GetFloat()
	newPreset.SolverParams.RestDistanceRatio = gellyx.settings.get("fluid_rest_distance_ratio"):GetFloat()

	selectPreset(newPreset)
end)

gellyx.settings.registerMultipleOnChange({ "fluid_color_hex", "fluid_color_scale", "fluid_roughness", "fluid_opaque" },
	function()
		if not GELLY_ACTIVE_PRESET then
			return
		end

		-- CSS-style: #rrggbb
		local colorHex = gellyx.settings.get("fluid_color_hex"):GetString()
		local colorScale = gellyx.settings.get("fluid_color_scale"):GetFloat()

		local numericR = tonumber(colorHex:sub(2, 3), 16) / 255
		local numericG = tonumber(colorHex:sub(4, 5), 16) / 255
		local numericB = tonumber(colorHex:sub(6, 7), 16) / 255

		-- invert and multiply by the scale to hopefully approach the absorption that the user wants
		local color = Vector(1 - numericR, 1 - numericG, 1 - numericB) * colorScale
		local diffuseColor = Vector(numericR, numericG, numericB)

		local newPreset = gellyx.presets.getActivePreset()
		newPreset.Material.Absorption = color
		newPreset.Material.DiffuseColor = diffuseColor
		newPreset.Material.Roughness = gellyx.settings.get("fluid_roughness"):GetFloat()
		newPreset.Material.IsSpecularTransmission = not gellyx.settings.get("fluid_opaque"):GetBool()
		selectPreset(newPreset)
	end)
