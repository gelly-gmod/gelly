local showPopup = include("gelly/ui/show-popup.lua")

local GELLY_MISSING_TITLE = "Gelly Missing"
local GELLY_MISSING_CONTENT =
	"The binary module for gelly-gmod is missing, or can't be loaded. Please check your installation."

-- NOTE: The hook type used is highly important. gelly-gmod internally reads the current rendering context from the game,
-- and at something that would sound sensible, such as InitPostEntity, the rendering context is not yet available--so gelly-gmod reads garbage.
-- so we need something which signifies that the first frame has passed, and the rendering context is available which is the first PostRender call.

hook.Add("PostRender", "gelly.load-gelly", function()
	hook.Remove("PostRender", "gelly.load-gelly")

	local isGellyLoaded = pcall(require, "gelly-gmod")

	if isGellyLoaded then
		showPopup(GELLY_MISSING_TITLE, GELLY_MISSING_CONTENT)
	end
end)
