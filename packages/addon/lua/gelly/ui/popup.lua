---@module "util.declare-elements"
local declareElements = include("util/declare-elements.lua")
local E = declareElements.Elements

local PANEL = {}
PANEL.TopBarColor = Color(100, 100, 100)
PANEL.TopBarFont = "ChatFont"
PANEL.ContentBGColor = Color(50, 50, 50)
PANEL.ContentFont = "ChatFont"
PANEL.ButtonOKString = "OK"
PANEL.ButtonBGColor = Color(100, 100, 100)
PANEL.Roundness = 4 -- px of roundness
PANEL.WidthProportion = 0.5 -- % of the screen width
PANEL.HeightProportion = 0.2 -- % of the screen height
PANEL.Padding = 10 -- px of padding

local POPUP_DECLARATION = {
	E.Frame({
		Size = { ScrW() * PANEL.WidthProportion, ScrH() * PANEL.HeightProportion },
		Center = {},
		DockPadding = { PANEL.Padding, PANEL.Padding, PANEL.Padding, PANEL.Padding },
		Paint = function(_, _, width, height)
			draw.RoundedBox(PANEL.Roundness, 0, 0, width, height, PANEL.ContentBGColor)
		end,

		Children = {
			TopBar = E.Label({
				Font = PANEL.TopBarFont,
				Color = color_white,
				Text = "null",
				ContentAlignment = 5,
				Paint = function(_, _, width, height)
					draw.RoundedBox(
						PANEL.Roundness,
						0,
						0,
						width,
						height,
						PANEL.TopBarColor
					)
				end,
				Dock = TOP,
			}),

			Content = E.Label({
				Font = PANEL.ContentFont,
				Color = color_white,
				Text = "null",
				ContentAlignment = 7,
				Wrap = true,
				Paint = function(_, _, width, height)
					draw.RoundedBox(
						PANEL.Roundness,
						0,
						0,
						width,
						height,
						PANEL.ContentBGColor
					)
				end,
				Dock = FILL,
				DockMargin = { 0, 5, 0, 5 },
				DockPadding = { 5, 5, 5, 5 },
			}),

			ButtonOK = E.Button({
				Text = PANEL.ButtonOKString,
				DoClick = function(root)
					root:Remove()
				end,
				Paint = function(_, _, width, height)
					draw.RoundedBox(
						PANEL.Roundness,
						0,
						0,
						width,
						height,
						PANEL.ButtonBGColor
					)
				end,
				TextColor = color_white,
				Dock = BOTTOM,
			}),
		},
	}),
}

function PANEL:Init()
	declareElements.instantiateElements(self, POPUP_DECLARATION)
	self:InvalidateLayout()
end

function PANEL:SetTopBarText(text)
	self.TopBar:SetText(text)
end

function PANEL:SetContentText(text)
	self.Content:SetText(text)
end

vgui.Register("GellyPopup", PANEL, "DPanel")
