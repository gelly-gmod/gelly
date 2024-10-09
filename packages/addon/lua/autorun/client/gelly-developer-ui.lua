-- A simple UI for developers to check the status of gelly-gmod.

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

		local activePresetName = gellyx.presets.getActivePreset()
			and gellyx.presets.getActivePreset().Name
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
		gellyx.presets.select(gellyx.presets.getActivePreset().Name)
	end

	local timeScaleSlider = vgui.Create("DNumSlider")
	timeScaleSlider:SetPos(ScrW() - 200, 200)
	timeScaleSlider:SetSize(200, 50)
	timeScaleSlider:SetText("Time Scale")
	timeScaleSlider:SetMin(0)
	timeScaleSlider:SetMax(20)
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
		local name = gellyx.presets.getActivePreset().Name
		gellyx.presets.loadPresetFiles()
		gellyx.presets.select(name)
	end

	local selectPresetButton = vgui.Create("DButton")
	selectPresetButton:SetPos(ScrW() - 200, 250)
	selectPresetButton:SetSize(200, 50)
	selectPresetButton:SetText("Select Preset")
	selectPresetButton.DoClick = function()
		local frame = vgui.Create("DFrame")
		frame:SetSize(200, 200)
		frame:Center()
		frame:SetTitle("Select Preset")
		frame:MakePopup()

		local presetList = vgui.Create("DListView", frame)
		presetList:Dock(FILL)
		presetList:AddColumn("Name")

		for _, preset in pairs(GELLY_PRESETS) do
			presetList:AddLine(preset.Name)
		end

		presetList.OnRowSelected = function(_, _, row)
			local name = row:GetValue(1)
			gellyx.presets.select(name)
			frame:Close()
		end
	end

	local testTossButton = vgui.Create("DButton")
	testTossButton:SetPos(ScrW() - 200, 300)
	testTossButton:SetSize(200, 50)
	testTossButton:SetText("Test Toss")
	testTossButton.DoClick = function()
		local entity = LocalPlayer():GetEyeTrace().Entity
		if not entity then
			return
		end

		gelly.TestToss(entity:EntIndex(), Vector(10000, 0, 0))
	end

	local toggleKillPlayerOnContactButton = vgui.Create("DButton")
	toggleKillPlayerOnContactButton:SetPos(ScrW() - 200, 350)
	toggleKillPlayerOnContactButton:SetSize(200, 50)
	toggleKillPlayerOnContactButton:SetText("Toggle Kill Player On Contact (N)")
	toggleKillPlayerOnContactButton.DoClick = function()
		GELLY_KILL_PLAYER_ON_CONTACT = not GELLY_KILL_PLAYER_ON_CONTACT
		local suffix = GELLY_KILL_PLAYER_ON_CONTACT and "(Y)" or "(N)"
		toggleKillPlayerOnContactButton:SetText(
			("Toggle Kill Player On Contact %s"):format(suffix)
		)
	end

	local thresholdRatioSlider = vgui.Create("DNumSlider")
	thresholdRatioSlider:SetPos(ScrW() - 200, 400)
	thresholdRatioSlider:SetSize(200, 50)
	thresholdRatioSlider:SetText("Threshold Ratio")
	thresholdRatioSlider:SetMin(0)
	thresholdRatioSlider:SetMax(15)
	thresholdRatioSlider:SetDecimals(2)
	thresholdRatioSlider:SetValue(3)
	thresholdRatioSlider.OnValueChanged = function(_, value)
		gelly.ChangeThresholdRatio(value)
	end

	local iterationSlider = vgui.Create("DNumSlider")
	iterationSlider:SetPos(ScrW() - 200, 450)
	iterationSlider:SetSize(200, 50)
	iterationSlider:SetText("Iteration Count")
	iterationSlider:SetMin(0)
	iterationSlider:SetMax(800)
	iterationSlider:SetDecimals(0)
	iterationSlider:SetValue(15)
	iterationSlider.OnValueChanged = function(_, value)
		local gellySettings = gelly.GetGellySettings()
		gellySettings.FilterIterations = value
		gelly.SetGellySettings(gellySettings)
	end

	if gelly.IsRWDIBuild() then
		local reloadShadersButton = vgui.Create("DButton")
		reloadShadersButton:SetPos(ScrW() - 200, 500)
		reloadShadersButton:SetSize(200, 50)
		reloadShadersButton:SetText("Reload Shaders")
		reloadShadersButton.DoClick = function()
			gelly.ReloadAllShaders()
		end
	end

	local resetButton = vgui.Create("DButton")
	resetButton:SetPos(ScrW() - 200, 550)
	resetButton:SetSize(200, 50)
	resetButton:SetText("Reset")
	resetButton.DoClick = function()
		gelly.Reset()
	end

	local toggleSimulationButton = vgui.Create("DButton")
	toggleSimulationButton:SetPos(ScrW() - 200, 600)
	toggleSimulationButton:SetSize(200, 50)
	toggleSimulationButton:SetText("Toggle Simulation (Y)")
	toggleSimulationButton.DoClick = function()
		SIMULATE_GELLY = not SIMULATE_GELLY
		local suffix = SIMULATE_GELLY and "(Y)" or "(N)"
		toggleSimulationButton:SetText(("Toggle Simulation %s"):format(suffix))
	end

	local diffuseScaleSlider = vgui.Create("DNumSlider")
	diffuseScaleSlider:SetPos(ScrW() - 200, 700)
	diffuseScaleSlider:SetSize(200, 50)
	diffuseScaleSlider:SetText("Diffuse Scale")
	diffuseScaleSlider:SetMin(0)
	diffuseScaleSlider:SetMax(20)
	diffuseScaleSlider:SetDecimals(2)
	diffuseScaleSlider:SetValue(1)
	diffuseScaleSlider.OnValueChanged = function(_, value)
		gelly.SetDiffuseScale(value)
	end

	local diffuseMotionBlurSlider = vgui.Create("DNumSlider")
	diffuseMotionBlurSlider:SetPos(ScrW() - 400, 700)
	diffuseMotionBlurSlider:SetSize(200, 50)
	diffuseMotionBlurSlider:SetText("Diffuse Motion Blur")
	diffuseMotionBlurSlider:SetMin(0)
	diffuseMotionBlurSlider:SetMax(25)
	diffuseMotionBlurSlider:SetDecimals(2)
	diffuseMotionBlurSlider:SetValue(1)
	diffuseMotionBlurSlider.OnValueChanged = function(_, value)
		gelly.SetDiffuseMotionBlur(value)
	end

	local syncCheckbox = vgui.Create("DCheckBoxLabel")
	syncCheckbox:SetPos(ScrW() - 500, 700)
	syncCheckbox:SetText("Enable GPU Sync")
	syncCheckbox:SetValue(true)
	syncCheckbox:SizeToContents()
	syncCheckbox.OnChange = function(_, value)
		local gellySettings = gelly.GetGellySettings()
		gellySettings.EnableGPUSynchronization = value
		gelly.SetGellySettings(gellySettings)
	end

	local openPresetDebuggerMenu = vgui.Create("DButton")
	openPresetDebuggerMenu:SetPos(ScrW() - 800, 700)
	openPresetDebuggerMenu:SetSize(200, 50)
	openPresetDebuggerMenu:SetText("Preset Debugger")
	openPresetDebuggerMenu.DoClick = function()
		local frame = vgui.Create("DFrame")
		frame:SetSize(400, 700)
		frame:Center()
		frame:SetTitle("Preset Debugger")
		frame:MakePopup()

		local preset = GELLY_ACTIVE_PRESET
		local radiusSlider = vgui.Create("DNumSlider", frame)
		radiusSlider:SetPos(0, 50)
		radiusSlider:SetSize(200, 50)
		radiusSlider:SetText("Radius")
		radiusSlider:SetMin(0)
		radiusSlider:SetMax(50)
		radiusSlider:SetDecimals(2)
		radiusSlider:SetValue(preset.Radius)
		radiusSlider.OnValueChanged = function(_, value)
			preset.Radius = value
			gellyx.presets.selectEphemeralPreset(preset)
		end

		local viscositySlider = vgui.Create("DNumSlider", frame)
		viscositySlider:SetPos(0, 100)
		viscositySlider:SetSize(200, 50)
		viscositySlider:SetText("Viscosity")
		viscositySlider:SetMin(0)
		viscositySlider:SetMax(50)
		viscositySlider:SetDecimals(2)
		viscositySlider:SetValue(preset.SolverParams.Viscosity)
		viscositySlider.OnValueChanged = function(_, value)
			preset.SolverParams.Viscosity = value
			gellyx.presets.selectEphemeralPreset(preset)
		end

		local surfaceTensionSlider = vgui.Create("DNumSlider", frame)
		surfaceTensionSlider:SetPos(0, 150)
		surfaceTensionSlider:SetSize(200, 50)
		surfaceTensionSlider:SetText("Surface Tension")
		surfaceTensionSlider:SetMin(0)
		surfaceTensionSlider:SetMax(50)
		surfaceTensionSlider:SetDecimals(2)
		surfaceTensionSlider:SetValue(preset.SolverParams.SurfaceTension)
		surfaceTensionSlider.OnValueChanged = function(_, value)
			preset.SolverParams.SurfaceTension = value
			gellyx.presets.selectEphemeralPreset(preset)
		end

		local adhesionSlider = vgui.Create("DNumSlider", frame)
		adhesionSlider:SetPos(0, 200)
		adhesionSlider:SetSize(200, 50)
		adhesionSlider:SetText("Adhesion")
		adhesionSlider:SetMin(0)
		adhesionSlider:SetMax(50)
		adhesionSlider:SetDecimals(2)
		adhesionSlider:SetValue(preset.SolverParams.Adhesion)
		adhesionSlider.OnValueChanged = function(_, value)
			preset.SolverParams.Adhesion = value
			gellyx.presets.selectEphemeralPreset(preset)
		end

		local cohesionSlider = vgui.Create("DNumSlider", frame)
		cohesionSlider:SetPos(0, 250)
		cohesionSlider:SetSize(200, 50)
		cohesionSlider:SetText("Cohesion")
		cohesionSlider:SetMin(0)
		cohesionSlider:SetMax(50)
		cohesionSlider:SetDecimals(2)
		cohesionSlider:SetValue(preset.SolverParams.Cohesion)
		cohesionSlider.OnValueChanged = function(_, value)
			preset.SolverParams.Cohesion = value
			gellyx.presets.selectEphemeralPreset(preset)
		end

		local vorticityConfinementSlider = vgui.Create("DNumSlider", frame)
		vorticityConfinementSlider:SetPos(0, 300)
		vorticityConfinementSlider:SetSize(200, 50)
		vorticityConfinementSlider:SetText("Vorticity Confinement")
		vorticityConfinementSlider:SetMin(0)
		vorticityConfinementSlider:SetMax(50)
		vorticityConfinementSlider:SetDecimals(2)
		vorticityConfinementSlider:SetValue(preset.SolverParams.VorticityConfinement)
		vorticityConfinementSlider.OnValueChanged = function(_, value)
			preset.SolverParams.VorticityConfinement = value
			gellyx.presets.selectEphemeralPreset(preset)
		end

		local dynamicFrictionSlider = vgui.Create("DNumSlider", frame)
		dynamicFrictionSlider:SetPos(0, 350)
		dynamicFrictionSlider:SetSize(200, 50)
		dynamicFrictionSlider:SetText("Dynamic Friction")
		dynamicFrictionSlider:SetMin(0)
		dynamicFrictionSlider:SetMax(50)
		dynamicFrictionSlider:SetDecimals(2)
		dynamicFrictionSlider:SetValue(preset.SolverParams.DynamicFriction)
		dynamicFrictionSlider.OnValueChanged = function(_, value)
			preset.SolverParams.DynamicFriction = value
			gellyx.presets.selectEphemeralPreset(preset)
		end

		local restDistanceRatioSlider = vgui.Create("DNumSlider", frame)
		restDistanceRatioSlider:SetPos(0, 400)
		restDistanceRatioSlider:SetSize(200, 50)
		restDistanceRatioSlider:SetText("Rest Distance Ratio")
		restDistanceRatioSlider:SetMin(0)
		restDistanceRatioSlider:SetMax(3)
		restDistanceRatioSlider:SetDecimals(2)
		restDistanceRatioSlider:SetValue(preset.SolverParams.RestDistanceRatio)
		restDistanceRatioSlider.OnValueChanged = function(_, value)
			preset.SolverParams.RestDistanceRatio = value
			gellyx.presets.selectEphemeralPreset(preset)
		end
	end
end)
