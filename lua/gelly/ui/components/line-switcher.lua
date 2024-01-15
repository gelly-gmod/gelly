-- GellyLineSwitcher
-- This component implements a single-line option switcher, which looks like: <  OPTION  >.
-- The option is selected by clicking on the left or right arrow, but is internally implemented by checking which half of the switcher the mouse is over
-- for better usability.

---@module "gelly.ui.util.declare-elements"
local declareElements = include("gelly/ui/util/declare-elements.lua")
local E = declareElements.Elements
local PANEL = {}
PANEL.OptionFont = "ChatFont"
PANEL.LeftArrow = "<"
PANEL.RightArrow = ">"
PANEL.HoveredSide = nil
PANEL.Option = 1
PANEL.OptionList = {}

local HOVERED_SIDE = {
	LEFT = 1,
	RIGHT = 2,
}

local LINE_SWITCHER_DECLARATION = {
	E.Panel({
		Paint = function(_, _, width, height)
			-- Invisible panel to capture mouse events
			return
		end,

		OnCursorMoved = function(_, self, x, y)
			local width = self:GetWide()
			if x > width / 2 then
				self.HoveredSide = HOVERED_SIDE.RIGHT
			else
				self.HoveredSide = HOVERED_SIDE.LEFT
			end
		end,

		OnMousePressed = function(_, self, key)
			if key == MOUSE_LEFT then
				self:OnOptionSelect()
			end
		end,

		Children = {
			LeftArrow = E.Label({
				Font = PANEL.OptionFont,
				Text = PANEL.LeftArrow,
				ContentAlignment = 5,
				Dock = LEFT,
				Paint = function(_, _, width, height)
					draw.RoundedBox(0, 0, 0, width, height, Color(0, 0, 0, 0))
				end,
			}),

			OptionLabel = E.Label({
				Font = PANEL.OptionFont,
				Text = "null",
				ContentAlignment = 5,
				Dock = FILL,
				Paint = function(_, _, width, height)
					draw.RoundedBox(0, 0, 0, width, height, Color(0, 0, 0, 0))
				end,
			}),

			RightArrow = E.Label({
				Font = PANEL.OptionFont,
				Text = PANEL.RightArrow,
				ContentAlignment = 5,
				Dock = RIGHT,
				Paint = function(_, _, width, height)
					draw.RoundedBox(0, 0, 0, width, height, Color(0, 0, 0, 0))
				end,
			}),
		},
	}),
}

function PANEL:Init()
	declareElements.instantiateElements(self, LINE_SWITCHER_DECLARATION)
	self:SetTall(20)
	self:SetMouseInputEnabled(true)
	self:SetCursor("hand")
end

function PANEL:SetOptions(options)
	self.OptionList = options
	self:UpdateOption()
end

function PANEL:UpdateOption()
	self.OptionLabel:SetText(self.OptionList[self.Option])
end

function PANEL:SetOptionIndex(option)
	self.Option = option
	self:UpdateOption()
end

function PANEL:OnOptionSelect()
	if self.HoveredSide == HOVERED_SIDE.LEFT then
		self.Option = math.max(self.Option - 1, 1)
	else
		self.Option = math.min(self.Option + 1, #self.OptionList)
	end

	self:UpdateOption()
	if self.OnOptionChanged then
		self:OnOptionChanged(self.Option)
	end
end

vgui.Register("GellyLineSwitcher", PANEL, "DPanel")
