local logging = include("gelly/logging.lua")
local isMenuAlreadyBound = input.LookupBinding("gelly_toggle_customization") ~= nil

if isMenuAlreadyBound then
	logging.warn("Gelly menu keybind is already bound. Exiting auto-bind.")
	return
end

logging.info("Creating a pseudo-bind for the Gelly menu. (M)")
hook.Add("GellyXButtonDown", "gelly.menu.bind", function(key)
	if key == KEY_M then
		RunConsoleCommand("gelly_toggle_customization")
	end
end)
