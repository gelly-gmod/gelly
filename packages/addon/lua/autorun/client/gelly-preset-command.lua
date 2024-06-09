include("gelly/ui/preset-creator/preset-creator.lua")

local presetCreatorInstance = nil

hook.Add("GellyLoaded", "gelly.register-preset-command", function()
	concommand.Add("gelly_preset_creator", function()
		if IsValid(presetCreatorInstance) then
			presetCreatorInstance:ToggleVisibility()
			return
		end

		presetCreatorInstance = vgui.Create("GellyPresetCreator")
		presetCreatorInstance:SetSize(ScrW() * 0.8, ScrH() * 0.8)
		presetCreatorInstance:Center()
		presetCreatorInstance:MakePopup()
	end)
end)
