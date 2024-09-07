local function changeMaxParticles(_, _, args)
	if #args ~= 1 then
		print("Usage: gelly_change_max_particles <number>")
		return
	end

	local maxParticles = tonumber(args[1])
	if not maxParticles then
		print("Invalid number")
		return
	end

	gelly.ChangeMaxParticles(maxParticles)
	-- then reload the settings
	gellyx.presets.select(gellyx.presets.getActivePreset().Name)
	hook.Run("GellyRestarted")
	print("Max particles set to " .. maxParticles .. "!")
end

local function toggleCustomization()
	GELLY_CUSTOMIZATION:ToggleVisibility()
end

hook.Add("GellyLoaded", "gelly.add-settings-command", function()
	concommand.Add("gelly_change_max_particles", changeMaxParticles)
	concommand.Add("gelly_toggle_customization", toggleCustomization)
end)
