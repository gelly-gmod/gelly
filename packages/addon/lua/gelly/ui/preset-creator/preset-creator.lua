---@module 'gelly.api.presets.convert-color-to-absorption'
local convertColorToAbsorption =
	include("gelly/api/presets/convert-color-to-absorption.lua")

local PANEL = {}

function PANEL:Init()
	self.HTML = self:Add("DHTML")
	self.HTML:Dock(FILL)
	self.HTML:OpenURL("asset://garrysmod/lua/html/preset_creator.html.lua")
	self:SetVisible(false)
	self:SetSizable(true)
	self:SetDraggable(true)
	self:SetDeleteOnClose(false)
	self:SetTitle("")
	self.HTML.OnDocumentReady = function()
		self:SetupJSEnvironment()
		self:SetVisible(true)
	end
	self.Settings = {}
end

function PANEL:ToggleVisibility()
	local isVisible = self:IsVisible()
	self:SetVisible(not isVisible)
	isVisible = not isVisible

	self:SetMouseInputEnabled(isVisible)
	self:SetKeyboardInputEnabled(isVisible)
end

function PANEL:SetupJSEnvironment()
	self.HTML:AddFunction("gelly", "setSetting", function(key, value)
		self.Settings[key] = value
		print(key, value)
	end)

	self.HTML:AddFunction("gelly", "testPreset", function()
		gellyx.presets.selectEphemeralPreset(self:GetPreset())
	end)
end

function PANEL:GetColorSettingAsColor()
	local colorHexCode = self.Settings["preset-color"] or "#000000"
	local r = tonumber(colorHexCode:sub(2, 3), 16)
	local g = tonumber(colorHexCode:sub(4, 5), 16)
	local b = tonumber(colorHexCode:sub(6, 7), 16)

	return Color(r, g, b)
end

function PANEL:GetColorSettingAsNormalizedVector()
	local color = self:GetColorSettingAsColor()
	return Vector(color.r / 255, color.g / 255, color.b / 255)
end

function PANEL:GetPreset()
	return {
		Name = self.Settings["preset-name"] or "Unnamed",
		Radius = tonumber(self.Settings["preset-radius"]) or 3,
		DiffuseScale = tobool(self.Settings["preset-foam"]) and 0.25 or 0,
		SolverParams = {
			Viscosity = tonumber(self.Settings["preset-gloopiness"]) or 0,
			SurfaceTension = tonumber(self.Settings["preset-surface-tension"]) or 0,
			Adhesion = tonumber(self.Settings["preset-stickiness"]) or 0,
			Cohesion = tonumber(self.Settings["preset-cohesion"]) or 0,
			VorticityConfinement = 0,
			DynamicFriction = tonumber(self.Settings["preset-friction"]) or 0,
		},
		Material = {
			Roughness = tonumber(self.Settings["preset-roughness"]) or 0,
			IsSpecularTransmission = not tobool(self.Settings["preset-opaque"] or false),
			RefractiveIndex = tonumber(self.Settings["preset-refractive-index"]) or 1.333,
			Absorption = convertColorToAbsorption(self:GetColorSettingAsColor())
				* (tonumber(self.Settings["preset-color-strength"]) or 1),
			DiffuseColor = self:GetColorSettingAsNormalizedVector(),
		},
	}
end

function PANEL:Paint(w, h)
	local x, y = self:LocalToScreen(0, 0)

	render.SetScissorRect(x, y, x + w, y + h, true)
	Derma_DrawBackgroundBlur(self, 0)
	render.SetScissorRect(0, 0, 0, 0, false)
end

vgui.Register("GellyPresetCreator", PANEL, "DFrame")
