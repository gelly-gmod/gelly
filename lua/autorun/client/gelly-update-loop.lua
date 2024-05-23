-- Should never be zero, or the simulation will not run
local FPS_AT_MAX_PARTICLES = 2
START_SIM_FPS = 144

GELLY_SIM_TIMESCALE = 10
GELLY_KILL_PLAYER_ON_CONTACT = false

SIMULATE_GELLY = true

hook.Add("GellyLoaded", "gelly.update-loop", function()
	hook.Add("RenderScene", "gelly.render", function()
		if SIMULATE_GELLY then
			gelly.SetTimeStepMultiplier(GELLY_SIM_TIMESCALE)
			gelly.Simulate(1 / 60) -- flex is programmed to assume a fixed timestep, normally 60hz
		end
		gelly.Render()
	end)

	hook.Add("PostDrawOpaqueRenderables", "gelly.composite", function(depth, skybox)
		gelly.Composite()
	end)
end)
