local SUN_VISIBILITY_UPDATE_INTERVAL_SECONDS = 0.1
local logging = include("gelly/logging.lua")

hook.Add("GellyLoaded", "gelly.initialize-sun", function()
	local sunEnabled = gellyx.settings.get("sun_visible"):GetBool()
	gelly.SetSunEnabled(sunEnabled)

	local sunInfo = util.GetSunInfo()
	if not sunInfo then
		logging.warn("Sun info not available, not initializing")
		return
	end

	logging.info("Sun info available, initializing")

	local sunDirection = sunInfo.direction
	gelly.SetSunDirection(sunDirection)

	timer.Create("gelly.update-sun-visibility", SUN_VISIBILITY_UPDATE_INTERVAL_SECONDS, 0, function()
		if not gellyx.settings.get("sun_visible"):GetBool() then
			gelly.SetSunEnabled(false)
			return
		end

		local eyePos = LocalPlayer():EyePos()
		local sunPos = sunDirection * 65535

		local shadowTrace = util.QuickTrace(eyePos, sunPos, LocalPlayer())
		local sunVisible = shadowTrace.HitSky
		gelly.SetSunEnabled(sunVisible)
	end)
end)
