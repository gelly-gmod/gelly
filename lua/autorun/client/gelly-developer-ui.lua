-- A simple UI for developers to check the status of gelly-gmod.
---@module "gelly.fluid-presets"
local presets = include("gelly/fluid-presets.lua")

hook.Add("GellyLoaded", "gelly.init-dev-ui", function()
	local developerConvar = GetConVar("developer")
	local isDeveloper = developerConvar and developerConvar:GetInt() > 0

	if not isDeveloper then
		return
	end

	hook.Add("HUDPaint", "gelly.developer-ui", function()
		local statusInfo = gelly.GetStatus()

		draw.SimpleText(
			("Active GPU: %s"):format(statusInfo.ComputeDeviceName),
			"ChatFont",
			ScrW() - 10,
			10,
			color_white,
			TEXT_ALIGN_RIGHT
		)
		draw.SimpleText(
			("Current particle count: %d"):format(statusInfo.ActiveParticles),
			"ChatFont",
			ScrW() - 10,
			30,
			color_white,
			TEXT_ALIGN_RIGHT
		)
		draw.SimpleText(
			("Maximum particle count: %d"):format(statusInfo.MaxParticles),
			"ChatFont",
			ScrW() - 10,
			50,
			color_white,
			TEXT_ALIGN_RIGHT
		)

		local activePresetName = presets.getActivePreset()
				and presets.getActivePreset().Name
			or "None"

		draw.SimpleText(
			("Active fluid preset: %s"):format(activePresetName),
			"ChatFont",
			ScrW() - 10,
			70,
			color_white,
			TEXT_ALIGN_RIGHT
		)
	end)
end)
