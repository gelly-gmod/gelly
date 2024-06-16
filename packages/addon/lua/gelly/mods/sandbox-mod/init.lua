-- The sandbox mod really just enables the Gelly Gun and thats about it.

local DEFAULT_PRESET = "Water"
gellyx.presets.select(DEFAULT_PRESET)

hook.Add("GellyModsShutdown", "gellyx.builtin.sandbox-mod", function()
	gelly.Reset()
end)
