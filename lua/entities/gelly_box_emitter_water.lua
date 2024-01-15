-- Really just a normal box but with a gelly emitter on it

local BOX_MODEL = "models/hunter/blocks/cube075x075x075.mdl"

AddCSLuaFile()

ENT.Base = "base_anim"
ENT.Type = "anim"
ENT.Category = "Gelly"
ENT.PrintName = "Gelly Box Emitter (Water)"
ENT.Spawnable = true
ENT.AdminOnly = false
ENT.Emitting = false
ENT.Preset = "Water"

function ENT:Initialize()
	self.CubeSize = self:OBBMaxs() - self:OBBMins()
	if SERVER then
		self:SetModel(BOX_MODEL)
		self:PhysicsInit(SOLID_VPHYSICS)
		self:SetMoveType(MOVETYPE_VPHYSICS)
		self:SetSolid(SOLID_VPHYSICS)

		self:DrawShadow(false)
		self:SetUseType(SIMPLE_USE)

		local phys = self:GetPhysicsObject()

		if phys:IsValid() then
			phys:Wake()
		end
	end
end

function ENT:Use()
	-- notify clients that we're emitting
	self.Emitting = not self.Emitting
	self:EmitSound("buttons/button1.wav")

	if SERVER then
		self:SetNWBool("Emitting", self.Emitting)
	end
end

function ENT:Think()
	if SERVER then
		return
	end

	local isEmitting = self:GetNWBool("Emitting")
	if isEmitting then
		gellyx.presets.select(self.Preset)
		gellyx.emitters.Cube({
			center = self:GetPos(),
			velocity = self:GetVelocity(),
			bounds = self.CubeSize,
			density = 25,
		})
	end

	self:SetNextClientThink(CurTime() + 0.01)
	return true
end
