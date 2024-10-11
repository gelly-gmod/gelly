SWEP.Category = "Gelly"
SWEP.Spawnable = true
SWEP.AdminOnly = false
SWEP.PrintName = "Gelly Liquifier"

SWEP.ViewModel = "models/weapons/c_pistol.mdl"
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

SWEP.TriangleDensity = 5
SWEP.FireRate = 1 -- projectiles per second

local POP_SOUND = Sound("garrysmod/balloon_pop_cute.wav")
local function getRandomTadaSound()
	return "garrysmod/save_load" .. math.random(1, 4) .. ".wav"
end

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
		self:GetOwner():ViewPunch(AngleRand(-10, 10))
		self:CallOnClient("PrimaryAttack")
		SafeRemoveEntityDelayed(hitEntity, 0) -- a tick later
		return
	end

	gellyx.emitters.Mesh({
		entity = hitEntity,
		density = self.TriangleDensity,
	})

	CreateParticleSystemNoEntity("Liquifier_ChargeBlast", hitPos)
	self:SetNextPrimaryFire(CurTime() + 1 / self.FireRate)

	local effectData = EffectData()
	effectData:SetOrigin(hitPos)
	util.Effect("gelly_liquify", effectData)
	util.ScreenShake(hitPos, 5, 45, 1, 1000)
	self:EmitSounds()
end

function SWEP:EmitSounds()
	local pitch = math.Rand(100, 180)
	sound.Play(POP_SOUND, self:GetOwner():GetShootPos(), 125, pitch, 1)
	surface.PlaySound(getRandomTadaSound())
end

function SWEP:PreDrawViewModel(vm, weapon, ply)
	self.ChargeEffect = self.ChargeEffect
		or CreateParticleSystem(vm, "Liquifier_ChargePrepare", PATTACH_POINT_FOLLOW, 1)
end
