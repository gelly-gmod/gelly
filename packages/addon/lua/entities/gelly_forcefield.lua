local setupEditableVars = include("gelly/util/setup-editable-vars.lua")

-- gellyx_forcefield is the wrapper entity which exposes a game-oriented interface to forcefields,
-- this is the player-facing entity that is used to create forcefields in the game.
AddCSLuaFile()

local BALL_MODEL = "models/hunter/misc/sphere025x025.mdl"
local MATERIAL = "phoenix_storms/wire/pcb_green"

ENT.Base = "base_anim"
ENT.Type = "anim"
ENT.Category = "Gelly"
ENT.PrintName = "Gelly Forcefield"
ENT.Spawnable = true
ENT.AdminOnly = false
ENT.Editable = true

function ENT:SetupDataTables()
	setupEditableVars(self, {
		{ name = "Radius",   type = "Float", min = 1,     max = 1000 },
		{ name = "Strength", type = "Float", min = -1000, max = 1000 },
		{ name = "Visible",  type = "Bool" },
	})
end

function ENT:InitializeDefaultSettings()
	self:SetRadius(100)
	self:SetStrength(-100)
	self:SetVisible(true)
end

function ENT:UpdateTransmitState()
	return TRANSMIT_ALWAYS
end

function ENT:CreateForcefield()
	assert(CLIENT, "CreateForcefield should only be called on the client")

	self.LastRadius = self:GetRadius()
	self.LastStrength = self:GetStrength()
	return gellyx.forcefield.create({
		Position = self:GetPos(),
		Radius = self:GetRadius(),
		Strength = self:GetStrength(),
		LinearFalloff = false,
		Mode = gellyx.forcefield.Mode.Force,
	})
end

function ENT:Initialize()
	if SERVER then
		self:SetModel(BALL_MODEL)
		self:PhysicsInit(SOLID_VPHYSICS)
		self:SetMoveType(MOVETYPE_VPHYSICS)
		self:SetSolid(SOLID_VPHYSICS)

		self:DrawShadow(false)
		self:SetUseType(SIMPLE_USE)
		self:SetMaterial(MATERIAL)

		local phys = self:GetPhysicsObject()

		if phys:IsValid() then
			phys:Wake()
		end

		self:InitializeDefaultSettings()
		return
	end

	self.Forcefield = self:CreateForcefield()
end

function ENT:OnRemove()
	if CLIENT then
		if IsValid(self.Forcefield) then
			self.Forcefield:Remove()
		end
	end
end

function ENT:Think()
	if SERVER then
		return
	end

	if not IsValid(self.Forcefield) or self.LastRadius ~= self:GetRadius() or self.LastStrength ~= self:GetStrength() then
		if self.Forcefield then
			self.Forcefield:Remove()
		end

		self.Forcefield = self:CreateForcefield()
	end

	self:SetNoDraw(not self:GetVisible())
	self.Forcefield:SetPos(self:GetPos())
	self:SetNextClientThink(CurTime())
	return true
end
