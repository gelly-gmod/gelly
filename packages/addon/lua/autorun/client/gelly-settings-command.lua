local function toggleCustomization()
	GELLY_CUSTOMIZATION:ToggleVisibility()
end

hook.Add("GellyLoaded", "gelly.add-settings-command", function()
	concommand.Add("gelly_toggle_customization", toggleCustomization)
end)
