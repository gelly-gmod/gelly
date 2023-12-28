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

	local radiusSlider = vgui.Create("DNumSlider")
	radiusSlider:SetPos(ScrW() - 200, 100)
	radiusSlider:SetSize(200, 50)
	radiusSlider:SetText("Radius")
	radiusSlider:SetMin(0)
	radiusSlider:SetMax(50)
	radiusSlider:SetDecimals(2)

	radiusSlider.OnValueChanged = function(_, value)
		gelly.Reset()
		gelly.ChangeParticleRadius(value)
		-- Re apply the preset as that causes an invalidation of the simulation
		presets.selectPreset(presets.getActivePreset().Name)
	end

	local timeScaleSlider = vgui.Create("DNumSlider")
	timeScaleSlider:SetPos(ScrW() - 200, 200)
	timeScaleSlider:SetSize(200, 50)
	timeScaleSlider:SetText("Time Scale")
	timeScaleSlider:SetMin(1)
	timeScaleSlider:SetMax(10)
	timeScaleSlider:SetDecimals(2)
	timeScaleSlider:SetValue(GELLY_SIM_TIMESCALE)
	timeScaleSlider.OnValueChanged = function(_, value)
		GELLY_SIM_TIMESCALE = value
	end

	local reloadPresetsButton = vgui.Create("DButton")
	reloadPresetsButton:SetPos(ScrW() - 200, 150)
	reloadPresetsButton:SetSize(200, 50)
	reloadPresetsButton:SetText("Reload Presets")
	reloadPresetsButton.DoClick = function()
		local name = presets.getActivePreset().Name
		presets.loadPresets()
		presets.selectPreset(name)
	end
end)
