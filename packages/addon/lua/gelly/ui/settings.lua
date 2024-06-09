include("settings/graphics.lua")
include("settings/performance.lua")
include("settings/viewmodel.lua")

local PANEL = {}
PANEL.Size = { 0.8, 0.4 }
PANEL.Padding = 5
PANEL.Roundness = 5
PANEL.BackgroundColor = Color(0, 0, 0, 200)
PANEL.ForegroundColor = Color(255, 255, 255)
PANEL.Tabs = {
	{
		Name = "Graphics",
		Panel = "GellySettingsGraphics",
	},
	{
		Name = "Performance",
		Panel = "GellySettingsPerformance",
	},
}

PANEL.DefaultTab = 1

function PANEL:Init()
	self:SetSize(ScrW() * self.Size[1], ScrH() * self.Size[2])
	self:Center()
	self:MakePopup()
	self:SetKeyboardInputEnabled(true)
	self:SetMouseInputEnabled(true)

	self.ExitButton = vgui.Create("DButton", self)
	self.ExitButton:SetSize(20, 20)
	self.ExitButton:SetPos(self:GetWide() - 20, 0)
	self.ExitButton:SetText("X")
	self.ExitButton:SetTextColor(self.ForegroundColor)
	self.ExitButton.Paint = function(_, width, height)
		draw.RoundedBox(0, 0, 0, width, height, Color(255, 0, 0))
	end

	self.ExitButton.DoClick = function()
		self:Remove()
	end

	self:DockPadding(self.Padding, self.Padding + 20, self.Padding, self.Padding)

	self.PropertySheet = vgui.Create("DPropertySheet", self)
	self.PropertySheet:Dock(FILL)

	for _, tab in ipairs(self.Tabs) do
		local panel = vgui.Create(tab.Panel, self.PropertySheet)
		self.PropertySheet:AddSheet(tab.Name, panel)
	end
end

function PANEL:Paint(width, height)
	draw.RoundedBox(self.Roundness, 0, 0, width, height, self.BackgroundColor)
	draw.SimpleText(
		"Gelly Settings",
		"ChatFont",
		self.Padding,
		self.Padding,
		self.ForegroundColor,
		TEXT_ALIGN_LEFT,
		TEXT_ALIGN_TOP
	)
end

vgui.Register("GellySettings", PANEL, "EditablePanel")
