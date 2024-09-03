local PANEL = {}

function PANEL:Init()
	self.HTML = self:Add("DHTML")
	self.HTML:Dock(FILL)
	self.HTML:OpenURL("asset://garrysmod/lua/html/customization.html.lua")
	self:SetVisible(false)
	self:SetSize(ScrW() * 0.8, ScrH() * 0.6)
	self:Center()
	self:SetupJSEnvironment()

	self.HTML.OnDocumentReady = function()
		self:SetVisible(true)
		self:MakePopup()
	end
end

function PANEL:Paint(w, h)
	local x, y = self:LocalToScreen(0, 0)

	render.SetScissorRect(x, y, x + w, y + h, true)
	Derma_DrawBackgroundBlur(self, 0);
	render.SetScissorRect(0, 0, 0, 0, false)
end

function PANEL:SetupJSEnvironment()

end

vgui.Register("GellyCustomizationMenu", PANEL, "EditablePanel")
