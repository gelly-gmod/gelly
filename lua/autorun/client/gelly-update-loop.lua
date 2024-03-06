-- Should never be zero, or the simulation will not run
local FPS_AT_MAX_PARTICLES = 2
local START_SIM_FPS = 60

GELLY_SIM_TIMESCALE = 10
GELLY_SIM_INITIAL_FPS = 1 / START_SIM_FPS
GELLY_KILL_PLAYER_ON_CONTACT = false

hook.Add("GellyLoaded", "gelly.update-loop", function()
	hook.Add("RenderScene", "gelly.render", gelly.Render)
	hook.Add("PostDrawOpaqueRenderables", "gelly.composite", gelly.Composite)
end)

local function calculateThrottledDeltaTime()
	return 1
		/ math.floor(
			(FPS_AT_MAX_PARTICLES - START_SIM_FPS)
					* (gelly.GetStatus().ActiveParticles / gelly.GetStatus().MaxParticles)
				+ START_SIM_FPS
		)
end

timer.Create("gelly.update", GELLY_SIM_INITIAL_FPS, 0, function()
	local dt = calculateThrottledDeltaTime()
	print(math.floor(1 / dt))
	gelly.Simulate(dt * GELLY_SIM_TIMESCALE)
	timer.Adjust("gelly.update", dt, nil, nil)
end)
