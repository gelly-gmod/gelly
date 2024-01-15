GELLY_SIM_TIMESCALE = 4
GELLY_KILL_PLAYER_ON_CONTACT = false

local compositeCount = 0

hook.Add("GellyLoaded", "gelly.update-loop", function()
	hook.Add("RenderScene", "gelly.render-loop", function()
		gelly.Render()
		gelly.Simulate(1 / 60 * GELLY_SIM_TIMESCALE)
		compositeCount = 0
	end)

	hook.Add("PostDrawOpaqueRenderables", "gelly.update-loop", function()
		gelly.Composite()
		if
			GELLY_KILL_PLAYER_ON_CONTACT
			and gelly.IsEntityCollidingWithParticles(LocalPlayer():EntIndex())
		then
			RunConsoleCommand("kill")
		end
	end)
end)
