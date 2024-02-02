local showPopup = include("gelly/ui/show-popup.lua")
local GELLY_MISSING_TITLE = "Gelly Missing"
local GELLY_MISSING_CONTENT =
	"The binary module for gelly-gmod is missing, or can't be loaded. Please check your installation."

local GELLY_MAP_TITLE = "Failed to parse map"
local GELLY_MAP_CONTENT =
	"Failed to parse the map. This is due to the underlying parser failing to parse certain lumps of the map. This is usually due to the map using non-standard features or uncommon features from different BSP versions."

-- NOTE: The hook type used is highly important. gelly-gmod internally reads the current rendering context from the game,
-- and at something that would sound sensible, such as InitPostEntity, the rendering context is not yet available--so gelly-gmod reads garbage.
-- so we need something which signifies that the first frame has passed, and the rendering context is available which is the first PostRender call.

hook.Add("PostRender", "gelly.load-gelly", function()
	hook.Remove("PostRender", "gelly.load-gelly")

	local isGellyLoaded, errorMessage = pcall(require, "gelly-gmod")

	if not isGellyLoaded then
		showPopup(GELLY_MISSING_TITLE, GELLY_MISSING_CONTENT)
		ErrorNoHalt(errorMessage)
		return
	end

	local mapPath = ("maps/%s.bsp"):format(game.GetMap())
	local didMapLoad, mapErrorMessage = pcall(gelly.LoadMap, mapPath)
	if not didMapLoad then
		showPopup(GELLY_MAP_TITLE, GELLY_MAP_CONTENT)
		-- we can continue, but the map won't be loaded
		ErrorNoHalt(mapErrorMessage)
	end

	-- setup the gellyx api before other addons can use it
	include("gelly/api/gx-init.lua")

	hook.Run("GellyLoaded")
	-- also, add the particle system
	game.AddParticles("particles/gelly.pcf")
end)
