-- One of the benefits of the mod system is that it'll never be ran on the server, so most conditional realm blocks are unnecessary.
print("Blood mod loaded")

local DAMAGE_TYPE_BLOOD_CONFIGS = {
	{
		DamageFlags = bit.bor(DMG_BULLET, DMG_ALWAYSGIB),
		MinDensity = 200,
		MaxDensity = 400,
		VelocityPower = -7, -- bullets usually are rotating so they can end up flinging blood
		Randomness = 0.8, -- spray in the direction of the normal
		CubeSize = 9,
		DamageMultiplier = 15, -- density is added by the damage * this
	},
	{
		DamageFlags = DMG_BLAST,
		MinDensity = 3200,
		MaxDensity = 5200,
		VelocityPower = 15,
		Randomness = 0.9,
		CubeSize = 10,
		DamageMultiplier = 90,
		FromEntity = true,
	},
	{
		DamageFlags = bit.bor(DMG_SLASH, DMG_CLUB),
		MinDensity = 200,
		MaxDensity = 2200,
		VelocityPower = -15,
		Randomness = 0.95,
		CubeSize = 10,
		DamageMultiplier = 5,
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
		VelocityPower = -14, -- bullets usually are rotating so they can end up flinging blood 
		Randomness = 0.9, -- spray in the direction of the normal
		CubeSize = 52,
		DamageMultiplier = 35, -- density is added by the damage * this
	},

	-- shotgun
	weapon_shotgun = {
		MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = -5, -- blood should be launched at the player from the wound not from the wound at the victim
		Randomness = 0.5,
		CubeSize = 15,
		DamageMultiplier = 35,
	},

	weapon_357 = {
		MinDensity = 100,
		MaxDensity = 200,
		VelocityPower = 5, -- a .357 bullet should maybe penetrate through the entire victim
		Randomness = 0.1,
		CubeSize = 10,
		DamageMultiplier = 40,
	},
}

local BLOOD_COLOR_ABSORPTION = {
	[BLOOD_COLOR_RED] = Vector(0.3, 1.1, 1.1),
	[BLOOD_COLOR_YELLOW] = Vector(0.3, 0.3, 1.1), -- yellow is formed by mixing red and green
	[BLOOD_COLOR_GREEN] = Vector(0.3, 1.1, 1.1),
	[BLOOD_COLOR_MECH] = Vector(10, 10, 10),
	[BLOOD_COLOR_ANTLION] = Vector(0.3, 0.2, 1.1), -- a little yellow-green
	[BLOOD_COLOR_ZOMBIE] = Vector(0.3, 1.1, 1.1),
	[BLOOD_COLOR_ANTLION_WORKER] = Vector(0, 0, 0),
}

local function getDamageTypeConfig(damageType)
	for _, config in ipairs(DAMAGE_TYPE_BLOOD_CONFIGS) do
		if bit.band(damageType, config.DamageFlags) ~= 0 then
			return config
		end
	end
	return nil
end

local function getConfig(attacker, damageType)
	local config = nil

	if attacker:IsValid() and attacker.GetActiveWeapon and IsValid(attacker:GetActiveWeapon()) then
		config = WEAPON_BLOOD_CONFIGS[attacker:GetActiveWeapon():GetClass()]
	end

	if damageType and not config then
		config = getDamageTypeConfig(damageType)
	end

	return config
end

local function sprayBlood(damageType, victim, attacker, position, force, damage, material)
	local config = getConfig(attacker, damageType)
	if not config then return end

	local entpos = (victim:IsNPC() or victim:IsPlayer()) and
		victim:GetBonePosition(0) or
		victim:GetPos()

	local normal = config.FromEntity and
		(entpos - position):GetNormalized() or
		attacker == LocalPlayer() and
		attacker:GetAimVector() or
		force:GetNormalized()

	local density = math.random(config.MinDensity, config.MaxDensity)
	local velocity = normal * config.VelocityPower

	local damageMultiplier = config.DamageMultiplier or 0
	density = density + damageMultiplier * damage

	gellyx.emitters.Sphere({
		center = config.FromEntity and
			entpos or
			position,
		velocity = velocity,
		radius = config.CubeSize,
		density = density,
		randomness = config.Randomness,
		material = material,
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
				not victim:IsRagdoll() )
		then
			return
		end

		sprayBlood(type, victim, attacker, position, force, damage, {
			Roughness = 0, -- blood isn't rough at all
			IsSpecularTransmission = false, -- blood is translucent
			RefractiveIndex = 1.373, -- blood has a slightly higher refractive index than water
			Absorption = BLOOD_COLOR_ABSORPTION[victim:GetBloodColor() or BLOOD_COLOR_RED],
			DiffuseColor = Vector(0, 0, 0),
		})
	end
)

gellyx.presets.select("Blood")

hook.Add("GellyModsShutdown", "gelly.builtin.blood-mod", function()
	hook.Remove("GellyXDamage", "gelly.builtin.blood-mod")
	-- remove any left over blood
	gelly.Reset()

	print("Blood mod unloaded")
end)
