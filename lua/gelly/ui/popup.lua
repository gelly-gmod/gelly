local PANEL = {}
PANEL.TopBarColor = Color(100, 100, 100)
PANEL.TopBarFont = "ChatFont"
PANEL.ContentBGColor = Color(50, 50, 50)
PANEL.ContentFont = "ChatFont"
PANEL.ButtonOKString = "OK"
PANEL.ButtonBGColor = Color(100, 100, 100)
PANEL.Roundness = 4 -- px of roundness
PANEL.WidthProportion = 0.4 -- % of the screen width
PANEL.HeightProportion = 0.2 -- % of the screen height
PANEL.Padding = 10 -- px of padding

function PANEL:Paint(width, height)
	draw.RoundedBox(self.Roundness, 0, 0, width, height, PANEL.ContentBGColor)
end

function PANEL:Init()
	self:DockPadding(self.Padding, self.Padding, self.Padding, self.Padding)

	self.TopBar = self:Add("DLabel")
	self.TopBar:SetFont(self.TopBarFont)
	self.TopBar:SetColor(Color(255, 255, 255))
	self.TopBar:SetText("null")
	self.TopBar:SetContentAlignment(5)
	self.TopBar.Paint = function(panel, width, height)
		draw.RoundedBox(PANEL.Roundness, 0, 0, width, height, self.TopBarColor)
	end

	self.Content = self:Add("DLabel")
	self.Content:SetFont(self.ContentFont)
	self.Content:SetColor(Color(255, 255, 255))
	self.Content:SetText("null")
	self.Content:SetContentAlignment(7)
	self.Content:SetWrap(true)
	self.Content.Paint = function(panel, width, height)
		draw.RoundedBox(PANEL.Roundness, 0, 0, width, height, self.ContentBGColor)
	end

	self.Content:DockPadding(5, 5, 5, 5)

	self.ButtonOK = self:Add("DButton")
	self.ButtonOK:SetFont("DermaDefault")
	self.ButtonOK:SetText(self.ButtonOKString)
	self.ButtonOK.DoClick = function()
		self:Remove()
	end

	self.ButtonOK.Paint = function(panel, width, height)
		draw.RoundedBox(PANEL.Roundness, 0, 0, width, height, self.ButtonBGColor)
	end

	self.TopBar:Dock(TOP)
	self.Content:Dock(FILL)
	self.Content:DockMargin(0, 5, 0, 5)
	self.ButtonOK:Dock(BOTTOM)
	self.ButtonOK:SetColor(color_white)

	self:SetSize(ScrW() * self.WidthProportion, ScrH() * self.HeightProportion)
	self:Center()
end

function PANEL:SetTopBarText(text)
	self.TopBar:SetText(text)
end

function PANEL:SetContentText(text)
	self.Content:SetText(text)
end

vgui.Register("GellyPopup", PANEL, "DPanel")
