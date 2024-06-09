--- The viewmodel for the settings panel.
--- It's not necessarily a good thing that this is being hoisted globally, but Derma's
--- design is not very friendly to passing data around.
GELLY_SETTINGS_VIEWMODEL = {}

local qualityMappings = {
	["Low"] = function()
		gelly.SetRenderSettings({ SmoothingIterations = 1, ThicknessIterations = 3 })
	end,
	["Medium"] = function()
		gelly.SetRenderSettings({ SmoothingIterations = 2, ThicknessIterations = 8 })
	end,
	["High"] = function()
		gelly.SetRenderSettings({ SmoothingIterations = 2, ThicknessIterations = 13 })
	end,
}

function GELLY_SETTINGS_VIEWMODEL.OnGraphicsQualityChange(qualityType)
	qualityMappings[qualityType]()
end

function GELLY_SETTINGS_VIEWMODEL.OnSimFPSChange(fps)
	START_SIM_FPS = fps
end
