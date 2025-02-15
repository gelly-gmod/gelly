---@module "gelly.util.timing-sampler"
local TimingSampler = include("gelly/util/timing-sampler.lua")

local EMIT_SPOT_A = Vector(-184.198822, -606.370178, 512.743347)
local EMIT_SPOT_B = Vector(-185.235199, -156.371429, 512.743347)
local CAM_POS = Vector(85.694962, -390.874725, 548.456665)

local function performanceDebugger()
	-- We will perform a simple test to understand the performance dynamics of the system that we're working with.
	if game.GetMap() ~= "gm_construct" then
		print("This script is only intended to be run on gm_construct.")
		return
	end

	gui.HideGameUI()
	gelly.Reset()
	gellyx.presets.select("Water")

	local sampler = TimingSampler.new()

	hook.Add("CalcView", "gelly.performance-debugger", function(ply, pos, angles, fov)
		return {
			origin = CAM_POS,
			angles = Angle(0, -180, 0),
			fov = fov,
			drawviewer = false,
		}
	end)

	gelly.SetTimeStepMultiplier(0)

	for i = 1, 40 do
		gellyx.emitters.Cube({
			center = EMIT_SPOT_A,
			velocity = Vector(0, 80, 10), -- make them fly towards each other
			bounds = Vector(50, 50, 50),
			density = 1000,
		})
	end

	for i = 1, 40 do
		gellyx.emitters.Cube({
			center = EMIT_SPOT_B,
			velocity = Vector(0, -80, 10),
			bounds = Vector(50, 50, 50),
			density = 2000,
		})
	end

	sampler:InjectTimingHooks()
	gelly.SetTimeStepMultiplier(1)

	local settings = gelly.GetGellySettings()
	settings.EnableGPUTiming = true
	gelly.SetGellySettings(settings)
	timer.Simple(2.8, function()
		local timings = gelly.GetGellyTimings()
		settings.EnableGPUTiming = false
		gelly.SetGellySettings(settings)

		local totalGPUTimeMs = 0
		for _, timing in pairs(timings) do
			if type(timing) ~= "number" then
				continue
			end

			totalGPUTimeMs = totalGPUTimeMs + timing
		end

		sampler:RemoveTimingHooks()
		gelly.SetTimeStepMultiplier(1)
		gelly.Reset()
		hook.Remove("CalcView", "gelly.performance-debugger")
		print("Performance test complete.")
		print(("Render time (CPU, ms): %.2f"):format(sampler:GetRenderAverage()))
		print(("    + Compute acceleration: %.2fms"):format(timings.ComputeAcceleration))
		print(("    + Spray splatting: %.2fms"):format(timings.SpraySplatting))
		print(("    + Ellipsoid splatting: %.2fms"):format(timings.EllipsoidSplatting))
		print(("    + Thickness splatting: %.2fms"):format(timings.ThicknessSplatting))
		print(("    + Albedo downsampling: %.2fms"):format(timings.AlbedoDownsampling))
		print(("    + Raw normal estimation: %.2fms"):format(timings.RawNormalEstimation))
		print(("    + Surface filtering: %.2fms"):format(timings.SurfaceFiltering))
		print(("Render time (GPU, ms): %.2f"):format(totalGPUTimeMs))
		print(("Simulate time (ms): %.2f"):format(sampler:GetSimulateAverage()))
		print(("Composite time (ms): %.2f"):format(sampler:GetCompositeAverage()))
		print("----------------------")
		print(("GPU: %s"):format(gelly.GetStatus().ComputeDeviceName))
		print(("Branch: %s"):format(BRANCH))
		print(("Sim iterations: %d"):format(gellyx.settings.get("simulation_iterations"):GetInt()))
		print(("Sim substeps: %d"):format(gellyx.settings.get("simulation_substeps"):GetInt()))
		print(("Sim rate: %dHz"):format(gellyx.settings.get("simulation_rate"):GetInt()))
		print(("Smoothness: %d"):format(gellyx.settings.get("smoothness"):GetInt()))
		print(("Screen resolution: %dx%d"):format(ScrW(), ScrH()))
		print(("Max particles: %d"):format(gellyx.settings.get("max_particles"):GetInt()))

		if timings.IsDisjoint then
			print("WARNING !!!")
			print("----------------------")
			print(
				"The GPU timings are disjointed. Typically, this is a sign of heavy power or thermal throttling, such as Laptop Power Savings or AC Power Savings mode.")
		end

		gui.ActivateGameUI()
	end)
end

hook.Add("GellyLoaded", "gelly.performance-debugger-command", function()
	concommand.Add("gelly_performance_debugger", performanceDebugger)
end)
