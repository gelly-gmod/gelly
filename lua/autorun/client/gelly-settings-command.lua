local function toggleSettings()
	if not IsValid(GELLY_SETTINGS) then
		vgui.Create("GellySettings")
	else
		GELLY_SETTINGS:Remove()
	end
end

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

	print("Max particles set to " .. maxParticles .. "!")
end

hook.Add("GellyLoaded", "gelly.add-settings-command", function()
	concommand.Add("gelly_change_max_particles", changeMaxParticles)
end)
