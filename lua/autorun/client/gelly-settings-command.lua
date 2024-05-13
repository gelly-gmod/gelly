local function toggleSettings()
	if not IsValid(GELLY_SETTINGS) then
		vgui.Create("GellySettings")
	else
		GELLY_SETTINGS:Remove()
	end
end

hook.Add("GellyLoaded", "gelly.add-settings-command", function()
	concommand.Add("gelly_settings", toggleSettings)
end)
