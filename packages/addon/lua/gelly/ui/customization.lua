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

function PANEL:SetupJSEnvironment()
	self.HTML:AddFunction("gelly", "setSimulationRate", function(rate)
		local numericRate = tonumber(rate)
		if not numericRate then
			return
		end

		GELLY_SIM_RATE_HZ = numericRate
	end)

	self.HTML:AddFunction("gelly", "setSmoothness", function(smoothness)
		local numericSmoothness = tonumber(smoothness)
		if not numericSmoothness then
			return
		end

		local gellySettings = gelly.GetGellySettings()
		gellySettings.FilterIterations = numericSmoothness
		gelly.SetGellySettings(gellySettings)
	end)

	self.HTML:AddFunction("gelly", "setRadiusScale", function(radiusScale)
		local numericRadiusScale = tonumber(radiusScale)
		if not numericRadiusScale then
			return
		end

		gellyx.presets.scaleConVar:SetFloat(numericRadiusScale)
	end)

	self.HTML:AddFunction("gelly", "hide", function()
		self:Hide()
	end)
end

vgui.Register("GellyCustomizationMenu", PANEL, "EditablePanel")
