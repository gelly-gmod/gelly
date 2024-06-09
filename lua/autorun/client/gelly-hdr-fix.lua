hook.Add("GellyLoaded", "gelly.fix-hdr-cubemaps", function()
	local isHDREnabled = GetConVar("mat_hdr_level"):GetInt() >= 2
	if not isHDREnabled then
		return
	end

	gelly.SetCubemapStrength(10)
end)
