local setupEditableVars = include("gelly/util/setup-editable-vars.lua")
local DISC_MODEL = "models/props_phx/trains/wheel_medium.mdl"
AddCSLuaFile()

ENT.Base = "base_anim"
ENT.Type = "anim"
ENT.Category = "Gelly"
ENT.PrintName = "Gelly Disc Emitter"
ENT.Spawnable = true
ENT.AdminOnly = false
ENT.Editable = true

function ENT:SetupDataTables()
	setupEditableVars(self, {
		{ name = "Density",       type = "Float",  min = 50,                max = 1000 },
		{ name = "Speed",         type = "Float",  min = 1,                 max = 255 },
		{ name = "ColorOverride", type = "Vector", editType = "VectorColor" },
		{ name = "ColorScale",    type = "Float",  min = 1,                 max = 4 },
		{ name = "Enabled",       type = "Bool" }
	})
end

function ENT:InitializeDefaultSettings()
	self:SetDensity(200)
	self:SetSpeed(50)
	self:SetEnabled(true)
end

function ENT:Initialize()
	if SERVER then
		self:SetModel(DISC_MODEL)
		self:PhysicsInit(SOLID_VPHYSICS)
		self:SetMoveType(MOVETYPE_VPHYSICS)
		self:SetSolid(SOLID_VPHYSICS)

		self:DrawShadow(false)
		self:SetUseType(SIMPLE_USE)

		local phys = self:GetPhysicsObject()

		if phys:IsValid() then
			phys:Wake()
		end

		self:InitializeDefaultSettings()
		return
	end

	self.PresetMaterial = gellyx.presets.copyPresetMaterial(gellyx.presets.getActivePreset().Name)
end

function ENT:GetAbsorptionOverride()
	local absorptionR = 1 - self:GetColorOverride().x
	local absorptionG = 1 - self:GetColorOverride().y
	local absorptionB = 1 - self:GetColorOverride().z

	return Vector(absorptionR, absorptionG, absorptionB) * self:GetColorScale()
end

function ENT:IsColorOverrided()
	local override = self:GetColorOverride()
	return override.x ~= 0 or override.y ~= 0 or override.z ~= 0
end

function ENT:Think()
	if SERVER then
		return
	end

	if self:IsColorOverrided() then
		self.PresetMaterial.Absorption = self:GetAbsorptionOverride()
	end

	if self:GetEnabled() then
		local transform = self:GetWorldTransformMatrix()

		gellyx.emitters.Cube({
			center = Vector(0, 0, 3.0 * gellyx.presets.getEffectiveRadius()),
			velocity = Vector(0, 0, self:GetSpeed()),
			bounds = Vector(2.0 * gellyx.presets.getEffectiveRadius(), 2.0 * gellyx.presets.getEffectiveRadius(), 3.0 * gellyx.presets.getEffectiveRadius()),
			density = self:GetDensity(),
			transform = transform,
			material = self:IsColorOverrided() and self.PresetMaterial or nil
		})
	end

	self:SetNextClientThink(CurTime() + 0.01)
	return true
end

function ENT:Use()
	self:SetEnabled(not self:GetEnabled())
end
