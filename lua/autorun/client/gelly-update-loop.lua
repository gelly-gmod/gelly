GELLY_SIM_TIMESCALE = 10
SIMULATE_GELLY = true

local lastTimescale = GELLY_SIM_TIMESCALE

hook.Add("GellyLoaded", "gelly.update-loop", function()
	timer.Create("gelly.flex-update-timer", 1 / 60, 0, function()
		if SIMULATE_GELLY then
			if lastTimescale ~= GELLY_SIM_TIMESCALE then
				lastTimescale = GELLY_SIM_TIMESCALE
				gelly.SetTimeStepMultiplier(GELLY_SIM_TIMESCALE)
			end

			gelly.Simulate(1 / 60) -- flex is programmed to assume a fixed timestep, normally 60hz
		end
	end)

	hook.Add("RenderScene", "gelly.render", function()
		gelly.Render()
	end)

	hook.Add("PostDrawOpaqueRenderables", "gelly.composite", function(depth, skybox)
		gelly.Composite()
	end)
end)
