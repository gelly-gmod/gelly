GELLY_SIM_TIMESCALE = 4

hook.Add("GellyLoaded", "gelly.update-loop", function()
	hook.Add("PostDrawOpaqueRenderables", "gelly.update-loop", function()
		gelly.Render()
		gelly.Simulate(1 / 60 * GELLY_SIM_TIMESCALE)
	end)
end)
