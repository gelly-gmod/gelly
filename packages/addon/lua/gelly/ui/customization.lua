local PANEL = {}

function PANEL:Init()
	self.HTML = self:Add("DHTML")
	self.HTML:Dock(FILL)
	self.HTML:OpenURL(self:GetUILocation())
	self:SetVisible(false)
	self:SetSize(ScrW() * 0.8, ScrH() * 0.6)
	self:Center()
	self:SetupJSEnvironment()
end

function PANEL:GetUILocation()
	return "http://127-0-0-1.nip.io/ui/customization.html"
end

function PANEL:Paint(w, h)
	local x, y = self:LocalToScreen(0, 0)

	render.SetScissorRect(x, y, x + w, y + h, true)
	Derma_DrawBackgroundBlur(self, 0);
	render.SetScissorRect(0, 0, 0, 0, false)
end

function PANEL:Hide()
	self:SetVisible(false)
	self:SetKeyboardInputEnabled(false)
	self:SetMouseInputEnabled(false)
end

function PANEL:Show()
	self:SetVisible(true)
	self:SetKeyboardInputEnabled(true)
	self:SetMouseInputEnabled(true)
	self:MakePopup()
end

function PANEL:ToggleVisibility()
	if self:IsVisible() then
		self:Hide()
	else
		self:Show()
	end
end

function PANEL:Think()
	local minX, minY = self:LocalToScreen(0, 0)
	local maxX, maxY = self:LocalToScreen(self:GetWide(), self:GetTall())

	local isMouseOutside = gui.MouseX() < minX or gui.MouseY() < minY or gui.MouseX() > maxX or gui.MouseY() > maxY
	-- Amazingly, we can capture the mouse outside the panel without screwing up the HTML focus by enabling/disabling
	-- a capture panel each frame. It's kind of a hack and not at the same time.

	self:MouseCapture(isMouseOutside)
end

function PANEL:OnMousePressed()
	self:Hide()
end

function PANEL:SetupJSEnvironment()
	self.HTML:AddFunction("gelly", "getSettingAsFloat", function(key)
		return gellyx.settings.get(key):GetFloat()
	end)

	self.HTML:AddFunction("gelly", "setSettingAsFloat", function(key, value)
		gellyx.settings.get(key):SetFloat(value)
	end)

	self.HTML:AddFunction("gelly", "hide", function()
		self:Hide()
	end)
end

function PANEL:ForceSettingUpdate()
	self.HTML:RunJavascript("gellySync.notifyListeners()")
end

vgui.Register("GellyCustomizationMenu", PANEL, "EditablePanel")
