local DISC_MODEL = "models/props_phx/trains/wheel_medium.mdl"
AddCSLuaFile()

ENT.Base = "base_anim"
ENT.Type = "anim"
ENT.Category = "Gelly"
ENT.PrintName = "Gelly Disc Emitter"
ENT.Spawnable = true
ENT.AdminOnly = false

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
	end
end

function ENT:Think()
	if SERVER then
		return
	end

	local transform = self:GetWorldTransformMatrix()
	gellyx.emitters.Cube({
		center = Vector(0, 0, 50),
		velocity = Vector(0, 0, 55),
		bounds = Vector(7, 5, 80),
		density = 355,
		transform = transform,
	})

	self:SetNextClientThink(CurTime() + 0.01)
	return true
end
