AddCSLuaFile()

ENT.Base = "base_anim"
ENT.Type = "anim"
ENT.Category = "GellyX"
ENT.PrintName = "GellyX Forcefield"
ENT.Spawnable = false
ENT.AdminOnly = false

AccessorFunc(ENT, "Radius", "Radius", FORCE_NUMBER)
AccessorFunc(ENT, "Strength", "Strength", FORCE_NUMBER)
AccessorFunc(ENT, "LinearFalloff", "LinearFalloff", FORCE_BOOL)
AccessorFunc(ENT, "Mode", "Mode", FORCE_NUMBER) -- type is actually gellyx.forcefield.Mode

function ENT:Initialize()
	if CLIENT then
		self.ForcefieldHandle = gelly.AddForcefieldObject({
			Position = Vector(),
			Radius = self.Radius,
			Strength = self.Strength,
			LinearFalloff = self.LinearFalloff,
			Mode = self.Mode,
		})

		self:SetNextClientThink(CurTime())
	end
end

function ENT:Think()
	if SERVER then
		return
	end

	if self.ForcefieldHandle then
		gelly.UpdateForcefieldPosition(self.ForcefieldHandle, self:GetPos())
	end

	self:SetNextClientThink(CurTime() + 0.01)
	return true
end

function ENT:OnRemove()
	if CLIENT then
		if self.ForcefieldHandle then
			gelly.RemoveForcefieldObject(self.ForcefieldHandle)
		end
	end
end
