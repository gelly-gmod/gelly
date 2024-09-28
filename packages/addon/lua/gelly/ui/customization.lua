local logging = include("gelly/logging.lua")
local gelly_ui_point_to_localhost = CreateClientConVar("gelly_ui_point_to_localhost", "0", true, false,
	"Point the Gelly UI to localhost for development purposes.")

local PANEL = {}

function PANEL:Init()
	local localhostUI = gelly_ui_point_to_localhost:GetBool()
	if localhostUI then
		logging.warn("Gelly UI is pointing to localhost.")
	end

	self.HTML = self:Add("DHTML")
	self.HTML:Dock(FILL)
	self.HTML:OpenURL(localhostUI and "http://127-0-0-1.nip.io/customization.html" or
		"asset://garrysmod/lua/html/gelly-ui/customization.html.lua")
	self:SetVisible(false)
	self:SetSize(ScrW() * 0.8, ScrH() * 0.6)
	self:Center()
	self:SetupJSEnvironment()
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

	-- The pros outweigh the cons for this hack, as this will ensure transluceny
	-- is removed when the panel is shown. Usually, transluceny can get stuck if
	-- some awkward condition arises, such as the user lets go suddenly outside of our input sink or
	-- a browser-provided input control suddenly loses focus.
	self:ForceRemoveTranslucency()
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

	self:MouseCapture(isMouseOutside and self:IsMousePositionValidData())
end

function PANEL:IsMousePositionValidData()
	-- Really weird bug: the mouse position is sometimes (0, 0) at random frames
	-- once the user interacts with a system-provided control modal (color picker, most of the time, but sliders can also trigger this)
	-- It then lasts a good 10-20 seconds and then goes back to normal. This is a workaround for that.

	return gui.MouseX() ~= 0 and gui.MouseY() ~= 0
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

	self.HTML:AddFunction("gelly", "getSettingAsString", function(key)
		return gellyx.settings.get(key):GetString()
	end)

	self.HTML:AddFunction("gelly", "setSettingAsString", function(key, value)
		gellyx.settings.get(key):SetString(value)
	end)

	self.HTML:AddFunction("gelly", "getActiveParticles", function()
		return gelly.GetStatus().ActiveParticles
	end)

	self.HTML:AddFunction("gelly", "hide", function()
		self:Hide()
	end)
end

function PANEL:ForceSettingUpdate()
	self.HTML:RunJavascript("gellySync.notifyListeners()")
end

function PANEL:ForceRemoveTranslucency()
	self.HTML:RunJavascript("document.body.classList.remove('translucent')")
end

vgui.Register("GellyCustomizationMenu", PANEL, "EditablePanel")