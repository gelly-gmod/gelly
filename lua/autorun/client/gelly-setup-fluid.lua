---@module "gelly.fluid-presets"
local presets = include("gelly/fluid-presets.lua")
local DEFAULT_PRESET = "Water"

hook.Add("GellyLoaded", "gelly.presets-initialize", function()
	presets.loadPresets()
	presets.selectPreset(DEFAULT_PRESET)
end)
