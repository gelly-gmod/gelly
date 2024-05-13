local PANEL = {}
PANEL.BackgroundColor = Color(0, 0, 0, 255)
PANEL.ForegroundColor = Color(255, 255, 255, 255)
PANEL.Padding = 10
PANEL.TitleSpace = 15
PANEL.SidePadding = 300
PANEL.Roundness = 1
PANEL.RowHeight = 20

local GRAPHICS_SETTINGS = {
	Quality = {
		Options = {
			"Low",
			"Medium",
			"High",
		},
	},
}

function PANEL:Init()
	-- width height and position are set for the panel
	self:DockPadding(
		self.SidePadding,
		self.Padding + self.TitleSpace,
		self.SidePadding,
		self.Padding
	)

	self.QualityRow = vgui.Create("DPanel", self)
	self.QualityRow:Dock(TOP)
	self.QualityRow:DockMargin(0, 0, 0, self.RowHeight)
	self.QualityRow:SetTall(self.RowHeight)
	self.QualityRow.Paint = function(_, width, height)
		draw.RoundedBox(0, 0, 0, width, height, self.BackgroundColor)
	end

	self.QualityLabel = vgui.Create("DLabel", self.QualityRow)
	self.QualityLabel:Dock(LEFT)
	self.QualityLabel:SetWide(100)
	self.QualityLabel:SetText("Quality")
	self.QualityLabel:SetTextColor(self.ForegroundColor)
	self.QualityLabel:SetContentAlignment(4)

	self.QualityComboBox = vgui.Create("DComboBox", self.QualityRow)
	self.QualityComboBox:Dock(RIGHT)
	self.QualityComboBox:SetTextColor(self.BackgroundColor)
	self.QualityComboBox:SetValue(GRAPHICS_SETTINGS.Quality.Options[1])
	self.QualityComboBox.OnSelect = function(_, index, value)
		GELLY_SETTINGS_VIEWMODEL.OnGraphicsQualityChange(value)
	end

	for _, option in ipairs(GRAPHICS_SETTINGS.Quality.Options) do
		self.QualityComboBox:AddChoice(option)
	end
end

function PANEL:Paint(width, height)
	draw.RoundedBox(self.Roundness, 0, 0, width, height, self.BackgroundColor)
	draw.SimpleText(
		"Graphics",
		"DermaDefault",
		self.Padding,
		self.TitleSpace,
		self.ForegroundColor,
		TEXT_ALIGN_LEFT,
		TEXT_ALIGN_TOP
	)
end

vgui.Register("GellySettingsGraphics", PANEL, "DPanel")
