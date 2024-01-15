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
SWEP.RapidFireBoost = 2 -- how much proportional quantity of particles to emit when rapid firing

function SWEP:Initialize()
	self:SetHoldType("pistol")
end

function SWEP:PrimaryAttack()
	print("Primary attack")
	if SERVER then
		self:CallOnClient("PrimaryAttack")
		return
	end

	---@type Player
	local owner = self:GetOwner()

	gellyx.emitters.Cube({
		center = owner:GetShootPos() + owner:GetAimVector() * 50,
		velocity = owner:GetAimVector() * 2,
		bounds = Vector(50, 50, 50),
		density = self.ParticleDensity,
	})

	self:SetNextPrimaryFire(CurTime() + 1 / self.FireRate)
end

function SWEP:SecondaryAttack()
	print("Secondary attack")
	if SERVER then
		self:CallOnClient("SecondaryAttack")
		return
	end

	local owner = self:GetOwner()

	gellyx.emitters.Cube({
		center = owner:GetShootPos() + owner:GetAimVector() * 50,
		velocity = owner:GetAimVector() * 50,
		bounds = Vector(50, 50, 50),
		density = self.ParticleDensity * self.RapidFireBoost,
	})

	self:SetNextSecondaryFire(CurTime() + 1 / self.FireRate * self.RapidFireBoost)
end
