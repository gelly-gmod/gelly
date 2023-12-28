SWEP.Category = "Gelly"
SWEP.Spawnable = true
SWEP.AdminOnly = false
SWEP.PrintName = "Gelly Hose"

SWEP.ViewModel = "models/weapons/v_pistol.mdl"
SWEP.WorldModel = "models/weapons/w_pistol.mdl"
SWEP.ViewModelFOV = 54
SWEP.UseHands = true

SWEP.Primary.ClipSize = -1
SWEP.Primary.DefaultClip = -1
SWEP.Primary.Automatic = true
SWEP.Primary.Ammo = ""

SWEP.Secondary.ClipSize = -1
SWEP.Secondary.DefaultClip = -1
SWEP.Secondary.Automatic = true
SWEP.Secondary.Ammo = ""

SWEP.ParticleDensity = 300
SWEP.FireRate = 40 -- bursts per second

---@module "gelly.emitters.cube-emitter"
local emitCube = include("gelly/emitters/cube-emitter.lua")

function SWEP:Initialize()
	self:SetHoldType("pistol")
end

function SWEP:PrimaryAttack()
	print("Primary attack")
	if SERVER then
		self:CallOnClient("PrimaryAttack")
		return
	end

	local owner = self:GetOwner()
	local position = owner:GetShootPos()
	local velocity = owner:GetAimVector() * 50
	position = position + owner:GetAimVector() * 50
	local size = Vector(1, 1, 1) * 50
	local density = 100

	emitCube(position, velocity, size, density)

	self:SetNextPrimaryFire(CurTime() + 1 / self.FireRate)
end
