GELLY_SIM_TIMESCALE = 10
SIMULATE_GELLY = true

local lastTimescale = GELLY_SIM_TIMESCALE

local function isGellyActive()
	return gelly.GetStatus().ActiveParticles > 0
end

hook.Add("GellyLoaded", "gelly.update-loop", function()
	timer.Create("gelly.flex-update-timer", 1 / 60, 0, function()
		if SIMULATE_GELLY then -- we don't check if gelly is active because we do need to update deferred particles (to prevent flicker)
			if lastTimescale ~= GELLY_SIM_TIMESCALE then
				lastTimescale = GELLY_SIM_TIMESCALE
				gelly.SetTimeStepMultiplier(GELLY_SIM_TIMESCALE)
			end

			gelly.Simulate(1 / 60) -- flex is programmed to assume a fixed timestep, normally 60hz
		end
	end)

	hook.Add("RenderScene", "gelly.render", function()
		if not isGellyActive() then return end
		gelly.Render()
	end)

	hook.Add("PostDrawOpaqueRenderables", "gelly.composite", function()
		if not isGellyActive() then return end
		gelly.Composite()
	end)
end)
