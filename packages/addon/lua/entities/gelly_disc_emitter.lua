local DISC_MODEL = "models/props_phx/trains/wheel_medium.mdl"
AddCSLuaFile()

ENT.Base = "base_anim"
ENT.Type = "anim"
ENT.Category = "Gelly"
ENT.PrintName = "Gelly Disc Emitter"
ENT.Spawnable = true
ENT.AdminOnly = false
ENT.Editable = true

--- Setups the given list of network vars
---@param vars {name: string, type: string, min: number?, max: number?}[]
function ENT:SetupEditableNetworkVars(vars)
	local perTypeCounters = {}
	for index, var in ipairs(vars) do
		perTypeCounters[var.type] = perTypeCounters[var.type] or 0
		local counter = perTypeCounters[var.type]
		self:NetworkVar(var.type, counter, var.name,
			{ KeyName = var.name:lower(), Edit = { type = var.type, order = index + 1, min = var.min, max = var.max } })
		perTypeCounters[var.type] = counter + 1
	end
end

function ENT:SetupDataTables()
	self:SetupEditableNetworkVars({
		{ name = "Density", type = "Float", min = 50, max = 1000 },
		{ name = "Speed",   type = "Float", min = 1,  max = 255 },
		{ name = "Enabled", type = "Bool" }
	})
end

function ENT:InitializeDefaultSettings()
	self:SetDensity(355)
	self:SetSpeed(55)
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
	end
end

function ENT:Think()
	if SERVER then
		return
	end

	if self:GetEnabled() then
		local transform = self:GetWorldTransformMatrix()
		gellyx.emitters.Cube({
			center = Vector(0, 0, 50),
			velocity = Vector(0, 0, self:GetSpeed()),
			bounds = Vector(2.3 * gellyx.presets.getEffectiveRadius(), 1.6 * gellyx.presets.getEffectiveRadius(), 80),
			density = self:GetDensity(),
			transform = transform,
		})
	end

	self:SetNextClientThink(CurTime() + 0.01)
	return true
end
