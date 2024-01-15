local DEFAULT_PRESET = "Water"

hook.Add("GellyLoaded", "gelly.presets-initialize", function()
	gellyx.presets.loadPresetFiles()
	gellyx.presets.select(DEFAULT_PRESET)
end)
