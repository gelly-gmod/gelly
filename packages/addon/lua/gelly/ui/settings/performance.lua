local PANEL = {}
PANEL.BackgroundColor = Color(0, 0, 0, 255)
PANEL.ForegroundColor = Color(255, 255, 255, 255)
PANEL.Padding = 10
PANEL.TitleSpace = 15
PANEL.SidePadding = 300
PANEL.Roundness = 1
PANEL.RowHeight = 40

function PANEL:Init()
	-- width height and position are set for the panel
	self:DockPadding(
		self.SidePadding,
		self.Padding + self.TitleSpace,
		self.SidePadding,
		self.Padding
	)

	self.SimFPSRow = vgui.Create("DPanel", self)
	self.SimFPSRow:Dock(TOP)
	self.SimFPSRow:DockMargin(0, 0, 0, self.RowHeight)
	self.SimFPSRow:SetTall(self.RowHeight)
	self.SimFPSRow.Paint = function(_, width, height)
		draw.RoundedBox(0, 0, 0, width, height, self.BackgroundColor)
	end

	self.SimFPSLabelColumn = vgui.Create("DPanel", self.SimFPSRow)
	self.SimFPSLabelColumn:Dock(LEFT)
	self.SimFPSLabelColumn:SetWide(400)
	self.SimFPSLabelColumn.Paint = function(_, width, height)
		draw.RoundedBox(0, 0, 0, width, height, self.BackgroundColor)
	end

	self.SimFPSLabel = vgui.Create("DLabel", self.SimFPSLabelColumn)
	self.SimFPSLabel:Dock(FILL)
	self.SimFPSLabel:SetText("Simulation FPS")
	self.SimFPSLabel:SetTextColor(self.ForegroundColor)
	self.SimFPSLabel:SetContentAlignment(4)
	self.SimFPSLabel:DockMargin(0, 0, 0, 10)

	self.SimFPSDescription = vgui.Create("DLabel", self.SimFPSLabelColumn)
	self.SimFPSDescription:SetFont("DefaultSmall")
	self.SimFPSDescription:Dock(BOTTOM)
	self.SimFPSDescription:SetText(
		"The initial FPS that the simulation begins with, going down to 30 FPS."
	)
	self.SimFPSDescription:SetTextColor(self.ForegroundColor)
	self.SimFPSDescription:SetContentAlignment(4)

	self.SimFPSNumberWang = vgui.Create("DNumberWang", self.SimFPSRow)
	self.SimFPSNumberWang:Dock(RIGHT)
	self.SimFPSNumberWang:SetTextColor(self.BackgroundColor)
	self.SimFPSNumberWang:SetValue(60)
	self.SimFPSNumberWang:SetMin(30)
	self.SimFPSNumberWang:SetMax(120)
	self.SimFPSNumberWang.OnValueChanged = function(_, value)
		if value < 30 then
			self.SimFPSNumberWang:SetValue(30)
			return
		end

		GELLY_SETTINGS_VIEWMODEL.OnSimFPSChange(value)
	end
end

function PANEL:Paint(width, height)
	draw.RoundedBox(self.Roundness, 0, 0, width, height, self.BackgroundColor)
	draw.SimpleText(
		"Performance",
		"DermaDefault",
		self.Padding,
		self.TitleSpace,
		self.ForegroundColor,
		TEXT_ALIGN_LEFT,
		TEXT_ALIGN_TOP
	)
end

vgui.Register("GellySettingsPerformance", PANEL, "DPanel")
