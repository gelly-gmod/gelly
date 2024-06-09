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

	gelly.Reset()
	gellyx.presets.select("Water")
	gelly.ChangeMaxParticles(128000)
	gellyx.presets.select("Water") -- for safety

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
			velocity = Vector(0, 20, 10), -- make them fly towards each other
			bounds = Vector(50, 50, 50),
			density = 1000,
		})
	end

	for i = 1, 40 do
		gellyx.emitters.Cube({
			center = EMIT_SPOT_B,
			velocity = Vector(0, -20, 10),
			bounds = Vector(50, 50, 50),
			density = 1000,
		})
	end

	sampler:InjectTimingHooks()
	gelly.SetTimeStepMultiplier(3)

	timer.Simple(5, function()
		sampler:RemoveTimingHooks()
		gelly.SetTimeStepMultiplier(1)
		gelly.Reset()
		hook.Remove("CalcView", "gelly.performance-debugger")
		print("Performance test complete.")
		print(("Render time (ms): %.2f"):format(sampler:GetRenderAverage()))
		print(("Simulate time (ms): %.2f"):format(sampler:GetSimulateAverage()))
		print(("Composite time (ms): %.2f"):format(sampler:GetCompositeAverage()))
		print("----------------------")
		print(("GPU: %s"):format(gelly.GetStatus().ComputeDeviceName))
		print(("Branch: %s"):format(BRANCH))
	end)
end

hook.Add("GellyLoaded", "gelly.performance-debugger-command", function()
	concommand.Add("gelly_performance_debugger", performanceDebugger)
end)
