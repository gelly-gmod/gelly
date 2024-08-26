local logging = include("gelly/logging.lua")

local gellyHDRFixConvar = CreateClientConVar("gelly_hdr_fix", "0", true, false,
	"Fixes HDR cubemaps for Gelly. Set to 1 if some maps have black or faint reflections.")

-- Collection of maps which have reproducible bad reflections when HDR is enabled.
local KNOWN_BAD_MAPS = {
	["gm_construct"] = true,
	["gm_flatgrass"] = true
}

hook.Add("GellyLoaded", "gelly.fix-hdr-cubemaps", function()
	local isFixEnabled = gellyHDRFixConvar:GetBool()
	local isKnownBadMap = KNOWN_BAD_MAPS[game.GetMap()]
	local isHDREnabled = GetConVar("mat_hdr_level"):GetInt() >= 2
	if not isHDREnabled then
		return
	end

	if not isFixEnabled and not isKnownBadMap then
		return
	end

	logging.info("Applying HDR fix for map %s", game.GetMap())
	gelly.SetCubemapStrength(30)
end)
