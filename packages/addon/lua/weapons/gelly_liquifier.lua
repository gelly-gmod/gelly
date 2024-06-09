SWEP.Category = "Gelly"
SWEP.Spawnable = true
SWEP.AdminOnly = false
SWEP.PrintName = "Gelly Liquifier"

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

SWEP.TriangleDensity = 25
SWEP.FireRate = 1 -- projectiles per second

function SWEP:Initialize()
	self:SetHoldType("pistol")
end

function SWEP:PrimaryAttack()
	local eyeTrace = self:GetOwner():GetEyeTrace()
	local hitEntity = eyeTrace.Entity
	local hitPos = eyeTrace.HitPos
	if not IsValid(hitEntity) then
		return
	end

	if SERVER then
		self:CallOnClient("PrimaryAttack")
		print("CallOnClient")
		SafeRemoveEntityDelayed(hitEntity, 0) -- a tick later
		return
	end

	print("Primary attack")
	gellyx.emitters.Mesh({
		entity = hitEntity,
		density = self.TriangleDensity,
	})
	CreateParticleSystemNoEntity("Liquifier_ChargeBlast", hitPos)
	self:SetNextPrimaryFire(CurTime() + 1 / self.FireRate)
end

function SWEP:PreDrawViewModel(vm, weapon, ply)
	self.ChargeEffect = self.ChargeEffect
		or CreateParticleSystem(vm, "Liquifier_ChargePrepare", PATTACH_POINT_FOLLOW, 1)
end
