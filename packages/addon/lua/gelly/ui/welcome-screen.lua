local PANEL = {}

function PANEL:Init()
	self.HTML = self:Add("DHTML")
	self.HTML:Dock(FILL)
	self.HTML:OpenURL("asset://garrysmod/lua/html/welcome_screen.html.lua")
	self:SetVisible(false)
	self:SetSize(ScrW(), ScrH())
	self:SetPos(0, 0)
	self:SetupJSEnvironment()

	self.HTML.OnDocumentReady = function()
		surface.PlaySound("gelly_welcome.wav")
		self:SetVisible(true)
		self:MakePopup()
	end
end

function PANEL:Paint(w, h)
	local x, y = self:LocalToScreen(0, 0)

	render.SetScissorRect(x, y, x + w, y + h, true)
	Derma_DrawBackgroundBlur(self, 0)
	render.SetScissorRect(0, 0, 0, 0, false)
end

function PANEL:SetupJSEnvironment()
	self.HTML:AddFunction("gelly", "closeWelcomeScreen", function()
		self:Remove()
	end)

	self.HTML:AddFunction("gelly", "openURL", function(url)
		gui.OpenURL(url)
	end)
end

vgui.Register("GellyWelcomeScreen", PANEL, "EditablePanel")
