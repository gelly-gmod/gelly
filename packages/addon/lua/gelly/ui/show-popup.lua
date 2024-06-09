include("popup.lua")

local function showPopup(title, content)
	local popup = vgui.Create("GellyPopup")
	popup:SetTopBarText(title)
	popup:SetContentText(content)
	popup:MakePopup()
end

return showPopup
