-- One of the benefits of the mod system is that it'll never be ran on the server, so most conditional realm blocks are unnecessary.
print("Blood mod loaded")

local DAMAGE_TYPE_BLOOD_CONFIGS = {
    {
        DamageFlags = bit.bor(DMG_BULLET, DMG_ALWAYSGIB),
        MinDensity = 200,
		MaxDensity = 400,
		VelocityPower = 10, -- bullets usually are rotating so they can end up flinging blood
		Randomness = 0.8, -- spray in the direction of the normal
		CubeSize = 9,
		DamageMultiplier = 15, -- density is added by the damage * this
    },
    {
        DamageFlags = DMG_BLAST,
        MinDensity = 3200,
		MaxDensity = 5200,
		VelocityPower = 40,
		Randomness = 1,
		CubeSize = 10,
		DamageMultiplier = 90,
    },
	{
        DamageFlags = bit.bor(DMG_SLASH, DMG_CLUB),
        MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 4,
		Randomness = 0.1,
		CubeSize = 3,
    },
	{
        DamageFlags = DMG_CRUSH,
        MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 4,
		Randomness = 1,
		CubeSize = 15,
		DamageMultiplier = 20,
    },
}

local WEAPON_BLOOD_CONFIGS = {
	-- double barrel shotgun in m9k
	m9k_dbarrel = {
		MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 14, -- bullets usually are rotating so they can end up flinging blood
		Randomness = 0.9, -- spray in the direction of the normal
		CubeSize = 52,
		DamageMultiplier = 35, -- density is added by the damage * this
	},

	-- shotgun
	weapon_shotgun = {
		MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 5,
		Randomness = 0.5,
		CubeSize = 15,
		DamageMultiplier = 35,
	},

	weapon_357 = {
		MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 5,
		Randomness = 0.1,
		CubeSize = 5,
		DamageMultiplier = 30,
	},
}

local BLOOD_COLOR_MATERIALS = {
	[BLOOD_COLOR_RED] = {
		Roughness = 0,
		IsSpecularTransmission = true,
		RefractiveIndex = 1.373,
		Absorption = Vector(0.05, 0.5, 0.5),
		DiffuseColor = Vector(0, 0, 0),
	},
	[BLOOD_COLOR_YELLOW] = {
		Roughness = 0,
		IsSpecularTransmission = true,
		RefractiveIndex = 1.373,
		Absorption = Vector(0.25, 0.3, 0.5),
		DiffuseColor = Vector(0, 0, 0),
	},
	[BLOOD_COLOR_GREEN] = {
		Roughness = 0,
		IsSpecularTransmission = true,
		RefractiveIndex = 1.373,
		Absorption = Vector(0.3, 0.25, 0.5),
		DiffuseColor = Vector(0, 0, 0),
	},
	[BLOOD_COLOR_ANTLION] = {
		Roughness = 0,
		IsSpecularTransmission = true,
		RefractiveIndex = 1.373,
		Absorption = Vector(0.45, 0.1, 0.5),
		DiffuseColor = Vector(0, 0, 0),
	},
	[BLOOD_COLOR_ZOMBIE] = {
		Roughness = 0,
		IsSpecularTransmission = true,
		RefractiveIndex = 1.373,
		Absorption = Vector(0.2, 0.35, 0.5),
		DiffuseColor = Vector(0, 0, 0),
	},
	[BLOOD_COLOR_ANTLION_WORKER] = {
		Roughness = 0,
		IsSpecularTransmission = true,
		RefractiveIndex = 1.373,
		Absorption = Vector(0.5, 0.05, 0.5),
		DiffuseColor = Vector(0, 0, 0),
	},
}

local function getConfig(damageType)
	for _, config in ipairs(DAMAGE_TYPE_BLOOD_CONFIGS) do
        if bit.band(damageType, config.DamageFlags) ~= 0 then
            return config
        end
	end
	return nil
end

local function getWeaponConfig(attacker)
	if attacker:IsNPC() or attacker:IsPlayer() then
		return WEAPON_BLOOD_CONFIGS[attacker:GetActiveWeapon():GetClass()]
	end
	return nil
end

local function sprayBlood(damageType, attacker, position, force, damage)
	local normal = attacker == LocalPlayer() and 
					attacker:GetAimVector() or 
					force:GetNormalized()

	local config = getWeaponConfig(attacker) or getConfig(damageType)
	if not config then return end

	local density = math.random(config.MinDensity, config.MaxDensity)
	local velocity = normal * config.VelocityPower

	local bounds = Vector(config.CubeSize, config.CubeSize, config.CubeSize)
	local damageMultiplier = config.DamageMultiplier or 0
	density = density + damageMultiplier * damage

	gellyx.emitters.Sphere({
		center = position,
		velocity = velocity,
		radius = config.CubeSize,
		density = density,
		randomness = config.Randomness,
	})
end

hook.Add(
	"GellyXDamage",
	"gelly.builtin.blood-mod",
	function(victim, attacker, position, force, damage, type)
		if
			not victim:IsValid() or
			(not victim:IsPlayer() and
			not victim:IsNPC() and
			not victim:IsRagdoll())
		then
			return
		end

		local bloodColor = BLOOD_COLOR_RED

		if victim:GetBloodColor() ~= nil and
			victim:GetBloodColor() ~= DONT_BLEED and
			victim:GetBloodColor() ~= BLOOD_COLOR_MECH
		then
			bloodColor = victim:GetBloodColor()
		end
		
		local oldMaterial = gellyx.presets.getActivePreset().Material
		local material = BLOOD_COLOR_MATERIALS[bloodColor]

		gelly.SetFluidMaterial(material)

		sprayBlood(type, attacker, position, force, damage)

		gelly.SetFluidMaterial(oldMaterial)
	end
)

gellyx.presets.select("Blood")
